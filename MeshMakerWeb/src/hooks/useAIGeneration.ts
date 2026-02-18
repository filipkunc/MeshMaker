import { useState, useCallback, useRef } from 'react';
import { Client, handle_file } from '@gradio/client';

// ── localStorage keys ──────────────────────────────────────────────
const KEY_SERVER_URL = 'hunyuan-server-url';

// Default Hunyuan server address (for display only)
const DEFAULT_SERVER = 'http://localhost:8080';

// ── Public helpers for persisted settings ──────────────────────────
export function getStoredServerUrl(): string {
  return localStorage.getItem(KEY_SERVER_URL) || DEFAULT_SERVER;
}

export function setServerUrl(url: string) {
  // Store empty string to mean "use default (local proxy)"
  if (url === DEFAULT_SERVER || !url) {
    localStorage.removeItem(KEY_SERVER_URL);
  } else {
    localStorage.setItem(KEY_SERVER_URL, url);
  }
}

// ── Internal: resolve Gradio server URL ────────────────────────────
// When connecting to the default localhost:8080, route through the Vite
// proxy (/hunyuan) to avoid CORS. Custom URLs are used directly.
function getServerUrl(): string {
  const stored = localStorage.getItem(KEY_SERVER_URL);
  if (stored) return stored;
  // Use Vite proxy to avoid CORS (same-origin requests)
  return `${window.location.origin}/hunyuan`;
}

// ── Hook state ─────────────────────────────────────────────────────
interface AIGenerationState {
  isGenerating: boolean;
  progress: string;
  error: string | null;
}

export interface AIGenerationOptions {
  seed?: number;
  text?: string;
  imageFile?: File | Blob;
  octreeResolution?: number;
  numInferenceSteps?: number;
  guidanceScale?: number;
  numChunks?: number;
  texture?: boolean;
  noRembg?: boolean;
}

// ── Hook ───────────────────────────────────────────────────────────
export function useAIGeneration() {
  const [state, setState] = useState<AIGenerationState>({
    isGenerating: false,
    progress: '',
    error: null,
  });
  const cancelRef = useRef<(() => void) | null>(null);

  const generate = useCallback(
    async (options: AIGenerationOptions): Promise<Uint8Array | null> => {
      const serverUrl = getServerUrl();

      const useTexture = options.texture ?? false;
      const apiName = useTexture ? '/generation_all' : '/shape_generation';
      const mode = options.text ? 'text prompt' : 'image';
      setState({ isGenerating: true, progress: `Connecting to Hunyuan3D-2...`, error: null });
      const startTime = Date.now();

      try {
        // Connect to the Gradio app
        const app = await Client.connect(serverUrl, {
          events: ['data', 'status'],
        });

        // Build image input using handle_file for original File/Blob data
        let imageInput: ReturnType<typeof handle_file> | null = null;
        if (options.imageFile) {
          imageInput = handle_file(options.imageFile);
        }

        // Submit the job — inputs match btn.click/btn_all.click order:
        //   caption, image, mv_front, mv_back, mv_left, mv_right,
        //   steps, guidance_scale, seed, octree_resolution, check_box_rembg,
        //   num_chunks, randomize_seed
        const inputs = [
          options.text ?? null,              // caption
          imageInput,                        // image
          null,                              // mv_image_front
          null,                              // mv_image_back
          null,                              // mv_image_left
          null,                              // mv_image_right
          options.numInferenceSteps ?? 30,   // steps
          options.guidanceScale ?? 5.0,      // guidance_scale
          options.seed ?? 1234,              // seed
          options.octreeResolution ?? 256,   // octree_resolution
          !(options.noRembg ?? false),        // check_box_rembg (inverted)
          options.numChunks ?? 8000,         // num_chunks
          false,                             // randomize_seed
        ];

        let cancelled = false;
        cancelRef.current = () => {
          cancelled = true;
          setState({ isGenerating: false, progress: '', error: null });
        };
        setState((s) => ({ ...s, progress: `Generating from ${mode}...` }));

        const progressTimer = window.setInterval(() => {
          if (cancelled) return;
          const elapsed = Math.round((Date.now() - startTime) / 1000);
          setState((s) => ({ ...s, progress: `Generating 3D model... (${elapsed}s)` }));
        }, 1000);

        let resultData: unknown[] | null = null;
        try {
          const predictResult = await app.predict(apiName, inputs);
          resultData = extractPredictData(predictResult);
        } finally {
          window.clearInterval(progressTimer);
        }

        if (cancelled) {
          return null;
        }

        if (!resultData) {
          throw new Error('No result data received from Gradio');
        }

        // Find the GLB file in the results
        // shape_generation returns: [file, html, stats, seed]
        // generation_all returns:   [file, file2, html, stats, seed]
        // For generation_all with texture, we want file2 (the last .glb)
        setState((s) => ({ ...s, progress: 'Downloading model...' }));

        const glbUrl = findGlbUrl(resultData);
        if (!glbUrl) throw new Error('No GLB file found in Gradio response');

        // Rewrite absolute Gradio URLs to go through the proxy
        const downloadUrl = proxyRewrite(glbUrl);
        const fileRes = await fetch(downloadUrl);
        if (!fileRes.ok) throw new Error(`Failed to download GLB: ${fileRes.status}`);
        const arrayBuffer = await fileRes.arrayBuffer();
        const bytes = new Uint8Array(arrayBuffer);

        setState({ isGenerating: false, progress: '', error: null });
        return bytes;
      } catch (err) {
        if ((err as Error).name === 'AbortError') {
          setState({ isGenerating: false, progress: '', error: null });
          return null;
        }
        const message = getErrorMessage(err);
        setState({ isGenerating: false, progress: '', error: message });
        return null;
      } finally {
        cancelRef.current = null;
      }
    },
    []
  );

  const cancel = useCallback(() => {
    cancelRef.current?.();
  }, []);

  const testConnection = useCallback(async (): Promise<boolean> => {
    const serverUrl = getServerUrl();
    try {
      const app = await Client.connect(serverUrl);
      const info = await app.view_api();
      // Check that our expected endpoints exist
      return 'shape_generation' in (info?.named_endpoints ?? {})
        || '/shape_generation' in (info?.named_endpoints ?? {});
    } catch {
      return false;
    }
  }, []);

  return {
    ...state,
    generate,
    cancel,
    testConnection,
  };
}

