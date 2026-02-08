import type { MeshMakerModule } from '../types/meshmaker';

/**
 * Defines the scripting API surface — every function available to user scripts.
 * Each entry maps a global function name to a description and the module method it wraps.
 */
export interface ScriptingFunction {
  name: string;
  description: string;
  signature: string;
}

/** All functions exposed to the scripting sandbox */
export const SCRIPTING_FUNCTIONS: ScriptingFunction[] = [
  // Primitives
  { name: 'addCube', description: 'Add a cube to the scene', signature: '(): void' },
  { name: 'addPlane', description: 'Add a plane to the scene', signature: '(): void' },
  { name: 'addCylinder', description: 'Add a cylinder with given steps', signature: '(steps: number): void' },
  { name: 'addSphere', description: 'Add a sphere with given steps', signature: '(steps: number): void' },
  { name: 'addIcosahedron', description: 'Add an icosahedron to the scene', signature: '(): void' },

  // Edit mode
  { name: 'getEditMode', description: 'Get current edit mode (0=Items, 1=Vertices, 2=Triangles, 3=Edges)', signature: '(): number' },
  { name: 'setEditMode', description: 'Set edit mode (0=Items, 1=Vertices, 2=Triangles, 3=Edges)', signature: '(mode: number): void' },

  // Transform mode
  { name: 'getTransformMode', description: 'Get current transform mode (0=None, 1=Translate, 2=Rotate, 3=Scale)', signature: '(): number' },
  { name: 'setTransformMode', description: 'Set transform mode (0=None, 1=Translate, 2=Rotate, 3=Scale)', signature: '(mode: number): void' },

  // Selection
  { name: 'getSelectionCount', description: 'Get number of selected items/components', signature: '(): number' },
  { name: 'selectAll', description: 'Select all items/components', signature: '(): void' },
  { name: 'deselectAll', description: 'Deselect all items/components', signature: '(): void' },
  { name: 'deleteSelection', description: 'Delete selected items/components', signature: '(): void' },
  { name: 'duplicateSelection', description: 'Duplicate selected items/components', signature: '(): void' },

  // Undo/Redo
  { name: 'canUndo', description: 'Check if undo is available', signature: '(): boolean' },
  { name: 'canRedo', description: 'Check if redo is available', signature: '(): boolean' },
  { name: 'undo', description: 'Undo last operation', signature: '(): void' },
  { name: 'redo', description: 'Redo last undone operation', signature: '(): void' },

  // Mesh operations
  { name: 'flipSelectedFaces', description: 'Flip normals of selected faces', signature: '(): void' },
  { name: 'subdivideSelectedFaces', description: 'Catmull-Clark subdivide selected faces', signature: '(): void' },
  { name: 'splitSelected', description: 'Split selected edges/faces', signature: '(): void' },
  { name: 'triangulateSelectedFaces', description: 'Triangulate selected faces', signature: '(): void' },
  { name: 'extrudeSelectedFaces', description: 'Extrude selected faces', signature: '(): void' },
  { name: 'mergeSelectedVertices', description: 'Merge selected vertices', signature: '(): void' },

  // Edge selection
  { name: 'selectEdgeLoop', description: 'Expand selection along edge loops', signature: '(): void' },
  { name: 'selectEdgeRing', description: 'Expand selection along edge rings', signature: '(): void' },
  { name: 'growEdgeSelection', description: 'Grow edge selection to neighbors', signature: '(): void' },

  // View settings
  { name: 'getViewMode', description: 'Get view mode (0=Solid, 1=Wireframe, 2=SolidWireframe)', signature: '(): number' },
  { name: 'setViewMode', description: 'Set view mode (0=Solid, 1=Wireframe, 2=SolidWireframe)', signature: '(mode: number): void' },
  { name: 'getShowGrid', description: 'Check if grid is visible', signature: '(): boolean' },
  { name: 'setShowGrid', description: 'Show or hide grid', signature: '(show: boolean): void' },
  { name: 'getShowNormals', description: 'Check if normals are visible', signature: '(): boolean' },
  { name: 'setShowNormals', description: 'Show or hide normals', signature: '(show: boolean): void' },

  // Info
  { name: 'getItemCount', description: 'Get number of items in the scene', signature: '(): number' },

  // Selection values (unified, respects transform mode)
  { name: 'getSelectionX', description: 'Get selection X value (position/rotation/scale based on transform mode)', signature: '(): number' },
  { name: 'getSelectionY', description: 'Get selection Y value', signature: '(): number' },
  { name: 'getSelectionZ', description: 'Get selection Z value', signature: '(): number' },
  { name: 'setSelectionX', description: 'Set selection X value', signature: '(value: number): void' },
  { name: 'setSelectionY', description: 'Set selection Y value', signature: '(value: number): void' },
  { name: 'setSelectionZ', description: 'Set selection Z value', signature: '(value: number): void' },

  // Direct transform API
  { name: 'setSelectionPosition', description: 'Set position of selected items', signature: '(x: number, y: number, z: number): void' },
  { name: 'rotateSelection', description: 'Rotate selected items by degrees', signature: '(xDeg: number, yDeg: number, zDeg: number): void' },
  { name: 'scaleSelection', description: 'Scale selected items by offset', signature: '(x: number, y: number, z: number): void' },

  // Mesh steps
  { name: 'getMeshSteps', description: 'Get mesh subdivision steps for primitives', signature: '(): number' },
  { name: 'setMeshSteps', description: 'Set mesh subdivision steps for primitives (3-100)', signature: '(steps: number): void' },

  // Serialization
  { name: 'exportToOBJ', description: 'Export scene to OBJ format string', signature: '(): string' },
  { name: 'importFromOBJ', description: 'Import scene from OBJ format string', signature: '(objData: string): boolean' },
  { name: 'clearScene', description: 'Clear all items from the scene', signature: '(): void' },

  // UV mapping
  { name: 'unwrapSelectedUVs', description: 'Unwrap UVs for selected faces (0=Box, 1=Planar, 2=Cylindrical, 3=Spherical)', signature: '(projectionType: number): void' },
  { name: 'unwrapAllUVs', description: 'Unwrap UVs for all faces', signature: '(projectionType: number): void' },
  { name: 'markSelectedEdgesAsSeam', description: 'Mark/unmark selected edges as UV seams', signature: '(isSeam: boolean): void' },
  { name: 'clearAllSeams', description: 'Clear all UV seams', signature: '(): void' },
];

