# Spectra Vision Developer Scripts Guide

Complete guide to using the developer scripts for building, running, and testing Spectra Vision.

---

## 📋 Quick Reference

| Script | Purpose | When to Use |
|--------|---------|------------|
| `build.bat` / `build.sh` | Build Debug configuration | After code changes, first build |
| `run.bat` / `run.sh` | Start the backend server | Test running application |
| `clean.bat` / `clean.sh` | Remove all build artifacts | Clean rebuild, fix build issues |
| `dev.bat` / `dev.sh` | Build + Run in one command | Daily development workflow |

---

## 🔨 Build Script (`build.bat` / `build.sh`)

### Purpose
Configure CMake and compile the project in Debug mode with full error checking.

### Usage
```bash
# Simple build (configure if needed)
./build.bat          # Windows
./build.sh           # Linux/macOS

# With extra CMake options
./build.bat -- -DSV_ENABLE_SANITIZERS=ON
./build.sh -- -DSV_ENABLE_LTO=ON
```

### How It Works

1. **Validates Environment**
   - Checks CMake is installed
   - Shows helpful error messages if missing

2. **Manages Build Directory**
   - Creates `build/debug/` if not present
   - Detects incremental vs. full builds

3. **CMake Configuration**
   - Runs once when `CMakeCache.txt` missing
   - Uses `debug` preset from `CMakePresets.json`
   - Skipped on subsequent builds (faster)

4. **Compilation**
   - Runs `cmake --build` with parallel jobs
   - Color-codes output for easy reading
   - Exits immediately on errors

5. **Outputs**
   - Success: Binary at `build/debug/bin/SpectraVision.exe` (Windows) or `build/debug/bin/SpectraVision` (Unix)
   - Failure: Detailed error messages, non-zero exit code

### Exit Codes
- `0` — Build successful
- `1` — CMake not found, configuration failed, or compilation failed

### Example Output
```
╔═════════════════════════════════════════════════════════════╗
║  Spectra Vision Build Script                               ║
║  Configuration: Debug                                       ║
║  Platform: Windows (MSVC/Ninja)                             ║
╚═════════════════════════════════════════════════════════════╝

[INFO] Building project...
...compilation output...
[OK] CMake configuration complete

╔═════════════════════════════════════════════════════════════╗
║  BUILD SUCCESSFUL                                           ║
║  Binary: build\debug\bin\SpectraVision.exe                  ║
║  Run with: run.bat                                          ║
║  Or:       dev.bat  (build + run)                           ║
╚═════════════════════════════════════════════════════════════╝
```

---

## 🚀 Run Script (`run.bat` / `run.sh`)

### Purpose
Start the Spectra Vision backend server with safety checks.

### Usage
```bash
# Simple run
./run.bat
./run.sh

# With arguments (forwarded to server)
./run.bat --config production.json
./run.sh --log-level debug
```

### How It Works

1. **Validates Binary**
   - Checks if executable exists at expected path
   - Suggests build command if missing
   - Makes binary executable on Unix (chmod +x)

2. **Starts Server**
   - Runs the compiled binary
   - Forwards all command-line arguments
   - Displays connection information

3. **Error Handling**
   - Captures non-zero exit codes
   - Shows friendly error messages
   - Returns proper exit code to caller

### Exit Codes
- `0` — Server ran and exited cleanly
- `1` — Binary not found or server error

### Example Output
```
╔═════════════════════════════════════════════════════════════╗
║  Spectra Vision Backend Server                             ║
╚═════════════════════════════════════════════════════════════╝

[INFO] Starting Spectra Vision...
[INFO] Listening on http://localhost:8080
[INFO] Press Ctrl+C to stop

[Drogon server output...]
```

### Common Issues

| Issue | Solution |
|-------|----------|
| "Executable not found" | Run `build.bat` or `./build.sh` first |
| Server won't start | Check port 8080 is free: `netstat -an \| grep 8080` |
| Permission denied (Unix) | Run `chmod +x ./run.sh` and retry |

---

