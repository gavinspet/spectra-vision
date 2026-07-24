# Spectra Vision — Build Guide

Production-grade CMake configuration for the Spectra Vision computer vision platform.

## Requirements

- **CMake** 3.16+
- **C++ Compiler** with C++17 support (GCC 7+, Clang 5+, MSVC 2019+)
- **Dependencies:**
  - Drogon (Web framework)
  - OpenCV (4.0+, with DNN module)
  - ONNX Runtime (1.10+)
  - JsonCpp
  - Threads (std::thread)
  - GTest (for unit tests)

### macOS Dependencies

```bash
brew install cmake ninja drogon opencv onnxruntime jsoncpp googletest
```

### Ubuntu/Debian

```bash
sudo apt-get install cmake ninja-build build-essential libdrogon-dev libopencv-dev \
    libonnxruntime-dev libjsoncpp-dev libgtest-dev
```

### Windows (VCPKG)

```bash
vcpkg install cmake ninja drogon opencv onnxruntime jsoncpp gtest
```

---

## Quick Start

### 1. Configure (Release Build)

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
       -DSVC_BUILD_TESTS=ON \
       -DSVC_ENABLE_LTO=ON \
       -B build .
```

### 2. Build

```bash
cmake --build build --parallel
```

### 3. Run Tests

```bash
ctest --test-dir build --verbose
```

### 4. Install

```bash
cmake --install build --prefix /usr/local
```

---

## Using CMake Presets (Recommended)

Modern approach using CMakePresets.json (CMake 3.20+):

### List Available Presets

```bash
cmake --list-presets
```

### Configure with Preset

```bash
# Debug build with sanitizers
cmake --preset debug

# Release with LTO
cmake --preset release-lto

# AddressSanitizer
cmake --preset asan
```

### Build with Preset

```bash
cmake --build --preset release-build
```

### Run Tests with Preset

```bash
ctest --preset debug-test
```

---

## Build Configurations

### `default`
- **C++ Standard:** C++17
- **Optimizations:** Compiler defaults
- **Tests:** Enabled
- **Sanitizers:** Disabled
- **LTO:** Disabled

```bash
cmake --preset default
```

### `debug` (Recommended for Development)
- **C++ Standard:** C++17
- **Optimizations:** -O0 -g
- **Tests:** Enabled
- **Sanitizers:** **Enabled** (AddressSanitizer + UBSan)
- **LTO:** Disabled
- **Warnings:** All enabled, as errors

```bash
cmake --preset debug
```

### `release` (Recommended for Production)
- **C++ Standard:** C++17
- **Optimizations:** -O3 (or `/O2` on MSVC)
- **Tests:** Enabled
- **Sanitizers:** Disabled
- **LTO:** Disabled

```bash
cmake --preset release
```

### `release-lto` (Maximum Performance)
- **C++ Standard:** C++17
- **Optimizations:** -O3 + LTO
- **Tests:** Disabled (speeds up build)
- **Sanitizers:** Disabled
- **LTO:** **Enabled**
- **Build time:** ~2-3x longer than `release`
- **Binary size:** Smaller
- **Performance:** Best

```bash
cmake --preset release-lto
```

### `asan` (AddressSanitizer - Strict Testing)
- **C++ Standard:** C++17
- **Optimizations:** -O0 -g
- **Tests:** Enabled
- **Sanitizers:** **Enabled**
- **Runtime overhead:** High
- **Use case:** Finding memory bugs, use-after-free, buffer overflows

```bash
cmake --preset asan
```

---

## Compiler Warnings

All warnings are **enabled** and treated as **errors** during development:

### GCC/Clang Warnings
```
-Wall              # Most common warnings
-Wextra            # Additional warnings
-Wpedantic         # Strictly conforming code
-Wcast-align       # Potential alignment issues
-Wconversion       # Implicit conversions
-Wsign-conversion  # Sign conversions
-Wunused           # Unused variables/functions
-Wshadow           # Variable shadowing
-Woverloaded-virtual  # Virtual function issues
-Wnon-virtual-dtor    # Missing virtual destructors
-Wold-style-cast      # Old C-style casts
-Wformat=2            # Format string issues
```

### MSVC Warnings
```
/W4            # Warning level 4
/WX            # Warnings as errors
/permissive-   # Standards conformance
```

### Exception
- `-Wno-unused-parameter`: Allowed (intentional in interfaces)

---

## Sanitizers (Debug Builds)

Enable with `-DSV_ENABLE_SANITIZERS=ON`:

### AddressSanitizer (ASAN)
Detects:
- Heap buffer overflows
- Stack buffer overflows
- Use-after-free
- Double-free
- Memory leaks

### UndefinedBehaviorSanitizer (UBSAN)
Detects:
- Integer overflows
- Null pointer dereferences
- Out-of-bounds shifts
- Invalid type casting
- Unaligned pointer dereferences

### Run with Sanitizers

```bash
cmake --preset asan
cmake --build --preset asan-build
ctest --test-dir build/asan --verbose
```

### Suppress Known Issues (if needed)

```bash
UBSAN_OPTIONS=halt_on_error=1 ./build/bin/SpectraVision
LSAN_OPTIONS=verbosity=1:log_threads=1 ./build/bin/SpectraVision
```

---

## Link-Time Optimization (LTO)

Enable with `-DSV_ENABLE_LTO=ON`:

**Benefits:**
- Smaller binary (~10-15% reduction)
- Better performance (~5-10% improvement)
- Enables cross-file optimizations

**Trade-offs:**
- Longer build time (~2-3x)
- Higher memory usage during linking
- Not compatible with all libraries

**Usage:**

```bash
cmake --preset release-lto
cmake --build --preset release-lto-build
```

---

## Install

### Local Development (no install needed)

```bash
# Run directly from build directory
./build/bin/SpectraVision config.json
```

### System-wide Installation

```bash
cmake --install build --prefix /usr/local
# Then:
SpectraVision /etc/spectra_vision/config.json
```

### Custom Installation Path

```bash
cmake --install build --prefix ~/spectra_vision_install
```

### Installed Files

```
${PREFIX}/bin/SpectraVision          # Main executable
${PREFIX}/include/spectra_vision/    # Public headers
${PREFIX}/lib/libSpectraVision.a     # Static library (if built)
${PREFIX}/etc/spectra_vision/        # Config templates
${PREFIX}/share/spectra_vision/      # Documentation
```

---

## compile_commands.json

Exported automatically by all presets. Location:
```
build/compile_commands.json
```

Use with:
- **Clang-Tidy:** `clang-tidy -p build src/main.cpp`
- **LSP:** Copy to project root or use symlink
- **IDE:** Point your IDE's compilation database path to `build/compile_commands.json`

---

## Static Analysis

### Clang-Tidy

Run on all sources:

```bash
clang-tidy -p build -checks=* src/**/*.cpp
```

Or use the project's .clang-tidy config:

```bash
clang-tidy -p build src/**/*.cpp
```

### Code Formatting

Format all code:

```bash
clang-format -i src/**/*.cpp include/**/*.h
```

Or check without modifying:

```bash
clang-format --dry-run -Werror src/**/*.cpp
```

---

## Troubleshooting

### CMake not found

```bash
# Install via Homebrew (macOS)
brew install cmake

