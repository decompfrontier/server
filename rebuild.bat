@echo off
setlocal

set "SERVER_DIR=%~dp0"
set "VCPKG_ROOT=C:\Users\Evan\BF\vcpkg"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "BUILD_DIR=%SERVER_DIR%out\build\debug-win64"

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
echo  BF Server Rebuild  (Ninja Multi-Config)
echo  VS:       %VS_PATH%
echo  vcpkg:    %VCPKG_ROOT%
echo  Build dir: %BUILD_DIR%
echo ============================================================
echo.

:: ── Config choice ─────────────────────────────────────────────────────────────
set CONFIG=Debug
set /p CONFIG="Build configuration? (Debug/Release, default Debug): "
if /i "%CONFIG%"=="" set CONFIG=Debug
if /i "%CONFIG%"=="d" set CONFIG=Debug
if /i "%CONFIG%"=="r" set CONFIG=Release

:: Map to the build preset name
if /i "%CONFIG%"=="Debug"   set BUILD_PRESET=debug-win64-debug
if /i "%CONFIG%"=="Release" set BUILD_PRESET=debug-win64-release

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
echo.
echo [1/2] Configuring (debug-win64 preset)...
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
echo [Building %CONFIG%]...
cmd /c ""%VSDEVCMD%" -arch=amd64 -host_arch=amd64 && cd /d "%SERVER_DIR%" && cmake --build --preset %BUILD_PRESET%"

:done
if errorlevel 1 (
    echo.
    echo BUILD FAILED. Check the output above for errors.
) else (
    echo.
    echo Build succeeded.  Artifacts: %BUILD_DIR%\%CONFIG%\
)
pause
endlocal
