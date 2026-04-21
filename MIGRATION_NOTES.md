# Migration Notes

Companion to `BUILD.md`. Targets someone who has landed from the older Seltraeh/server fork (at `C:\Users\Evan\BF\BF-WorkingDir\server\`) and needs to port its hand-written handlers onto the new, KDL-driven architecture.

If you are reverse-engineering a brand-new request from scratch, the old tree's `HANDLER_REVERSING_PLAYBOOK.md` and `HANDLER_BLUEPRINT.md` still apply — with the response-construction step swapped out for this doc's KDL recipe.

---

## What changed between the two trees

| Dimension | Old fork (`BF-WorkingDir/server`) | New tree (this repo) |
|---|---|---|
| **Request parsing** | `req["Km35HAXv"][0]["edy7fq3L"].asUInt()` by hand | `FooReq req{}; glz::read_json(req, json)` |
| **Response building** | `Json::Value res; res["fEi17cnx"] = ...; snap->Serialize(res);` | Populate `FooResp` struct; `glz::write_json(resp)` |
| **Handler base class** | Virtual `HandlerBase` with `GetGroupId()` / `GetAesKey()` | `HANDLEF(Name)` macro emitting a coroutine that returns `HandleResult`. **AES key lives on the `REGISTER(...)` line, not on the handler class.** |
| **Registration** | `REGISTER_HANDLER(FooRequestHandler)` plus auto-registration | Single line in `gimuserver/gme/GmeControllerHandlers.cpp`: `REGISTER("<8-char ID>", FuncName, "<AES key>");` |
| **Async DB** | Callback pyramids: `GME_DB->execSqlAsync(sql, succ_cb, err_cb, args...)` nested two or three deep | C++20 coroutines: linear `co_await`. No pyramid. |
| **Crypto** | `BfCrypt::ReadGME` / `BfCrypt::BuildGME` | Same names, unchanged. |
| **Handlers wired** | 288 | 12 (see list below) |
| **Response structs hand-coded** | 326 | 0 — all from KDL |
| **MST container** | `gimuserver/system/MstConfig.hpp` with hand-typed structs (`UnitMstData`, `ItemMstEntry`, etc.) | `gimuserver/drogon/ServerCacheMst.hpp` with an `auto_cache(key, name)` macro; MST structs come from `packet-generator/assets/mst/*.kdl`. **`UnitMst` is `auto_cache`d but not loaded** — the `LoadJson<UnitMstCache>` call in `ServerCache.cpp` is commented out pending a hashed-key `unit.json` source file (see "Outstanding blockers" below). |

### Handlers already ported

`Initialize`, `BadgeInfo`, `ControlCenterEnter`, `DeckEdit`, `FriendGet`, `GatchaList`, `HomeInfo`, `MissionStart`, `UpdateInfoLight`, `UserInfo`, `UnitFavorite`, `UnitEvo` (stub — see notes).

Wired in `gimuserver/gme/GmeControllerHandlers.cpp`. Skip these when porting. `UnitEvo` is registered but currently returns `{}`; the real implementation is gated on `UnitMst` being loaded.

### Database schema

The `user_units` table is now created by `gimuserver/db/MigrationManager.cpp` migrations (previously `#if 0`'d out). Schema mirrors the old fork: base columns (`id`, `user_id`, `unit_id`), 28 stat columns (level, base/add/ext/limit_over for hp/atk/def/heal, exp, skill ids, element, fe_bp, unit_type_id), 4 sphere-equip slots, and `favorite_flg`. AUTOINCREMENT is bumped to ≥10000 via a sentinel insert/delete — the client crashes on instance ids below ~10407.

### Outstanding blockers

**Hashed-key `unit.json` MST file.** The new tree's `deploy/system/` does not contain a wire-format `unit.json` for `LoadJson<UnitMstCache>` to consume. The old fork ships friendly-key `F_UNIT_MST_*.json`; the field map for converting friendly → hashed lives in `packet-generator/assets/mst/unit.kdl`. Until this file is produced, `ServerCache::unitMst()` returns an empty vector and any handler that does an MST lookup (`UnitMix`, `UnitEvo` real impl, `UnitSell`) will silently no-op.

---

## Handler shape, side by side

**Old fork** — `UnitFavoriteRequestHandler::Handle`:

```cpp
void Handler::UnitFavoriteRequestHandler::Handle(UserInfo& user, DrogonCallback cb,
                                                  const Json::Value& req) const
{
    const uint32_t userUnitId = req["Km35HAXv"][0]["edy7fq3L"].asUInt();
    const bool     favorite   = req["Zf3Cq6t9"].asBool();  // example

    GME_DB->execSqlAsync(
        "UPDATE user_units SET favorite=? WHERE user_id=? AND id=?",
        [cb](const drogon::orm::Result&) {
            Json::Value res;
            res["Zf3Cq6t9"] = true;
            cb(newGmeOkResponse(GetGroupId().c_str(), GetAesKey().c_str(), res));
        },
        [cb](const drogon::orm::DrogonDbException& e) {
            Json::Value res;
            cb(newGmeOkResponse(GetGroupId().c_str(), GetAesKey().c_str(), res));
        },
        favorite, user.info.userID, userUnitId
    );
}
```

**New tree** — same handler as a coroutine:

```cpp
// gimuserver/gme/UnitFavorite.cpp
#include "App.hpp"
#include "Handlers.hpp"

HANDLEF(UnitFavorite)
{
    UnitFavoriteReq req{};
    if (const auto ec = glz::read_json(req, json); ec) {
        co_return HandleResult::error("malformed request");
    }

    auto db = drogon::app().getDbClient();
    co_await db->execSqlCoro(
        "UPDATE user_units SET favorite=? WHERE user_id=? AND id=?",
        req.favorite, session->get<std::string>("userID"), req.user_unit_id
    );

    UnitFavoriteResp resp{};
    resp.favorite = req.favorite;
    co_return HandleResult::success(glz::write_json(resp));
}
```

Plus one line in `GmeControllerHandlers.cpp`:

```cpp
REGISTER("<8-char request ID>", UnitFavorite, "<16-char AES key>");
```

The AES key carries over unchanged from the old `GetAesKey()`.

---

## Coroutine translation traps

Three shapes in the old code that don't translate mechanically:

1. **Fire-and-forget writes.** Old code occasionally dispatches a DB write and returns a response *without* awaiting the write — typically for telemetry, logging, or non-critical state. Mechanically translating to `co_await` serializes the response behind the write.
   **Fix:** keep it fire-and-forget with `drogon::app().getLoop()->queueInLoop([]{ /* write */ });` before you build and return the response.

2. **Implicit transaction boundaries.** The old pyramid style sometimes held an implicit per-connection transaction across the callback chain. The coroutine form doesn't — every `co_await` can land on a different connection.
   **Fix:** when the write sequence must be atomic (e.g. `UnitMix` DELETEing sacrificed units **and** crediting XP to the target), wrap in an explicit `drogon::orm::Transaction`.

3. **Dynamic AES keys.** The new `REGISTER("id", func, "literal_key")` form bakes the AES key as a string literal at compile time. If any old handler returned a non-literal from `GetAesKey()` (session-derived, per-user-state), the one-line `REGISTER` can't represent it.
   **Audit before porting:** `rg -n 'GetAesKey\b' C:\Users\Evan\BF\BF-WorkingDir\server\gimuserver\gme\handlers\` and look for any return that isn't a string literal. No known cases today, but confirm per batch.

---

## How to build a KDL schema

The language reference lives in the upstream packet-generator repo's HOWTO. What follows is the **repo-local** recipe: the exact steps that have worked for porting handlers on this branch, in order.

1. **Find the captured request/response JSON.**
   `C:\Users\Evan\BF\BF-WorkingDir\server\deploy\log_req\<8-char ID>_*.json` and `log_res\<8-char ID>_*.json`. The old tree has 1,056 request and 1,044 response captures — ample ground truth.

2. **Harvest hashed keys from the old response struct.**
   Open `C:\Users\Evan\BF\BF-WorkingDir\server\gimuserver\gme\response\Foo*.hpp`. The `Serialize(Json::Value&)` method spells every hashed key. Copy them to a scratchpad.

3. **Pick a target KDL file.**
   If the handler belongs to an existing feature topic, extend the relevant file under `packet-generator/assets/net/` (e.g. `user.kdl`, `items.kdl`, `gacha.kdl`). Otherwise author a new `foo.kdl` and add `import "net/foo.kdl"` to `packet-generator/assets/all.kdl`.

4. **Author `FooReq` and `FooResp`.**
   ```kdl
   json FooReq {
       doc "Short summary of this request"

       field user_unit_id type="i32::str" {
           key "edy7fq3L"
           doc "Target unit ID (quoted int on the wire)"
       }
       field favorite type="bool::int" {
           key "Zf3Cq6t9"
           doc "1 to favorite, 0 to unfavorite"
       }
   }
   ```
   Every node needs a `doc`. The generator fails on a missing `doc` — that's intentional, so future-you can grep for what a hashed key means.

5. **Pick type modifiers carefully.**
   - Quoted integers → `i32::str` / `i64::str`.
   - IDs, zel, timestamps → `i64` or `u64` (i32 overflows).
   - Comma-joined arrays → `[i32::int]::sep(comma)`. Also `sep(at)` → `@`, `sep(colon)` → `|`.
   - Always-one-element arrays → `[Foo]::size(1)` — see `packet-generator/assets/net/gme.kdl` for examples.
   - `bool::int` for `0`/`1`, `bool::str` for `"false"`/`"true"`.

6. **Rebuild.**
   `cmake --build --preset debug-win64-debug` — the `pkgen_generate` target re-runs and rewrites `gimuserver/packets/all.hpp`. Read that file for the generated struct and confirm the shape.

7. **Write the handler.**
   Create `gimuserver/gme/Foo.cpp` mirroring `gimuserver/gme/Initialize.cpp`. Keep any business logic (MST lookups, DB work, XP math) **verbatim** from the old handler — the only parts that change are request parse (was `req["..."]`, now `glz::read_json`) and response build (was `Json::Value res["..."]`, now `glz::write_json(FooResp{...})`).

8. **Wire it.**
   Add a `REGISTER(...)` line in `gimuserver/gme/GmeControllerHandlers.cpp` between the existing entries. ID comes from the client capture; AES key is the string literal that was returned from the old handler's `GetAesKey()`.

9. **Golden-diff test.**
   Launch both servers side by side. Trigger the same action in the client. Diff the decrypted request and response JSON between the two captures. Any diff that isn't a deliberate improvement is a migration bug.

---

## Porting order (big picture)

1. **Dry-run: `UnitFavorite`.** Single DB column toggle, no formula math, tiny KDL. Validates the whole workflow before tackling anything larger.
2. **Prerequisite: surface `UnitMst` in `ServerCacheMst`.** ✅ Partially done. `auto_cache("2r9cNSdt", UnitMst)` is wired in `ServerCacheMst.hpp` and the `unitMst()` getter exists in `ServerCache.hpp/.cpp`. The `LoadJson<UnitMstCache>(mstRoot, "unit.json")` line is **commented out** because no hashed-key `unit.json` exists in `deploy/system/`. Producing that file (friendly-key `F_UNIT_MST_*.json` → hashed wire format using the `unit.kdl` field map) is the gating task before any of the unit-math handlers below.
3. **Unit verbs**, in this order: `UnitMix` → `UnitEvo` → `UnitSell`. Mix validates the XP-math translation; Evo reuses Mix infrastructure for DELETE; Sell is simplest but depends on `UnitMst.sell_price`, so do it last in the family.
4. **Item verbs**: `ItemSphereEqp`, `ItemEdit`, `ItemSell`.
5. **Mission / Campaign Start+End, Gift/Receipt.** Audit the new tree for a `UserState::clear` equivalent before porting — old code relied on that helper.
6. **Arena / Challenge Arena / Colosseum / Vortex.** Each gets its own `.kdl` under `assets/net/`.
7. **Raid.** 40+ handlers — largest single chunk. Port last.

Don't try to batch more than one feature family per branch.

---

## Static data dedup rule

The old tree's `server/deploy/system/` has 61 MST JSON files. Many are already present in the new `deploy/system/`. Before copying anything, check:

1. **Exists in both, identical** → leave alone.
2. **Missing from new tree** → copy from old tree.
3. **Different contents** → investigate. Use these tiebreakers in order:
   - Size or row-count ratio (old > 2× new, or vice versa → flag for manual review).
   - Cross-check against captures in `deploy/log_res/`: whichever JSON's field values match the captures wins.
   - `git log -- deploy/system/<file>` on the new tree: a single commit with "stub", "placeholder", or "todo" in the message tells you to keep the old file.

A dedup helper script at `scripts/compare_deploy_data.py` (to be written before the first static-data port) automates the size/row-count report.

---

## Quick references

| Concern | Where to look |
|---|---|
| New-tree handler registration | `gimuserver/gme/GmeControllerHandlers.cpp` |
| Generated C++ from KDL | `gimuserver/packets/all.hpp` (gitignored) |
| KDL schemas | `packet-generator/assets/net/*.kdl`, `packet-generator/assets/mst/*.kdl` |
| MST caching | `gimuserver/drogon/ServerCacheMst.hpp`, `ServerCache.cpp` |
| Crypto | `gimuserver/utils/BfCrypt.hpp` (unchanged from old tree) |
| Sample ported handler | `gimuserver/gme/Initialize.cpp` |
| Old handler reference | `C:\Users\Evan\BF\BF-WorkingDir\server\gimuserver\gme\handlers\` |
| Old response reference | `C:\Users\Evan\BF\BF-WorkingDir\server\gimuserver\gme\response\` |
| Old reverse-engineering playbooks | `C:\Users\Evan\BF\BF-WorkingDir\server\HANDLER_BLUEPRINT.md`, `HANDLER_REVERSING_PLAYBOOK.md` |
| Upstream migration guide | `C:\Users\Evan\Downloads\Decompfrontier_Server_Migration_Guide (1).docx` |
