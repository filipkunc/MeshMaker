# usd-io

Lazy-loaded WebAssembly module giving MeshMakerWeb real OpenUSD import/export.
Lives beside the main editor module (`MeshMakerWebGL2.wasm`) as a **separate**
Emscripten build with its own memory; mesh data crosses as flat typed arrays
through a narrow C API (`src/usd_io.cpp`). The point: USD's
`faceVertexCounts`/`faceVertexIndices` preserve MeshMaker's mixed quad/tri
topology, which the `.glb` exporter destroys by triangulating. Face-varying
UVs are stored as the standard `primvars:st`; textured exports use USDZ so the
`UsdPreviewSurface` material and PNG bitmap travel in one browser download.

## Building

Requires emsdk (`~/emsdk`), CMake ≥ 3.24, Ninja.

```bash
./deps/build-tbb.sh    # oneTBB v2021.12.0 → deps/tbb_wasm
./deps/build-usd.sh    # OpenUSD (USD_TAG=v26.08) → deps/openusd_wasm
emcmake cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
node test/roundtrip.test.mjs
```

The test imports the checked-in `test/models/mixed-topology.usda` and `.usdc`
fixtures as well as exercising fresh writes and round trips in both encodings.
Regenerate the models after an intentional writer change with
`node test/generate-models.mjs`.

Both builds follow the WebAssembly section of OpenUSD's BUILDING.md:
`-pthread` everywhere (required — TBB), `--use-port=zlib`, static libs,
imaging/python/GL off.

## The four things that make USD-wasm actually work

Each of these failed silently or fatally before being fixed; all are wired
into CMakeLists.txt / usd_io.cpp already:

1. **Plugin registration**: the wasm build has no compiled-in plugin search
   path and env vars don't reach libc in time, so `usd_io.cpp` calls
   `PlugRegistry::RegisterPlugins("/usd/")` before first use. The resource
   tree gets there via `--embed-file <install>/lib/usd@/usd`.
2. **Whole-archive linking**: USD registers schemas and file formats through
   `TF_REGISTRY_FUNCTION` objects nothing references; without
   `$<LINK_LIBRARY:WHOLE_ARCHIVE,...>` the linker strips them (symptoms:
   "unregistered C++ type pxr_half::half", "Failed to create primSpec").
3. **`-fexceptions` on compile and link**: USD throws through its error
   machinery using Emscripten JS exception handling; a consumer built
   without it aborts with "exception catching is not enabled".
4. **Diagnostics delegate**: USD fatals write their reason only to a crash
   log MEMFS can't create ("Cannot create a log file", exit 134, reason
   lost). `usd_io.cpp` installs a `TfDiagnosticMgr::Delegate` that prints
   everything to stderr instead.

## MeshMakerWeb integration

`MeshMakerWeb/src/lib/usdIo.ts` lazy-loads the module (native dynamic
`import()` hidden from Vite via `new Function` — public/ assets can't go
through Vite's module pipeline) and bridges the editor's embind API:
export walks items with the per-face API and sends local points + item TRS
(authored as translate/rotateZYX/scale xformOps — rotateZYX matches
`glm::quat(euler)` = Rx*Ry*Rz); import creates items via
addCube/clearMesh/addMeshVertex/addMeshQuad/addMeshTriangle/rebuildMesh with
world-baked Y-up points. Imported USDZ textures are decoded by the editor and
assigned to their corresponding items. `npm run copy-usd-io` syncs artifacts to
`public/usd-io/`. E2E coverage in `e2e/usd-io.spec.ts`.

## Deployment note

`-pthread` means shared `WebAssembly.Memory`, which browsers require
cross-origin isolation for. Headers (set in vite.config.ts for dev/preview,
public/_headers, and needed in the site server for /meshmaker/*):

    Cross-Origin-Opener-Policy: same-origin
    Cross-Origin-Embedder-Policy: credentialless

`credentialless`, NOT `require-corp`: Monaco loads from a CDN and
`require-corp` blocks it (broke the script-editor e2e tests).

## Known gaps (follow-ups)

- USDA/USDC texture references are external files, which the browser's
  single-file import picker cannot resolve. Use USDZ for textured scenes.
- `UsdPreviewSurface` base color, opacity, metallic, roughness, emissive color,
  clearcoat, clearcoat roughness, IOR, diffuse bitmap, and tangent-space normal
  map are mapped. Arbitrary MaterialX networks are not yet represented by
  MeshMaker's item material model.
- Item transforms survive USD as xformOps but import bakes them into points.
