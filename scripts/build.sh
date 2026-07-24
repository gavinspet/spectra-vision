#!/usr/bin/env bash
#
# build.sh — Production build script for Spectra Vision
#
# Usage:
#   ./scripts/build.sh                # Debug build
#   ./scripts/build.sh release        # Release build
#   ./scripts/build.sh release-lto    # Release with LTO
#   ./scripts/build.sh asan           # ASAN build
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_TYPE="${1:-debug}"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[spectra-vision]${NC} $1"
}

print_error() {
    echo -e "${RED}[error]${NC} $1" >&2
}

print_warning() {
    echo -e "${YELLOW}[warning]${NC} $1"
}

# Validate preset
case "$BUILD_TYPE" in
    debug|release|release-lto|asan|default)
        ;;
    *)
        print_error "Unknown build type: $BUILD_TYPE"
        echo "Usage: $0 {debug|release|release-lto|asan|default}"
        exit 1
        ;;
esac

# Check for required tools
command -v cmake >/dev/null 2>&1 || {
    print_error "cmake not found. Please install CMake 3.16+"
    exit 1
}

command -v ninja >/dev/null 2>&1 || {
    print_warning "ninja not found, using Unix Makefiles"
    GENERATOR="Unix Makefiles"
} || GENERATOR="Ninja"

print_status "Building Spectra Vision [${BUILD_TYPE}]"
print_status "Generator: ${GENERATOR}"

cd "$PROJECT_DIR"

# Configure
print_status "Configuring..."
cmake --preset "$BUILD_TYPE" -G "$GENERATOR"

# Build
print_status "Building..."
cmake --build "build/$BUILD_TYPE" --parallel "$(nproc 2>/dev/null || echo 4)"

print_status "Build complete!"
print_status "Executable: $(pwd)/build/$BUILD_TYPE/bin/SpectraVision"
print_status ""
print_status "Next steps:"
echo "  1. Run tests: ctest --test-dir build/$BUILD_TYPE --verbose"
echo "  2. Install: cmake --install build/$BUILD_TYPE --prefix /usr/local"
echo "  3. Run: ./build/$BUILD_TYPE/bin/SpectraVision config.json"
