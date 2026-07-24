@echo off
REM ═══════════════════════════════════════════════════════════════════════════
REM Spectra Vision Run Script (Windows)
REM ═══════════════════════════════════════════════════════════════════════════
REM
REM Purpose:  Start the Spectra Vision backend server
REM Usage:    run.bat [args...]
REM
REM Features:
REM   - Automatic executable detection
REM   - Verifies binary exists before running
REM   - Forwards all command-line arguments to the binary
REM   - Friendly error messages if build is missing
REM   - Suggests build command if binary not found
REM
REM ═══════════════════════════════════════════════════════════════════════════

setlocal enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"
REM Try Ninja Multi-Config output first
set "BINARY=%PROJECT_ROOT%build\debug\Debug\SpectraVision.exe"

REM Fallback to Visual Studio generator output
if not exist "%BINARY%" (
    if exist "%PROJECT_ROOT%build\debug\Debug\SpectraVision.exe" (
        set "BINARY=%PROJECT_ROOT%build\debug\Debug\SpectraVision.exe"
    )
)

REM Fallback to old bin location
if not exist "%BINARY%" (
    if exist "%PROJECT_ROOT%build\debug\bin\SpectraVision.exe" (
        set "BINARY=%PROJECT_ROOT%build\debug\bin\SpectraVision.exe"
    )
)

echo.
echo ╔═════════════════════════════════════════════════════════════╗
echo ║  Spectra Vision Backend Server                             ║
echo ╚═════════════════════════════════════════════════════════════╝
echo.

REM Check if the binary exists
if not exist "%BINARY%" (
    color 0C
    echo [ERROR] Executable not found: %BINARY%
    color 07
    echo.
    echo Build the project first with:
    echo   build.bat
    echo or:
    echo   dev.bat
    echo.
    exit /b 1
)

REM Display server information
echo [INFO] Starting Spectra Vision...
echo [INFO] Listening on http://localhost:8080
echo [INFO] Press Ctrl+C to stop
echo.

REM Run the binary with all forwarded arguments
"%BINARY%" %*

REM Capture exit code
set "EXIT_CODE=%ERRORLEVEL%"

if not "%EXIT_CODE%"=="0" (
    echo.
    color 0C
    echo [ERROR] Server exited with code: %EXIT_CODE%
    color 07
    echo.
    exit /b %EXIT_CODE%
)

exit /b 0