# Install via apt (Ubuntu/Debian)
sudo apt-get install cmake

# Install via Chocolatey (Windows)
choco install cmake
```

### Drogon not found

```bash
# If installed but not in CMake path, set:
cmake -DCMAKE_PREFIX_PATH=/usr/local/opt/drogon ..

# Or use vcpkg
vcpkg install drogon
```

### Build fails with "ninja: not found"

```bash
# Install Ninja
brew install ninja          # macOS
sudo apt install ninja-build  # Ubuntu

# Or use Unix Makefiles instead
cmake -G "Unix Makefiles" ..
```

### Tests not running

```bash
# Ensure GTest is installed
brew install googletest  # macOS
sudo apt install libgtest-dev  # Ubuntu

# Rebuild
rm -rf build && cmake --preset debug && cmake --build --preset debug-build
```

### Sanitizer false positives

If ASAN reports leaks that aren't real:

```bash
LSAN_OPTIONS=suppressions=suppressions.txt ./build/bin/SpectraVision
```

Create `suppressions.txt`:
```
leak:libc
leak:libstdc++
```

---

## Performance Tips

### For Development
```bash
cmake --preset debug
```
- Fast builds
- Full debugging info
- Memory safety checks

### For Profiling
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DSVC_ENABLE_LTO=OFF -B build_profile .
cmake --build build_profile
perf record ./build_profile/bin/SpectraVision
perf report
```

### For Release
```bash
cmake --preset release-lto
```
- Longest build time
- Smallest binary
- Best runtime performance

---

## CI/CD Integration

### GitHub Actions Example

```yaml
name: Build & Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install deps
        run: sudo apt-get install -y cmake ninja-build libdrogon-dev libopencv-dev libonnxruntime-dev libjsoncpp-dev libgtest-dev
      - name: Configure
        run: cmake --preset release
      - name: Build
        run: cmake --build --preset release-build
      - name: Test
        run: ctest --test-dir build/release --verbose
      - name: Install
        run: cmake --install build/release --prefix $HOME/sv_install
```

---

## See Also

- [CMake Documentation](https://cmake.org/cmake/help/latest/)
- [Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
- [Drogon Framework](https://github.com/drogonframework/drogon)
- [ONNX Runtime](https://onnxruntime.ai/)
