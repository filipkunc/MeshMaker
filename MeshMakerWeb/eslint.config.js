import js from '@eslint/js'
import globals from 'globals'
import reactHooks from 'eslint-plugin-react-hooks'
import reactRefresh from 'eslint-plugin-react-refresh'
import tseslint from 'typescript-eslint'
import { defineConfig, globalIgnores } from 'eslint/config'

export default defineConfig([
  globalIgnores(['dist']),
  {
    files: ['**/*.{ts,tsx}'],
    extends: [
      js.configs.recommended,
      tseslint.configs.recommended,
      reactHooks.configs.flat.recommended,
      reactRefresh.configs.vite,
    ],
    languageOptions: {
      ecmaVersion: 2020,
      globals: globals.browser,
    },
    rules: {
      '@typescript-eslint/no-unused-vars': ['error', {
        argsIgnorePattern: '^_',
        caughtErrorsIgnorePattern: '^_',
        varsIgnorePattern: '^_',
      }],
      // Transform inputs intentionally mirror values owned by the WASM engine.
      'react-hooks/set-state-in-effect': 'off',
    },
  },
  {
    files: [
      'e2e/**/*.ts',
      'src/components/ScriptEditor.tsx',
      'src/hooks/useMeshMaker.ts',
    ],
    rules: {
      // These files cross untyped browser, Monaco, Playwright, and Emscripten APIs.
      '@typescript-eslint/no-explicit-any': 'off',
    },
  },
])
