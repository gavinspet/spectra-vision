#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"
BUILD_TYPE="${1:-Release}"

echo "[spectra-vision] Configuring — type: ${BUILD_TYPE}"
cmake -S . -B "${BUILD_DIR}" \
      -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
      -DBUILD_TESTS=ON \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "[spectra-vision] Building"
cmake --build "${BUILD_DIR}" --parallel "$(nproc)"

echo "[spectra-vision] Build complete → ${BUILD_DIR}/bin/spectra_vision"
