// Bridge to the lazy-loaded OpenUSD wasm module (public/usd-io/).
//
// usd-io is a second Emscripten module with its own (shared, pthread-backed)
// memory, fully separate from the editor module — mesh data crosses between
// them here as flat typed arrays. It is ~10 MB and only fetched on the first
// USD import/export. Requires cross-origin isolation (COOP/COEP headers):
// the module uses pthreads, so its WebAssembly.Memory is shared.
import type { MeshMakerModule } from '../types/meshmaker';

interface UsdIoModule {
  _malloc(size: number): number;
  _free(ptr: number): void;
  _usdio_last_error(): number;
  _usdio_export_begin(): number;
  _usdio_export_add_mesh(
    exp: number, name: number,
    points: number, numPoints: number,
    counts: number, numFaces: number,
    indices: number, numIndices: number,
    translate: number, rotateDeg: number, scale: number,
  ): void;
  _usdio_export_end(exp: number, format: number, outLen: number): number;
  _usdio_free_buffer(ptr: number): void;
  _usdio_import(bytes: number, len: number, ext: number): number;
  _usdio_scene_mesh_count(scene: number): number;
  _usdio_scene_mesh_name(scene: number, i: number): number;
  _usdio_mesh_num_points(scene: number, i: number): number;
  _usdio_mesh_points(scene: number, i: number): number;
  _usdio_mesh_num_faces(scene: number, i: number): number;
  _usdio_mesh_counts(scene: number, i: number): number;
  _usdio_mesh_num_indices(scene: number, i: number): number;
  _usdio_mesh_indices(scene: number, i: number): number;
  _usdio_scene_free(scene: number): void;
  UTF8ToString(ptr: number): string;
  HEAPF32: Float32Array;
  HEAP32: Int32Array;
  HEAPU8: Uint8Array;
}

export const USD_EXTENSIONS = ['usd', 'usda', 'usdc', 'usdz'] as const;

export function isUsdExtension(ext: string | undefined): boolean {
  return !!ext && (USD_EXTENSIONS as readonly string[]).includes(ext);
}

let modulePromise: Promise<UsdIoModule> | null = null;

export function loadUsdIo(): Promise<UsdIoModule> {
  if (!modulePromise) {
    if (!crossOriginIsolated) {
      return Promise.reject(new Error(
        'USD support needs cross-origin isolation (COOP/COEP headers); ' +
        'this page is not crossOriginIsolated.',
      ));
    }
    const url = `${import.meta.env.BASE_URL}usd-io/usd-io.mjs`;
    // Native dynamic import, hidden from Vite: assets under public/ cannot go
    // through its module pipeline, and the Emscripten glue must load untouched
    // anyway (it spawns pthread workers from its own URL).
    const nativeImport = new Function('u', 'return import(u)') as
      (u: string) => Promise<{ default: () => Promise<UsdIoModule> }>;
    modulePromise = nativeImport(url)
      .then((m) => m.default())
      .catch((err) => {
        modulePromise = null; // allow retry
        throw err;
      });
  }
  return modulePromise;
}

function alloc(usd: UsdIoModule, arr: Float32Array | Int32Array): number {
  const ptr = usd._malloc(arr.byteLength);
  (arr instanceof Float32Array ? usd.HEAPF32 : usd.HEAP32).set(arr, ptr / 4);
  return ptr;
}

function allocBytes(usd: UsdIoModule, bytes: Uint8Array): number {
  const ptr = usd._malloc(bytes.byteLength);
  usd.HEAPU8.set(bytes, ptr);
  return ptr;
}

function allocCString(usd: UsdIoModule, s: string): number {
  return allocBytes(usd, new TextEncoder().encode(s + '\0'));
}

function lastError(usd: UsdIoModule): string {
  return usd.UTF8ToString(usd._usdio_last_error()) || 'unknown USD error';
}

/**
 * Export every item in the scene to USD. Local-space points plus the item's
 * T*R*S transform go across; usd-io authors the transform as USD xformOps
 * (translate / rotateZYX / scale — matching glm's euler convention).
 */
