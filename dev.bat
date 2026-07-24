@echo off
REM ═══════════════════════════════════════════════════════════════════════════
REM Spectra Vision Dev Script (Windows)
REM ═══════════════════════════════════════════════════════════════════════════
REM
REM Purpose:  Complete development workflow: build → run
REM Usage:    dev.bat
REM
REM Workflow:
REM   1. Build the project (configure CMake if needed)
REM   2. If build succeeds, start the server
REM   3. If build fails, stop immediately with error
REM
REM Features:
REM   - One-command build and run
REM   - Stops on build failure
REM   - Displays friendly status messages
REM   - Perfect for development cycle
REM
REM ═══════════════════════════════════════════════════════════════════════════

setlocal enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"

echo.
echo ╔═════════════════════════════════════════════════════════════╗
echo ║  Spectra Vision Development Workflow                       ║
echo ║  Mode: Build + Run                                          ║
echo ╚═════════════════════════════════════════════════════════════╝
echo.

REM Step 1: Build
echo [STEP 1/2] Building project...
call "%PROJECT_ROOT%build.bat"

if errorlevel 1 (
    color 0C
    echo.
    echo [FATAL] Build failed, not starting server
    color 07
    echo.
    exit /b 1
)

REM Step 2: Run
echo.
echo [STEP 2/2] Starting server...
call "%PROJECT_ROOT%run.bat"

exit /b %ERRORLEVEL%
