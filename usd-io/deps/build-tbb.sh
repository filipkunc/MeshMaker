#!/bin/bash
# Build oneTBB v2021.12.0 for wasm32 (prerequisite for the OpenUSD wasm build).
set -euo pipefail

source ~/emsdk/emsdk_env.sh

DEPS="$(cd "$(dirname "$0")" && pwd)"
SRC="$DEPS/src/oneTBB"
BUILD="$DEPS/build/tbb"
INSTALL="$DEPS/tbb_wasm"

if [ ! -d "$SRC" ]; then
    git clone --depth 1 --branch v2021.12.0 https://github.com/uxlfoundation/oneTBB.git "$SRC"
fi

emcmake cmake -S "$SRC" -B "$BUILD" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$INSTALL" \
    -DBUILD_SHARED_LIBS=OFF \
    -DTBB_TEST=OFF \
    -DTBB_EXAMPLES=OFF \
    -DTBB_STRICT=OFF \
    -DCMAKE_CXX_FLAGS="-pthread" \
    -DCMAKE_C_FLAGS="-pthread"

cmake --build "$BUILD" --target install -j"$(nproc)"
echo "TBB_WASM_DONE"
