import { useEffect, useRef, useState, useCallback } from 'react';
import type { MeshMakerModule } from '../types/meshmaker';

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

      // Guard against Emscripten's GLFW keydown handler which registers in
      // the capture phase and unconditionally calls preventDefault() on
      // Backspace and Tab — even when the user is typing in an <input>.
      // By registering our own capture-phase listener BEFORE glfwInit runs,
      // we can stopImmediatePropagation so GLFW never sees the event.
      const inputKeyGuard = (e: KeyboardEvent) => {
        if (
          (e.target instanceof HTMLInputElement || e.target instanceof HTMLTextAreaElement) &&
          (e.key === 'Backspace' || e.key === 'Tab')
        ) {
          e.stopImmediatePropagation();
        }
      };
      window.addEventListener('keydown', inputKeyGuard, true);

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
