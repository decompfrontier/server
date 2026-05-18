@echo off
setlocal

:: ─────────────────────────────────────────────────────────────────────────────
::  BF Server — Release rebuild (APPX-embedded, 32-bit, PROXYAPPX frontend)
::
::  Parallel to rebuild.bat (debug) but targets the release-win32 preset:
::    - VCPKG_TARGET_TRIPLET = x86-windows-static   (deps as 32-bit)
::    - GIMUSRV_FRONTEND      = PROXYAPPX            (statically embeds the
::                                                    server into the BF game's
::                                                    APPX package)
::    - VsDevCmd flags        = -arch=x86 -host_arch=amd64
::                              (x64-hosted cl.exe targeting x86 — uses
::                              Hostx64\x86\cl.exe.  The default rebuild.bat
::                              uses -arch=amd64 which is wrong for x86 output.)
::
::  Requires drogon WITHOUT the `ctl` feature in vcpkg.json — the upstream
::  drogon[ctl] port is marked `supports: "native"` and refuses to install on
::  the x86-windows-static (cross-compile) triplet.  This script does NOT edit
::  vcpkg.json; if the configure step trips on `drogon[ctl]`, see vcpkg.json
::  and drop "ctl" from the drogon features list.
:: ─────────────────────────────────────────────────────────────────────────────

set "SERVER_DIR=%~dp0"
set "VCPKG_ROOT=C:\Users\Evan\BF\vcpkg"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "BUILD_DIR=%SERVER_DIR%out\build\release-win32"

:: ── Locate VsDevCmd.bat via vswhere (VS 2017-2026+) ──────────────────────────
if not exist "%VSWHERE%" (
    echo ERROR: vswhere.exe not found. Visual Studio does not appear to be installed.
    pause & exit /b 1
)

for /f "usebackq tokens=*" %%i in (
    `"%VSWHERE%" -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`
) do set "VS_PATH=%%i"

if not defined VS_PATH (
    echo ERROR: No Visual Studio installation with C++ tools found.
    echo Install the "Desktop development with C++" workload from the VS Installer.
    pause & exit /b 1
)

set "VSDEVCMD=%VS_PATH%\Common7\Tools\VsDevCmd.bat"
if not exist "%VSDEVCMD%" (
    echo ERROR: VsDevCmd.bat not found at:
    echo   %VSDEVCMD%
    pause & exit /b 1
)

:: ── Check vcpkg ───────────────────────────────────────────────────────────────
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo ERROR: vcpkg not found at %VCPKG_ROOT%
    echo Run the BF installer first to set up vcpkg.
    pause & exit /b 1
)

echo ============================================================
echo  BF Server Rebuild  (Release / APPX / x86 / Ninja Multi-Config)
echo  VS:       %VS_PATH%
echo  vcpkg:    %VCPKG_ROOT%
echo  Build dir: %BUILD_DIR%
echo ============================================================
echo.

:: ── Config choice ─────────────────────────────────────────────────────────────
set CONFIG=Release
set /p CONFIG="Build configuration? (Debug/Release, default Release): "
if /i "%CONFIG%"=="" set CONFIG=Release
if /i "%CONFIG%"=="d" set CONFIG=Debug
if /i "%CONFIG%"=="r" set CONFIG=Release

:: Map to the build preset name
if /i "%CONFIG%"=="Debug"   set BUILD_PRESET=release-win32-debug
if /i "%CONFIG%"=="Release" set BUILD_PRESET=release-win32-release

if not defined BUILD_PRESET (
    echo ERROR: Unknown configuration "%CONFIG%". Use Debug or Release.
    pause & exit /b 1
)

:: ── Configure choice ──────────────────────────────────────────────────────────
:: Auto-suggest reconfigure if the build directory doesn't exist yet.
set RECONFIGURE=N
if not exist "%BUILD_DIR%\build.ninja" set RECONFIGURE=Y

set /p RECONFIGURE="Re-run CMake configure? (y/N, default %RECONFIGURE%): "
if /i "%RECONFIGURE%"=="y" goto :configure
if /i "%RECONFIGURE%"=="Y" goto :configure
goto :build_only

:configure
:: If a previous configure failed (no build.ninja but cache files present),
:: wipe the build dir.  Half-configured caches frequently cause "compiler not
:: set" errors that look like missing toolchain but are really cache corruption.
if exist "%BUILD_DIR%\CMakeCache.txt" if not exist "%BUILD_DIR%\build.ninja" (
    echo Detected partial / failed previous configure — wiping %BUILD_DIR%
    rmdir /s /q "%BUILD_DIR%"
)

echo.
echo [1/2] Configuring (release-win32 preset, x86 target via -arch=x86)...
echo NOTE: First build will compile the Rust packet-generator (~2-5 min).
echo NOTE: vcpkg may take 10-20 min on a cold cache to build x86-windows-static deps.
echo.
cmd /c ""%VSDEVCMD%" -arch=x86 -host_arch=amd64 && cd /d "%SERVER_DIR%" && cmake --preset release-win32"
if errorlevel 1 (
    echo.
    echo ERROR: CMake configure failed.
    echo.
    echo Common causes:
    echo   * drogon[ctl] in vcpkg.json — the `ctl` feature is unsupported on
    echo     x86-windows-static. Drop "ctl" from the drogon features list.
    echo   * VCPKG_ROOT not pointing at a valid vcpkg checkout.
    echo   * First-time build: check out/build/release-win32/vcpkg-manifest-install.log
    echo     for the real vcpkg failure (errors elsewhere are often downstream symptoms).
    pause & exit /b 1
)
echo.

:build_only
echo [Building %CONFIG%]...
cmd /c ""%VSDEVCMD%" -arch=x86 -host_arch=amd64 && cd /d "%SERVER_DIR%" && cmake --build --preset %BUILD_PRESET%"

:done
if errorlevel 1 (
    echo.
    echo BUILD FAILED. Check the output above for errors.
) else (
    echo.
    echo Build succeeded.  Artifacts: %BUILD_DIR%\%CONFIG%\
    echo.
    echo Drop the resulting binaries into the BF game's APPX package directory
    echo to embed the server.  PROXYAPPX-mode binaries are loaded by the game
    echo process at startup; no separate standalone executable is produced.
)
pause
endlocal
