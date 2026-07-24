@echo off
REM Spectra Vision - Windows C++ Build Script
REM Requirements: CMake, Ninja, LLVM/Clang or MSVC

cls
echo ============================================================
echo Spectra Vision - C++ Backend Build
echo ============================================================
echo.

REM Check for CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found. Install from cmake.org or: winget install cmake
    exit /b 1
)

REM Check for Ninja
ninja --version >nul 2>&1
if errorlevel 1 (
    echo [WARNING] Ninja not found. Install from: winget install ninja
    echo Falling back to Visual Studio generator...
    set GENERATOR=Visual Studio 17 2022
) else (
    set GENERATOR=Ninja
)

REM Create and enter build directory
if exist backend\build (
    echo [INFO] Removing old build directory...
    rmdir /s /q backend\build
)

echo [INFO] Creating build directory...
mkdir backend\build
cd /d backend\build

echo.
echo [INFO] Configuring CMake with generator: %GENERATOR%
echo.

REM Try to find C++ compiler
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\bin\Hostx64\x64\cl.exe" (
    echo [INFO] Found MSVC compiler
    set CXX=cl.exe
    set CC=cl.exe
) else if exist "C:\Program Files\LLVM\bin\clang-cl.exe" (
    echo [INFO] Found Clang compiler
    set CXX=clang-cl.exe
    set CC=clang-cl.exe
) else (
    echo [WARNING] No compiler found, using default
)

REM Configure with CMake
if "%GENERATOR%"=="Ninja" (
    cmake -G "%GENERATOR%" -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_COMPILER=cl.exe ..
) else (
    cmake -G "%GENERATOR%" -DCMAKE_CXX_STANDARD=17 ..
)

if errorlevel 1 (
    echo [ERROR] CMake configuration failed
    echo [INFO] Try installing: winget install LLVM or Visual Studio Community
    exit /b 1
)

echo.
echo [INFO] Building project...
echo.

REM Build
if "%GENERATOR%"=="Ninja" (
    cmake --build . --parallel 4
) else (
    cmake --build . --config Release --parallel 4
)

if errorlevel 1 (
    echo [ERROR] Build failed
    exit /b 1
)

echo.
echo ============================================================
echo [OK] Build successful!
echo ============================================================
echo.

REM Find binary
if exist bin\SpectraVision.exe (
    set BINARY=bin\SpectraVision.exe
) else if exist Release\SpectraVision.exe (
    set BINARY=Release\SpectraVision.exe
) else if exist SpectraVision.exe (
    set BINARY=SpectraVision.exe
) else (
    echo [ERROR] Binary not found after build
    exit /b 1
)

echo [OK] Binary: %BINARY%
echo.
echo To run the server:
echo   cd backend\build
echo   %BINARY%
echo.
exit /b 0
