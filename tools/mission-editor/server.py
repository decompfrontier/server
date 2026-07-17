#!/usr/bin/env python3
"""
Mission Editor — local backend.

A dependency-free (Python stdlib only) local web server for the community
mission-authoring tool.  Serves the editor UI and a small REST API that reads
and writes the curated archive files the game server loads at boot:

    deploy/archive/mission.json   the missions being authored
    deploy/archive/unit.json      the enemy/unit visual+stat palette (read-only)
    deploy/archive/ai.json        the AI-behavior palette (read-only)

Run:
    python tools/mission-editor/server.py
    # then open http://localhost:8777

See DATA_MODEL.md for the schema every field maps to.
"""

import json
import os
import shutil
import sys
import urllib.error
import urllib.request
from datetime import datetime
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import urlparse, parse_qs

# ---------------------------------------------------------------------------
# Paths — resolve the repo's deploy/archive relative to this file, overridable.
# ---------------------------------------------------------------------------
HERE = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(HERE, "web")
REPO_ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
ARCHIVE_DIR = os.environ.get(
    "BF_ARCHIVE_DIR", os.path.join(REPO_ROOT, "deploy", "archive")
)
# Original decoded MST used to default rewards for a mission id (optional).
MISSION_MST = os.path.join(REPO_ROOT, "deploy", "system", "mission_mst.json")

PORT = int(os.environ.get("BF_EDITOR_PORT", "8777"))
# The running game server (drogon) whose archive we hot-reload for "Test in game".
GAME_SERVER = os.environ.get("BF_GAME_SERVER", "http://127.0.0.1:9960")

# Element / personality-type display names (see DATA_MODEL.md §4, §7).
ELEMENTS = {1: "Fire", 2: "Water", 3: "Earth", 4: "Thunder", 5: "Light", 6: "Dark"}
UNIT_TYPES = {1: "Lord", 2: "Anima", 3: "Breaker", 4: "Guardian", 5: "Oracle", 6: "Special"}

# Battlefield canvas + authoring limits (verified bounds from the mission-10
# capture, with margin; see DATA_MODEL.md §4/§6).
META = {
    "field": {"xMin": 0, "xMax": 320, "yMin": 200, "yMax": 400},
    "slots": [{"x": 180, "y": 302}, {"x": 120, "y": 248}, {"x": 96, "y": 352}],
    "limits": {"maxMonstersPerStage": 6, "maxPartyConditions": 4,
               "maxSelfConditions": 5, "maxActionFlags": 12},
    "elements": ELEMENTS,
    "unitTypes": UNIT_TYPES,
    "treasureTypes": {1: "Zel", 2: "Karma", 3: "Crystal", 4: "Item"},
    # AI authoring vocabulary. These are the dev-documented, client-accepted
    # values (archive.hpp) — the wire fields are free-form strings, so the UI
    # offers these as suggestions but allows custom values; contributors confirm
    # new ones empirically via "Test in game". See DATA_MODEL.md §5/§9.
    "aiVocab": {
        "actionTypes": ["attack", "guard", "wait", "turn_end", "skill"],
        "searchTerms": ["random", "hp_min", "hp_max"],
        "selfConditionTypes": ["skill", "hp_pr_under", "party_members"],
    },
}


def _read_json(path):
    with open(path, "r", encoding="utf-8-sig") as fh:
        return json.load(fh)


def _load_archive(name):
    return _read_json(os.path.join(ARCHIVE_DIR, name))


def _unit_palette():
    """Return the enemy palette: the visual/identity fields the UI needs."""
    out = []
    for u in _load_archive("unit.json"):
        out.append({
            "id": u.get("id"),
            "name": u.get("name", ""),
            "element": u.get("element"),
            "rarity": u.get("rarity"),
        })
    out.sort(key=lambda u: (u.get("element") or 0, u.get("id") or 0))
    return out


def _ais_full():
    """Full AI records (id, name, actions[...]) sorted by id — for the AI builder."""
    ais = _load_archive("ai.json")
    ais.sort(key=lambda a: a.get("id") or 0)
    return ais


def _mission_mst_rewards(mission_id):
    """Best-effort reward defaults from the original mission MST for an id."""
    try:
        data = _read_json(MISSION_MST)
    except Exception:
        return None
    rows = next(iter(data.values())) if isinstance(data, dict) else data
    for r in rows:
        # j28VNcUW=id, exp, Najhr8m6/zel, karma, stamina_cost (see mst schema)
        if str(r.get("j28VNcUW")) == str(mission_id):
            def _i(v):
                try:
                    return int(v)
                except Exception:
                    return 0
            return {
                "zel": _i(r.get("Najhr8m6")),
                "karma": _i(r.get("karma")),
                "exp": _i(r.get("exp")),
                "energy_cost": _i(r.get("69vnphig")),
            }
    return None