/**
 * Build the runtime bindings object for the scripting sandbox.
 * Returns an object mapping function names to bound module methods + a log function.
 */
export function buildScriptingBindings(
  module: MeshMakerModule,
  logFn: (...args: unknown[]) => void
): Record<string, (...args: unknown[]) => unknown> {
  const bindings: Record<string, (...args: unknown[]) => unknown> = {};

  for (const fn of SCRIPTING_FUNCTIONS) {
    const method = (module as unknown as Record<string, unknown>)[fn.name];
    if (typeof method === 'function') {
      bindings[fn.name] = method.bind(module);
    }
  }

  // Add log utility
  bindings['log'] = logFn;

  return bindings;
}

/**
 * Generate a TypeScript declaration string for Monaco autocompletion.
 */
export function generateTypeDeclarations(): string {
  const lines: string[] = [
    '// MeshMaker Scripting API',
    '// These functions are available as globals in the script editor.',
    '',
    '/** Log values to the console output pane */',
    'declare function log(...args: any[]): void;',
    '',
  ];

  for (const fn of SCRIPTING_FUNCTIONS) {
    lines.push(`/** ${fn.description} */`);
    lines.push(`declare function ${fn.name}${fn.signature};`);
    lines.push('');
  }

  // Add enum constants for convenience
  lines.push('// Edit mode constants');
  lines.push('declare const EditMode: { Items: 0; Vertices: 1; Triangles: 2; Edges: 3 };');
  lines.push('');
  lines.push('// Transform mode constants');
  lines.push('declare const TransformMode: { None: 0; Translate: 1; Rotate: 2; Scale: 3 };');
  lines.push('');
  lines.push('// View mode constants');
  lines.push('declare const ViewMode: { Solid: 0; Wireframe: 1; SolidWireframe: 2 };');
  lines.push('');
  lines.push('// UV projection constants');
  lines.push('declare const UVProjection: { Box: 0; Planar: 1; Cylindrical: 2; Spherical: 3 };');

  return lines.join('\n');
}
