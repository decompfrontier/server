"""GME wire test harness — call any handler on a running local server
without launching the client (and without Frida).

Speaks the full BF envelope (handbook §2.1 / §8.39):
  AES-ECB, key = handler AES key zero-padded/truncated to 16 bytes
  (BfCrypt.cpp AES::MIN_KEYLENGTH), PKCS#7 padding, base64, wrapped as
  {"F4q6i9xe":{"Hhgi79M1":"<GroupId>","aV6cLn3v":"..."},
   "a3vSYuq2":{"Kn51uR4Y":"<b64(AES(inner JSON))>"}}
  POST /bf/gme/action.php; the response body decrypts with the same key.

Identity: handlers that resolve the user via gme::getUserIdentity need the
inner body to carry the Gumi Live id:
  {"IKqx1Cn9":[{"iN7buP2h":"<gumi_live_users.id>"}]}
(iN7buP2h = gumi_live_userid.  h7eY3sAK is the LOCAL user id — a different
field; sending only that yields "Gumi Live user ID mismatch".)
Handlers on the getSoleUserId bridge need no identity at all.

Requires: pip install pycryptodome.  Server must be running (default
127.0.0.1:9960).

Usage examples:
  python tools/gme_wire_test.py 0IXGiC9t CZE56XAY '{"wx1ZLFj9":[{"a2utCvs8":"1:0:1006:30005:0"}]}'
  python tools/gme_wire_test.py cTZ3W2JG ScJx6ywWEb0A3njT '{"IKqx1Cn9":[{"iN7buP2h":"ynFLY46O"}]}'

GroupId/AES pairs live in gimuserver/gme/handlers/GmeControllerHandlers.cpp.
"""
import base64
import gzip
import json
import sys
import urllib.request

from Crypto.Cipher import AES

HOST = "http://127.0.0.1:9960/bf/gme/action.php"


def pad_key(key: str) -> bytes:
    b = key.encode()[:16]
    return b + b"\x00" * (16 - len(b))


def encrypt(inner: str, key: str) -> str:
    c = AES.new(pad_key(key), AES.MODE_ECB)
    data = inner.encode()
    pad = 16 - (len(data) % 16)
    data += bytes([pad]) * pad
    return base64.b64encode(c.encrypt(data)).decode()


def decrypt(b64: str, key: str) -> str:
    c = AES.new(pad_key(key), AES.MODE_ECB)
    raw = c.decrypt(base64.b64decode(b64))
    raw = raw[: -raw[-1]]  # strip PKCS#7 pad
    return raw.decode(errors="replace")


def call(group_id: str, aes_key: str, inner: dict, host: str = HOST) -> dict | None:
    """POST one GME request; prints and returns the decrypted response dict."""
    envelope = {
        "F4q6i9xe": {"Hhgi79M1": group_id, "aV6cLn3v": "wire-test"},
        "a3vSYuq2": {"Kn51uR4Y": encrypt(json.dumps(inner), aes_key)},
    }
    req = urllib.request.Request(
        host,
        data=json.dumps(envelope).encode(),
        headers={"Content-Type": "application/json", "Accept-Encoding": "gzip"},
    )
    with urllib.request.urlopen(req, timeout=30) as r:
        body = r.read()
        if r.headers.get("Content-Encoding") == "gzip":
            body = gzip.decompress(body)
    resp = json.loads(body)
    if "b5PH6mZa" in resp:
        print("ERROR block:", resp["b5PH6mZa"])
        return None
    if "a3vSYuq2" in resp and resp["a3vSYuq2"].get("Kn51uR4Y"):
        inner_resp = json.loads(decrypt(resp["a3vSYuq2"]["Kn51uR4Y"], aes_key))
        print(json.dumps(inner_resp, ensure_ascii=False)[:4000])
        return inner_resp
    print("empty response envelope:", json.dumps(resp)[:400])
    return None


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print(__doc__)
        sys.exit(1)
    call(sys.argv[1], sys.argv[2], json.loads(sys.argv[3]))
