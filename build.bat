@echo off
setlocal enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"
if "%PROJECT_ROOT:~-1%"=="\" set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"
set "BUILD_DIR=%PROJECT_ROOT%\build\debug"
set "CLANG_CL_PATH=C:\Program Files\LLVM\bin\clang-cl.exe"

REM Initialize MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

cmake --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found
    exit /b 1
)

ninja --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Ninja not found
    exit /b 1
)

if not exist "%CLANG_CL_PATH%" (
    echo [ERROR] Clang not found
    exit /b 1
)

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

if exist "%BUILD_DIR%\CMakeCache.txt" (
    del /f "%BUILD_DIR%\CMakeCache.txt" 2>nul
)

cd /d "%BUILD_DIR%"

echo [INFO] Configuring with Ninja and Clang-CL...
cmake -G "Ninja" -DCMAKE_CXX_STANDARD=17 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_C_COMPILER="%CLANG_CL_PATH%" -DCMAKE_CXX_COMPILER="%CLANG_CL_PATH%" "-DCMAKE_CXX_FLAGS=/I\"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\include\" /I\"C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\ucrt\"" "-DCMAKE_C_FLAGS=/I\"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\include\" /I\"C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\ucrt\"" -DCMAKE_RC_COMPILER="rc.exe" -DCMAKE_RC_COMPILE_OBJECT="" -DCMAKE_C_COMPILER_WORKS=TRUE -DCMAKE_CXX_COMPILER_WORKS=TRUE -DSV_BUILD_TESTS=OFF "%PROJECT_ROOT%"
if errorlevel 1 (
    echo [ERROR] Configuration failed
    exit /b 1
)

echo [INFO] Building...
cmake --build . --parallel 4
if errorlevel 1 (
    echo [ERROR] Build failed
    exit /b 1
)

echo [OK] Build successful
exit /b 0
