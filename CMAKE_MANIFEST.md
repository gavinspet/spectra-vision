# CMake Build System — File Manifest

Complete list of CMake configuration files created for Spectra Vision production build system.

## Root Level Files

### CMakeLists.txt
**Purpose:** Root project configuration
**Key Responsibilities:**
- Project metadata (name, version, description)
- Global CMake policies
- Build type defaults
- Options (BUILD_TESTS, ENABLE_SANITIZERS, ENABLE_LTO)
- Subdirectory management
- Build summary output

**Key Content:**
```cmake
cmake_minimum_required(VERSION 3.16)
project(SpectraVision)
option(SV_BUILD_TESTS "Build unit tests" ON)
add_subdirectory(backend)
```

### CMakePresets.json
**Purpose:** Reusable build configurations for developers
**Key Features:**
- 5 pre-configured presets: default, debug, release, release-lto, asan
- Cache variable defaults
- Build directory per preset
- Test configuration
- Generator expressions

**Usage:**
```bash
cmake --preset release
cmake --build --preset release-build
ctest --preset debug-test
```

---

## CMake Module Files (cmake/)

### cmake/FindOnnxRuntime.cmake
**Purpose:** Custom CMake finder for ONNX Runtime
**Provides:**
- `ONNXRUNTIME_FOUND` — Whether found
- `ONNXRUNTIME_INCLUDE_DIR` — Header directory
- `ONNXRUNTIME_LIBRARY` — Library path
- `ONNXRUNTIME_VERSION` — Version string
- `ONNXRUNTIME::ONNXRUNTIME` — Imported target

**Usage:**
```cmake
find_package(ONNXRUNTIME REQUIRED)
target_link_libraries(target PRIVATE ONNXRUNTIME::ONNXRUNTIME)
```

### cmake/SpectraVisionHelpers.cmake
**Purpose:** Reusable CMake helper functions
**Functions Provided:**

1. **sv_add_compiler_flags(TARGET)**
   - Applies strict compiler warnings
   - GCC/Clang: -Wall, -Wextra, -Wpedantic, etc.
   - MSVC: /W4, /WX, /permissive-

2. **sv_enable_sanitizers(TARGET)**
   - Adds AddressSanitizer + UBSan
   - Debug builds only
   - Flags: -fsanitize=address,undefined

3. **sv_enable_lto(TARGET)**
   - Enables Link-Time Optimization
   - Sets INTERPROCEDURAL_OPTIMIZATION

4. **sv_setup_executable(TARGET)**
   - One-shot setup for executables
   - Applies: flags, sanitizers, LTO, optimizations

5. **sv_setup_library(TARGET)**
   - One-shot setup for libraries
   - Adds: POSITION_INDEPENDENT_CODE

6. **sv_print_summary()**
   - Prints build configuration summary
   - Displays compiler, build type, options

---

## Backend CMakeLists.txt

### backend/CMakeLists.txt
**Purpose:** Backend target definition and linking
**Key Sections:**

1. **Project Setup**
   - Version, language, C++ standard
   - CMake policies, output directories

2. **Dependency Discovery**
   ```cmake
   find_package(Threads REQUIRED)
   find_package(Drogon REQUIRED)
   find_package(OpenCV REQUIRED)
   find_package(jsoncpp REQUIRED)
   find_package(onnxruntime REQUIRED)
   ```

3. **Main Target**
   - Sources organized by component
   - Target properties and linking
   - Installation rules

4. **Test Targets** (if SV_BUILD_TESTS=ON)
   - Google Test integration
   - Automatic test discovery

---

## Test CMakeLists.txt

### backend/tests/CMakeLists.txt
**Purpose:** Unit test configuration
**Key Features:**
- GTest and GMock integration
- Test discovery with `gtest_discover_tests()`
- Per-test compiler flags
- Sanitizer support in tests

**Test Helper Function:**
```cmake
sv_add_test(test_name TestFile.cpp)
```

---

## Configuration Files

### config.json.example
**Purpose:** Example configuration template
**Contains:**
- Server configuration (host, port, threads)
- Security settings (auth, rate limiting)
- Model registry with example entries
- Input shape definitions