def _reload_game_server():
    """Ask the running game server to hot-reload its mission archive."""
    url = GAME_SERVER.rstrip("/") + "/offline_mod/reload_missions"
    try:
        req = urllib.request.Request(url, method="POST")
        with urllib.request.urlopen(req, timeout=5) as r:
            return {"reached": True, "status": r.status,
                    "result": json.loads(r.read().decode("utf-8"))}
    except urllib.error.HTTPError as ex:
        return {"reached": True, "status": ex.code, "error": ex.read().decode("utf-8", "replace")}
    except Exception as ex:
        return {"reached": False,
                "error": f"Could not reach the game server at {GAME_SERVER}. "
                         f"Is it running? ({type(ex).__name__}: {ex})"}


def _save_archive_array(filename, prefix, data):
    """Write an archive JSON array, keeping a timestamped backup first."""
    path = os.path.join(ARCHIVE_DIR, filename)
    if os.path.exists(path):
        backups = os.path.join(HERE, "backups")
        os.makedirs(backups, exist_ok=True)
        stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        shutil.copy2(path, os.path.join(backups, f"{prefix}_{stamp}.json"))
    tmp = path + ".tmp"
    with open(tmp, "w", encoding="utf-8") as fh:
        json.dump(data, fh, indent=1, ensure_ascii=False)
        fh.write("\n")
    os.replace(tmp, path)


def _save_missions(missions):
    _save_archive_array("mission.json", "mission", missions)


def _save_ais(ais):
    _save_archive_array("ai.json", "ai", ais)


class Handler(BaseHTTPRequestHandler):
    server_version = "MissionEditor/0.1"

    # -- helpers ----------------------------------------------------------
    def _send_json(self, obj, code=200):
        body = json.dumps(obj, ensure_ascii=False).encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _send_file(self, path, ctype):
        try:
            with open(path, "rb") as fh:
                body = fh.read()
        except FileNotFoundError:
            self.send_error(404, "Not found")
            return
        self.send_response(200)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _error(self, msg, code=400):
        self._send_json({"error": msg}, code)

    def log_message(self, fmt, *args):  # quieter console
        sys.stderr.write("  [editor] " + (fmt % args) + "\n")

    # -- routing ----------------------------------------------------------
    def do_GET(self):
        parsed = urlparse(self.path)
        route = parsed.path
        try:
            if route == "/" or route == "/index.html":
                return self._send_file(os.path.join(WEB_DIR, "index.html"),
                                       "text/html; charset=utf-8")
            if route == "/style.css":
                return self._send_file(os.path.join(WEB_DIR, "style.css"), "text/css")
            if route == "/app.js":
                return self._send_file(os.path.join(WEB_DIR, "app.js"),
                                       "application/javascript")
            if route == "/api/meta":
                return self._send_json(META)
            if route == "/api/missions":
                return self._send_json(_load_archive("mission.json"))
            if route == "/api/units":
                return self._send_json(_unit_palette())
            if route == "/api/ais":
                return self._send_json(_ais_full())
            if route == "/api/mission-mst":
                mid = parse_qs(parsed.query).get("id", [None])[0]
                return self._send_json(_mission_mst_rewards(mid) or {})
            self.send_error(404, "Not found")
        except Exception as ex:  # never crash the editor on a bad read
            self._error(f"{type(ex).__name__}: {ex}", 500)

    def do_POST(self):
        route = urlparse(self.path).path
        length = int(self.headers.get("Content-Length", "0"))
        raw = self.rfile.read(length) if length else b""
        try:
            payload = json.loads(raw.decode("utf-8")) if raw else None
        except Exception as ex:
            return self._error(f"Bad JSON: {ex}")
        try:
            if route == "/api/missions":
                if not isinstance(payload, list):
                    return self._error("Expected a JSON array of missions")
                _save_missions(payload)
                return self._send_json({"ok": True, "count": len(payload)})
            if route == "/api/ais":
                if not isinstance(payload, list):
                    return self._error("Expected a JSON array of AI records")
                _save_ais(payload)
                return self._send_json({"ok": True, "count": len(payload)})
            if route == "/api/test":
                # Save, then hot-reload the game server so the mission is playable now.
                if not isinstance(payload, list):
                    return self._error("Expected a JSON array of missions")
                _save_missions(payload)
                return self._send_json({"ok": True, "saved": len(payload),
                                        "reload": _reload_game_server()})
            self.send_error(404, "Not found")
        except Exception as ex:
            self._error(f"{type(ex).__name__}: {ex}", 500)


def main():
    if not os.path.isdir(ARCHIVE_DIR):
        sys.exit(f"archive dir not found: {ARCHIVE_DIR}\n"
                 f"Set BF_ARCHIVE_DIR to override.")
    httpd = ThreadingHTTPServer(("127.0.0.1", PORT), Handler)
    print(f"\n  Mission Editor running.")
    print(f"  Archive:  {ARCHIVE_DIR}")
    print(f"  Open:     http://localhost:{PORT}\n")
    print("  Ctrl+C to stop.\n")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n  Stopped.")


if __name__ == "__main__":
    main()