function extractPredictData(result: unknown): unknown[] | null {
  if (Array.isArray(result)) {
    return result;
  }
  if (result && typeof result === 'object') {
    const obj = result as Record<string, unknown>;
    if (Array.isArray(obj.data)) {
      return obj.data;
    }
  }
  return null;
}

// ── Find GLB download URL in Gradio result data ───────────────────
function findGlbUrl(data: unknown[]): string | null {
  // Walk backwards: for generation_all, outputs are [untextured, textured, ...],
  // so the last .glb is the textured mesh which is what we want.
  let lastGlb: string | null = null;
  let lastFile: string | null = null;
  for (const item of data) {
    const url = extractFileUrl(item);
    if (url) {
      lastFile = url;
      if (url.endsWith('.glb') || url.includes('.glb')) lastGlb = url;
    }
  }
  return lastGlb ?? lastFile;
}

function extractFileUrl(item: unknown): string | null {
  if (!item || typeof item !== 'object') return null;
  const obj = item as Record<string, unknown>;
  // @gradio/client returns FileData with url field for downloading
  if (typeof obj.url === 'string') return obj.url;
  // Fallback: path field
  if (typeof obj.path === 'string') return obj.path;
  // Nested value (gr.update format)
  if (obj.value && typeof obj.value === 'object') {
    return extractFileUrl(obj.value);
  }
  return null;
}

// ── Rewrite absolute Gradio server URLs to go through the proxy ────
// Result data from Gradio contains absolute URLs like
// http://localhost:8080/gradio_api/file=..., which would hit CORS if
// fetched directly. Rewrite them to go through the /hunyuan proxy.
function proxyRewrite(url: string): string {
  const stored = localStorage.getItem(KEY_SERVER_URL);
  // Only rewrite when using the default (proxied) server
  if (stored) return url;
  try {
    const u = new URL(url);
    // If the URL points to the Gradio server, rewrite to proxy
    return `${window.location.origin}/hunyuan${u.pathname}${u.search}${u.hash}`;
  } catch {
    return url;
  }
}

// ── Error formatting ───────────────────────────────────────────────
function getErrorMessage(err: unknown): string {
  if (err instanceof TypeError && (err.message.includes('fetch') || err.message.includes('NetworkError'))) {
    return 'Cannot connect to Hunyuan3D-2 server. Make sure the server is running (python gradio_app.py --low_vram_mode)';
  }
  return (err as Error).message || 'Unknown error';
}
