// Script to copy WASM files from WebGL2 build to public folder
import { copyFileSync, existsSync, mkdirSync } from 'fs';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const projectRoot = join(__dirname, '..');
const wasmSourceDir = join(projectRoot, '..', 'WebGL2', 'build-wasm');
const wasmDestDir = join(projectRoot, 'public', 'wasm');

// Files to copy
const files = [
  'MeshMakerWebGL2.js',
  'MeshMakerWebGL2.wasm',
];

// Ensure destination directory exists
if (!existsSync(wasmDestDir)) {
  mkdirSync(wasmDestDir, { recursive: true });
}

// Copy files
let copied = 0;
for (const file of files) {
  const src = join(wasmSourceDir, file);
  const dest = join(wasmDestDir, file);
  
  if (existsSync(src)) {
    copyFileSync(src, dest);
    console.log(`✓ Copied ${file}`);
    copied++;
  } else {
    console.warn(`⚠ Source file not found: ${src}`);
  }
}

if (copied === files.length) {
  console.log(`\n✓ All ${copied} WASM files copied successfully!`);
} else {
  console.log(`\n⚠ Copied ${copied}/${files.length} files. Run 'ninja' in WebGL2/build first.`);
}
