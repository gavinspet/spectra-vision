# Spectra Vision — Development Tooling Guide

This document describes the development tools configured for code quality, static analysis, and formatting.

## Tools Overview

| Tool | Purpose | Config File | Mode |
|---|---|---|---|
| **CMake** | Build system | `CMakeLists.txt`, `CMakePresets.json` | Active |
| **Clang-Format** | Code formatting | `.clang-format` | Active |
| **Clang-Tidy** | Static analysis | `.clang-tidy` | Active |
| **Sanitizers** | Runtime checks | Enabled in CMake | Optional |
| **AddressSanitizer** | Memory safety | CMake option | Optional |
| **UndefinedBehaviorSanitizer** | UB detection | CMake option | Optional |
| **Google Test** | Unit testing | `backend/tests/CMakeLists.txt` | Active |

---

## Code Formatting with Clang-Format

### Configuration

File: `.clang-format`

Based on: Google C++ Style Guide with customizations for Spectra Vision

Key settings:
- **Indentation:** 4 spaces
- **Line length:** 100 characters
- **Namespace indentation:** All namespaces indented
- **Pointer alignment:** Left (`Type* var`, not `Type *var`)

### Usage

#### Format All Code

```bash
clang-format -i src/**/*.cpp include/**/*.h
clang-format -i backend/**/*.cpp backend/**/*.h
```

#### Format Specific File

```bash
clang-format -i src/main.cpp
```

#### Check Without Modifying

```bash
clang-format --dry-run -Werror src/**/*.cpp
```

#### Format on Save (VS Code)

Install "Clang-Format" extension and add to `.vscode/settings.json`:

```json
{
  "editor.defaultFormatter": "xaver.clang-format",
  "editor.formatOnSave": true,
  "[cpp]": {
    "editor.defaultFormatter": "xaver.clang-format"
  }
}
```

#### Format via Git Pre-Commit Hook

```bash
#!/bin/bash
# .git/hooks/pre-commit
clang-format -i $(git diff --cached --name-only --diff-filter=ACM | grep '\.cpp$\|\.h$')
git add $(git diff --cached --name-only --diff-filter=ACM | grep '\.cpp$\|\.h$')
```

---

## Static Analysis with Clang-Tidy

### Configuration

File: `.clang-tidy`

Enabled checks:
- Core analysis
- C++ core guidelines
- Performance
- Readability
- Modernization
- Bug-prone patterns

Disabled checks:
- Platform-specific (altera, android, fuchsia, zircon)
- LLVM-specific (llvm-*)
- Magic numbers (readability-magic-numbers)

### Usage

#### Run on Single File

```bash
clang-tidy -p build src/main.cpp
```

#### Run on All Sources

```bash
clang-tidy -p build src/**/*.cpp backend/**/*.cpp
```

#### Fix Issues Automatically

```bash
clang-tidy -p build -fix src/**/*.cpp
```

#### Specific Checks

```bash
# Only modernization checks
clang-tidy -checks="modernize-*" -p build src/**/*.cpp

# Only safety checks
clang-tidy -checks="cppcoreguidelines-*,bugprone-*" -p build src/**/*.cpp

# Exclude specific checks
clang-tidy -checks="-readability-*" -p build src/**/*.cpp
```

### Common Issues & Fixes

#### Naming Convention

```cpp
// ✗ Bad: snake_case for class
class my_detector { };

// ✓ Good: CamelCase for class
class MyDetector { };

// ✗ Bad: CONSTANT_CASE for member variable
int my_value = 0;

// ✓ Good: camelBack_ for member variable
int myValue_ = 0;
```

**Fix:** `clang-tidy -p build -fix --fix-errors src/myfile.cpp`

#### Using `auto` Without Type

```cpp
// ✗ Bad: Unclear type
auto detector = factory.create("onnx");

// ✓ Good: Explicit type
std::shared_ptr<IDetector> detector = factory.create("onnx");
```

#### Missing Virtual Destructor

```cpp
// ✗ Bad: Base class without virtual destructor
class IStrategy {
public:
    virtual ~IStrategy() {}  // ← Must have
    virtual void run() = 0;
};
```

