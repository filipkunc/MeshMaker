import { useEffect, useRef, useState, useCallback } from 'react';
import type { MeshMakerModule } from '../types/meshmaker';

// Module-level guard: WASM module should only be loaded once, even in
// React StrictMode (which double-invokes effects in development).
let wasmModuleLoaded = false;

interface UseMeshMakerOptions {
  wasmPath?: string;
  jsPath?: string;
}

interface UseMeshMakerResult {
  canvasRef: React.RefObject<HTMLCanvasElement | null>;
  containerRef: React.RefObject<HTMLDivElement | null>;
  isLoading: boolean;
  error: string | null;
  module: MeshMakerModule | null;
}

export function useMeshMaker(options: UseMeshMakerOptions = {}): UseMeshMakerResult {
  const {
    wasmPath = '/wasm/MeshMakerWebGL2.wasm',
    jsPath = '/wasm/MeshMakerWebGL2.js',
  } = options;

  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const containerRef = useRef<HTMLDivElement | null>(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [module, setModule] = useState<MeshMakerModule | null>(null);
  const moduleRef = useRef<MeshMakerModule | null>(null);

  const loadModule = useCallback(async () => {
    // Prevent double-load in React StrictMode
    if (wasmModuleLoaded) return;
    wasmModuleLoaded = true;

    const canvas = canvasRef.current;
    const container = containerRef.current;
    if (!canvas || !container) {
      setError('Canvas or container element not available');
      return;
    }

    try {
      setIsLoading(true);
      setError(null);

      // Set initial canvas size to match container before WASM loads
      const rect = container.getBoundingClientRect();
      canvas.style.width = `${rect.width}px`;
      canvas.style.height = `${rect.height}px`;

      // ---- Keyboard isolation for Monaco editor & input elements ----
      // Emscripten's GLFW registers capture-phase keyboard handlers on window
      // (and potentially document) that unconditionally call preventDefault(),
      // stealing keys from <input> elements and the Monaco code editor.
      //
      // We patch EventTarget.prototype.addEventListener BEFORE loading the
      // Emscripten script so every keyboard handler registered by GLFW/Emscripten
      // — regardless of whether it's on window, document, or canvas, and
      // regardless of cached references — gets wrapped with a focus check.
      //
      // After the module initializes we restore the prototype so that Monaco's
      // own handlers (loaded lazily later) are NOT wrapped.
      const origProtoAEL = EventTarget.prototype.addEventListener;
      EventTarget.prototype.addEventListener = function (
        this: EventTarget,
        type: string,
        listener: EventListenerOrEventListenerObject | null,
        options?: boolean | AddEventListenerOptions,
      ) {
        if (
          listener &&
          (type === 'keydown' || type === 'keyup' || type === 'keypress')
        ) {
          const original = listener;
          const wrapped: EventListener = function (this: unknown, ev: Event) {
            if (ev instanceof KeyboardEvent) {
              const t = ev.target as HTMLElement | null;
              // Skip when the Monaco editor has focus
              if (t?.closest?.('.monaco-editor')) return;
              // Skip ALL keys when a regular input/textarea has focus —
              // GLFW should never intercept keyboard events from form elements
              if (t instanceof HTMLInputElement || t instanceof HTMLTextAreaElement) return;
            }
            if (typeof original === 'function') {
              original.call(this, ev);
            } else if (original && typeof (original as EventListenerObject).handleEvent === 'function') {
              (original as EventListenerObject).handleEvent(ev);
            }
          };
          return origProtoAEL.call(this, type, wrapped, options);
        }
        return origProtoAEL.call(this, type, listener, options);
      };

      // Load the Emscripten JS file
      const script = document.createElement('script');
      script.src = jsPath;
      
      await new Promise<void>((resolve, reject) => {
        script.onload = () => resolve();
        script.onerror = () => reject(new Error(`Failed to load ${jsPath}`));
        document.head.appendChild(script);
      });

      // Wait for the module factory to be available
      // Emscripten creates a global function with the module name
      const factory = (window as unknown as { createMeshMakerModule?: (opts: unknown) => Promise<MeshMakerModule> }).createMeshMakerModule;
      
      if (!factory) {
        throw new Error('MeshMaker module factory not found');
      }

      // Initialize the module with our canvas
      const moduleInstance = await factory({
        canvas: canvasRef.current,
        locateFile: (path: string) => {
          if (path.endsWith('.wasm')) {
            return wasmPath;
          }
          return path;
        },
        print: (text: string) => { if (import.meta.env.DEV) console.log('[MeshMaker]', text); },
        printErr: (text: string) => console.error('[MeshMaker]', text),
      });

      // Restore the original prototype — GLFW's handlers are already wrapped,
      // future handlers (e.g. Monaco's own) will be registered normally.
      EventTarget.prototype.addEventListener = origProtoAEL;

      moduleRef.current = moduleInstance;
      setModule(moduleInstance);
      setIsLoading(false);
      
      // Expose module on window for Playwright E2E tests
      (window as any).Module = moduleInstance;
      
      if (import.meta.env.DEV) {
        console.log('MeshMaker WASM module loaded successfully');
      }
      
      // Trigger initial resize after module is loaded
      window.dispatchEvent(new Event('resize'));
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : 'Unknown error loading WASM module';
      setError(errorMessage);
      setIsLoading(false);
      console.error('Failed to load MeshMaker:', err);
    }
  }, [wasmPath, jsPath]);

  useEffect(() => {
    // Wait for layout to be ready before loading module
    const waitForLayout = () => {
      const container = containerRef.current;
      if (!container) return;
      
      const rect = container.getBoundingClientRect();
      if (rect.width > 0 && rect.height > 0) {
        loadModule();
      } else {
        // Layout not ready, try again
        requestAnimationFrame(waitForLayout);
      }
    };
    
    // Use double RAF to ensure layout is computed
    requestAnimationFrame(() => {
      requestAnimationFrame(waitForLayout);
    });
  }, [loadModule]);

  return {
    canvasRef,
    containerRef,
    isLoading,
    error,
    module,
  };
}
