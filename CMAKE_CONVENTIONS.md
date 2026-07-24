# CMake Conventions & Best Practices

This document describes the conventions and best practices used in the Spectra Vision CMake build system.

## Directory Structure

```
spectra-vision/
├── CMakeLists.txt              # Root configuration
├── CMakePresets.json           # Build presets
├── .clang-format               # Code formatting rules
├── .clang-tidy                 # Static analysis rules
├── BUILD.md                    # Build instructions
├── cmake/
│   ├── FindOnnxRuntime.cmake   # Custom finder for ONNX Runtime
│   ├── SpectraVisionHelpers.cmake  # Common helper functions
├── backend/
│   ├── CMakeLists.txt          # Backend target definition
│   ├── src/                    # Source files
│   ├── include/                # Public headers
│   ├── tests/
│   │   └── CMakeLists.txt      # Test targets
│   └── config.json.example     # Config template
└── scripts/
    ├── build.sh                # Unix build script
    └── build.bat               # Windows build script
```

## Design Principles

### 1. Modern CMake

- **Minimum version:** CMake 3.16 (for better defaults)
- **Targets over variables:** Use `target_*` commands, not global variables
- **Interface targets:** Use `INTERFACE` for header-only libraries
- **Generator expressions:** For configuration-dependent settings

### 2. Separation of Concerns

- **Root CMakeLists.txt:** Project-level configuration, subdirectory management
- **backend/CMakeLists.txt:** Target definitions and build settings
- **tests/CMakeLists.txt:** Test targets only
- **cmake/*.cmake:** Reusable helpers and finders

### 3. Modular Configuration

All compiler/linker flags are configured once in helper functions and applied consistently:

```cmake
# ✓ Good: Reusable, maintainable
sv_add_compiler_flags(target)
sv_enable_sanitizers(target)

# ✗ Avoid: Repetitive, error-prone
target_compile_options(target PRIVATE -Wall -Wextra ...)
```

## Key Files

### CMakeLists.txt Hierarchy

#### Root (`CMakeLists.txt`)

```cmake
cmake_minimum_required(VERSION 3.16)
project(SpectraVision)

# Include helpers
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SpectraVisionHelpers)

# Configure options
option(SV_BUILD_TESTS "Build unit tests" ON)

# Add subdirectories
add_subdirectory(backend)
```

**Responsibilities:**
- Global project metadata
- Root-level options
- Subdirectory orchestration

#### Backend (`backend/CMakeLists.txt`)

```cmake
project(SpectraVision VERSION 1.0.0 LANGUAGES CXX)

# Find dependencies
find_package(Drogon REQUIRED)
find_package(OpenCV REQUIRED)

# Create target
add_executable(${PROJECT_NAME} ${SV_SOURCES})

# Configure target
target_link_libraries(...)
sv_setup_executable(${PROJECT_NAME})
```

**Responsibilities:**
- Dependency discovery
- Target definition
- Target configuration

#### Tests (`backend/tests/CMakeLists.txt`)

```cmake
find_package(GTest REQUIRED)

# Create test targets
sv_add_test(test_name TestFile.cpp)
```

**Responsibilities:**
- Test target creation only

### CMakePresets.json

Defines reusable build configurations:

```json
{
  "configurePresets": [
    {
      "name": "release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "SV_ENABLE_LTO": "ON"
      }
    }
  ]
}
```

**Usage:**

```bash
cmake --preset release
cmake --build --preset release-build
```

## Helper Functions

All defined in `cmake/SpectraVisionHelpers.cmake`:

### `sv_add_compiler_flags(TARGET)`

Adds standard compiler warnings:

```cmake
sv_add_compiler_flags(my_target)
# Adds: -Wall -Wextra -Wpedantic -Wno-unused-parameter ...
```

### `sv_enable_sanitizers(TARGET)`

Enables AddressSanitizer + UBSan (Debug only):

```cmake
sv_enable_sanitizers(my_target)
# Adds: -fsanitize=address -fsanitize=undefined
```

### `sv_enable_lto(TARGET)`

Enables Link-Time Optimization:

```cmake
sv_enable_lto(my_target)
# Sets: INTERPROCEDURAL_OPTIMIZATION=TRUE
```

### `sv_setup_executable(TARGET)`

One-shot setup for executables:

```cmake
sv_setup_executable(my_executable)
# Applies: flags, sanitizers, LTO, build-type optimization
```

### `sv_setup_library(TARGET)`

One-shot setup for libraries:

```cmake
sv_setup_library(my_library)
# Applies: all of setup_executable + POSITION_INDEPENDENT_CODE
```

### `sv_print_summary()`

Prints build configuration summary:

```cmake
sv_print_summary()
# Output:
# ╔════════════════════════════════════════════════════════╗
# ║        Spectra Vision Build Configuration Summary      ║
# ...
```

## Target Linking

### Public Headers

Headers that downstream code needs:

```cmake
target_include_directories(my_target PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/spectra_vision>
)
```

### Private Implementation

Implementation headers only for this target:

```cmake
target_include_directories(my_target PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```

### Transitive Dependencies

Libraries the target needs:

```cmake
target_link_libraries(my_target
    PUBLIC   # Downstream needs these
        spectra_vision::interfaces
    PRIVATE  # Only this target needs these
        opencv_core
        drogon::drogon
)
```

## Compiler Warnings

### C++ Standard Warnings

- `-Wall`: Most common warnings
- `-Wextra`: Additional warnings
- `-Wpedantic`: Strict standard compliance
- `-Wconversion`: Implicit conversions
- `-Wshadow`: Variable shadowing
- `-Woverloaded-virtual`: Virtual function issues

### Exceptions (Allowed)

- `-Wno-unused-parameter`: Intentional in interfaces

### MSVC Equivalents

- `/W4`: Warning level 4
- `/WX`: Warnings as errors
- `/permissive-`: Standards conformance

## Build Types

### Debug

```bash
cmake -DCMAKE_BUILD_TYPE=Debug
# Optimizations: -O0 -g
# Sanitizers: Optional (enable with -DSV_ENABLE_SANITIZERS=ON)
# Tests: Enabled
```

### Release

```bash
cmake -DCMAKE_BUILD_TYPE=Release
# Optimizations: -O3 / /O2
# Sanitizers: Disabled
# LTO: Optional (enable with -DSV_ENABLE_LTO=ON)
# Tests: Enabled
```

### RelWithDebInfo

```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
# Optimizations: -O2 -g
# Use case: Production debugging
```

### MinSizeRel

```bash
cmake -DCMAKE_BUILD_TYPE=MinSizeRel
# Optimizations: -Os
# Use case: Embedded systems
```

## Dependency Management

### Find Modules

Located in `cmake/`:

- `FindOnnxRuntime.cmake` — Custom finder for ONNX Runtime
- Standard: `FindDrogon.cmake`, `FindOpenCV.cmake` (system-provided)

### Usage

```cmake
find_package(ONNXRUNTIME REQUIRED)
target_link_libraries(my_target PRIVATE ONNXRUNTIME::ONNXRUNTIME)
```

### Custom Finder Attributes

```cmake
# After find_package():
if(ONNXRUNTIME_FOUND)
    message(STATUS "ONNX Runtime version: ${ONNXRUNTIME_VERSION}")
    target_link_libraries(my_target PRIVATE ONNXRUNTIME::ONNXRUNTIME)
endif()
```

## Generated Files

### compile_commands.json

```bash
# Automatically generated with:
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Location: build/compile_commands.json
# Use with: clang-tidy, LSP, IDE tooling
```

### Installation Files

```bash
cmake --install build --prefix /usr/local
# Installs to:
# /usr/local/bin/SpectraVision
# /usr/local/include/spectra_vision/
# /usr/local/lib/cmake/SpectraVision/
```

## IDE Integration

### Visual Studio Code

1. Install CMake Tools extension
2. Configure with CMakePresets.json (auto-detected)
3. Select preset from status bar
4. Build/Debug via UI

### Visual Studio 2022

1. Open `CMakeLists.txt` file
2. Presets auto-loaded from `CMakePresets.json`
3. Select configuration from dropdown

### CLion

1. Presets auto-imported
2. Configure → CMake
3. Select preset from dropdown

## Cross-Compilation

```cmake
# Specify toolchain
cmake \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  ..
```

Example toolchain for ARM:

```cmake
# arm-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
```

## Common Tasks

### Add a New Source File

1. Add to `SV_SOURCES` in `backend/CMakeLists.txt`
2. No need to touch CMake again — GLOB_RECURSE handles it

### Add a New Dependency

1. Add `find_package(PackageName REQUIRED)` to `backend/CMakeLists.txt`
2. Link with `target_link_libraries(SpectraVision PRIVATE PackageName::...)`

### Add a New Test

1. Create test file in `backend/tests/`
2. Call `sv_add_test(test_name TestFile.cpp)` in `backend/tests/CMakeLists.txt`

### Add a Custom Compiler Flag

1. Edit `cmake/SpectraVisionHelpers.cmake`
2. Update the appropriate helper function
3. All targets using that helper will inherit the flag

### Configure for Sanitizers

```bash
# Via CMake option
cmake -DCMAKE_BUILD_TYPE=Debug -DSV_ENABLE_SANITIZERS=ON ..

# Via preset
cmake --preset asan
```

## Static Analysis

### Clang-Tidy

```bash
# Run on all sources
clang-tidy -p build src/**/*.cpp