**Location:** `backend/config.json.example`

---

## Code Quality Configuration

### .clang-format
**Purpose:** Code formatting rules
**Based On:** Google C++ Style Guide
**Key Settings:**
- 4-space indentation
- 100-character line limit
- Namespace indentation: all
- Pointer alignment: left

**Usage:**
```bash
clang-format -i src/**/*.cpp include/**/*.h
clang-format --dry-run -Werror src/**/*.cpp  # Check only
```

### .clang-tidy
**Purpose:** Static analysis configuration
**Enabled Checks:**
- Core analysis
- C++ core guidelines
- Performance warnings
- Readability suggestions
- Modernization opportunities
- Bug-prone patterns

**Disabled Checks:**
- Platform-specific (altera, android, etc.)
- LLVM-specific
- Magic numbers
- Excessive cognitive complexity

**Usage:**
```bash
clang-tidy -p build src/**/*.cpp
clang-tidy -p build -fix src/**/*.cpp  # Auto-fix
```

### .gitignore
**Purpose:** Git ignore patterns for C++ projects
**Covers:**
- Build directories (build/, cmake_build/)
- IDE files (.vscode/, .idea/)
- Compiled objects (*.o, *.a, *.so)
- Generated files
- Runtime artifacts (logs/, config.json)
- Sanitizer outputs

---

## Documentation Files

### BUILD.md
**Purpose:** Comprehensive build guide
**Sections:**
1. Requirements and installation
2. Quick start (configure, build, test)
3. CMake Presets reference
4. Build configurations explained
5. Compiler warnings reference
6. Sanitizers (ASAN, UBSAN)
7. Link-Time Optimization (LTO)
8. Installation instructions
9. Static analysis tools
10. Troubleshooting guide
11. CI/CD integration examples

**Length:** ~500 lines

### CMAKE_CONVENTIONS.md
**Purpose:** CMake best practices and conventions
**Sections:**
1. Directory structure
2. Design principles
3. Key file descriptions
4. Helper functions reference
5. Target linking patterns
6. Compiler warnings
7. Build types (Debug, Release, etc.)
8. Dependency management
9. Generated files
10. IDE integration
11. Cross-compilation
12. Common tasks
13. Troubleshooting

**Length:** ~600 lines

### TOOLING.md
**Purpose:** Development tools guide
**Sections:**
1. Tools overview table
2. Clang-Format usage
3. Clang-Tidy usage
4. Sanitizers (ASAN, UBSAN)
5. Unit testing with GTest
6. Test patterns and mocking
7. Build types for different use cases
8. CI/CD integration
9. Performance profiling
10. IDE setup (VS Code, CLion, VS2022)
11. Cheat sheet of useful commands

**Length:** ~800 lines

### CMAKE_README.md
**Purpose:** High-level overview of the build system
**Sections:**
1. What's included (bullet-point summary)
2. Build presets quick reference
3. Directory structure
4. Key features
5. Quick start guide
6. CMake helpers reference
7. Common tasks
8. Dependencies table
9. Build outputs
10. Compiler support
11. Advanced usage
12. Troubleshooting
13. Performance characteristics

**Length:** ~400 lines

---

## Build Automation Scripts

### scripts/build.sh
**Purpose:** Unix/Linux build automation
**Features:**
- Preset selection (debug, release, release-lto, asan)
- Automatic generator detection
- Parallel build
- Color output
- Helpful next steps

**Usage:**
```bash
./scripts/build.sh           # Debug (default)
./scripts/build.sh release   # Release
./scripts/build.sh asan      # AddressSanitizer
```

### scripts/build.bat
**Purpose:** Windows build automation
**Features:**
- Preset selection
- Visual Studio 2022 generator
- 64-bit configuration
- Error checking
- Next steps guidance

**Usage:**
```cmd
build.bat                 REM Debug (default)
build.bat release        REM Release
build.bat release-lto    REM Release with LTO
```

---

## File Size Summary

