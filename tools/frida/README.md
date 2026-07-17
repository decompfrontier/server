# tools/frida — client runtime debugging

Frida makes the BF client observable at runtime so we can **capture real wire
structures** instead of guessing them from static IDA reversing. Every field still
carrying `// UNVERIFIED:` in the server or KDL is one capture away from being
confirmed and *preserved* as generated schema.

Companion docs on the Desktop: `BF_FRIDA_GUIDE.md` (the *why*, Android-oriented),
`BF_WORKFLOW_CHECKLISTS.md` (the handler workflow this plugs into).

> **Active target: the Windows AppX client** (`BraveFrontier.Windows.exe`). It needs
> no emulator / adb / frida-server — Frida attaches to the local process. The
> Android `libgame.so` path is kept as an alternate at the bottom. Wire *keys* are
> identical across both clients; only *addresses* differ.

---

## What's here

| File | Role | Committed? |
|---|---|---|
| `agent_socket.js` | **Stage B (recommended): transport-agnostic capture at Winsock (`ws2_32`)** — catches all traffic to `:9960` no matter the HTTP library, no offsets | yes |
| `agent_curl.js` | Stage B alt: raw bytes at the libcurl boundary (only if the client actually uses libcurl exports) | yes |
| `agent_json.js` | Stage C: plaintext key/value at the JSON layer (offsets injected) | yes |
| `capture.py` | host driver: attach, load an agent, stream captures to `captures/` | yes |
| `win_offsets.json` | the two IDA offsets for the JSON layer | no (git-ignored) |
| `captures/` | capture output | no (git-ignored) |

> **Field note (first attempt):** `frida-trace -i "curl_*"` caught only background
> `WinHttp*` proxy-resolution calls and no `curl_*` — the client does **not** use the
> libcurl `curl_easy_*` path we first guessed. Use **`agent_socket.js`** (Winsock) as
> the default capture, and remember to **drive an in-game action after attaching** —
> nothing is captured until the client actually hits the server.

---

## The client (verified facts)

- `C:\Users\Evan\BF\BraveFrontierAppxClient\BraveFrontier.Windows.exe`
- **PE32, x86 (32-bit), ImageBase `0x400000`, ASLR on.** Cocos2d-x.
- Networking: `libcurl.dll` (the offline mod ships a patched drop-in that redirects
  to `127.0.0.1:9960`, HTTPS disabled), plus `libwebsockets.dll` / WinINet.
- Runs as a **UWP AppContainer** ⇒ Frida must run **as Administrator**, and the
  **loopback exemption** must be enabled or the client can't reach the local server.
- Offset source for Stage C: its own IDA db, `BraveFrontier.Windows.exe.i64`.

---

## Session prerequisites (every time)

1. **Loopback exemption** for the BF package — see the installer's Network
   Connection Guide (`CheckNetIsolation LoopbackExempt`). One-time per machine.
2. **Start the offline server:** from `deploy/`, run `gimuserverw.exe config.json`
   and wait for `:9960 LISTENING`.
3. **Launch the client via the offline mod** so its `libcurl.dll` redirects to the
   local server.
4. **Open the terminal as Administrator** for all `frida` / `capture.py` commands.

Confirm tooling once: `frida --version` (expect 17.2.17) and `frida-ps` list the
process. If the CLI errors, `pip install -U frida-tools`.

---

## Stage A — prove Frida attaches (zero offsets, first session)

Named exports, so nothing to reverse. This both proves injection into the
AppContainer process **and** tells you which transport carries the API traffic:

```
frida-trace -n BraveFrontier.Windows.exe -i "curl_easy_setopt" -i "curl_easy_perform" -i "InternetConnect*" -i "WinHttp*"
```

Attach **first**, then drive an action that hits the server (open **Summon**, or
clear a mission). **Pass:** trace stubs fire as you tap. Background `WinHttp*`
proxy calls on a timer do **not** count — those are the OS sniffing proxy settings,
not a game request. In practice on this client the `curl_*` stubs never fire, so
go straight to the Winsock agent in Stage B.

