cmake_minimum_required(VERSION 3.24)
project(UsdIO CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "usd-io is an Emscripten-only module; configure with emcmake")
endif()

set(USD_INSTALL "${CMAKE_CURRENT_SOURCE_DIR}/deps/openusd_wasm" CACHE PATH "OpenUSD wasm install prefix")
set(TBB_INSTALL "${CMAKE_CURRENT_SOURCE_DIR}/deps/tbb_wasm" CACHE PATH "oneTBB wasm install prefix")

list(APPEND CMAKE_PREFIX_PATH "${USD_INSTALL}" "${TBB_INSTALL}")
list(APPEND CMAKE_FIND_ROOT_PATH "${USD_INSTALL}" "${TBB_INSTALL}")

find_package(pxr REQUIRED CONFIG)

add_executable(usd-io src/usd_io.cpp)
# -fexceptions matches USD's own wasm defaults (ProjectDefaults.cmake): USD
# throws through its error machinery, so the consumer must enable catching.
target_compile_options(usd-io PRIVATE -pthread --use-port=zlib -fexceptions)

# Whole-archive is essential under wasm: USD registers schema types and file
# format plugins via TF_REGISTRY_FUNCTION objects nothing references, and the
# linker strips them from static archives otherwise (see the comment in USD's
# own extras/usd/examples/wasmFetchResolver/CMakeLists.txt).
set(PXR_WHOLE_ARCHIVE_LIBS
    arch tf gf js trace vt work plug kind ts ar sdf pcp sdr usd usdGeom usdShade usdUtils)
target_link_libraries(usd-io PRIVATE
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,${PXR_WHOLE_ARCHIVE_LIBS}>")

set(EXPORTED_FUNCS
    _usdio_last_error
    _usdio_export_begin
    _usdio_export_add_mesh
    _usdio_export_end
    _usdio_free_buffer
    _usdio_import
    _usdio_scene_mesh_count
    _usdio_scene_mesh_name
    _usdio_mesh_num_points
    _usdio_mesh_points
    _usdio_mesh_num_faces
    _usdio_mesh_counts
    _usdio_mesh_num_indices
    _usdio_mesh_indices
    _usdio_mesh_num_uvs
    _usdio_mesh_uvs
    _usdio_mesh_texture_size
    _usdio_mesh_texture
    _usdio_mesh_material
    _usdio_mesh_normal_texture_size
    _usdio_mesh_normal_texture
    _usdio_scene_free
    _malloc
    _free
)
list(JOIN EXPORTED_FUNCS "," EXPORTED_FUNCS_STR)

# USD's plugInfo.json resources must exist in the virtual FS at /usd or the
# usda file format plugin never registers (embedding path verified against
# the pxrConfig install).
target_link_options(usd-io PRIVATE
    -pthread
    --use-port=zlib
    -fexceptions
    "-sPTHREAD_POOL_SIZE=4"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sINITIAL_MEMORY=134217728"
    "-sMODULARIZE=1"
    "-sEXPORT_NAME=createUsdIo"
    "-sENVIRONMENT=web,worker,node"
    "-sEXPORTED_FUNCTIONS=${EXPORTED_FUNCS_STR}"
    "-sEXPORTED_RUNTIME_METHODS=ccall,cwrap,UTF8ToString,HEAPF32,HEAP32,HEAPU8,FS,ENV"
    "--embed-file=${USD_INSTALL}/lib/usd@/usd"
    "--no-entry"
)

option(USDIO_DIAG "Link with assertions + wasm function names for debugging" OFF)
if(USDIO_DIAG)
    target_link_options(usd-io PRIVATE "-sASSERTIONS=2" "--profiling-funcs")
endif()
set_target_properties(usd-io PROPERTIES SUFFIX ".mjs")
