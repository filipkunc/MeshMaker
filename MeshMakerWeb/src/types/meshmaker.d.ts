// Type definitions for MeshMaker WASM module

export interface MeshMakerModule extends EmscriptenModule {
  // Add exported functions here as we expose them from C++
  // Example:
  // _addCube(): void;
  // _selectAll(): void;
}

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
