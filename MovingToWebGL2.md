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
- Edge loop selection (expands selection through quad loops)
- Edge ring selection (expands selection perpendicular through quads)

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

**Manipulator System:**
- Translate, rotate, scale gizmos with axis widgets
- Axis picking via color-buffer selection
- Dragging uses ray-plane intersection (works on both Desktop and WebGL2)
- Rotation sphere backdrop for better 3D visualization
- All manipulators work identically on Desktop OpenGL and WebGL2

**Keyboard Shortcuts:**
- Matches original MeshMaker (W/E/R for tools, 1/2/3/4 for edit modes, etc.)
- Smart focus detection: shortcuts disabled when typing in UI text inputs

**Undo/Redo System:**
- Full undo/redo for item transformations (translate, rotate, scale)
- Full undo/redo for mesh operations (extrude, subdivide, merge, etc.)
- State capture and restore for both items and mesh topology

**React Frontend (MeshMakerWeb):**
- Vite + React 19 + TypeScript + Tailwind CSS
- WASM module integration via Embind API
- HiDPI canvas support with proper devicePixelRatio handling
- **Toolbar:** Edit mode buttons, transform mode buttons, primitive creation (with configurable mesh steps), view mode selector
- **Properties Panel:** Transform inputs (X/Y/Z) that show Position/Rotation/Scale based on active tool mode, Duplicate/Delete buttons
- Unified selection value API matching original macOS/Windows behavior

**File I/O (Serialization):**
- OBJ format export/import (Wavefront Object)
- GLB format export/import (Binary glTF 2.0)
- File import via file picker (drag & drop ready)
- File export via browser download

**UV Mapping System:**
- Per-face UV coordinate storage (supports UV seams)
- UV projection unwrap algorithms: Box, Planar, Cylindrical, Spherical
- **Seam-based conformal unwrap (LSCM):** Least Squares Conformal Mapping with union-find vertex splitting for correct seam handling, conjugate gradient solver, BFS island detection, shelf-based island packing
- **UV Editor:** Split-screen 3D/UV viewport, pan/zoom navigation, face selection in UV space (click and rectangle), checkerboard background with texture preview
- UV Editor shaders: dedicated vertex/fragment shaders for UV grid, faces, colored edges, background, and selection picking
- Selected faces highlighted in UV view
- UV transformation API: translate, rotate, scale
- **Seam Marking:** Mark/clear edges as seams for UV unwrapping (Edges edit mode)
- **Seam Visualization:** Seam edges rendered in green in both 3D viewport and UV editor
- UV coordinates preserved through triangulation and undo/redo

**Testing:**
- 115 Google Tests (C++) covering mesh operations, transforms, UV mapping, seam marking, LSCM unwrapping, and cross-unwrap verification
- 22 Playwright E2E tests covering app loading, primitives, edit modes, UV editor, seam workflow, undo/redo, and scene operations
- Test infrastructure: GPU operations disabled for unit tests, Playwright configured with Chromium + dev server

**Deployment:**
- Cloudflare Pages deployment via Wrangler CLI
- VS Code task chains for build → copy WASM → deploy workflow

### Remaining Work 🚧

**High Priority:**
- [x] File save/load (document serialization) ✅ OBJ and GLB formats
- [x] Edge loop/ring selection ✅ UI buttons in Edges mode

**Medium Priority:**
- [x] UV mapping ✅ Projection unwrap + UV Editor panel
- [x] UV Editor: vertex selection and manipulation ✅ Click to select, drag to move
- [x] Seam marking on edges for advanced unwrapping ✅ Mark/Clear buttons in UV Editor
- [x] Seam-based conformal UV unwrap (LSCM) ✅ Unwrap button in UV Editor
- [ ] Texture support (load and display textures on meshes)
- [ ] Texture paint tool
- [ ] Material/color per face

**Low Priority / Polish:**
- [ ] Soft selection / falloff
- [ ] Snap to grid
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

### WASM (Emscripten via CMake presets)

```bash
cd WebGL2
cmake --preset webgl-release    # or webgl-debug
ninja -C build-wasm             # or build-wasm-debug
```

### React Frontend (MeshMakerWeb)

```bash
cd MeshMakerWeb
npm run copy-wasm               # Copy WASM build output
npm run dev                     # Start dev server on port 5173
npm run build                   # Production build
```

### Running Tests

```bash
# C++ unit tests (from WebGL2/)
cd WebGL2/build
ctest --output-on-failure

# Playwright E2E tests (from MeshMakerWeb/)
cd MeshMakerWeb
npx playwright test --reporter=line
```

### Deploy to Cloudflare Pages

```bash
cd MeshMakerWeb
npx wrangler pages deploy dist --project-name=meshmaker --branch=main
```
