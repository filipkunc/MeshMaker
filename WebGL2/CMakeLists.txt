cmake_minimum_required(VERSION 3.16)
project(MeshMakerWebGL2 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for clangd and other tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Detect Emscripten
if(EMSCRIPTEN)
    message(STATUS "Building for WebGL2 with Emscripten")
    # Use .js suffix for modular builds (MODULARIZE=1) to allow custom EXPORT_NAME
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    set(USE_WEBGL2 TRUE)
else()
    message(STATUS "Building for Desktop OpenGL 3.3")
    set(USE_WEBGL2 FALSE)
endif()

# ============================================================================
# Fetch Dependencies
# ============================================================================

# GLM (header-only math library)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

# GoogleTest (for unit testing, desktop only)
if(NOT EMSCRIPTEN)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
    )
    # Prevent overriding parent project's compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Dear ImGui (docking branch)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

if(NOT EMSCRIPTEN)
    # GLFW
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(glfw)
    
    # GLAD2 (OpenGL loader with proper CMake support)
    FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG glad2
        GIT_SHALLOW TRUE
        SOURCE_SUBDIR cmake
    )
    FetchContent_MakeAvailable(glad)
    
    # Generate GLAD for OpenGL 3.3 Core
    glad_add_library(glad_gl_core_33 STATIC API gl:core=3.3)
endif()

# ImGui library
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

if(NOT EMSCRIPTEN)
    target_link_libraries(imgui PUBLIC glfw)
endif()

# ============================================================================
# Source files
# ============================================================================

# Core library sources (without main.cpp for testing)
set(CORE_SOURCES
    src/Camera.cpp
    src/Mesh.cpp
    src/Mesh2.cpp
    src/Item.cpp
    src/ItemCollection.cpp
    src/Grid.cpp
    src/Shader.cpp
    src/Manipulator.cpp
)

set(SOURCES
    src/main.cpp
    ${CORE_SOURCES}
)

set(HEADERS
    src/Camera.h
    src/Mesh.h
    src/Mesh2.h
    src/Item.h
    src/ItemCollection.h
    src/Grid.h
    src/Shader.h
    src/Vertex.h
    src/Manipulator.h
)

# ============================================================================
# Main executable
# ============================================================================

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE src)

if(EMSCRIPTEN)
    # Emscripten build
    target_link_libraries(${PROJECT_NAME} PRIVATE glm imgui)
    
    # Emscripten compile flags (these are compile-time only)
    set(EMSCRIPTEN_COMPILE_FLAGS
        "-sUSE_GLFW=3"
    )
    
    # Emscripten link flags (these are link-time only)
    set(EMSCRIPTEN_LINK_FLAGS
        "-sUSE_WEBGL2=1"
        "-sUSE_GLFW=3"
        "-sFULL_ES3=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sWASM=1"
        "-sMODULARIZE=1"
        "-sEXPORT_NAME=createMeshMakerModule"
        "--embed-file=${CMAKE_CURRENT_SOURCE_DIR}/shaders@/shaders"
    )
    
    target_compile_options(${PROJECT_NAME} PRIVATE ${EMSCRIPTEN_COMPILE_FLAGS})
    target_link_options(${PROJECT_NAME} PRIVATE ${EMSCRIPTEN_LINK_FLAGS})
    
    # Define for shader loading path
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        EMSCRIPTEN_BUILD
        IMGUI_IMPL_OPENGL_ES3
    )
else()
    # Desktop build
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        glad_gl_core_33
        glm
        imgui
    )
    
    # Platform-specific libraries
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
    elseif(APPLE)
        find_library(OPENGL_LIBRARY OpenGL)
        find_library(COCOA_LIBRARY Cocoa)
        find_library(IOKIT_LIBRARY IOKit)
        find_library(COREVIDEO_LIBRARY CoreVideo)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ${OPENGL_LIBRARY}
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREVIDEO_LIBRARY}
        )
    else()
        # Linux
        target_link_libraries(${PROJECT_NAME} PRIVATE GL dl pthread)
    endif()
    
    # Copy shaders to build directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    )
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy compile_commands.json to source directory for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMENT "Copying compile_commands.json to source directory for clangd"
    )
endif()

# ============================================================================
# Unit Tests (Desktop only)
# ============================================================================

if(NOT EMSCRIPTEN)
    enable_testing()
    
    # Core library for testing (no graphics dependencies)
    add_library(MeshMakerCore STATIC
        src/Mesh2.cpp
        src/Item.cpp
        src/ItemCollection.cpp
        src/Shader.cpp
    )
    target_include_directories(MeshMakerCore PUBLIC src)
    target_link_libraries(MeshMakerCore PUBLIC glm glad_gl_core_33)
    
    # Mesh2 unit tests
    add_executable(Mesh2Tests tests/Mesh2Tests.cpp)
    target_link_libraries(Mesh2Tests PRIVATE
        MeshMakerCore
        GTest::gtest_main
    )
    target_include_directories(Mesh2Tests PRIVATE src)
    
    include(GoogleTest)
    gtest_discover_tests(Mesh2Tests)
endif()
