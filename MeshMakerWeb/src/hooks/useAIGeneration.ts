import { useState, useCallback, useRef } from 'react';

// ── localStorage keys ──────────────────────────────────────────────
const KEY_SERVER_URL = 'hunyuan-server-url';

// ── Public helpers for persisted settings ──────────────────────────
export function getStoredServerUrl(): string {
  return localStorage.getItem(KEY_SERVER_URL) || 'http://localhost:8080/api';
}

export function setServerUrl(url: string) {
  localStorage.setItem(KEY_SERVER_URL, url);
}

// ── Internal: resolve base URL (use vite proxy in dev) ─────────────
function getApiBase(): string {
  const stored = localStorage.getItem(KEY_SERVER_URL);
  if (!stored) return '/api/hunyuan'; // vite proxy
  // Strip trailing /api if stored URL already includes it and we're proxying
  return stored;
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
  octreeResolution?: number;
  numInferenceSteps?: number;
  guidanceScale?: number;
  texture?: boolean;
  faceCount?: number;
  noRembg?: boolean;
}

// ── Hook ───────────────────────────────────────────────────────────
export function useAIGeneration() {
  const [state, setState] = useState<AIGenerationState>({
    isGenerating: false,
    progress: '',
    error: null,
  });
  const abortRef = useRef<AbortController | null>(null);

  const generate = useCallback(
    async (options: AIGenerationOptions & { image?: string }): Promise<Uint8Array | null> => {
      const baseUrl = getApiBase();
      abortRef.current = new AbortController();
      const signal = abortRef.current.signal;

      const mode = options.text ? 'text prompt' : 'image';
      setState({ isGenerating: true, progress: `Sending ${mode} to Hunyuan3D-2...`, error: null });
      const startTime = Date.now();

      try {
        // 1) Build request body
        const body: Record<string, unknown> = {
          seed: options.seed ?? 42,
          octree_resolution: options.octreeResolution ?? 128,
          num_inference_steps: options.numInferenceSteps ?? 5,
          guidance_scale: options.guidanceScale ?? 5.0,
          texture: options.texture ?? false,
          face_count: options.faceCount ?? 40000,
          no_rembg: options.noRembg ?? false,
        };

        if (options.image) {
          body.image = options.image;
        }
        if (options.text) {
          body.text = options.text;
        }

        // 2) Start async generation (endpoints: /api/send, /api/status, /api/health)
        const res = await fetch(`${baseUrl}/send`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(body),
          signal,
        });

        if (!res.ok) {
          const errBody = await res.json().catch(() => ({}));
          throw new Error(errBody.detail || errBody.text || `Server error: ${res.status} ${res.statusText}`);
        }

        const { uid } = await res.json();
        if (!uid) throw new Error('No uid returned from server');

        // 3) Poll for completion — Hunyuan returns base64-encoded GLB inline
        setState((s) => ({ ...s, progress: 'Generating 3D model...' }));
        const glbBase64 = await pollForCompletion(baseUrl, uid, signal, startTime, (msg) =>
          setState((s) => ({ ...s, progress: msg }))
        );

        // 4) Decode base64 to Uint8Array
        setState((s) => ({ ...s, progress: 'Decoding result...' }));
        const binaryString = atob(glbBase64);
        const bytes = new Uint8Array(binaryString.length);
        for (let i = 0; i < binaryString.length; i++) {
          bytes[i] = binaryString.charCodeAt(i);
        }

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
        abortRef.current = null;
      }
    },
    []
  );

  const cancel = useCallback(() => {
    abortRef.current?.abort();
  }, []);

  const testConnection = useCallback(async (): Promise<boolean> => {
    const baseUrl = getApiBase();
    try {
      const res = await fetch(`${baseUrl}/health`, {
        method: 'GET',
        signal: AbortSignal.timeout(5000),
      });
      return res.ok;
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

// ── Poll /status until completed or failed ─────────────────────────
async function pollForCompletion(
  baseUrl: string,
  uid: string,
  signal: AbortSignal,
  startTime: number,
  onProgress: (msg: string) => void
): Promise<string> {
  const POLL_INTERVAL = 2000;
  const MAX_POLLS = 300; // ~10 minutes

  for (let i = 0; i < MAX_POLLS; i++) {
    await new Promise((r) => setTimeout(r, POLL_INTERVAL));
    if (signal.aborted) throw new DOMException('Aborted', 'AbortError');

    const res = await fetch(`${baseUrl}/status/${uid}`, { signal });
    if (!res.ok) throw new Error(`Status poll error: ${res.status}`);

    const data = await res.json();

    if (data.status === 'completed') {
      if (!data.model_base64) throw new Error('Server returned completed but no model data');
      return data.model_base64;
    }

    if (data.status === 'failed') {
      throw new Error(data.error || 'Generation failed on server');
    }

    // Show percentage + stage + elapsed time
    const elapsed = Math.round((Date.now() - startTime) / 1000);
    const stage = data.stage || 'Processing';
    const percent = data.percent;
    const pctStr = percent != null ? `${percent}% — ` : '';
    onProgress(`${pctStr}${stage} (${elapsed}s)`);
  }

  throw new Error('Generation timed out');
}

// ── Error formatting ───────────────────────────────────────────────
function getErrorMessage(err: unknown): string {
  if (err instanceof TypeError && (err.message.includes('fetch') || err.message.includes('NetworkError'))) {
    return 'Cannot connect to Hunyuan3D-2 server. Make sure the server is running (python gradio_app.py --low_vram_mode)';
  }
  return (err as Error).message || 'Unknown error';
}