| File | Lines | Purpose |
|---|---|---|
| CMakeLists.txt | ~60 | Root config |
| CMakePresets.json | ~130 | Build presets |
| backend/CMakeLists.txt | ~180 | Backend config |
| backend/tests/CMakeLists.txt | ~50 | Test config |
| cmake/FindOnnxRuntime.cmake | ~65 | ONNX finder |
| cmake/SpectraVisionHelpers.cmake | ~140 | CMake helpers |
| .clang-format | ~70 | Format rules |
| .clang-tidy | ~60 | Analysis rules |
| .gitignore | ~70 | Git ignore |
| BUILD.md | ~500 | Build guide |
| CMAKE_CONVENTIONS.md | ~600 | Best practices |
| TOOLING.md | ~800 | Tools guide |
| CMAKE_README.md | ~400 | Overview |
| scripts/build.sh | ~60 | Build script |
| scripts/build.bat | ~45 | Build script |
| **TOTAL** | **~3,270** | **Complete system** |

---

## Usage Workflow

### 1. Initial Setup
```bash
cd spectra-vision
cmake --preset debug        # Configure
cmake --build --preset debug-build --parallel  # Build
ctest --test-dir build/debug --verbose         # Test
```

### 2. Development
```bash
# Format code
clang-format -i src/**/*.cpp

# Static analysis
clang-tidy -p build src/**/*.cpp

# Build with sanitizers
cmake --preset asan && cmake --build --preset asan-build

# Run tests
ctest --test-dir build/asan --verbose
```

### 3. Release Preparation
```bash
# Build optimized
cmake --preset release-lto
cmake --build --preset release-lto-build

# Run full test suite
ctest --test-dir build/release-lto --verbose

# Install
cmake --install build/release-lto --prefix /usr/local
```

---

## CMake Feature Checklist

✅ C++17 standard enforcement
✅ Modular source organization
✅ Strict compiler warnings (warnings as errors)
✅ Multi-configuration support (Debug/Release/RelWithDebInfo/MinSizeRel)
✅ Sanitizers (AddressSanitizer, UBSanitizer)
✅ Link-Time Optimization (LTO)
✅ Custom dependency finders (FindOnnxRuntime.cmake)
✅ Reusable helper functions (SpectraVisionHelpers.cmake)
✅ Test integration (Google Test with auto-discovery)
✅ Installation rules (binaries, headers, CMake exports)
✅ compile_commands.json export (IDE/tooling support)
✅ CMakePresets.json (modern preset system)
✅ Platform support (Linux/macOS/Windows)
✅ Cross-compilation support
✅ Continuous integration examples (GitHub Actions)

---

## Key Design Decisions

### 1. Target-Based Approach
- Use `target_*` commands instead of global variables
- Proper include directory scoping (PUBLIC/PRIVATE/INTERFACE)
- Enables clear dependency graphs

### 2. Helper Functions Over Copy-Paste
- `sv_add_compiler_flags()` instead of repeating -Wall
- `sv_setup_executable()` instead of per-target duplication
- Single source of truth for configuration

### 3. CMakePresets.json for Developer UX
- Pre-configured presets eliminate guessing
- Consistent configuration across team
- IDE auto-detection

### 4. Separate Configuration Files
- Code quality separate (.clang-format, .clang-tidy)
- Build configuration separate (CMakeLists.txt)
- Easier to maintain and update

### 5. Comprehensive Documentation
- BUILD.md for step-by-step instructions
- CMAKE_CONVENTIONS.md for architecture
- TOOLING.md for development workflows
- Each document serves a specific purpose

---

## Next Steps

1. **Review** — Read CMAKE_README.md for overview
2. **Setup** — Follow BUILD.md quick start
3. **Learn** — Study CMAKE_CONVENTIONS.md
4. **Develop** — Use presets and TOOLING.md for daily work
5. **Deploy** — Follow installation instructions in BUILD.md

---

## Support Resources

- **CMake Documentation:** https://cmake.org/cmake/help/latest/
- **Modern CMake:** https://cliutils.gitlab.io/modern-cmake/
- **Drogon Framework:** https://github.com/drogonframework/drogon
- **ONNX Runtime:** https://onnxruntime.ai/
- **Google Test:** https://google.github.io/googletest/
