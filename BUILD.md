# Building the Decompfrontier Server (Windows)

This repo uses CMake Presets + vcpkg. On Windows the only generator is **Ninja Multi-Config** — there is no Visual Studio solution preset. If you prefer the VS IDE, open the folder with `File > Open Folder` and VS will pick up the presets automatically.

## Prerequisites

1. **Visual Studio 2022 or 2026** with the "Desktop development with C++" workload.
   The toolset version doesn't matter — Ninja picks up whatever `cl.exe` is on `PATH`.
2. **CMake** 3.21 or newer (ships with VS).
3. **Ninja** (ships with VS; or `choco install ninja`).
4. **vcpkg**. Set `VCPKG_ROOT` in your environment to the vcpkg checkout.
5. **Rust toolchain** (`cargo` on `PATH`). Install from <https://rustup.rs/>. The build compiles the Rust-based `packet-generator` CLI from source on the first build.

## One-time setup

From any shell:

```cmd
git clone https://github.com/microsoft/vcpkg %USERPROFILE%\vcpkg
%USERPROFILE%\vcpkg\bootstrap-vcpkg.bat
setx VCPKG_ROOT %USERPROFILE%\vcpkg
```

Restart your shell so `VCPKG_ROOT` is visible.

## Configuring and building

Every command below must run from an **x64 Native Tools Command Prompt** — specifically one where `VsDevCmd.bat -arch=amd64 -host_arch=amd64` has been sourced. This ensures the **x64-hosted** `cl.exe` (`Hostx64\x64\cl.exe`) is on PATH.

> **Common mistake:** The regular "Developer PowerShell for VS 2026" and the default "Developer Command Prompt" both put the **x86-hosted** `cl.exe` on PATH. CMake then compiles x86 objects but tries to link them as x64, giving `LNK1112: module machine type 'x86' conflicts with target machine type 'x64'`. Always use one of the options below.

**Option A — Start menu shortcut (easiest):**
Launch `x64 Native Tools Command Prompt for VS 2026` from the Start menu.

**Option B — From any shell:**
```cmd
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
```

Then build:

```cmd
cd C:\path\to\BF-WorkingDirRust

:: Configure (first time, or after editing CMake files)
cmake --preset debug-win64

:: Build Debug
cmake --build --preset debug-win64-debug

:: Build Release
cmake --build --preset debug-win64-release
```

`rebuild.bat` handles all of this automatically — it calls `VsDevCmd.bat -arch=amd64 -host_arch=amd64` internally, so you can run it from any prompt.

Artifacts land under `out/build/debug-win64/`.

### Portable standalone release (`rebuild_release.bat`)

Builds the `debug-win64` preset in Release config and stages a portable,
drag-and-drop server folder at `out/build/release-win64/`:

```
out/build/release-win64/
  gimuserverw.exe
  *.dll                  (drogon, trantor, sqlite3, openssl, brotli, zlib, …)
  config.json            (copied from deploy/)
  system/                (master data JSONs, copied from deploy/system/)
```

`game_content/` is intentionally **not** bundled — those are game-owned
static assets the operator drops in alongside the bundle before
distributing. `gme.sqlite` is auto-created by `MigrationManager` on
first run. No `.pdb` is shipped.

```cmd
cd C:\path\to\BF-WorkingDirRust
rebuild_release.bat
```

The script sources `VsDevCmd.bat -arch=amd64 -host_arch=amd64` itself,
so it works from any prompt. It auto-detects whether to reconfigure
(first build, or after editing CMake/KDL files) and wipes the dist dir
between runs so deleted/renamed system files don't linger.