---

## Sanitizers

### Address Sanitizer (ASAN)

Detects memory errors:
- Heap buffer overflows
- Stack buffer overflows
- Use-after-free
- Double-free
- Memory leaks

#### Enable

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DSV_ENABLE_SANITIZERS=ON ..
cmake --build . --parallel
```

Or use preset:

```bash
cmake --preset asan
cmake --build --preset asan-build
```

#### Run with ASAN

```bash
./build/bin/SpectraVision config.json
# Output:
# =================================================================
# ==12345==ERROR: LeakSanitizer: detected memory leaks
# ...
```

#### Suppress Known Issues

Create `suppressions.txt`:

```
leak:libc
leak:libstdc++
leak:libm
```

Run with suppression:

```bash
LSAN_OPTIONS=suppressions=suppressions.txt \
  ./build/bin/SpectraVision config.json
```

### Undefined Behavior Sanitizer (UBSAN)

Detects undefined behavior:
- Integer overflows
- Null pointer dereferences
- Out-of-bounds shifts
- Invalid type casts
- Unaligned pointers

#### Enable

Same as ASAN (both enabled together with `-DSV_ENABLE_SANITIZERS=ON`)

#### Run with UBSAN

```bash
./build/bin/SpectraVision config.json
# Output:
# runtime error: index 10 out of bounds for type 'int [5]'
```

### Sanitizer Output

Sanitizers print stack traces to stderr:

```bash
# Redirect to file
./build/bin/SpectraVision config.json 2> asan.log

# View with symbolizer
cat asan.log | llvm-symbolizer
```

---

## Unit Testing with Google Test

### Run Tests

```bash
# All tests
ctest --test-dir build --verbose

# Specific test
ctest --test-dir build --verbose -R PreprocessorTest

# Stop on first failure
ctest --test-dir build --verbose --stop-on-failure

# Show output even on pass
ctest --test-dir build --verbose --output-on-failure
```

### Run with Sanitizers

```bash
# ASAN during testing
cmake --preset asan
cmake --build --preset asan-build
ctest --test-dir build/asan --verbose
```

### Write New Test

```cpp
#include <gtest/gtest.h>
#include "services/DetectionService.h"

class DetectionServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test fixtures
    }
};

TEST_F(DetectionServiceTest, Detect_ReturnsValidResponse) {
    // Arrange
    dto::DetectionRequest request;
    
    // Act
    auto response = sut_->detect(request);
    
    // Assert
    EXPECT_FALSE(response.detections.empty());
}

TEST(ModuleName, FeatureName) {
    // Test without fixture
}
```

### Test Patterns

#### Mocking with GMock

```cpp
#include <gmock/gmock.h>

class MockDetector : public IDetector {
public:
    MOCK_METHOD(void, loadModel, (const std::string&), (override));
    MOCK_METHOD(std::vector<BoundingBox>, detect,
                (const models::Frame&), (override));
};

TEST(MyTest, UseMock) {
    auto mock = std::make_shared<MockDetector>();
    EXPECT_CALL(*mock, detect(_))
        .Times(AtLeast(1))
        .WillOnce(Return(std::vector<BoundingBox>{...}));
    
    // Use mock...
}
```

#### Parametrized Tests

```cpp
class ResizePreprocessorTest
    : public ::testing::TestWithParam<std::tuple<int, int>> {
};

TEST_P(ResizePreprocessorTest, ResizeProducesCorrectDimensions) {
    auto [width, height] = GetParam();
    auto resized = preprocessor_.preprocess(frame_);
    EXPECT_EQ(resized.width(), width);
    EXPECT_EQ(resized.height(), height);
}

