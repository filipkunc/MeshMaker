#!/bin/bash
# Build OpenUSD core for wasm32 against the wasm oneTBB, per BUILDING.md's
# WebAssembly section (monolithic static libs, no imaging/python).
set -euo pipefail

source ~/emsdk/emsdk_env.sh

DEPS="$(cd "$(dirname "$0")" && pwd)"
USD_TAG="${USD_TAG:-v26.08}"
SRC="$DEPS/src/OpenUSD"
BUILD="$DEPS/build/usd"
INSTALL="$DEPS/openusd_wasm"

if [ ! -d "$SRC" ]; then
    git clone --depth 1 --branch "$USD_TAG" https://github.com/PixarAnimationStudios/OpenUSD.git "$SRC"
fi

emcmake cmake -S "$SRC" -B "$BUILD" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$INSTALL" \
    -DCMAKE_PREFIX_PATH="$INSTALL" \
    -DCMAKE_FIND_ROOT_PATH="$DEPS/tbb_wasm" \
    -DBUILD_SHARED_LIBS=OFF \
    -DPXR_BUILD_TESTS=OFF \
    -DPXR_BUILD_EXAMPLES=OFF \
    -DPXR_BUILD_TUTORIALS=OFF \
    -DPXR_BUILD_USD_TOOLS=OFF \
    -DPXR_BUILD_IMAGING=OFF \
    -DPXR_ENABLE_PYTHON_SUPPORT=OFF \
    -DPXR_ENABLE_GL_SUPPORT=OFF \
    -DCMAKE_CXX_FLAGS="-pthread --use-port=zlib" \
    -DCMAKE_C_FLAGS="-pthread --use-port=zlib" \
    -DCMAKE_EXE_LINKER_FLAGS="-pthread"

cmake --build "$BUILD" --target install -j"$(nproc)"
echo "USD_WASM_DONE"