**Manual recipe** (if you'd rather call CMake directly):

```cmd
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
set VCPKG_ROOT=C:\Users\Evan\BF\vcpkg

cd C:\path\to\BF-WorkingDirRust
cmake --preset debug-win64
cmake --build --preset debug-win64-release
cmake --install out\build\debug-win64 --config Release --prefix out\build\release-win64
```

### APPX deployment build (`release-win32`)

The `release-win32` preset produces the **PROXYAPPX** server — a 32-bit
static library that's embedded into the BF game's APPX package, loaded
by the game process at startup. There is no separate executable.

This is a **different deliverable** from the portable standalone release
above. `rebuild_release.bat` does NOT build it.

**Manual recipe**:

```cmd
:: x86 cross-compile env (x64 host, x86 target — uses Hostx64\x86\cl.exe).
:: NOTE: -arch=x86, NOT -arch=amd64 like the debug build.
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64

set VCPKG_ROOT=C:\Users\Evan\BF\vcpkg

cd C:\path\to\BF-WorkingDirRust
cmake --preset release-win32
cmake --build --preset release-win32-release
```

**Important gotchas specific to `release-win32`** (also documented in
handbook §8.X if landed):

1. **`drogon[ctl]` must NOT be in `vcpkg.json`.** The upstream drogon
   port marks the `ctl` feature (the `drogon_ctl` CLI scaffolding tool)
   as `supports: "native"` — vcpkg refuses to install it on the
   `x86-windows-static` triplet. Our `vcpkg.json` lists only
   `sqlite3` and `orm` for drogon. If you see
   `drogon[ctl] is only supported on 'native'` in
   `out/build/release-win32/vcpkg-manifest-install.log`, someone has
   re-added the feature.

2. **cl.exe arch must match the target.** Using `-arch=amd64`
   (the debug script's flag) builds x64 objects that fail to link
   against the x86-windows-static vcpkg deps. Always use `-arch=x86
   -host_arch=amd64` for release-win32.

3. **A failed configure leaves a half-broken `CMakeCache.txt` behind**
   that downstream cmake invocations interpret as "no compiler found".
   If you see `CMAKE_CXX_COMPILER not set, after EnableLanguage` and
   `CMake was unable to find a build program corresponding to "Ninja
   Multi-Config"`, the real failure happened earlier — usually vcpkg.
   Always read `out/build/release-win32/vcpkg-manifest-install.log`
   first; the toolchain errors are almost always downstream symptoms.
   `rebuild_release.bat` auto-wipes a partial cache before reconfiguring.

### Linux

```bash
cmake --preset debug-lnx64
cmake --build --preset debug-lnx64
```

## Presets at a glance

| Preset | Generator | Triplet | Frontend |
|---|---|---|---|
| `debug-win64` | Ninja Multi-Config | `x64-windows` | `STANDALONE` |
| `release-win32` | Ninja Multi-Config | `x86-windows-static` | `PROXYAPPX` |
| `debug-lnx64` | Ninja | `x64-linux` | `STANDALONE` |

Both Windows presets produce Debug **and** Release binaries from one build tree — pick the configuration at build time via `--config Debug` / `--config Release`, or use the paired build presets (`debug-win64-debug`, `debug-win64-release`, etc.).

## The packet-generator step

The Rust CLI at `packet-generator/` compiles the KDL schemas in `packet-generator/assets/` into a single C++ header at `gimuserver/packets/all.hpp`. CMake runs this automatically via the `pkgen_generate` custom target whenever a `.kdl` file changes.

- **First build is slow** (2–5 min) because Cargo compiles the generator itself.
- **Subsequent builds re-run the generator only when a `.kdl` file is newer than `gimuserver/packets/all.hpp`.**
- **Generated file is gitignored** — `gimuserver/packets/.gitignore` excludes `*.hpp`.

To invoke the generator by hand (rarely needed):

```cmd
cd packet-generator
cargo run --release -- generate --cxx --glaze -i assets/all.kdl -o ../gimuserver/packets
```

## Troubleshooting

**`MSB8020: build tools for Visual Studio 2022 (Platform Toolset = 'v143') cannot be found`**
You're on an old preset that pinned the VS 17 2022 generator. Pull the latest presets — they all use Ninja Multi-Config now, which is toolset-agnostic.

**`LNK1112: module machine type 'x86' conflicts with target machine type 'x64'`**
You ran CMake from a shell that has the **x86-hosted** `cl.exe` on PATH (e.g. regular Developer PowerShell or the default Developer Command Prompt). Delete `out/`, then re-run from the **x64 Native Tools Command Prompt for VS 2026** or via `rebuild.bat`.

**`cl : command line error D8021 : invalid numeric argument`** or linker complains it can't find `kernel32.lib`
You're not in a Developer Command Prompt at all. `cl.exe` needs the VC environment set up first.

**`cargo: command not found`**
Rust isn't on `PATH`. Install via <https://rustup.rs/> and restart the shell.

**First build hangs at `Regenerating C++ packet headers from KDL schemas`**
Cargo is compiling `packet-generator` from source. Wait it out; subsequent builds are fast.

**Generated types don't exist after editing a KDL file**
Build again — the custom target re-runs on any `.kdl` change. If it still doesn't regenerate, delete `gimuserver/packets/all.hpp` and build; that forces a rerun.

**`release-win32` configure fails with `drogon[ctl] is only supported on 'native'`**
The upstream drogon port refuses to build the `ctl` feature on cross-compile triplets. Drop `"ctl"` from the drogon features list in `vcpkg.json`. The `drogon_ctl` binary is a project-scaffolding helper not used by the offline server.

**`release-win32` configure fails with `Ninja Multi-Config not found` / `CMAKE_CXX_COMPILER not set`**
These are almost always downstream symptoms of a vcpkg dependency-install failure (the configure aborted before compiler probing). Read `out/build/release-win32/vcpkg-manifest-install.log` for the real cause. If you're running from a fresh shell, also confirm you sourced `VsDevCmd.bat -arch=x86 -host_arch=amd64` — not the debug build's `-arch=amd64`. `rebuild_release.bat` handles both.

**`release-win32` build fails in `game_frontend/bootstrap_windows.cpp` with `error C2059: syntax error: '__declspec(dllexport)'`**
MSVC requires `__declspec(dllexport)` BEFORE the return type, not between `__stdcall` and the function name. The `__APPX__`-gated block in this file had never been compiled before because the debug-win64 preset uses `STANDALONE` (no `__APPX__` define) — the bug only surfaces on the first real release build. Fix: rewrite the declaration as `extern "C" __declspec(dllexport) void __stdcall <name>(args)`.
