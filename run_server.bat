@echo off
REM Spectra Vision - Python HTTP Server Launcher
REM Simplified alternative to C++ build

echo ============================================================
echo Spectra Vision - Python Server
echo ============================================================

REM Check if Python is installed
python --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Python not found. Please install Python 3.8+
    exit /b 1
)

echo [OK] Python found
echo.
echo [INFO] Installing Flask...
pip install flask -q
if errorlevel 1 (
    echo [ERROR] Failed to install Flask
    exit /b 1
)

echo [OK] Flask installed
echo.
echo [INFO] Starting Spectra Vision server...
echo.
python server.py
