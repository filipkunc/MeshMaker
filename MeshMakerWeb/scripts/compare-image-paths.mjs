import { Client, handle_file } from '@gradio/client';
import { readFile } from 'node:fs/promises';

const IMAGE_PATH = 'C:/Users/kuncf/OneDrive/Pictures/20251108_085028.jpg';
const FULL_TIMEOUT_MS = 12 * 60 * 1000;
const API_NAME = process.env.API_NAME || '/shape_generation';

const commonArgs = [
  null,        // caption
  null,        // image (filled per run)
  null,        // mv_front
  null,        // mv_back
  null,        // mv_left
  null,        // mv_right
  30,          // steps
  5.0,         // guidance_scale
  42,          // seed
  256,         // octree_resolution
  true,        // check_box_rembg
  8000,        // num_chunks
  false,       // randomize_seed
];

function extractFileUrl(item) {
  if (!item || typeof item !== 'object') return null;
  if (typeof item.url === 'string') return item.url;
  if (typeof item.path === 'string') return item.path;
  if (item.value && typeof item.value === 'object') return extractFileUrl(item.value);
  return null;
}

function findBestOutputUrl(data) {
  const urls = data.map((item) => extractFileUrl(item)).filter((url) => !!url);
  if (urls.length === 0) return null;
  const glbs = urls.filter((url) => /\.glb(\b|$)/i.test(url));
  if (glbs.length > 0) {
    // generation_all returns untextured first and textured second
    return glbs[glbs.length - 1];
  }
  return urls[urls.length - 1];
}

async function testEndpoint(name, endpoint, imageMode = 'path') {
  const startedAt = Date.now();
  const result = {
    name,
    endpoint,
    imageMode,
    apiName: API_NAME,
    connected: false,
    completed: false,
    durationSec: null,
    statusStages: [],
    glbUrl: null,
    glbBytes: null,
    dataSummary: null,
    firstStatus: null,
    error: null,
  };

  try {
    const app = await Client.connect(endpoint, { events: ['status', 'data'] });
    result.connected = true;

    let imageInput;
    if (imageMode === 'blob') {
      const bytes = await readFile(IMAGE_PATH);
      imageInput = handle_file(new Blob([bytes], { type: 'image/jpeg' }));
    } else {
      imageInput = handle_file(IMAGE_PATH);
    }
    const submission = app.submit(API_NAME, [
      ...commonArgs.slice(0, 1),
      imageInput,
      ...commonArgs.slice(2),
    ]);

    const deadline = Date.now() + FULL_TIMEOUT_MS;
    for await (const msg of submission) {
      if (msg.type === 'status') {
        if (msg.stage) {
          result.statusStages.push(msg.stage);
        }
        if (!result.firstStatus) {
          result.firstStatus = {
            stage: msg.stage,
            queue: msg.position,
            message: msg.message ?? null,
          };
        }
        if (msg.stage === 'error') {
          result.error = msg.message || 'status:error';
          break;
        }
      }

      if (msg.type === 'data') {
        const data = Array.isArray(msg.data) ? msg.data : [];
        result.dataSummary = data.map((item) => {
          if (item === null || item === undefined) return item;
          if (typeof item !== 'object') return typeof item;
          return {
            keys: Object.keys(item),
            url: item.url ?? null,
            path: item.path ?? null,
            valueType: item.value ? typeof item.value : null,
          };
        });

        const glbUrl = findBestOutputUrl(data);
        result.glbUrl = glbUrl;

        if (glbUrl) {
          const response = await fetch(glbUrl);
          if (!response.ok) {
            throw new Error(`Failed downloading output GLB (${response.status})`);
          }
          const buffer = await response.arrayBuffer();
          result.glbBytes = buffer.byteLength;
        }

        result.completed = true;
        result.durationSec = Math.round((Date.now() - startedAt) / 1000);
        break;
      }

      if (Date.now() > deadline) {
        result.durationSec = Math.round((Date.now() - startedAt) / 1000);
        result.error = `timeout after ${result.durationSec}s`;
        submission.cancel();
        break;
      }
    }
  } catch (error) {
    result.error = String(error?.message || error);
    result.durationSec = Math.round((Date.now() - startedAt) / 1000);
  }

  return result;
}

const runs = [
  ['direct-gradio-path', 'http://localhost:8080', 'path'],
  ['direct-gradio-blob', 'http://localhost:8080', 'blob'],
  ['proxy-gradio-path', 'http://localhost:5173/hunyuan', 'path'],
  ['proxy-gradio-blob', 'http://localhost:5173/hunyuan', 'blob'],
];

const out = [];
for (const [name, endpoint, imageMode] of runs) {
  out.push(await testEndpoint(name, endpoint, imageMode));
}

console.log(JSON.stringify(out, null, 2));