## 🧹 Clean Script (`clean.bat` / `clean.sh`)

### Purpose
Remove all build artifacts and CMake cache for a fresh rebuild.

### Usage
```bash
# Interactive (asks for confirmation)
./clean.bat
./clean.sh

# Automatic (for CI/CD, add -y flag if supported)
./clean.bat
./clean.sh
```

### How It Works

1. **Checks Build Directory**
   - Safe no-op if build folder doesn't exist
   - Can be run multiple times

2. **Prompts for Confirmation**
   - Shows what will be deleted: `build/` directory
   - Requires user confirmation to proceed
   - Easy to cancel with `n`

3. **Removes Artifacts**
   - Deletes entire `build/` tree
   - Removes `CMakeUserPresets.json` (optional)
   - Does NOT touch source code

4. **Confirmation**
   - Shows success message when complete
   - Suggests next step: rebuild with `build.bat`

### Exit Codes
- `0` — Successful clean or user cancelled
- `1` — Failed to remove directory

### Example Output
```
╔═════════════════════════════════════════════════════════════╗
║  Spectra Vision Clean Script                               ║
╚═════════════════════════════════════════════════════════════╝

[INFO] The following will be deleted:
  - C:\...\build\

Continue? (y/n): y

[INFO] Removing build artifacts...
[OK] Build artifacts removed

╔═════════════════════════════════════════════════════════════╗
║  CLEAN COMPLETE                                             ║
║  Build directory removed successfully                       ║
║  Rebuild with: build.bat                                    ║
╚═════════════════════════════════════════════════════════════╝
```

### When to Use
- **Before major merges**: Ensure clean build
- **Fixing build issues**: Wipe cache and rebuild from scratch
- **Switching branches**: Clean old build artifacts
- **Fresh start after dependency changes**: Remove stale CMake cache

---

## 🔄 Dev Script (`dev.bat` / `dev.sh`)

### Purpose
Complete development workflow: build → run in one command.

### Usage
```bash
# The typical daily command
./dev.bat
./dev.sh
```

### How It Works

1. **Calls build script**
   - Runs full build with error checking
   - Stops immediately if build fails

2. **On Build Success**
   - Calls run script to start server
   - Server runs and takes over terminal

3. **Error Handling**
   - Shows "BUILD FAILED" and exits if step 1 fails
   - Never tries to run broken binary

### Exit Codes
- `0` — Build and run successful
- `1` — Build failed

### Example Workflow
```
$ dev.bat
╔═════════════════════════════════════════════════════════════╗
║  Spectra Vision Development Workflow                       ║
║  Mode: Build + Run                                          ║
╚═════════════════════════════════════════════════════════════╝

[STEP 1/2] Building project...
[...build output...]
[OK] Build successful

[STEP 2/2] Starting server...
[INFO] Listening on http://localhost:8080
```

---

## 📅 Recommended Daily Workflow

### Morning: Fresh Start
```bash
# Clean slate, full rebuild
./clean.sh          # Remove old artifacts
./build.sh          # Build from scratch
./run.sh            # Start server
```

### Development Cycle
```bash
# Edit code → build → run
./dev.sh            # Build + run in one command
                    # Server runs and you can test
                    # Ctrl+C to stop when done
```

### After Code Changes
```bash
# Quick rebuild (incremental)
./build.sh          # Very fast (only changed files)
./run.sh            # Restart server
```

### Before Committing
```bash
# Full clean build to verify nothing broken
./clean.sh          # Wipe everything
./dev.sh            # Build from scratch + run
                    # Verify works end-to-end
```

### Switching Branches
```bash
# Clean old build artifacts
./clean.sh
./dev.sh            # Build new branch from scratch
```

---

## 🐛 Common Failure Cases

### 1. Build Fails: "CMake not found"
**Cause:** CMake not installed or not in PATH

**Solution:**
```bash
# Windows: Use Chocolatey or download from cmake.org
choco install cmake

# Linux: Use package manager
sudo apt-get install cmake       # Debian/Ubuntu
sudo dnf install cmake           # Fedora
sudo pacman -S cmake             # Arch

# macOS: Use Homebrew
brew install cmake
```

