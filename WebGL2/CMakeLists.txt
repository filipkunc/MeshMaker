cmake_minimum_required(VERSION 3.16)
project(MeshMakerWebGL2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect Emscripten
if(EMSCRIPTEN)
    message(STATUS "Building for WebGL2 with Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(USE_WEBGL2 TRUE)
else()
    message(STATUS "Building for Desktop OpenGL 3.3")
    set(USE_WEBGL2 FALSE)
endif()

# External dependencies paths
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(GLAD_DIR ${EXTERNAL_DIR}/glad)
set(GLM_DIR ${EXTERNAL_DIR}/glm)
set(IMGUI_DIR ${EXTERNAL_DIR}/imgui)

# GLM (header-only)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${GLM_DIR})

# Source files
set(SOURCES
    src/main.cpp
    src/Camera.cpp
    src/Mesh.cpp
    src/Grid.cpp
    src/Shader.cpp
)

set(HEADERS
    src/Camera.h
    src/Mesh.h
    src/Grid.h
    src/Shader.h
    src/Vertex.h
)

# ImGui sources
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

if(EMSCRIPTEN)
    # Emscripten build
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${IMGUI_SOURCES})
    
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )
    
    target_link_libraries(${PROJECT_NAME} PRIVATE glm)
    
    # Emscripten compile and link flags
    set(EMSCRIPTEN_FLAGS
        "-sUSE_WEBGL2=1"
        "-sUSE_GLFW=3"
        "-sFULL_ES3=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sWASM=1"
        "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/shaders@/shaders"
    )
    
    target_compile_options(${PROJECT_NAME} PRIVATE ${EMSCRIPTEN_FLAGS})
    target_link_options(${PROJECT_NAME} PRIVATE ${EMSCRIPTEN_FLAGS})
    
    # Define for shader loading path
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        EMSCRIPTEN_BUILD
        IMGUI_IMPL_OPENGL_ES3
    )
    
else()
    # Desktop build with GLAD
    add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
    target_include_directories(glad PUBLIC ${GLAD_DIR}/include)
    
    # Find GLFW
    find_package(glfw3 3.3 REQUIRED)
    
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${IMGUI_SOURCES})
    
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${GLAD_DIR}/include
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )
    
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        glad
        glm
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
