#!/bin/bash

set -e

SCRIPT_DIR="$(realpath -m "$(dirname "$0")")"

BUILD_DIR="${SCRIPT_DIR}/build"
INSTALL_DIR="${BUILD_DIR}/install"

if [[ -d "${INSTALL_DIR}" ]];then
  rm -rf "${INSTALL_DIR}"
fi

cmake -B "${BUILD_DIR}" -S "${SCRIPT_DIR}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DNXPILOT_INSTALL=ON \
  -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}"

cmake --build "${BUILD_DIR}" --target install --parallel $(nproc)