**Prevention:** Check CMake is installed at start of day:
```bash
cmake --version
```

---

### 2. Build Fails: "Ninja not found"
**Cause:** CMake is using Ninja preset but Ninja isn't installed

**Solution:**
```bash
# Install Ninja build system
choco install ninja              # Windows
sudo apt-get install ninja-build # Linux
brew install ninja               # macOS
```

**Or use Unix Makefiles preset:**
```bash
./build.sh -- -G "Unix Makefiles"
```

---

### 3. "Executable not found" when running
**Cause:** Haven't built yet or build in wrong location

**Solution:**
```bash
# Verify build succeeded
./build.bat              # or ./build.sh
echo %ERRORLEVEL%        # Windows: should be 0
echo $?                  # Unix: should be 0

# Check if binary exists
dir build\debug\bin      # Windows
ls build/debug/bin       # Linux/macOS

# If missing, try clean rebuild
./clean.bat
./build.bat
```

---

### 4. Port 8080 Already in Use
**Cause:** Another process using same port, or server didn't stop cleanly

**Solution:**
```bash
# Windows: Find process using port 8080
netstat -ano | findstr :8080

# Kill the process (replace PID with actual number)
taskkill /PID <PID> /F

# Linux/macOS: Find and kill process
lsof -i :8080
kill -9 <PID>

# Or use different port (modify config.json)
```

---

### 5. "Permission denied" on Unix
**Cause:** Run script not executable

**Solution:**
```bash
# Make scripts executable
chmod +x build.sh
chmod +x run.sh
chmod +x clean.sh
chmod +x dev.sh

# Or make all scripts executable at once
chmod +x *.sh
```

---

### 6. Build Fails: "Missing dependency"
**Cause:** Drogon, OpenCV, ONNX Runtime, or other dep not installed

**Solution:**
```bash
# Install Drogon (Ubuntu)
sudo apt-get install libdrogon-dev libpoco-dev

# Install OpenCV
sudo apt-get install libopencv-dev

# Install ONNX Runtime
# Download from https://github.com/microsoft/onnxruntime/releases

# Verify CMake can find deps
./build.sh -- -DCMAKE_PREFIX_PATH=/usr/local
```

---

### 7. CMake Configuration Fails
**Cause:** Incompatible compiler version, wrong C++ standard, or corrupted cache

**Solution:**
```bash
# Clean and reconfigure with specific compiler
./clean.bat
./build.bat -- -DCMAKE_CXX_COMPILER=clang++

# Or force specific C++ standard
./build.bat -- -DCMAKE_CXX_STANDARD=17
```

---

## 🔧 How to Extend the Scripts Later

### 1. Add Release Build Support
Modify `build.bat`:
```batch
REM Build release configuration too
echo [INFO] Building Release...
cmake --build "%BUILD_DIR%" --config Release --parallel

REM Then output both
echo Release Binary: build\release\bin\SpectraVision.exe
```

### 2. Add Unit Test Support
Create `test.bat`:
```batch
@echo off
call build.bat
if errorlevel 1 exit /b 1

echo Running tests...
ctest --test-dir build\debug --verbose
```

Usage:
```bash
./test.bat           # Build + run tests
```

### 3. Add Docker Build Support
Create `docker-build.bat`:
```batch
@echo off
echo Building Docker image...
docker build -t spectra-vision:latest .
```

### 4. Add Code Quality Checks
Create `lint.bat`:
```batch
@echo off
echo Running clang-tidy...
clang-tidy backend/src/*.cpp -- -I backend/include
```

### 5. Add Deployment/Install Support
Modify `build.bat` to install after build:
```batch
REM After successful build, install to system
cmake --install "%BUILD_DIR%" --config Debug --prefix "C:\Program Files\SpectraVision"
```

