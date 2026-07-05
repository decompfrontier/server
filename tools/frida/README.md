# tools/frida — client runtime debugging

Full guide: `C:\Users\Evan\Desktop\BF_FRIDA_GUIDE.md`.

This directory holds Frida agents that observe/drive the BF client at runtime.
Filled-in agents and captures are git-ignored (they embed reversed offsets and
are machine-specific); this README is the shareable part.

## Target
- Primary: Android `libgame.so` (arm64) under an emulator — its offsets match
  the project's IDA exports (`response_mappings.json` `func_addr`).
- Package: `com.gumi.bravefrontier` (confirm in the APK manifest).

## Offsets to fill (from the IDA database, as module-base-relative)
| Symbol | Purpose | Offset |
|---|---|---|
| `JsonNode::addParam(const char*, ...)` | dump outgoing request key/values | `0x????` |
| `GameResponseParser::getResponseObject` | dump response dispatch keys | `0x????` |
| `TutorialManager::<advanceStep>` (TBD) | drive guided tutorial UI | `0x????` |

## Agents (build these — templates in the guide §2)
- `dump_request.js` — logs every `addParam(key, value)`.
- `dump_response.js` — logs every dispatched response key.
- `agent.js` — combined dump used by the tutorial driver (git-ignored once
  offsets are filled).

## Driver (to build)
- `tutorial_drive.py` — subscribes to the Frida message stream, sequences
  `adb input tap` per guided step, and asserts against
  `deploy/log/http_log_*.log`. See guide §3.

## First-session smoke test
Attach the dump agent, open Summon in the client, confirm the Frida log matches
an existing `http_log_Uo86DcRh_*.log`. That proves the pipeline before capturing
anything new.
