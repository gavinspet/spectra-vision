#!/bin/bash
################################################################################
# Spectra Vision Build Script (Linux/macOS)
################################################################################
#
# Purpose:  Configure CMake and build the project in Debug mode
# Usage:    ./build.sh [-- extra cmake args]
#
# Features:
#   - Automatic CMake configuration if build folder missing
#   - Incremental builds supported
#   - Debug configuration by default
#   - ANSI color-coded output
#   - Proper exit codes for CI/CD integration
#   - Works on Linux and macOS
#
################################################################################

set -o pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
RESET='\033[0m'

# Paths
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build/debug"
PRESET="debug"

# Parse command line arguments
EXTRA_CMAKE_ARGS=""
if [[ $# -gt 0 ]]; then
    if [[ "$1" == "--" ]]; then
        shift
        EXTRA_CMAKE_ARGS="$@"
    else
        EXTRA_CMAKE_ARGS="$@"
    fi
fi

# Print header
echo
echo -e "${BOLD}${BLUE}╔═════════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}${BLUE}║  Spectra Vision Build Script                               ║${RESET}"
echo -e "${BOLD}${BLUE}║  Configuration: Debug                                       ║${RESET}"
echo -e "${BOLD}${BLUE}║  Platform: Linux/macOS (Ninja/Unix Makefiles)               ║${RESET}"
echo -e "${BOLD}${BLUE}╚═════════════════════════════════════════════════════════════╝${RESET}"
echo

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}[ERROR] CMake not found in PATH${RESET}"
    echo "Install CMake using:"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  macOS:         brew install cmake"
    echo
    exit 1
fi

# Create build directory if it doesn't exist
if [[ ! -d "$BUILD_DIR" ]]; then
    echo -e "${YELLOW}[INFO]${RESET} Build directory not found, creating..."
    mkdir -p "$BUILD_DIR" || {
        echo -e "${RED}[ERROR] Failed to create build directory: $BUILD_DIR${RESET}"
        exit 1
    }
fi

# Configure CMake if CMakeCache.txt doesn't exist
if [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
    echo -e "${YELLOW}[INFO]${RESET} Configuring project with CMake preset: $PRESET"
    cd "$PROJECT_ROOT"
    
    cmake --preset "$PRESET" -B "$BUILD_DIR" $EXTRA_CMAKE_ARGS || {
        echo -e "${RED}[ERROR] CMake configuration failed${RESET}"
        exit 1
    }
    echo -e "${GREEN}[OK]${RESET} CMake configuration complete"
    echo
fi

# Build the project
echo -e "${YELLOW}[INFO]${RESET} Building project..."
cmake --build "$BUILD_DIR" --config Debug --parallel

if [[ $? -ne 0 ]]; then
    echo
    echo -e "${BOLD}${RED}╔═════════════════════════════════════════════════════════════╗${RESET}"
    echo -e "${BOLD}${RED}║  BUILD FAILED                                               ║${RESET}"
    echo -e "${BOLD}${RED}║  Check the errors above for details                         ║${RESET}"
    echo -e "${BOLD}${RED}╚═════════════════════════════════════════════════════════════╝${RESET}"
    echo
    exit 1
fi

echo
echo -e "${BOLD}${GREEN}╔═════════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}${GREEN}║  BUILD SUCCESSFUL                                           ║${RESET}"
echo -e "${BOLD}${GREEN}║  Binary: build/debug/bin/SpectraVision                      ║${RESET}"
echo -e "${BOLD}${GREEN}║  Run with: ./run.sh                                         ║${RESET}"
echo -e "${BOLD}${GREEN}║  Or:       ./dev.sh  (build + run)                          ║${RESET}"
echo -e "${BOLD}${GREEN}╚═════════════════════════════════════════════════════════════╝${RESET}"
echo

exit 0