# Configuration from .clang-tidy
clang-tidy -p build include/spectra_vision/*.h

# Fix issues automatically
clang-tidy -p build -fix src/**/*.cpp
```

### Code Formatting

```bash
# Format all code
clang-format -i src/**/*.cpp

# Check without modifying
clang-format --dry-run -Werror src/**/*.cpp
```

## Performance Profiling

```bash
# Build with profiling symbols
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake --build . --parallel

# Profile with Linux perf
perf record ./build/bin/SpectraVision config.json
perf report
```

## Troubleshooting

### CMake Can't Find a Package

```bash
# Provide hint
cmake -DCMAKE_PREFIX_PATH=/usr/local/opt/drogon ..

# Or set environment variable
export DROGON_ROOT=/usr/local/opt/drogon
cmake ..
```

### Linker Error: Undefined Reference

1. Verify library is found: `cmake -LH | grep LIBRARY`
2. Verify linking order: Check `target_link_libraries()`
3. Check for cyclic dependencies

### Out-of-Memory During Linking (LTO)

Disable LTO and link with `-fuse-ld=gold`:

```cmake
set(SV_ENABLE_LTO OFF)
target_link_options(target PRIVATE -fuse-ld=gold)
```

## References

- [CMake Documentation](https://cmake.org/cmake/help/latest/)
- [Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
- [CMake Best Practices](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/)
