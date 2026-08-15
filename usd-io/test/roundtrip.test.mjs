// Round-trip tests for the v2 usd-io API:
//  1. two meshes (quad cube with a transform + single triangle) -> usda -> back
//  2. same scene -> usdc (binary) -> back
//  3. hand-written Z-up usda with a pentagon: up-axis fix + fan triangulation
// Run: node test/roundtrip.test.mjs (needs the wasm build in build/)
import assert from 'node:assert/strict';
import { readFile } from 'node:fs/promises';
import createUsdIo from '../build/usd-io.mjs';

const mod = await createUsdIo();

function alloc(arr) {
  const ptr = mod._malloc(arr.byteLength);
  (arr instanceof Float32Array ? mod.HEAPF32 : mod.HEAP32).set(arr, ptr / 4);
  return ptr;
}
function allocBytes(bytes) {
  const ptr = mod._malloc(bytes.byteLength);
  mod.HEAPU8.set(bytes, ptr);
  return ptr;
}
function cstr(s) {
  return allocBytes(new TextEncoder().encode(s + '\0'));
}

const cube = {
  points: new Float32Array([
    -1, -1, -1,   1, -1, -1,   1,  1, -1,  -1,  1, -1,
    -1, -1,  1,   1, -1,  1,   1,  1,  1,  -1,  1,  1,
  ]),
  counts: new Int32Array([4, 4, 4, 4, 4, 4]),
  indices: new Int32Array([
    0, 3, 2, 1,  4, 5, 6, 7,  0, 1, 5, 4,
    2, 3, 7, 6,  0, 4, 7, 3,  1, 2, 6, 5,
  ]),
};
const tri = {
  points: new Float32Array([0, 0, 0, 1, 0, 0, 0, 1, 0]),
  counts: new Int32Array([3]),
  indices: new Int32Array([0, 1, 2]),
};

function addMesh(exp, name, mesh, translate, rotate, scale) {
  const pName = cstr(name);
  const pPts = alloc(mesh.points), pCounts = alloc(mesh.counts), pIdx = alloc(mesh.indices);
  const pT = alloc(new Float32Array(translate));
  const pR = alloc(new Float32Array(rotate));
  const pS = alloc(new Float32Array(scale));
  const uv = new Float32Array(mesh.indices.length * 2);
  for (let i = 0; i < mesh.indices.length; i++) {
    uv[i * 2] = i % 2;
    uv[i * 2 + 1] = (i >> 1) % 2;
  }
  const pUv = alloc(uv);
  const pMaterial = alloc(new Float32Array(
    [1, 1, 1, 1, 0, 0.4, 0, 0, 0, 0, 0.01, 1.5]));
  mod._usdio_export_add_mesh(exp, pName,
    pPts, mesh.points.length / 3, pCounts, mesh.counts.length, pIdx, mesh.indices.length,
    pT, pR, pS, pMaterial, pUv, mesh.indices.length, 0, 0, 0, 0, 0, 0);
  [pName, pPts, pCounts, pIdx, pT, pR, pS, pMaterial, pUv].forEach((p) => mod._free(p));
}

function exportScene(format) {
  const exp = mod._usdio_export_begin();
  assert.notEqual(exp, 0, 'export_begin');
  addMesh(exp, 'Cube', cube, [2, 0, 0], [0, 0, 0], [1, 1, 1]);
  addMesh(exp, 'Tri', tri, [0, 0, 0], [0, 0, 0], [1, 1, 1]);
  const pLen = mod._malloc(4);
  const pData = mod._usdio_export_end(exp, format, pLen);
  assert.notEqual(pData, 0, `export_end format=${format}`);
  const len = mod.HEAP32[pLen / 4];
  const bytes = new Uint8Array(mod.HEAPU8.buffer, pData, len).slice();
  mod._usdio_free_buffer(pData);
  mod._free(pLen);
  return bytes;
}

function importScene(bytes, ext) {
  const pBytes = allocBytes(bytes);
  const pExt = cstr(ext);
  const h = mod._usdio_import(pBytes, bytes.byteLength, pExt);
  mod._free(pBytes); mod._free(pExt);
  if (!h) throw new Error('import failed: ' + mod.UTF8ToString(mod._usdio_last_error()));
  const meshes = [];
  const n = mod._usdio_scene_mesh_count(h);
  for (let i = 0; i < n; i++) {
    const numPts = mod._usdio_mesh_num_points(h, i);
    const numFaces = mod._usdio_mesh_num_faces(h, i);
    const numIdx = mod._usdio_mesh_num_indices(h, i);
    meshes.push({
      name: mod.UTF8ToString(mod._usdio_scene_mesh_name(h, i)),
      points: new Float32Array(mod.HEAPF32.buffer, mod._usdio_mesh_points(h, i), numPts * 3).slice(),
      counts: new Int32Array(mod.HEAP32.buffer, mod._usdio_mesh_counts(h, i), numFaces).slice(),
      indices: new Int32Array(mod.HEAP32.buffer, mod._usdio_mesh_indices(h, i), numIdx).slice(),
      uvs: new Float32Array(mod.HEAPF32.buffer, mod._usdio_mesh_uvs(h, i),
        mod._usdio_mesh_num_uvs(h, i) * 2).slice(),
      texture: new Uint8Array(mod.HEAPU8.buffer, mod._usdio_mesh_texture(h, i),
        mod._usdio_mesh_texture_size(h, i)).slice(),
      material: new Float32Array(mod.HEAPF32.buffer,
        mod._usdio_mesh_material(h, i), 12).slice(),
      normalTexture: new Uint8Array(mod.HEAPU8.buffer,
        mod._usdio_mesh_normal_texture(h, i),
        mod._usdio_mesh_normal_texture_size(h, i)).slice(),
    });
  }
  mod._usdio_scene_free(h);
  return meshes;
}

