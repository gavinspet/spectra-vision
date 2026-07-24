@echo off
REM build.bat - Production build script for Spectra Vision (Windows)
REM
REM Usage:
REM   build.bat                  REM Debug build
REM   build.bat release          REM Release build
REM   build.bat release-lto      REM Release with LTO
REM

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%.."
set "BUILD_TYPE=%1"

if "%BUILD_TYPE%"=="" set "BUILD_TYPE=debug"

REM Validate preset
if "%BUILD_TYPE%"=="debug" goto valid_type
if "%BUILD_TYPE%"=="release" goto valid_type
if "%BUILD_TYPE%"=="release-lto" goto valid_type
if "%BUILD_TYPE%"=="default" goto valid_type
echo [error] Unknown build type: %BUILD_TYPE%
echo Usage: %0 {debug^|release^|release-lto^|default}
exit /b 1

:valid_type
echo [spectra-vision] Building with preset: %BUILD_TYPE%

cd /d "%PROJECT_DIR%" || exit /b 1

REM Configure
echo [spectra-vision] Configuring...
cmake --preset %BUILD_TYPE% -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo [error] CMake configuration failed
    exit /b 1
)

REM Build
echo [spectra-vision] Building...
cmake --build build\%BUILD_TYPE% --config Release --parallel
if errorlevel 1 (
    echo [error] Build failed
    exit /b 1
)

echo [spectra-vision] Build complete!
echo [spectra-vision] Executable: %PROJECT_DIR%\build\%BUILD_TYPE%\bin\Release\SpectraVision.exe
echo.
echo Next steps:
echo   1. Run tests: ctest --test-dir build\%BUILD_TYPE% --verbose -C Release
echo   2. Install: cmake --install build\%BUILD_TYPE% --prefix "C:\Program Files\SpectraVision"
echo   3. Run: build\%BUILD_TYPE%\bin\Release\SpectraVision.exe config.json

endlocal
