#!/usr/bin/env python3
"""capture.py — host driver for the BF Frida agents (Windows AppX target).

Attaches to the running client, loads one agent, and streams structured `send()`
records to both the console and tools/frida/captures/<flow>-<timestamp>.log.

Run **as Administrator** (the client is a UWP AppContainer process).

Prereqs each session (see README):
  1. Enable the UWP loopback exemption for the BF package.
  2. Start the offline server:  gimuserverw.exe config.json  (wait for :9960 LISTENING)
  3. Launch the client via the offline mod so its libcurl.dll redirects to :9960.

Examples:
  python capture.py --agent curl --flow summon
  python capture.py --agent json --flow mission_end --offsets win_offsets.json
"""
import argparse
import json
import os
import sys
import time

try:
    import frida
except ImportError:
    sys.exit("frida not importable — `pip install frida` (host core is 17.2.17).")

HERE = os.path.dirname(os.path.abspath(__file__))
CAPTURES = os.path.join(HERE, "captures")


def load_agent(agent, offsets_path):
    """Return agent JS source, prepending injected OFFSETS for the json agent."""
    fname = {"curl": "agent_curl.js", "json": "agent_json.js",
             "socket": "agent_socket.js"}[agent]
    with open(os.path.join(HERE, fname), "r", encoding="utf-8") as f:
        src = f.read()
    if agent == "json":
        raw = _read_offsets(offsets_path)
        # convert hex strings -> ints for the two hook addresses
        inj = {
            "addParam": int(str(raw.get("addParam", "0x0")), 16),
            "getResponseObject": int(str(raw.get("getResponseObject", "0x0")), 16),
        }
        if not (inj["addParam"] or inj["getResponseObject"]):
            print("[!] Both offsets are 0x0 in", offsets_path,
                  "- fill them from BraveFrontier.Windows.exe.i64 first.", file=sys.stderr)
        src = "globalThis.OFFSETS = %s;\n%s" % (json.dumps(inj), src)
    return src


def _read_offsets(path):
    if not path:
        path = os.path.join(HERE, "win_offsets.json")
    if not os.path.exists(path):
        return {}
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--agent", choices=["curl", "json", "socket"], default="socket")
    ap.add_argument("--process", default="BraveFrontier.Windows.exe")
    ap.add_argument("--flow", default="capture", help="label for the output file")
    ap.add_argument("--offsets", default=None, help="path to win_offsets.json (json agent)")
    args = ap.parse_args()

    os.makedirs(CAPTURES, exist_ok=True)
    stamp = time.strftime("%Y%m%d-%H%M%S")
    out_path = os.path.join(CAPTURES, "%s-%s.log" % (args.flow, stamp))
    out = open(out_path, "a", encoding="utf-8")

    def emit(obj):
        line = json.dumps(obj, ensure_ascii=False)
        print(line)
        out.write(line + "\n")
        out.flush()

    def on_message(message, data):
        if message.get("type") == "send":
            emit(message["payload"])
        elif message.get("type") == "error":
            emit({"tag": "js-error", "desc": message.get("description"),
                  "stack": message.get("stack")})
        else:
            emit({"tag": "raw", "message": message})

    try:
        device = frida.get_local_device()
        session = device.attach(args.process)
    except frida.ProcessNotFoundError:
        sys.exit("[x] '%s' not running. Launch the client via the offline mod first."
                 % args.process)
    except frida.PermissionDeniedError:
        sys.exit("[x] Permission denied. Re-run this terminal AS ADMINISTRATOR "
                 "(AppContainer processes require it).")

    script = session.create_script(load_agent(args.agent, args.offsets))
    script.on("message", on_message)
    script.load()

    print("[*] Attached to %s with agent '%s'." % (args.process, args.agent))
    print("[*] Writing capture to %s" % out_path)
    print("[*] Drive the client now (Ctrl-C to stop).")
    try:
        sys.stdin.read()
    except KeyboardInterrupt:
        pass
    finally:
        try:
            script.unload()
            session.detach()
        except Exception:
            pass
        out.close()
        print("\n[*] Capture saved: %s" % out_path)


if __name__ == "__main__":
    main()
