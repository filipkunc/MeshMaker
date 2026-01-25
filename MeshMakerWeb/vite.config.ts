import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import tailwindcss from '@tailwindcss/vite'

// https://vite.dev/config/
export default defineConfig({
  plugins: [react(), tailwindcss()],
  // Allow loading WASM files from the public folder
  assetsInclude: ['**/*.wasm'],
  server: {
    // Allow serving files from the WebGL2 build directory
    fs: {
      allow: ['..'],
    },
  },
})
