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
