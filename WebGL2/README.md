# MeshMaker WebGL2

Modern OpenGL 3.3 / WebGL2 port of MeshMaker using pure C++.

## Features

- Cross-platform: Windows, macOS, Linux, and Web (via Emscripten)
- Modern OpenGL 3.3 Core Profile (compatible with WebGL2)
- Dear ImGui (docking branch) for UI
- GLM for mathematics
- Primitive mesh generation: Plane, Cube, Cylinder, Sphere, Icosahedron

## Dependencies

### System Dependencies

- **CMake** 3.16+
- **GLFW** 3.3+ (via package manager or vcpkg)

### Bundled Dependencies (in `external/`)

See [external/README.md](external/README.md) for setup instructions:
- GLAD (OpenGL loader)
- GLM (math library)
- Dear ImGui (docking branch)

## Building

### Desktop (Windows)

```powershell
# Install GLFW via vcpkg
vcpkg install glfw3:x64-windows

# Configure and build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

### Desktop (Linux)

```bash
# Install dependencies
sudo apt install cmake libglfw3-dev

# Configure and build
cmake -B build
cmake --build build

# Optional: verify context creation, shaders, GPU buffers, and one rendered frame.
./build/MeshMakerWebGL2 --smoke-test
```

### Desktop (macOS)

```bash
# Install dependencies
brew install cmake glfw

# Configure and build
cmake -B build
cmake --build build
```

### Web (Emscripten)

```bash
# Ensure Emscripten SDK is installed and activated
source /path/to/emsdk/emsdk_env.sh

# Configure and build
cmake --preset webgl
cmake --build --preset webgl

# Copy into and run the React frontend
cd ../MeshMakerWeb
npm install
npm run copy-wasm
npm run dev
```

See [`../usd-io/README.md`](../usd-io/README.md) to enable USDA, USDC, and
USDZ import/export in the web app.

## Controls

- **Left Mouse Drag**: Orbit camera
- **Middle Mouse Drag**: Pan camera
- **Scroll Wheel**: Zoom in/out

## Project Structure

```
WebGL2/
├── CMakeLists.txt          # Main build configuration
├── cmake/
│   └── Emscripten.cmake    # Emscripten helper
├── external/               # Bundled dependencies
│   ├── glad/              # OpenGL loader (desktop only)
│   ├── glm/               # Math library
│   └── imgui/             # Dear ImGui (docking branch)
├── shaders/
│   ├── mesh.vert          # Mesh vertex shader
│   ├── mesh.frag          # Mesh fragment shader (Phong lighting)
│   ├── grid.vert          # Grid vertex shader
│   └── grid.frag          # Grid fragment shader
└── src/
    ├── main.cpp           # Application entry point
    ├── Camera.h/cpp       # Orbit camera
    ├── Mesh.h/cpp         # Mesh generation and rendering
    ├── Grid.h/cpp         # Grid rendering
    ├── Shader.h/cpp       # Shader loading and uniform management
    └── Vertex.h           # Vertex data structures
```

## License

See [LICENSE.TXT](../LICENSE.TXT) in the root directory.
