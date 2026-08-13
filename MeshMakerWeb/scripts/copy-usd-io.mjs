// Copy the lazy-loaded OpenUSD wasm module from usd-io/build to public/usd-io
import { copyFileSync, existsSync, mkdirSync, statSync } from 'fs';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const projectRoot = join(__dirname, '..');

const srcDir = join(projectRoot, '..', 'usd-io', 'build');
const destDir = join(projectRoot, 'public', 'usd-io');

const files = ['usd-io.mjs', 'usd-io.wasm'];

if (!existsSync(destDir)) {
  mkdirSync(destDir, { recursive: true });
}

for (const file of files) {
  const src = join(srcDir, file);
  if (!existsSync(src)) {
    console.error(`Missing ${src} — build usd-io first (see usd-io/README.md)`);
    process.exit(1);
  }
  copyFileSync(src, join(destDir, file));
  const kb = (statSync(src).size / 1024).toFixed(0);
  console.log(`Copied ${file} (${kb} KB)`);
}
