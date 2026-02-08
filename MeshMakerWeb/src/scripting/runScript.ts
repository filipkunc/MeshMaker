import type { MeshMakerModule } from '../types/meshmaker';
import { buildScriptingBindings } from './scriptingApi';

export interface ConsoleEntry {
  type: 'log' | 'error';
  text: string;
  timestamp: number;
}

/**
 * Execute user script in a sandboxed Function scope.
 * All MeshMaker API functions are injected as named parameters.
 */
export function runScript(
  code: string,
  module: MeshMakerModule,
  onConsole: (entry: ConsoleEntry) => void
): void {
  const logFn = (...args: unknown[]) => {
    const text = args
      .map(a => {
        if (a === undefined) return 'undefined';
        if (a === null) return 'null';
        if (typeof a === 'object') {
          try { return JSON.stringify(a, null, 2); } catch { return String(a); }
        }
        return String(a);
      })
      .join(' ');
    onConsole({ type: 'log', text, timestamp: Date.now() });
  };

  const bindings = buildScriptingBindings(module, logFn);

  // Add enum constants
  const constants: Record<string, unknown> = {
    EditMode: { Items: 0, Vertices: 1, Triangles: 2, Edges: 3 },
    TransformMode: { None: 0, Translate: 1, Rotate: 2, Scale: 3 },
    ViewMode: { Solid: 0, Wireframe: 1, SolidWireframe: 2 },
    UVProjection: { Box: 0, Planar: 1, Cylindrical: 2, Spherical: 3 },
  };

  const allBindings = { ...bindings, ...constants };

  // Build parameter names and values arrays
  const paramNames = Object.keys(allBindings);
  const paramValues = paramNames.map(k => allBindings[k]);

  try {
    // Create a function with named parameters matching the API
    const fn = new Function(...paramNames, code);
    fn(...paramValues);
  } catch (err) {
    const message = err instanceof Error ? err.message : String(err);
    onConsole({ type: 'error', text: message, timestamp: Date.now() });
  }
}
