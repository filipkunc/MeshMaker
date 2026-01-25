# Plan to move to WebGL2

## Original state

Original codebase was working on Windows and contained C++/CLI code and C# code.
The C++/CLI code was in MeshMakerCppCLI project with Classes using confusingly the `.mm` extension from Objective-C++, but actually being compiled only for Windows C++/CLI.

## Road to WebGL2

Easiest would be to switch the code to pure C++ and OpenGL profile closest to WebGL2 with CMake project. This would allow to do a separate second step using Emscripten as a compiler target which would finally be using the WebGL2.
The steps would need to be incremental to reduce the complexity of the port and also to verify each stage to make sure it works.

My recommendation would be to make a separate folder with the CMake setup and clean room setup code using the OpenGL/WebGL2 and then incrementally port the code there.

## Current Progress (webgl2-port branch)

### Completed ✅

**Project Setup:**
- Created `WebGL2/` folder with CMake-based build system
- Dual build configuration:
  - Desktop: OpenGL 3.3 core profile via GLAD + GLFW
  - Web: WebGL2 (OpenGL ES 3.0) via Emscripten
- Dependencies managed via CMake FetchContent (glm, glfw)
- ImGui integration for UI panels
- VS Code tasks and launch configurations for both Desktop and WebGL builds

**Core Architecture:**
- `Mesh2` class with full topology (vertices, edges, faces supporting triangles and quads)
- `Item` class wrapping mesh with transform (position, rotation, scale)
- `ItemCollection` managing multiple items with edit modes
- Selection modes: Items, Vertices, Edges, Triangles
- `Manipulator` gizmo for translate/rotate/scale operations

**Rendering:**
- Shader system with GLSL ES 3.0 compatible shaders
- Solid, wireframe, and solid+wireframe view modes
- Wireframe overlay only for selected items (matching original behavior)
- Grid rendering with model transform support
- Cross-platform point size via `gl_PointSize` in vertex shaders
- `GL_PROGRAM_POINT_SIZE` enabled on Desktop for shader-controlled point sizes

**Selection System:**
- Color-buffer picking for all selection modes
- Point-click and rectangle selection
- Proper vertex/edge/triangle selection (not always selecting faces)
- Selection colors: red for selected vertices, blue for deselected

**Camera:**
- Orbit camera with mouse controls
- Zoom via scroll wheel
- Perspective and orthographic projections

**Mesh Operations:**
- Primitive generation: Cube, Plane, Cylinder, Sphere, Icosahedron
- Transform selected components (translate, rotate, scale)
- Flip, duplicate, delete selected faces
- Subdivide, triangulate, extrude operations
- Edge split, vertex merge

**Keyboard Shortcuts:**
- Matches original MeshMaker (W/E/R for tools, 1/2/3/4 for edit modes, etc.)

### Remaining Work 🚧

**High Priority:**
- [ ] Undo/redo system
- [ ] File save/load (document serialization)
- [ ] Test all mesh operations thoroughly
- [ ] Manipulator axis picking and dragging

**Medium Priority:**
- [ ] Texture support and UV mapping
- [ ] Texture paint tool
- [ ] Material/color per face
- [ ] More robust edge loop/ring selection

**Low Priority / Polish:**
- [ ] Soft selection / falloff
- [ ] Snap to grid
- [ ] Duplicate items properly (deep mesh copy)
- [ ] Status bar with selection info
- [ ] Preferences/settings persistence

**WebGL2 Specific:**
- [ ] Test thoroughly in browsers (Chrome, Firefox, Safari)
- [ ] Handle WebGL context loss/restore
- [ ] Touch input support for mobile
- [ ] Performance optimization for larger meshes

## Build Instructions

### Desktop (Windows)

```bash
cd WebGL2
cmake -B build -S .
cmake --build build --config Debug
# Run: build/Debug/MeshMakerWebGL2.exe
```

### WebGL2 (Emscripten)

```bash
cd WebGL2
mkdir build_web && cd build_web
emcmake cmake ..
emmake cmake --build .
emrun MeshMakerWebGL2.html
```
