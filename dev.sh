#!/bin/bash
################################################################################
# Spectra Vision Dev Script (Linux/macOS)
################################################################################
#
# Purpose:  Complete development workflow: build → run
# Usage:    ./dev.sh
#
# Workflow:
#   1. Build the project (configure CMake if needed)
#   2. If build succeeds, start the server
#   3. If build fails, stop immediately with error
#
# Features:
#   - One-command build and run
#   - Stops on build failure
#   - Displays friendly status messages
#   - Perfect for development cycle
#   - Works on Linux and macOS
#
################################################################################

# Colors for output
RED='\033[0;31m'
BOLD='\033[1m'
BLUE='\033[0;34m'
RESET='\033[0m'

# Paths
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo
echo -e "${BOLD}${BLUE}╔═════════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}${BLUE}║  Spectra Vision Development Workflow                       ║${RESET}"
echo -e "${BOLD}${BLUE}║  Mode: Build + Run                                          ║${RESET}"
echo -e "${BOLD}${BLUE}╚═════════════════════════════════════════════════════════════╝${RESET}"
echo

# Step 1: Build
echo "[STEP 1/2] Building project..."
"${PROJECT_ROOT}/build.sh"

if [[ $? -ne 0 ]]; then
    echo
    echo -e "${BOLD}${RED}[FATAL] Build failed, not starting server${RESET}"
    echo
    exit 1
fi

# Step 2: Run
echo
echo "[STEP 2/2] Starting server..."
"${PROJECT_ROOT}/run.sh"

exit $?
