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