---

## Stage B — raw body capture (recommended: Winsock, no offsets)

```
python capture.py --agent socket --flow mission_end
```

Then **drive the in-game action** (clear a mission). Produces
`captures/mission_end-<ts>.log` with `{tag:"connect"|"socket", dir, ascii, hex}`
records for every byte to/from `127.0.0.1:9960`. The HTTP request line + headers
are readable ASCII (URL path, Host); the body is AES-encrypted. **Verify by
correlating** with the server's decrypted log for the same handler — matching
timing + path proves the capture is real.

Use `--agent curl` instead only if Stage A showed `curl_*` firing.

### Fastest path for a field the server already receives
The server logs the **decrypted** request: `deploy/log/http_log_<GroupId>_*.log`
contains `REQUEST: { ...raw wire JSON... }` (see `GmeControllerHandlers.cpp:158`).
So for any request the server successfully parses, the wire keys are already there
— **you may not need Frida at all.** For the item-drop target, MissionEnd is
GroupId `9TvyNR5H` (AES key `oINq0rfUFPx5MgmT`): clear a mission that drops an item
and read `http_log_9TvyNR5H_*` for the real `4T0Q2Bh5` string. Use the socket/JSON
agents for requests the server *can't* parse or response fields the client reads
that the server doesn't send — that's Frida's unique value.

---

## Stage C — plaintext structure recovery (needs the two offsets)

1. **Get the offsets** from `BraveFrontier.Windows.exe.i64`. In IDA, run the
   project's exporter
   (`BraveFrontier IDA readParam and Handler exports and scripts/IDA Pro BF Extraction Scripts/11_export_pseudocode.py`):
   ```python
   export_functions_by_name("addParam")        # JsonNode::addParam(...)
   export_class("GameResponseParser")          # ::getResponseObject
   ```
   Fallbacks if unnamed: `export_class("JsonNode")`, or xref a known wire-key
   string via `02_string_dump.py` / `07_xref_heatmap.py`.
2. **Record them** as hex in `win_offsets.json` (`addParam`, `getResponseObject`).
   Do NOT convert — `capture.py` computes `moduleBase + (addr - 0x400000)` at
   runtime (x86, ASLR).
3. **Capture:**
   ```
   python capture.py --agent json --flow mission_end
   ```
4. **Calibrate once:** the `addParam` hook logs several candidate arg slots
   (x86 `__thiscall` puts `this` in ECX, so key/value are likely `args[0]`/`args[1]`,
   not the arm64 `args[1]`/`args[2]` from the Android guide). Clear a *known* request
   (Summon), match it against `http_log`, then trim `agent_json.js` to the correct
   pair.

---

## The recovery loop (why we capture)

A lost/wrong structure is any disagreement between:

1. `readparam_analysis.json` — the static catalog (class→key→type→setter→member).
2. Frida request capture (what the client **sends**).
3. Frida response capture (what the client **reads**).
4. `deploy/log/http_log_<GroupId>_*.log` — what the server parsed/sent.

Resolve, then **preserve**: write the confirmed shape into packet-generator KDL
(`packet-generator/assets/net/`, `mst/*.kdl`) and, if persisted, a
`PacketInterfaceFor<>` schema (workflow §1–§2). Drop the `// UNVERIFIED:` marker.

**First target:** `parseItemDrops` in `gimuserver/gme/handlers/Mission.cpp` — the
`item_rewards` request field (wire key `4T0Q2Bh5`) is a guessed `itemId:count`
split. Capture a mission that drops an item with the Stage-C `addParam` hook, read
the real bytes, correct the split.

---

## Alternate target: Android `libgame.so`

Its offsets line up 1:1 with `response_mappings.json` `func_addr`, but it needs an
emulator + adb + a matching `frida-server` (17.2.17). See `BF_FRIDA_GUIDE.md` §1.
Use only if you need the direct address correspondence to the Android IDA work.