### 6. Add Watch Mode (Auto-Rebuild)
Create `watch.bat`:
```batch
@echo off
REM Watch for file changes and rebuild
:watch_loop
timeout /t 2
cls
./build.bat
if errorlevel 1 (
    echo Build failed, waiting for changes...
) else (
    echo Build succeeded at %date% %time%
)
goto watch_loop
```

### 7. Add Multiple Configuration Support
```batch
REM build.bat with configuration parameter
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=debug

cmake --preset %CONFIG% -B "build\%CONFIG%"
cmake --build "build\%CONFIG%" --config Debug --parallel
```

Usage:
```bash
./build.bat debug       # Debug build
./build.bat release     # Release build
./build.bat asan        # AddressSanitizer build
```

### 8. Add Development Server with Auto-Reload
Create `watch-run.bat`:
```batch
@echo off
:loop
timeout /t 3
cls
echo Rebuilding and restarting...
call build.bat
if errorlevel 0 (
    start "" "%PROJECT_ROOT%\build\debug\bin\SpectraVision.exe"
)
goto loop
```

### 9. Add Performance Profiling
Create `profile.bat`:
```batch
@echo off
call build.bat -- -DSV_ENABLE_LTO=ON
call run.bat

REM Use Windows Performance Analyzer
wpa build\debug\bin\SpectraVision.exe
```

### 10. Add CI/CD Integration
Create `.github/workflows/build.yml`:
```yaml
name: Build
on: [push, pull_request]
jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v2
      - name: Build
        run: .\build.bat
      - name: Run Tests
        run: .\test.bat
```

---

## 📊 Script Features Summary

### Windows Scripts (`*.bat`)

| Feature | build.bat | run.bat | clean.bat | dev.bat |
|---------|-----------|---------|-----------|---------|
| CMake configure | ✅ | - | - | ✅ (via build) |
| Incremental build | ✅ | - | - | ✅ (via build) |
| Error checking | ✅ | ✅ | ✅ | ✅ |
| Colored output | ✅ | ✅ | ✅ | ✅ |
| Argument passing | ✅ | ✅ | - | - |
| Exit codes | ✅ | ✅ | ✅ | ✅ |

### Unix Scripts (`*.sh`)

| Feature | build.sh | run.sh | clean.sh | dev.sh |
|---------|----------|--------|----------|--------|
| CMake configure | ✅ | - | - | ✅ (via build) |
| Incremental build | ✅ | - | - | ✅ (via build) |
| Error checking | ✅ | ✅ | ✅ | ✅ |
| ANSI colors | ✅ | ✅ | ✅ | ✅ |
| Argument passing | ✅ | ✅ | - | - |
| Exit codes | ✅ | ✅ | ✅ | ✅ |
| Executable check | - | ✅ | - | - |
| Chmod support | - | ✅ | - | - |

---

## 🎯 Best Practices

### 1. Always Clean Before Major Changes
```bash
./clean.sh
./dev.sh
```

### 2. Check Exit Codes in CI/CD
```bash
./build.sh
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi
```

### 3. Use `dev.sh` for Day-to-Day
- Fastest for edit-build-test cycle
- One command instead of three

### 4. Keep Scripts in Repo Root
- Easy to find and remember
- Works from any subdirectory
- Simple documentation

### 5. Make Scripts Executable on Unix
```bash
chmod +x *.sh
```

### 6. Document Custom Extensions
If you add features, update this guide

### 7. Test Scripts on Both Platforms
- Windows: Run in PowerShell and Command Prompt
- Linux: Test on Ubuntu and other distros
- macOS: Verify on Intel and Apple Silicon

---

## 📝 Summary

| Use Case | Command |
|----------|---------|
| Build project | `build.bat` or `./build.sh` |
| Run server | `run.bat` or `./run.sh` |
| Clean build | `clean.bat` or `./clean.sh` |
| Daily workflow | `dev.bat` or `./dev.sh` |
| Full rebuild | `clean.bat && build.bat` or `./clean.sh && ./build.sh` |
| CI/CD build | `./build.sh` (or `build.bat` on Windows) |

**Status: ✅ PRODUCTION READY**
