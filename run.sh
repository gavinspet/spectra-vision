#!/bin/bash
################################################################################
# Spectra Vision Run Script (Linux/macOS)
################################################################################
#
# Purpose:  Start the Spectra Vision backend server
# Usage:    ./run.sh [args...]
#
# Features:
#   - Automatic executable detection
#   - Verifies binary exists before running
#   - Forwards all command-line arguments to the binary
#   - Friendly error messages if build is missing
#   - Suggests build command if binary not found
#   - Works on Linux and macOS
#
################################################################################

# Colors for output
RED='\033[0;31m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
RESET='\033[0m'

# Paths
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY="${PROJECT_ROOT}/build/debug/bin/SpectraVision"

echo
echo -e "${BOLD}╔═════════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}║  Spectra Vision Backend Server                             ║${RESET}"
echo -e "${BOLD}╚═════════════════════════════════════════════════════════════╝${RESET}"
echo

# Check if the binary exists
if [[ ! -f "$BINARY" ]]; then
    echo -e "${RED}[ERROR] Executable not found: $BINARY${RESET}"
    echo
    echo "Build the project first with:"
    echo "  ./build.sh"
    echo "or:"
    echo "  ./dev.sh"
    echo
    exit 1
fi

# Make sure binary is executable
if [[ ! -x "$BINARY" ]]; then
    chmod +x "$BINARY"
fi

# Display server information
echo -e "${YELLOW}[INFO]${RESET} Starting Spectra Vision..."
echo -e "${YELLOW}[INFO]${RESET} Listening on http://localhost:8080"
echo -e "${YELLOW}[INFO]${RESET} Press Ctrl+C to stop"
echo

# Run the binary with all forwarded arguments
"$BINARY" "$@"

# Capture exit code
EXIT_CODE=$?

if [[ $EXIT_CODE -ne 0 ]]; then
    echo
    echo -e "${RED}[ERROR] Server exited with code: $EXIT_CODE${RESET}"
    echo
    exit $EXIT_CODE
fi

exit 0
