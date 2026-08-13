import { defineConfig, type Plugin } from 'vite'
import react from '@vitejs/plugin-react'
import tailwindcss from '@tailwindcss/vite'

const GRADIO_TARGET = 'http://localhost:8080';

/**
 * Vite plugin that intercepts /hunyuan/config requests and rewrites the
 * Gradio config so that `root` (and all embedded absolute URLs) point back
 * through the Vite dev-server proxy instead of directly at the Gradio
 * server.  This is required because @gradio/client reads `config.root`
 * and uses it for every subsequent request (SSE, uploads, file downloads).
 */
function rewriteGradioConfig(): Plugin {
  return {
    name: 'rewrite-gradio-config',
    configureServer(server) {
      server.middlewares.use((req, res, next) => {
        if (req.url !== '/hunyuan/config') return next();

        // Fetch the real config from the Gradio server
        fetch(`${GRADIO_TARGET}/config`)
          .then(async (upstream) => {
            const text = await upstream.text();
            const proxyOrigin = `${req.headers['x-forwarded-proto'] || 'http'}://${req.headers.host}`;
            // Replace all absolute references to the Gradio server with
            // the proxy path so the browser never talks to :8080 directly.
            const rewritten = text.replaceAll(GRADIO_TARGET, `${proxyOrigin}/hunyuan`);
            res.setHeader('Content-Type', 'application/json');
            res.end(rewritten);
          })
          .catch((err) => {
            res.statusCode = 502;
            res.end(JSON.stringify({ error: 'Gradio server unreachable', detail: String(err) }));
          });
      });
    },
  };
}

// https://vite.dev/config/
export default defineConfig({
  plugins: [react(), tailwindcss(), rewriteGradioConfig()],
  // Allow loading WASM files from the public folder
  assetsInclude: ['**/*.wasm'],
  server: {
    cors: true,
    // Cross-origin isolation: the lazy-loaded usd-io module uses pthreads
    // (shared WebAssembly.Memory), which browsers gate behind COOP/COEP.
    headers: {
      'Cross-Origin-Opener-Policy': 'same-origin',
      'Cross-Origin-Embedder-Policy': 'credentialless',
    },
    // Allow serving files from the WebGL2 build directory
    fs: {
      allow: ['..'],
    },
    // Proxy Hunyuan3D-2 Gradio requests to avoid CORS issues
    // @gradio/client sends credentials:include which requires same-origin
    proxy: {
      '/hunyuan': {
        target: GRADIO_TARGET,
        changeOrigin: true,
        rewrite: (path) => path.replace(/^\/hunyuan/, ''),
        ws: true,
      },
    },
  },
  preview: {
    headers: {
      'Cross-Origin-Opener-Policy': 'same-origin',
      'Cross-Origin-Embedder-Policy': 'credentialless',
    },
  },
})
