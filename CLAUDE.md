# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

MeshMaker is a 3D mesh modeling application with a C++17/WebGL2 core compiled to WASM via Emscripten, and a React 19 + TypeScript frontend. It also supports native desktop builds (OpenGL 3.3). The active development branch is `main`.

## Build Commands

### C++ WASM Build (WebGL2)
```bash
cd WebGL2
cmake --preset webgl            # configure release build (output: build-wasm/)
cmake --preset webgl-debug      # configure debug build with DWARF symbols (output: build-wasm-debug/)
ninja -C build-wasm             # build release
ninja -C build-wasm-debug       # build debug
```

### C++ Desktop Build (OpenGL 3.3)
```bash
cd WebGL2
cmake --preset desktop-debug    # configure (output: build/)
cmake --preset desktop-release
ninja -C build                  # build debug
```

### React Frontend
```bash
cd MeshMakerWeb
npm install
npm run copy-wasm               # copies WASM artifacts from WebGL2/build-wasm → public/wasm/
npm run copy-wasm-debug         # copies from build-wasm-debug instead
npm run dev                     # Vite dev server at http://localhost:5173
npm run build                   # tsc -b && vite build → dist/
npm run lint                    # ESLint (flat config, TypeScript + React)
```

### Testing
```bash
# C++ unit tests (Google Test, desktop build only)
cd WebGL2/build && ctest --output-on-failure
# or use preset: ctest --preset desktop-debug

# E2E tests (Playwright, Chromium only, serial execution)
cd MeshMakerWeb
npx playwright test                    # run all
npx playwright test e2e/meshmaker.spec.ts  # run single file
npx playwright test --ui               # interactive UI mode
```

### Deployment
```bash
cd MeshMakerWeb && npx wrangler pages deploy dist --project-name=meshmaker
```

## Architecture

### Two-Layer System
- **WebGL2/src/**: C++ engine (~22K lines) compiled to WASM. Handles mesh topology, rendering, selection, transforms, undo/redo, UV unwrapping, and file I/O.
- **MeshMakerWeb/src/**: React frontend (~4K lines). Provides UI chrome, state polling, scripting editor, and AI generation dialog.

### C++ ↔ React Communication
- `WebGL2/src/Bindings.cpp` exposes ~100+ functions to JavaScript via Emscripten embind.
- React calls these directly on the WASM module (e.g., `module.addCube()`, `module.undo()`).
- React polls C++ state every 100ms in `App.tsx` via `useEffect` (selection counts, transform values, undo availability).
- TypeScript types for the WASM module are in `MeshMakerWeb/src/types/meshmaker.d.ts`.

### Key C++ Classes
- **Mesh2** (`Mesh2.cpp/h`): Core half-edge-like mesh topology — vertices, edges, faces (tris/quads), UV faces.
- **ItemCollection** (`ItemCollection.cpp`): Manages multiple mesh items, selection state, undo stack.
- **Manipulator** (`Manipulator.cpp`): 3D transform gizmo (translate/rotate/scale) with color-buffer picking.
- **Serialization** (`Serialization.cpp`): OBJ and GLB (glTF 2.0 binary) import/export with phased progress reporting.
- **OpenSubdivHelper** (`OpenSubdivHelper.cpp`): Catmull-Clark subdivision via Pixar's OpenSubdiv library.

### Key React Components
- **App.tsx**: Main layout, WASM module loading via `useMeshMaker()` hook, state polling loop.
- **TopToolbar.tsx**: Edit modes, transform tools, primitives, mesh operations, file I/O, AI generate button.
- **BottomPanel.tsx**: Transform value inputs, view mode, texture loading, mesh operations.
- **ScriptEditor.tsx**: Monaco editor with ~150 scripting API functions; keyboard isolation prevents Emscripten from capturing keystrokes.

### Selection System
Uses color-buffer rendering (render to offscreen framebuffer, read back pixel color to identify clicked element). Supports item, vertex, triangle, and edge selection modes with edge loop/ring/grow expansion.

### UV System
Projection unwrap (box, planar, cylindrical, spherical) and LSCM seam-based unwrap with conjugate gradient solver. Split-view mode renders 3D viewport and UV editor side-by-side on a single canvas.

### Keyboard Isolation
The `useMeshMaker` hook wraps Emscripten's keyboard handlers so that when Monaco editor or input fields are focused, keystrokes are not captured by the C++ event loop.

## Adding New Features

- **New mesh operation**: Implement in `Mesh2` class, expose via `Bindings.cpp` embind, add UI in React component.
- **New UI component**: Create in `MeshMakerWeb/src/components/`, wire into `App.tsx`.
- **New C++ test**: Add to `WebGL2/tests/` — CMake auto-discovers test files.
- **New E2E test**: Add `.spec.ts` file in `MeshMakerWeb/e2e/`.
- **New scripting function**: Add to the `SCRIPTING_FUNCTIONS[]` array in `scriptingApi.ts` and expose the underlying WASM binding.

## Build Dependencies

- **Emscripten SDK** (EMSDK) at `C:/emsdk` for WASM builds
- **LLVM/Clang** at `C:/Program Files/LLVM/bin/` for desktop builds
- **Ninja** build system
- **CMake** 3.21+
- C++ dependencies fetched automatically via CMake FetchContent: GLM, GLFW, GLAD, Dear ImGui (docking), OpenSubdiv v3_6_0, GoogleTest
