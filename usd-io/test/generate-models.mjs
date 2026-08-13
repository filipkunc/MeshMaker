// Regenerate the checked-in OpenUSD test models from the same public C API
// used by MeshMakerWeb. Run after building usd-io:
//   node test/generate-models.mjs
import { mkdir, writeFile } from 'node:fs/promises';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import createUsdIo from '../build/usd-io.mjs';

const mod = await createUsdIo();
const modelDir = join(dirname(fileURLToPath(import.meta.url)), 'models');

function alloc(array) {
  const ptr = mod._malloc(array.byteLength);
  (array instanceof Float32Array ? mod.HEAPF32 : mod.HEAP32).set(array, ptr / 4);
  return ptr;
}

function cstr(value) {
  const bytes = new TextEncoder().encode(`${value}\0`);
  const ptr = mod._malloc(bytes.byteLength);
  mod.HEAPU8.set(bytes, ptr);
  return ptr;
}

function addMesh(exporter, name, points, counts, indices, translate) {
  const values = [
    cstr(name), alloc(new Float32Array(points)), alloc(new Int32Array(counts)),
    alloc(new Int32Array(indices)), alloc(new Float32Array(translate)),
    alloc(new Float32Array([0, 0, 0])), alloc(new Float32Array([1, 1, 1])),
  ];
  mod._usdio_export_add_mesh(exporter, values[0], values[1], points.length / 3,
    values[2], counts.length, values[3], indices.length,
    values[4], values[5], values[6]);
  values.forEach((ptr) => mod._free(ptr));
}

function createModel(format) {
  const exporter = mod._usdio_export_begin();
  addMesh(exporter, 'Cube', [
    -1, -1, -1, 1, -1, -1, 1, 1, -1, -1, 1, -1,
    -1, -1, 1, 1, -1, 1, 1, 1, 1, -1, 1, 1,
  ], [4, 4, 4, 4, 4, 4], [
    0, 3, 2, 1, 4, 5, 6, 7, 0, 1, 5, 4,
    2, 3, 7, 6, 0, 4, 7, 3, 1, 2, 6, 5,
  ], [2, 0, 0]);
  addMesh(exporter, 'Tri', [0, 0, 0, 1, 0, 0, 0, 1, 0],
    [3], [0, 1, 2], [0, 0, 0]);

  const lengthPtr = mod._malloc(4);
  const dataPtr = mod._usdio_export_end(exporter, format, lengthPtr);
  if (!dataPtr) throw new Error(mod.UTF8ToString(mod._usdio_last_error()));
  let bytes = new Uint8Array(
    mod.HEAPU8.buffer, dataPtr, mod.HEAP32[lengthPtr / 4]).slice();
  mod._usdio_free_buffer(dataPtr);
  mod._free(lengthPtr);
  if (format === 0)
    bytes = new TextEncoder().encode(`${new TextDecoder().decode(bytes).trimEnd()}\n`);
  return bytes;
}

await mkdir(modelDir, { recursive: true });
await Promise.all([
  writeFile(join(modelDir, 'mixed-topology.usda'), createModel(0)),
  writeFile(join(modelDir, 'mixed-topology.usdc'), createModel(1)),
]);
console.log(`Wrote OpenUSD test models to ${modelDir}`);