function checkScene(meshes, label) {
  assert.equal(meshes.length, 2, `${label}: two meshes`);
  const c = meshes.find((m) => m.name.startsWith('Cube'));
  const t = meshes.find((m) => m.name.startsWith('Tri'));
  assert.ok(c && t, `${label}: names survive`);
  assert.deepEqual([...c.counts], [4, 4, 4, 4, 4, 4], `${label}: quads preserved`);
  assert.deepEqual([...t.counts], [3], `${label}: triangle preserved`);
  // translate (2,0,0) baked into world-space points
  assert.equal(c.points[0], 1, `${label}: cube transform baked (-1 + 2)`);
  assert.deepEqual([...c.indices], [...cube.indices], `${label}: cube indices`);
  assert.equal(c.uvs.length, c.indices.length * 2, `${label}: face-varying UVs`);
}

// 1. usda round trip
const usdaBytes = exportScene(0);
const usdaText = new TextDecoder().decode(usdaBytes);
assert.match(usdaText, /#usda 1.0/);
assert.match(usdaText, /def Mesh "Cube_0"/);
assert.match(usdaText, /faceVertexCounts = \[4, 4, 4, 4, 4, 4\]/);
assert.match(usdaText, /double3 xformOp:translate = \(2, 0, 0\)/);
assert.match(usdaText, /subdivisionScheme = "none"/);
checkScene(importScene(usdaBytes, 'usda'), 'usda');
console.log('OK: usda round trip (2 meshes, quads + transform intact)');

// 2. usdc (binary) round trip
const usdcBytes = exportScene(1);
assert.equal(new TextDecoder().decode(usdcBytes.slice(0, 8)), 'PXR-USDC', 'usdc magic');
checkScene(importScene(usdcBytes, 'usdc'), 'usdc');
console.log(`OK: usdc round trip (${usdcBytes.length} bytes, magic PXR-USDC)`);

// Checked-in models make import coverage independent from the writer. Keep a
// text and binary encoding of the same mixed-topology, transformed scene.
for (const ext of ['usda', 'usdc']) {
  const fixture = await readFile(new URL(`./models/mixed-topology.${ext}`, import.meta.url));
  if (ext === 'usdc')
    assert.equal(new TextDecoder().decode(fixture.subarray(0, 8)), 'PXR-USDC');
  checkScene(importScene(fixture, ext), `${ext} fixture`);
  console.log(`OK: ${ext} fixture imported`);
}

const texturedUsdz = await readFile(new URL('./models/textured.usdz', import.meta.url));
assert.equal(new TextDecoder().decode(texturedUsdz.subarray(0, 2)), 'PK', 'usdz zip magic');
const textured = importScene(texturedUsdz, 'usdz');
checkScene(textured, 'usdz fixture');
assert.ok(textured.find((mesh) => mesh.name.startsWith('Cube')).texture.length > 0,
  'USDZ texture bytes imported');
assert.ok(Math.abs(textured.find((mesh) => mesh.name.startsWith('Cube')).material[4] - 0.6) < 1e-5,
  'USDZ metallic imported');
assert.ok(textured.find((mesh) => mesh.name.startsWith('Cube')).normalTexture.length > 0,
  'USDZ normal texture bytes imported');
assert.deepEqual(
  [...textured.find((mesh) => mesh.name.startsWith('Cube')).material].map((v) => +v.toFixed(4)),
  [0.8, 0.2, 0.1, 0.75, 0.6, 0.3, 0.05, 0.1, 0.2, 0.7, 0.15, 1.45],
  'USDZ PreviewSurface scalar and color inputs imported');
console.log('OK: textured usdz fixture imported with UVs and bitmap');

// Z-up stage with a pentagon (n-gon): up-axis fix + fan triangulation
const zUpPentagon = `#usda 1.0
(
    upAxis = "Z"
)
def Mesh "Pent"
{
    int[] faceVertexCounts = [5]
    int[] faceVertexIndices = [0, 1, 2, 3, 4]
    point3f[] points = [(0, 0, 0), (1, 0, 0), (1.3, 0, 1), (0.5, 0, 1.8), (-0.3, 0, 1)]
}
`;
const pent = importScene(new TextEncoder().encode(zUpPentagon), 'usda');
assert.equal(pent.length, 1);
assert.deepEqual([...pent[0].counts], [3, 3, 3], 'pentagon fan-triangulated');
assert.deepEqual([...pent[0].indices], [0, 1, 2, 0, 2, 3, 0, 3, 4], 'fan order');
// Z-up (x, y, z) -> Y-up (x, z, -y): point (1, 0, 0) stays, (1.3, 0, 1) -> (1.3, 1, 0)
assert.equal(pent[0].points[7], 1, 'z became y');
assert.ok(Math.abs(pent[0].points[8]) < 1e-6, '-y became z (0)');
console.log('OK: Z-up pentagon imported as Y-up triangles');

console.log('\nAll usd-io round-trip tests passed');
