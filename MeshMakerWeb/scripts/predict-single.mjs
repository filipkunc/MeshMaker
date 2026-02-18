import { Client, handle_file } from '@gradio/client';

const IMAGE_PATH = 'C:/Users/kuncf/OneDrive/Pictures/20251108_085028.jpg';
const app = await Client.connect('http://localhost:5173/hunyuan');
const started = Date.now();
const out = await app.predict('/shape_generation', [
  null,
  handle_file(IMAGE_PATH),
  null,
  null,
  null,
  null,
  30,
  5.0,
  42,
  256,
  true,
  8000,
  false,
]);
console.log('durationSec', Math.round((Date.now()-started)/1000));
console.log(JSON.stringify(out?.data?.[0] ?? out?.[0] ?? out, null, 2));
