// Type definitions for MeshMaker WASM module

export interface MeshMakerModule extends EmscriptenModule {
  // Primitives
  addCube(): void;
  addPlane(): void;
  addCylinder(steps: number): void;
  addSphere(steps: number): void;
  addIcosahedron(): void;
  
  // Edit mode (0=Items, 1=Vertices, 2=Triangles, 3=Edges)
  getEditMode(): number;
  setEditMode(mode: number): void;
  
  // Transform mode (0=None, 1=Translate, 2=Rotate, 3=Scale)
  getTransformMode(): number;
  setTransformMode(mode: number): void;
  
  // Selection
  getSelectionCount(): number;
  selectAll(): void;
  deselectAll(): void;
  deleteSelection(): void;
  duplicateSelection(): void;
  
  // Undo/Redo
  canUndo(): boolean;
  canRedo(): boolean;
  undo(): void;
  redo(): void;
  
  // Mesh operations
  flipSelectedFaces(): void;
  subdivideSelectedFaces(): void;
  splitSelected(): void;
  triangulateSelectedFaces(): void;
  extrudeSelectedFaces(): void;
  mergeSelectedVertices(): void;
  
  // Edge selection
  selectEdgeLoop(): void;
  selectEdgeRing(): void;
  growEdgeSelection(): void;

  // View settings
  getViewMode(): number;
  setViewMode(mode: number): void;
  getShowGrid(): boolean;
  setShowGrid(show: boolean): void;
  getShowNormals(): boolean;
  setShowNormals(show: boolean): void;
  
  // ImGui visibility (hidden by default in React builds)
  getShowImGui(): boolean;
  setShowImGui(show: boolean): void;
  
  // Unified selection value API (respects current transform mode)
  // - In Translate mode: returns/sets position
  // - In Rotate mode: returns/sets euler angles in degrees
  // - In Scale mode: returns/sets scale values
  getSelectionX(): number;
  getSelectionY(): number;
  getSelectionZ(): number;
  setSelectionX(value: number): void;
  setSelectionY(value: number): void;
  setSelectionZ(value: number): void;
  
  // Legacy position-only API
  getSelectionCenterX(): number;
  getSelectionCenterY(): number;
  getSelectionCenterZ(): number;
  setSelectionPosition(x: number, y: number, z: number): void;
  rotateSelection(xDegrees: number, yDegrees: number, zDegrees: number): void;
  scaleSelection(xOffset: number, yOffset: number, zOffset: number): void;
  
  // Mesh steps for primitives (3-100)
  getMeshSteps(): number;
  setMeshSteps(steps: number): void;
  
  // Serialization - Import/Export
  exportToOBJ(): string;
  importFromOBJ(objData: string): boolean;
  exportToGLB(): Uint8Array | null;
  importFromGLB(data: Uint8Array): boolean;
  
  // Phased GLB Import (for progress reporting)
  importGLBParse(data: Uint8Array): { success: boolean; stepCount: number };
  importGLBStepInfo(stepIndex: number): { name: string; estimatedVertices: number };
  importGLBStep(stepIndex: number): boolean;
  importGLBFinalize(): boolean;
  
  clearScene(): void;
  
  // Texture API
  loadTextureFromRGBA(data: Uint8Array, width: number, height: number): boolean;
  loadTextureFromFileData(data: Uint8Array): boolean;
  removeTexture(): void;
  selectionHasTexture(): boolean;
  
  // UV Mapping API
  // Projection types: 0=Box, 1=Planar, 2=Cylindrical, 3=Spherical
  unwrapSelectedUVs(projectionType: number): void;
  unwrapAllUVs(projectionType: number): void;
  
  // Seam marking API
  markSelectedEdgesAsSeam(isSeam: boolean): void;
  clearAllSeams(): void;
  
  // UV View Mode API
  getUVViewMode(): boolean;
  setUVViewMode(enabled: boolean): void;
  getUVZoom(): number;
  setUVZoom(zoom: number): void;
  getUVOffset(): Float32Array;
  setUVOffset(x: number, y: number): void;
  renderUV(width: number, height: number): void;
  
  // Split View API (single canvas with two viewports)
  getSplitViewEnabled(): boolean;
  setSplitViewEnabled(enabled: boolean): void;
  getSplitRatio(): number;
  setSplitRatio(ratio: number): void;
  getActiveViewport(): number;  // 0 = 3D, 1 = UV
  setActiveViewport(viewport: number): void;
  getViewportAtPosition(x: number, y: number): number;  // Returns 0 (3D), 1 (UV), or -1 (divider)
  
  // Info
  getItemCount(): number;
  
  // Per-Item Iteration API
  getItemPositionX(index: number): number;
  getItemPositionY(index: number): number;
  getItemPositionZ(index: number): number;
  setItemPosition(index: number, x: number, y: number, z: number): void;
  getItemRotationX(index: number): number;
  getItemRotationY(index: number): number;
  getItemRotationZ(index: number): number;
  setItemRotation(index: number, xDeg: number, yDeg: number, zDeg: number): void;
  getItemScaleX(index: number): number;
  getItemScaleY(index: number): number;
  getItemScaleZ(index: number): number;
  setItemScale(index: number, x: number, y: number, z: number): void;
  isItemSelected(index: number): boolean;
  selectItemAtIndex(index: number): void;
  deselectItemAtIndex(index: number): void;
  isItemVisible(index: number): boolean;
  setItemVisible(index: number, visible: boolean): void;
  getItemVertexCount(index: number): number;
  getItemFaceCount(index: number): number;
  getItemEdgeCount(index: number): number;
  