export async function exportUsd(
  mm: MeshMakerModule, format: 'usda' | 'usdc',
): Promise<Uint8Array> {
  const usd = await loadUsdIo();

  const exp = usd._usdio_export_begin();
  if (!exp) throw new Error(lastError(usd));

  const itemCount = mm.getItemCount();
  for (let item = 0; item < itemCount; item++) {
    const vertexCount = mm.getItemVertexCount(item);
    const faceCount = mm.getItemFaceCount(item);
    if (vertexCount === 0 || faceCount === 0) continue;

    const points = new Float32Array(vertexCount * 3);
    for (let v = 0; v < vertexCount; v++) {
      points[v * 3] = mm.getVertexX(item, v);
      points[v * 3 + 1] = mm.getVertexY(item, v);
      points[v * 3 + 2] = mm.getVertexZ(item, v);
    }

    const counts = new Int32Array(faceCount);
    const indices: number[] = [];
    for (let f = 0; f < faceCount; f++) {
      const corners = mm.getFaceVertexCount(item, f);
      counts[f] = corners;
      for (let c = 0; c < corners; c++) {
        indices.push(mm.getFaceVertexIndex(item, f, c));
      }
    }

    const translate = new Float32Array([
      mm.getItemPositionX(item), mm.getItemPositionY(item), mm.getItemPositionZ(item),
    ]);
    const rotate = new Float32Array([
      mm.getItemRotationX(item), mm.getItemRotationY(item), mm.getItemRotationZ(item),
    ]);
    const scale = new Float32Array([
      mm.getItemScaleX(item), mm.getItemScaleY(item), mm.getItemScaleZ(item),
    ]);

    const indicesArr = new Int32Array(indices);
    const ptrs = [
      allocCString(usd, `Item_${item}`),
      alloc(usd, points), alloc(usd, counts), alloc(usd, indicesArr),
      alloc(usd, translate), alloc(usd, rotate), alloc(usd, scale),
    ];
    usd._usdio_export_add_mesh(exp, ptrs[0],
      ptrs[1], vertexCount, ptrs[2], faceCount, ptrs[3], indicesArr.length,
      ptrs[4], ptrs[5], ptrs[6]);
    ptrs.forEach((p) => usd._free(p));
  }

  const pLen = usd._malloc(4);
  const pData = usd._usdio_export_end(exp, format === 'usda' ? 0 : 1, pLen);
  if (!pData) {
    usd._free(pLen);
    throw new Error(lastError(usd));
  }
  const len = usd.HEAP32[pLen / 4];
  const bytes = new Uint8Array(usd.HEAPU8.buffer, pData, len).slice();
  usd._usdio_free_buffer(pData);
  usd._free(pLen);
  return bytes;
}

/**
 * Import a USD file (any encoding — ext picks the format) into the scene as
 * new items. usd-io returns world-space Y-up points with n-gons already fan
 * triangulated, so items are created with identity transforms.
 * Returns the number of meshes added.
 */
export async function importUsd(
  mm: MeshMakerModule, bytes: Uint8Array, ext: string,
): Promise<number> {
  const usd = await loadUsdIo();

  const pBytes = allocBytes(usd, bytes);
  const pExt = allocCString(usd, ext);
  const scene = usd._usdio_import(pBytes, bytes.byteLength, pExt);
  usd._free(pBytes);
  usd._free(pExt);
  if (!scene) throw new Error(lastError(usd));

  const meshCount = usd._usdio_scene_mesh_count(scene);
  try {
    for (let i = 0; i < meshCount; i++) {
      const numPoints = usd._usdio_mesh_num_points(scene, i);
      const numFaces = usd._usdio_mesh_num_faces(scene, i);
      const numIndices = usd._usdio_mesh_num_indices(scene, i);
      const points = new Float32Array(
        usd.HEAPF32.buffer, usd._usdio_mesh_points(scene, i), numPoints * 3);
      const counts = new Int32Array(
        usd.HEAP32.buffer, usd._usdio_mesh_counts(scene, i), numFaces);
      const indices = new Int32Array(
        usd.HEAP32.buffer, usd._usdio_mesh_indices(scene, i), numIndices);

      // New item: the mutation API works on an existing item's mesh, so add
      // a primitive and replace its geometry (same trick the scripts use).
      mm.addCube();
      const item = mm.getItemCount() - 1;
      mm.clearMesh(item);
      for (let v = 0; v < numPoints; v++) {
        mm.addMeshVertex(item, points[v * 3], points[v * 3 + 1], points[v * 3 + 2]);
      }
      let cursor = 0;
      for (let f = 0; f < numFaces; f++) {
        const c = counts[f];
        if (c === 4) {
          mm.addMeshQuad(item,
            indices[cursor], indices[cursor + 1], indices[cursor + 2], indices[cursor + 3]);
        } else {
          mm.addMeshTriangle(item,
            indices[cursor], indices[cursor + 1], indices[cursor + 2]);
        }
        cursor += c;
      }
      mm.rebuildMesh(item);
    }
  } finally {
    usd._usdio_scene_free(scene);
  }
  return meshCount;
}
