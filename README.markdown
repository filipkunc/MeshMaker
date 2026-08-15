# Welcome to MeshMaker

## License and submodules

MeshMaker is under [MIT license](http://opensource.org/licenses/mit-license.php). You find it in file "LICENSE.TXT". 

MeshMaker uses:

 * [OpenSubdiv library](http://graphics.pixar.com/opensubdiv) from Pixar Animation Studios under [Ms-PL license](http://www.microsoft.com/en-us/openness/licenses.aspx#MPL). Library is slightly modified for this project, modifications are at [https://github.com/filipkunc/OpenSubdiv](https://github.com/filipkunc/OpenSubdiv).
 * [RapidXml](http://rapidxml.sourceforge.net)
 
For working with submodules I recommend reading [Pro Git chapter about submodules](http://git-scm.com/book/en/Git-Tools-Submodules).

## About

MeshMaker is very basic modeling tool focused on low poly modeling with triangles and quads.

## Triangles and Quads

MeshMaker supports triangles and quads in same mesh. There is no support for arbitrary polygons.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/triquads.png" alt="Triangles and Quads" width="745px" height="569px"></img>

## Edge loops

MeshMaker enables edge loop selection with Double Click and edge expand selection with Command Double Click. Combinations of triangle/quad extrusion and expanded edge splitting enables simple and fast  modeling.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/edgeloops.png" alt="Edge loops" width="832px" height="653px"></img>

## Scripting

For scripting is used JavaScript via WebScriptObject and Fragaria editor for editing code. 
Example scripts are in Scripts folder.

All script actions are undoable.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/scripting.png" alt="Scripting" width="833px" height="652px"></img>

## Windows port

MeshMaker is now mostly C++ project with Objective-C++ on Mac part and C++/CLI and C# on Windows part.

<img src="https://github.com/filipkunc/MeshMaker/raw/more_cpp/Screenshots/triquads_win.png" alt="Triangles and Quads" width="715px" height="541px"></img>
 
## Camera manipulation

Similar to Maya, Unity. 

* Rotation - Alt + Left Mouse Button
* Pan - Alt + Middle Mouse Button
* Zoom - Alt + Right Mouse Button

Editor can be used also only with multitouch trackpad (MacBooks) and keyboard.

* Rotation - Alt + Two Fingers
* Pan - Control + Alt + Two Fingers
* Zoom - Two Fingers Zoom

## Selection

 * Normal selection - Left Mouse Button
 * Select through (selects back faces for example) - Control + Left Mouse Button
 * Adding to selection - Shift + Left Mouse Button
 * Inverting selection - Command + Left Mouse Button
 * Soft selection - global mode in Edit menu

## WebGL2 / React Web App

MeshMaker has a WebGL2 port built with Emscripten (C++ to WASM) and a React + TypeScript frontend.

### Prerequisites

- [Node.js](https://nodejs.org/) (v18+)
- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) (for building WASM)
- CMake + Ninja

### Building and Running

```bash
# Activate emsdk first, then build the editor WASM module
cd WebGL2
cmake --preset webgl
cmake --build --preset webgl

# Install the frontend and copy the editor module
cd ../MeshMakerWeb
npm install
npm run copy-wasm

# Start the development server
npm run dev
```

Open the URL printed by Vite (normally `http://localhost:5173`). For OpenUSD
support, first build the separate module as described in
[`usd-io/README.md`](usd-io/README.md), then run `npm run copy-usd-io` in
`MeshMakerWeb`.

The web editor reads and writes OBJ, GLB, USDA, USDC, and USDZ. OpenUSD keeps
mixed triangle/quad topology and supports face-varying UVs, `UsdPreviewSurface`
materials, diffuse textures, and tangent-space normal maps. Use USDZ when a
scene contains textures so the images travel with the model.

### Tests

```bash
# Native C++ suite
cd WebGL2
cmake --preset desktop-debug
cmake --build --preset desktop-debug
ctest --preset desktop-debug

# Browser end-to-end suite
cd ../MeshMakerWeb
npx playwright install chromium
npm run test:e2e
```

### AI 3D Generation (Hunyuan3D-2)

MeshMaker integrates with [Hunyuan3D-2](https://github.com/Tencent-Hunyuan/Hunyuan3D-2) (Tencent, MIT license) for AI-powered image-to-3D and text-to-3D generation. The server runs natively on Windows with CUDA.

#### Requirements

- NVIDIA GPU with **16 GB VRAM** (tested on RTX 5070 Ti)
- CUDA Toolkit 12.8+
- Visual Studio Build Tools (C++ workload)
- [uv](https://docs.astral.sh/uv/) package manager

#### Server Setup

```bash
cd Hunyuan3D-2

# Install Python dependencies
uv sync

# Build native CUDA extensions
.\build_extensions.ps1

# Start the Gradio server (includes both UI and REST API)
uv run python gradio_app.py --model_path tencent/Hunyuan3D-2mini --subfolder hunyuan3d-dit-v2-mini --texgen_model_path tencent/Hunyuan3D-2 --low_vram_mode

# With text-to-3D support
uv run python gradio_app.py --model_path tencent/Hunyuan3D-2mini --subfolder hunyuan3d-dit-v2-mini --texgen_model_path tencent/Hunyuan3D-2 --low_vram_mode --enable_t23d
```

Model weights are downloaded automatically from HuggingFace on first run (~2-3 GB for mini-turbo).

#### Usage

1. Start the Hunyuan3D-2 server (see command above)
2. Run the MeshMaker web app (`npm run dev`)
3. Click the sparkle button in the toolbar
4. Choose **Image to 3D** (upload/drag image) or **Text to 3D** (type a prompt)
5. Click **Generate**
6. The generated mesh imports directly into the viewport

The Gradio UI is also available at `http://localhost:8080` for standalone use.

#### Configuration

The server URL defaults to `http://localhost:8080/api`. To change it, open the AI dialog, expand **Advanced Options**, update the **Hunyuan3D-2 Server URL**, and click **Save**. The setting persists in your browser.

#### API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/send` | POST | Start async generation (`{image?, text?, seed?, texture?, ...}`) |
| `/api/status/{uid}` | GET | Poll task status (returns base64 GLB when completed) |
| `/api/health` | GET | Server health check |
