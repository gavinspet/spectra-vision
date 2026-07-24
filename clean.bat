@echo off
REM ═══════════════════════════════════════════════════════════════════════════
REM Spectra Vision Clean Script (Windows)
REM ═══════════════════════════════════════════════════════════════════════════
REM
REM Purpose:  Remove all build artifacts and CMake cache
REM Usage:    clean.bat
REM
REM Features:
REM   - Removes build directory
REM   - Clears CMake cache
REM   - Removes generated binaries
REM   - Does NOT delete source code
REM   - Safe to run multiple times
REM   - Asks for confirmation before deletion
REM
REM ═══════════════════════════════════════════════════════════════════════════

setlocal enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"
set "BUILD_DIR=%PROJECT_ROOT%build"

echo.
echo ╔═════════════════════════════════════════════════════════════╗
echo ║  Spectra Vision Clean Script                               ║
echo ╚═════════════════════════════════════════════════════════════╝
echo.

REM Check if build directory exists
if not exist "%BUILD_DIR%" (
    echo [INFO] Build directory does not exist
    echo [INFO] Nothing to clean
    echo.
    exit /b 0
)

REM Display what will be deleted
echo [INFO] The following will be deleted:
echo   - %BUILD_DIR%\
echo.

REM Confirm deletion
set /p "CONFIRM=Continue? (y/n): "
if /i not "%CONFIRM%"=="y" (
    echo [INFO] Clean operation cancelled
    echo.
    exit /b 0
)

echo.
echo [INFO] Removing build artifacts...

REM Delete build directory
rmdir /s /q "%BUILD_DIR%" >nul 2>&1
if errorlevel 1 (
    color 0C
    echo [ERROR] Failed to remove build directory
    color 07
    echo.
    exit /b 1
)

echo [OK] Build artifacts removed

REM Delete CMake user presets (optional, development artifacts)
set "CMAKE_PRESETS_USER=%PROJECT_ROOT%CMakeUserPresets.json"
if exist "%CMAKE_PRESETS_USER%" (
    del /q "%CMAKE_PRESETS_USER%" >nul 2>&1
    echo [OK] Removed CMake user presets
)

color 0A
echo.
echo ╔═════════════════════════════════════════════════════════════╗
echo ║  CLEAN COMPLETE                                             ║
echo ║  Build directory removed successfully                       ║
echo ║  Rebuild with: build.bat                                    ║
echo ╚═════════════════════════════════════════════════════════════╝
echo.
color 07

exit /b 0