  // Per-Vertex Iteration API
  getVertexX(itemIndex: number, vertexIndex: number): number;
  getVertexY(itemIndex: number, vertexIndex: number): number;
  getVertexZ(itemIndex: number, vertexIndex: number): number;
  setVertexPosition(itemIndex: number, vertexIndex: number, x: number, y: number, z: number): void;
  getVertexNormalX(itemIndex: number, vertexIndex: number): number;
  getVertexNormalY(itemIndex: number, vertexIndex: number): number;
  getVertexNormalZ(itemIndex: number, vertexIndex: number): number;
  isVertexSelected(itemIndex: number, vertexIndex: number): boolean;
  setVertexSelected(itemIndex: number, vertexIndex: number, selected: boolean): void;
  
  // Per-Face Iteration API
  isFaceSelected(itemIndex: number, faceIndex: number): boolean;
  setFaceSelected(itemIndex: number, faceIndex: number, selected: boolean): void;
  getFaceVertexCount(itemIndex: number, faceIndex: number): number;
  getFaceVertexIndex(itemIndex: number, faceIndex: number, cornerIndex: number): number;
  getFaceUVX(itemIndex: number, faceIndex: number, cornerIndex: number): number;
  getFaceUVY(itemIndex: number, faceIndex: number, cornerIndex: number): number;
  setFaceUV(itemIndex: number, faceIndex: number, cornerIndex: number, u: number, v: number): void;
  itemHasTexture(itemIndex: number): boolean;
  itemHasNormalTexture(itemIndex: number): boolean;
  getItemBaseColorR(itemIndex: number): number;
  getItemBaseColorG(itemIndex: number): number;
  getItemBaseColorB(itemIndex: number): number;
  getItemOpacity(itemIndex: number): number;
  getItemMetallic(itemIndex: number): number;
  getItemRoughness(itemIndex: number): number;
  getItemEmissiveR(itemIndex: number): number;
  getItemEmissiveG(itemIndex: number): number;
  getItemEmissiveB(itemIndex: number): number;
  getItemClearcoat(itemIndex: number): number;
  getItemClearcoatRoughness(itemIndex: number): number;
  getItemIor(itemIndex: number): number;
  setItemMaterial(itemIndex: number, r: number, g: number, b: number, opacity: number,
    metallic: number, roughness: number): void;
  setItemAdvancedMaterial(itemIndex: number, emissiveR: number, emissiveG: number,
    emissiveB: number, clearcoat: number, clearcoatRoughness: number, ior: number): void;
  getItemTextureWidth(itemIndex: number): number;
  getItemTextureHeight(itemIndex: number): number;
  getItemNormalTextureWidth(itemIndex: number): number;
  getItemNormalTextureHeight(itemIndex: number): number;
  getItemTexturePixels(itemIndex: number): Uint8Array | null;
  getItemNormalTexturePixels(itemIndex: number): Uint8Array | null;
  setItemTextureFromFileData(itemIndex: number, data: Uint8Array): boolean;
  setItemNormalTextureFromFileData(itemIndex: number, data: Uint8Array): boolean;
  
  // Mesh Mutation API
  clearMesh(itemIndex: number): void;
  addMeshVertex(itemIndex: number, x: number, y: number, z: number): number;
  addMeshTriangle(itemIndex: number, v0: number, v1: number, v2: number): number;
  addMeshQuad(itemIndex: number, v0: number, v1: number, v2: number, v3: number): number;
  rebuildMesh(itemIndex: number): void;
}

// Edit mode enum values
export const EditMode = {
  Items: 0,
  Vertices: 1,
  Triangles: 2,
  Edges: 3,
} as const;

// Transform mode enum values
export const TransformMode = {
  None: 0,
  Translate: 1,
  Rotate: 2,
  Scale: 3,
} as const;

// UV projection types
export const UVProjection = {
  Box: 0,
  Planar: 1,
  Cylindrical: 2,
  Spherical: 3,
} as const;

// View mode enum values
export const ViewMode = {
  Solid: 0,
  Wireframe: 1,
  SolidWireframe: 2,
} as const;

export interface EmscriptenModule {
  canvas: HTMLCanvasElement | null;
  onRuntimeInitialized?: () => void;
  print?: (text: string) => void;
  printErr?: (text: string) => void;
  setStatus?: (status: string) => void;
  monitorRunDependencies?: (left: number) => void;
  locateFile?: (path: string, prefix: string) => string;
  preRun?: Array<() => void>;
  postRun?: Array<() => void>;
}

export type MeshMakerFactory = (moduleOverrides?: Partial<MeshMakerModule>) => Promise<MeshMakerModule>;

declare global {
  interface Window {
    Module?: MeshMakerModule;
    createMeshMakerModule?: MeshMakerFactory;
  }
}
