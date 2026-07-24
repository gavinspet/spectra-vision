#!/bin/bash
################################################################################
# Spectra Vision Clean Script (Linux/macOS)
################################################################################
#
# Purpose:  Remove all build artifacts and CMake cache
# Usage:    ./clean.sh
#
# Features:
#   - Removes build directory recursively
#   - Clears CMake cache
#   - Removes generated binaries
#   - Does NOT delete source code
#   - Safe to run multiple times
#   - Asks for confirmation before deletion
#   - Works on Linux and macOS
#
################################################################################

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
RESET='\033[0m'

# Paths
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

echo
echo -e "${BOLD}╔═════════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}║  Spectra Vision Clean Script                               ║${RESET}"
echo -e "${BOLD}╚═════════════════════════════════════════════════════════════╝${RESET}"
echo

# Check if build directory exists
if [[ ! -d "$BUILD_DIR" ]]; then
    echo -e "${YELLOW}[INFO]${RESET} Build directory does not exist"
    echo -e "${YELLOW}[INFO]${RESET} Nothing to clean"
    echo
    exit 0
fi

# Display what will be deleted
echo -e "${YELLOW}[INFO]${RESET} The following will be deleted:"
echo "  - $BUILD_DIR/"
echo

# Confirm deletion (read from stdin)
read -p "Continue? (y/n): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${YELLOW}[INFO]${RESET} Clean operation cancelled"
    echo
    exit 0
fi

echo
echo -e "${YELLOW}[INFO]${RESET} Removing build artifacts..."

# Delete build directory
if rm -rf "$BUILD_DIR"; then
    echo -e "${GREEN}[OK]${RESET} Build artifacts removed"
else
    echo -e "${RED}[ERROR] Failed to remove build directory${RESET}"
    echo
    exit 1
fi

# Delete CMake user presets (optional, development artifacts)
CMAKE_PRESETS_USER="${PROJECT_ROOT}/CMakeUserPresets.json"
if [[ -f "$CMAKE_PRESETS_USER" ]]; then
    rm -f "$CMAKE_PRESETS_USER"
    echo -e "${GREEN}[OK]${RESET} Removed CMake user presets"
fi

echo
echo -e "${BOLD}${GREEN}╔═════════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}${GREEN}║  CLEAN COMPLETE                                             ║${RESET}"
echo -e "${BOLD}${GREEN}║  Build directory removed successfully                       ║${RESET}"
echo -e "${BOLD}${GREEN}║  Rebuild with: ./build.sh                                   ║${RESET}"
echo -e "${BOLD}${GREEN}╚═════════════════════════════════════════════════════════════╝${RESET}"
echo

exit 0
