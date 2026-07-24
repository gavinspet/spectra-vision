#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"
BINARY="${BUILD_DIR}/bin/spectra_vision"
CONFIG="${1:-config.json}"

if [[ ! -f "${BINARY}" ]]; then
    echo "[spectra-vision] Binary not found. Run scripts/build.sh first."
    exit 1
fi

echo "[spectra-vision] Starting server with config: ${CONFIG}"
exec "${BINARY}" "${CONFIG}"
