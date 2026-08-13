import { Buffer } from 'buffer'
// @gradio/client uses Buffer without a globalThis guard, so polyfill it
;(globalThis as { Buffer?: typeof Buffer }).Buffer = Buffer

import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './index.css'
import App from './App.tsx'

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <App />
  </StrictMode>,
)
