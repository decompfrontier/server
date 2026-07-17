// agent_curl.js — Stage B: raw request/response capture at the libcurl boundary.
//
// Target: BraveFrontier.Windows.exe (32-bit) using the offline mod's libcurl.dll.
// Requires NO reversed offsets — it hooks libcurl's *named exports*, so it runs
// on day one. Bodies are still AES-encrypted at the app layer (TLS is off), so
// pair the output with the server's deploy/log/http_log_<GroupId>_*.log (which is
// decrypted server-side) or decrypt offline with the per-GroupId key.
//
// Run standalone:   frida -n BraveFrontier.Windows.exe -l agent_curl.js   (as Admin)
// Or via driver:    python capture.py --agent curl --flow summon
//
// If Stage A (frida-trace curl_*) showed NO curl traffic, the API path is
// WinINet/WinHTTP instead — see agent_curl_wininet stub at the bottom.

'use strict';

// ---- libcurl option constants we care about --------------------------------
const CURLOPT_URL            = 10002;
const CURLOPT_POSTFIELDS     = 10015;
const CURLOPT_COPYPOSTFIELDS = 10165;
const CURLOPT_POSTFIELDSIZE  = 60;
const CURLOPT_WRITEFUNCTION  = 20011;
const CURLOPT_WRITEDATA      = 10001; // a.k.a. CURLOPT_FILE

// ---- resolve an export defensively (Frida 17 instance API) -----------------
function resolveCurl(name) {
  // 1. the expected drop-in name
  const curl = Process.findModuleByName('libcurl.dll');
  if (curl) { const p = curl.findExportByName(name); if (p) return p; }
  // 2. any loaded module whose name looks like libcurl
  for (const m of Process.enumerateModules()) {
    if (/curl/i.test(m.name)) {
      const p = m.findExportByName(name);
      if (p) return p;
    }
  }
  // 3. last resort: global export search
  return Module.findGlobalExportByName ? Module.findGlobalExportByName(name) : null;
}

// Per-handle scratch so we can join URL + body + response for one transfer.
const xfer = {};                 // curl handle (string) -> { url, body, resp:[] }
const writeCbSeen = {};          // write-callback addr (string) -> attached bool

function slot(handle) {
  const k = handle.toString();
  if (!xfer[k]) xfer[k] = { url: null, body: null, resp: [] };
  return xfer[k];
}

const setopt = resolveCurl('curl_easy_setopt');
const perform = resolveCurl('curl_easy_perform');

if (!setopt) {
  send({ tag: 'error', msg: 'curl_easy_setopt not found — API path is probably WinINet/WinHTTP, not libcurl. See agent_curl.js footer.' });
} else {
  send({ tag: 'ready', where: setopt.toString(), note: 'hooking curl_easy_setopt' });

  // curl_easy_setopt(handle, option, value)  — x86 cdecl varargs:
  //   args[0]=handle, args[1]=option, args[2]=value
  Interceptor.attach(setopt, {
    onEnter(args) {
      const handle = args[0];
      const opt = args[1].toInt32();
      const s = slot(handle);
      try {
        if (opt === CURLOPT_URL) {
          s.url = args[2].readUtf8String();
        } else if (opt === CURLOPT_POSTFIELDS || opt === CURLOPT_COPYPOSTFIELDS) {
          // may be non-terminated; POSTFIELDSIZE (if set) gives the length.
          s.bodyPtr = args[2];
        } else if (opt === CURLOPT_POSTFIELDSIZE) {
          s.bodyLen = args[2].toInt32();
        } else if (opt === CURLOPT_WRITEFUNCTION) {
          hookWriteCb(args[2]);
        } else if (opt === CURLOPT_WRITEDATA) {
          s.writeData = args[2].toString();
        }
      } catch (e) { /* ignore unreadable slots */ }
    }
  });
}

// Attach to the app's write callback once; it fires per response chunk:
//   size_t cb(char *ptr, size_t size, size_t nmemb, void *userdata)
function hookWriteCb(cbPtr) {
  if (cbPtr.isNull()) return;
  const k = cbPtr.toString();
  if (writeCbSeen[k]) return;
  writeCbSeen[k] = true;
  try {
    Interceptor.attach(cbPtr, {
      onEnter(a) {
        const total = a[1].toInt32() * a[2].toInt32();
        const userdata = a[3].toString();
        // find the transfer whose WRITEDATA matches this userdata
        for (const key in xfer) {
          if (xfer[key].writeData === userdata) {
            try { xfer[key].resp.push(a[0].readByteArray(total)); } catch (e) {}
            break;
          }
        }
      }
    });
    send({ tag: 'info', msg: 'attached write callback @ ' + k });
  } catch (e) {
    send({ tag: 'error', msg: 'write-cb attach failed: ' + e });
  }
}

if (perform) {
  // Flush one full transfer when curl_easy_perform returns.
  Interceptor.attach(perform, {
    onEnter(args) { this.handle = args[0]; },
    onLeave(retval) {
      const k = this.handle.toString();
      const s = xfer[k];
      if (!s) return;
      let body = null;
      if (s.bodyPtr) {
        try {
          body = (s.bodyLen != null)
            ? hexOf(s.bodyPtr.readByteArray(s.bodyLen))
            : s.bodyPtr.readUtf8String();
        } catch (e) { body = '<unreadable>'; }
      }
      let resp = null;
      if (s.resp.length) resp = s.resp.map(hexOf).join('');
      send({ tag: 'transfer', url: s.url, code: retval.toInt32(),
             reqBodyLen: s.bodyLen, reqBody: body, respHex: resp });
      delete xfer[k];
    }
  });
}

function hexOf(buf) {
  if (!buf) return null;
  const b = new Uint8Array(buf);
  let out = '';
  for (let i = 0; i < b.length; i++) out += b[i].toString(16).padStart(2, '0');
  return out;
}

// ---------------------------------------------------------------------------
// WinINet fallback (only if Stage A showed Internet* traffic instead of curl):
//   Hook wininet.dll!HttpSendRequestA/W (request) and InternetReadFile (response).
//   Left as a note rather than active code so this agent stays single-purpose.
// ---------------------------------------------------------------------------
