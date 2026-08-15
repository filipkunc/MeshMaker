# MeshMaker Web

React and TypeScript frontend for the WebGL2/Emscripten port of MeshMaker.
The editor itself runs in `MeshMakerWebGL2.wasm`; OpenUSD support is supplied
by a separate, lazily loaded `usd-io.wasm` module.

## Development

Build the editor module from `../WebGL2` and, when working with OpenUSD, the
USD module from `../usd-io`. Then:

```bash
npm install
npm run copy-wasm
npm run copy-usd-io
npm run dev
```

The Vite server supplies the cross-origin isolation headers required by the
threaded OpenUSD module. Do not serve `public/` with a server that omits those
headers.

## Useful commands

```bash
npm run build                 # type-check and production build
npm run lint                  # ESLint
npm run test:e2e              # complete Playwright suite
npx playwright test e2e/usd-io.spec.ts
```

OpenUSD test models live in `../usd-io/test/models/`. The textured USDZ model
exercises face-varying UVs, a diffuse texture, a tangent-space normal map, and
the supported `UsdPreviewSurface` values.