INSTANTIATE_TEST_SUITE_P(
    ResizeDimensions,
    ResizePreprocessorTest,
    ::testing::Values(
        std::make_tuple(224, 224),
        std::make_tuple(640, 640),
        std::make_tuple(1024, 1024)
    )
);
```

---

## Build Types for Different Use Cases

### Development Build

```bash
cmake --preset debug
cmake --build --preset debug-build
```

- Sanitizers: On
- Optimizations: None (-O0)
- Debug symbols: Full (-g)
- Best for: Finding bugs during development

### Release Build

```bash
cmake --preset release
cmake --build --preset release-build
```

- Sanitizers: Off
- Optimizations: Maximum (-O3)
- Debug symbols: None
- Best for: Production deployment

### Release with Debug Info

```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSVC_ENABLE_SANITIZERS=OFF ..
```

- Optimizations: High (-O2)
- Debug symbols: Full (-g)
- Best for: Profiling, production debugging

### Release with LTO

```bash
cmake --preset release-lto
```

- Link-Time Optimization: On
- Optimizations: Maximum
- Build time: 2-3x longer
- Binary size: 10-15% smaller
- Performance: 5-10% faster
- Best for: Final optimized release

---

## Continuous Integration

### GitHub Actions

```yaml
name: Build & Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build-type: [debug, release]
    steps:
      - uses: actions/checkout@v3
      
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake ninja-build \
            libdrogon-dev libopencv-dev \
            libonnxruntime-dev libjsoncpp-dev \
            libgtest-dev
      
      - name: Configure
        run: cmake --preset ${{ matrix.build-type }}
      
      - name: Build
        run: cmake --build build/${{ matrix.build-type }} --parallel
      
      - name: Test
        run: ctest --test-dir build/${{ matrix.build-type }} --verbose
```

### Local CI Simulation

```bash
# Run all checks locally before pushing
./scripts/check.sh

# Equivalent to:
cmake --preset debug
cmake --build --preset debug-build
clang-format --dry-run -Werror src/**/*.cpp
clang-tidy -p build src/**/*.cpp
ctest --test-dir build/debug --verbose
```

---

## Performance Profiling

### Linux perf

```bash
# Build with symbols
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake --build . --parallel

# Profile
perf record -g ./build/bin/SpectraVision config.json

# Report
perf report

# Flame graph
perf script | stackcollapse-perf.pl > out.perf-folded
flamegraph.pl out.perf-folded > perf.svg
```

### Valgrind

```bash
valgrind --leak-check=full --show-leak-kinds=all \
  ./build/bin/SpectraVision config.json
```

### CPU Profiling with Google Profiler

```bash
# Build with profiler
LDFLAGS="-lprofiler" cmake ..

# Profile
CPUPROFILE=cpu.prof ./build/bin/SpectraVision config.json

# Analyze
pprof ./build/bin/SpectraVision cpu.prof
```

---

## Useful Commands Cheat Sheet

```bash
# Format all code
clang-format -i $(find . -name '*.cpp' -o -name '*.h')

# Analyze all code
clang-tidy -p build $(find . -name '*.cpp' | grep -E 'src/|backend/')

# Run tests with sanitizers
cmake --preset asan && cmake --build --preset asan-build && \
  ctest --test-dir build/asan -V

# Generate compile_commands.json
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# Install locally
cmake --install build --prefix ~/.local

# Check what will be installed
cmake --install build --prefix ~/.local --verbose --dry-run

# Clean build directory
rm -rf build && mkdir build
```

---

## IDE Setup

### VS Code

Install extensions:
- `ms-vscode.cpptools`
- `ms-vscode.cmake-tools`
- `xaver.clang-format`
- `clang-tools-extra` (for clang-tidy integration)

`.vscode/settings.json`:

```json
{
  "cmake.configureOnOpen": true,
  "cmake.showOptionsMovedNotification": false,
  "C_Cpp.clangFormatPath": "/usr/bin/clang-format",
  "[cpp]": {
    "editor.defaultFormatter": "xaver.clang-format",
    "editor.formatOnSave": true
  }
}
```

### CLion

- Auto-detects CMakePresets.json
- Integrates clang-tidy and clang-format
- Configure: Settings → Languages & Frameworks → C++

### Visual Studio 2022

- CMake Presets auto-loaded
- Built-in code analysis
- CMake → Cache → Delete Cache → Reconfigure to reset

---

## References

- [Clang-Format Style Options](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
- [Clang-Tidy Checks](https://clang.llvm.org/extra/clang-tidy/checks/list.html)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)
- [Google Test Documentation](https://google.github.io/googletest/)
