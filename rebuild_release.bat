@echo off
setlocal

:: ─────────────────────────────────────────────────────────────────────────────
::  BF Server — Release rebuild (standalone, portable, x64)
::
::  Builds the debug-win64 preset in Release config and stages a portable
::  drag-and-drop server folder at:
::
::      out/build/release-win64/
::        gimuserverw.exe
::        *.dll                   (resolved by vcpkg applocal.ps1)
::        config.json             (from deploy/)
::        system/                 (from deploy/system/, master data)
::
::  Drop game_content/ in alongside before distributing (game-owned static
::  assets, not bundled here).  gme.sqlite is auto-created by MigrationManager
::  on first run.  No .pdb is shipped.
::
::  NOTE: this is the STANDALONE release, NOT the release-win32 APPX build.
::  The release-win32 preset is for embedding the server into the BF game's
::  APPX package; that's a different deliverable.  If you need that, build it
::  manually per BUILD.md §"Release / APPX deployment build".
:: ─────────────────────────────────────────────────────────────────────────────

set "SERVER_DIR=%~dp0"
set "VCPKG_ROOT=C:\Users\Evan\BF\vcpkg"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "BUILD_DIR=%SERVER_DIR%out\build\debug-win64"
set "DIST_DIR=%SERVER_DIR%out\build\release-win64"

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
echo  BF Server Release Rebuild  (standalone / portable / x64)
echo  VS:        %VS_PATH%
echo  vcpkg:     %VCPKG_ROOT%
echo  Build dir: %BUILD_DIR%
echo  Dist dir:  %DIST_DIR%
echo ============================================================
echo.

:: ── Configure choice ──────────────────────────────────────────────────────────
:: Auto-suggest reconfigure if the build directory doesn't exist yet.
set RECONFIGURE=N
if not exist "%BUILD_DIR%\build.ninja" set RECONFIGURE=Y

set /p RECONFIGURE="Re-run CMake configure? (y/N, default %RECONFIGURE%): "
if /i "%RECONFIGURE%"=="y" goto :configure
if /i "%RECONFIGURE%"=="Y" goto :configure
goto :build_only

:configure
:: Wipe a half-broken cache from a previous failed configure.  A
:: CMakeCache.txt without build.ninja means a downstream error (usually
:: vcpkg) aborted before the toolchain probe completed.
if exist "%BUILD_DIR%\CMakeCache.txt" if not exist "%BUILD_DIR%\build.ninja" (
    echo Detected partial / failed previous configure - wiping %BUILD_DIR%
    rmdir /s /q "%BUILD_DIR%"
)

echo.
echo [1/3] Configuring (debug-win64 preset, x64 target)...
echo NOTE: First build will compile the Rust packet-generator (~2-5 min).
echo.
cmd /c ""%VSDEVCMD%" -arch=amd64 -host_arch=amd64 && cd /d "%SERVER_DIR%" && cmake --preset debug-win64"
if errorlevel 1 (
    echo.
    echo ERROR: CMake configure failed.
    pause & exit /b 1
)
echo.

:build_only
echo [2/3] Building Release...
cmd /c ""%VSDEVCMD%" -arch=amd64 -host_arch=amd64 && cd /d "%SERVER_DIR%" && cmake --build --preset debug-win64-release"
if errorlevel 1 (
    echo.
    echo BUILD FAILED. Check the output above for errors.
    pause & exit /b 1
)
echo.

echo [3/3] Staging portable release at %DIST_DIR% ...
:: Wipe any previous dist so deleted/renamed system files don't linger.
if exist "%DIST_DIR%" rmdir /s /q "%DIST_DIR%"
cmd /c ""%VSDEVCMD%" -arch=amd64 -host_arch=amd64 && cd /d "%SERVER_DIR%" && cmake --install out\build\debug-win64 --config Release --prefix "%DIST_DIR%""
if errorlevel 1 (
    echo.
    echo INSTALL FAILED. Check the output above for errors.
    pause & exit /b 1
)

echo.
echo ============================================================
echo  Release succeeded.
echo  Portable server bundle: %DIST_DIR%
echo.
echo  Before distributing, drop deploy\game_content\ into the bundle
echo  if your release needs the master .dat files. gme.sqlite is
echo  auto-created by MigrationManager on first run.
echo ============================================================
pause
endlocal
