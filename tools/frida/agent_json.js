// agent_json.js — Stage C: plaintext capture at the JSON layer.
//
// This is where "lost data structures" are recovered: it logs every key/value
// the client writes into a request (pre-encryption) and every response class it
// dispatches on (post-decryption), so captures map directly onto
// readparam_analysis.json (class -> key -> type -> setter -> member).
//
// Offsets are NOT baked in (keeps this file shareable). The host driver
// (capture.py) reads win_offsets.json and injects `globalThis.OFFSETS` before
// this script runs. For standalone `frida -l` use, fill the fallback below.
//
// Windows client is PE32 / x86, IDA imagebase 0x400000, ASLR on — so the runtime
// address is: moduleBase + (ida_addr - 0x400000).  NEVER use the raw IDA address.

'use strict';

const MODULE    = 'BraveFrontier.Windows.exe';
const IMAGEBASE = 0x400000;

// Injected by capture.py; fallback lets the agent run via `frida -l` after you
// hand-edit these two IDA addresses.
const OFFSETS = (typeof globalThis.OFFSETS !== 'undefined') ? globalThis.OFFSETS : {
  addParam: 0x0,            // JsonNode::addParam(...)            <- fill from .i64
  getResponseObject: 0x0,   // GameResponseParser::getResponseObject <- fill from .i64
};

function rt(idaAddr) {
  const base = Process.getModuleByName(MODULE).base;   // Frida 17 instance API
  return base.add(ptr(idaAddr).sub(IMAGEBASE));
}

function tryStr(p) { try { return p.readUtf8String(); } catch (e) { return null; } }
function tryInt(p) { try { return p.toInt32(); } catch (e) { return null; } }

// --- request builder: dump every key/value the client emits -----------------
//
// CALIBRATION NOTE (x86 thiscall vs the Android arm64 template):
//   On arm64 `this`=x0 so key=arg1,value=arg2. On x86 an MSVC member function is
//   __thiscall: `this` is in ECX (this.context.ecx), and the *stack* args start
//   at args[0]. So the likely mapping here is key=args[0], value=args[1].
//   Because the exact signature isn't confirmed yet, the first run logs several
//   candidates; after you clear a KNOWN request (e.g. open Summon) and match it
//   against http_log, keep only the correct pair and delete the rest.
if (OFFSETS.addParam && OFFSETS.addParam !== 0x0) {
  const addr = rt(OFFSETS.addParam);
  send({ tag: 'ready', hook: 'addParam', at: addr.toString() });
  Interceptor.attach(addr, {
    onEnter(args) {
      const ecx = this.context.ecx;                 // candidate `this`
      send({
        tag: 'addParam',
        ecx: ecx ? ecx.toString() : null,
        a0_str: tryStr(args[0]), a0_int: tryInt(args[0]),
        a1_str: tryStr(args[1]), a1_int: tryInt(args[1]),
        a2_str: tryStr(args[2]), a2_int: tryInt(args[2]),
      });
    }
  });
} else {
  send({ tag: 'warn', msg: 'addParam offset not set — fill win_offsets.json' });
}

// --- response dispatch: which response class/key the client actually reads ---
if (OFFSETS.getResponseObject && OFFSETS.getResponseObject !== 0x0) {
  const addr = rt(OFFSETS.getResponseObject);
  send({ tag: 'ready', hook: 'getResponseObject', at: addr.toString() });
  Interceptor.attach(addr, {
    onEnter(args) {
      send({
        tag: 'getResponseObject',
        a0_str: tryStr(args[0]), a0_int: tryInt(args[0]),
        a1_str: tryStr(args[1]), a1_int: tryInt(args[1]),
      });
    }
  });
} else {
  send({ tag: 'warn', msg: 'getResponseObject offset not set — fill win_offsets.json' });
}
