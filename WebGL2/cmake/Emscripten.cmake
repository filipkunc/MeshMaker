# Emscripten Toolchain Configuration
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/Emscripten.cmake ..

# This file is used when building with Emscripten
# The actual toolchain file is provided by Emscripten SDK
# This is a helper file with common settings

message(STATUS "Emscripten build configuration")

# Set to use Emscripten's CMake toolchain
# Typically invoked as:
#   emcmake cmake -B build-web ..
#   cmake --build build-web

# Or with explicit toolchain:
#   cmake -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake ..

# Common Emscripten flags are set in the main CMakeLists.txt
