// agent_socket.js — transport-agnostic capture at the Winsock layer.
//
// Use this when Stage A (frida-trace) showed the API traffic does NOT go through
// libcurl (curl_* never fired). EVERY byte the client sends to / receives from the
// offline server (127.0.0.1:9960) passes through ws2_32.dll send/recv/WSASend/
// WSARecv regardless of the HTTP library, so this can't miss the traffic.
//
// What you get: the raw HTTP request line + headers (plaintext, so the URL path
// and Host are readable) followed by the AES-encrypted body. Correlate the body
// with the server's decrypted deploy/log/http_log_<GroupId>_*.log, or decrypt it
// offline with the per-GroupId AES key from exported_handlers.
//
// Run:  python capture.py --agent socket --flow mission_end   (terminal as Admin)
// Then DRIVE AN ACTION in-game (open Summon / clear a mission) — nothing is
// captured until the client actually talks to the server.

'use strict';

const SERVER_PORT = 9960;      // OFFLINEMOD_PORT default
const CAP = 8192;              // max bytes dumped per call (avoids huge messages)

const ws2 = Process.findModuleByName('ws2_32.dll');
if (!ws2) {
  send({ tag: 'error', msg: 'ws2_32.dll not loaded — is this the right process?' });
}

function exp(name) { return ws2 ? ws2.findExportByName(name) : null; }

// sockets we've seen connect to 127.0.0.1:9960 (as string keys)
const gameSockets = new Set();

function preview(bytes) {
  let ascii = '', hex = '';
  for (let i = 0; i < bytes.length; i++) {
    const b = bytes[i];
    ascii += (b >= 0x20 && b <= 0x7e) ? String.fromCharCode(b) : '.';
    hex += b.toString(16).padStart(2, '0');
  }
  return { ascii, hex };
}

function dump(dir, sock, ptr, len) {
  if (len <= 0) return;
  // If we've identified the game socket, ignore everything else. If we never
  // saw the connect (attached late), fall back to dumping all so nothing is lost.
  const key = sock.toString();
  if (gameSockets.size > 0 && !gameSockets.has(key)) return;
  const n = Math.min(len, CAP);
  let bytes;
  try { bytes = new Uint8Array(ptr.readByteArray(n)); }
  catch (e) { return; }
  const { ascii, hex } = preview(bytes);
  send({ tag: 'socket', dir, sock: key, len, shown: n,
         truncated: len > CAP, ascii, hex });
}

// ---- connect: learn which socket is the game's -----------------------------
for (const cname of ['connect', 'WSAConnect']) {
  const p = exp(cname);
  if (!p) continue;
  Interceptor.attach(p, {
    onEnter(args) {
      try {
        const sock = args[0];
        const name = args[1];                    // sockaddr*
        const family = name.readU16();           // AF_INET == 2
        const port = (name.add(2).readU8() << 8) | name.add(3).readU8(); // net order
        const ip = [0, 1, 2, 3].map(i => name.add(4 + i).readU8()).join('.');
        if (family === 2 && port === SERVER_PORT) {
          gameSockets.add(sock.toString());
          send({ tag: 'connect', sock: sock.toString(), ip, port });
        }
      } catch (e) {}
    }
  });
}

// ---- send / WSASend: data is present on entry ------------------------------
{
  const p = exp('send');
  if (p) Interceptor.attach(p, {          // int send(s, buf, len, flags)
    onEnter(args) { dump('send', args[0], args[1], args[2].toInt32()); }
  });
}
{
  const p = exp('WSASend');
  if (p) Interceptor.attach(p, {          // WSASend(s, lpBuffers, count, ...)
    onEnter(args) {
      const sock = args[0], bufs = args[1], count = args[2].toInt32();
      for (let i = 0; i < count; i++) {
        const wb = bufs.add(i * 8);        // WSABUF{ u_long len; char* buf } on x86
        dump('send', sock, wb.add(4).readPointer(), wb.readU32());
      }
    }
  });
}

// ---- recv / WSARecv: data is present on return -----------------------------
{
  const p = exp('recv');
  if (p) Interceptor.attach(p, {          // int recv(s, buf, len, flags)
    onEnter(args) { this.sock = args[0]; this.buf = args[1]; },
    onLeave(retval) {
      const n = retval.toInt32();
      if (n > 0) dump('recv', this.sock, this.buf, n);
    }
  });
}
{
  const p = exp('WSARecv');
  if (p) Interceptor.attach(p, {          // WSARecv(s, lpBuffers, count, lpBytesRecvd, ...)
    onEnter(args) { this.sock = args[0]; this.bufs = args[1]; this.pRecvd = args[3]; },
    onLeave(retval) {
      if (retval.toInt32() !== 0) return;  // ignore pending/overlapped
      let remaining;
      try { remaining = this.pRecvd.readU32(); } catch (e) { return; }
      // spread the received byte count across the buffer list
      let i = 0;
      while (remaining > 0) {
        const wb = this.bufs.add(i * 8);
        const cap = wb.readU32();
        const take = Math.min(cap, remaining);
        dump('recv', this.sock, wb.add(4).readPointer(), take);
        remaining -= take; i++;
        if (i > 16) break;
      }
    }
  });
}

send({ tag: 'ready', msg: 'ws2_32 hooks installed — drive an in-game action now' });
