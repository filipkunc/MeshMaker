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
  triangulateSelectedFaces(): void;
  extrudeSelectedFaces(): void;
  splitSelectedEdges(): void;
  mergeSelectedVertices(): void;
  
  // View settings
  getViewMode(): number;
  setViewMode(mode: number): void;
  getShowGrid(): boolean;
  setShowGrid(show: boolean): void;
  
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
