// Loop Subdivision – MeshMaker Scripting API
// Select an item, then run. Change 'iterations' for more smoothing.

const iterations = 1;

function edgeKey(a, b) { return a < b ? a + ',' + b : b + ',' + a; }

function readMesh(idx) {
  const vc = getItemVertexCount(idx), fc = getItemFaceCount(idx);
  const verts = [];
  for (let i = 0; i < vc; i++)
    verts.push([getVertexX(idx, i), getVertexY(idx, i), getVertexZ(idx, i)]);
  const faces = [];
  for (let i = 0; i < fc; i++) {
    const n = getFaceVertexCount(idx, i), f = [];
    for (let j = 0; j < n; j++) f.push(getFaceVertexIndex(idx, i, j));
    faces.push(f);
  }
  return { verts, faces };
}

function loopSubdivide(mesh) {
  // Triangulate quads
  const tris = [];
  for (const f of mesh.faces) {
    if (f.length === 3) tris.push(f);
    else { tris.push([f[0], f[1], f[2]]); tris.push([f[0], f[2], f[3]]); }
  }
  mesh.faces = tris;

  // Build edges
  const edges = new Map();
  for (let fi = 0; fi < mesh.faces.length; fi++) {
    const f = mesh.faces[fi];
    for (let i = 0; i < 3; i++) {
      const k = edgeKey(f[i], f[(i + 1) % 3]);
      if (!edges.has(k)) edges.set(k, { v: [Math.min(f[i], f[(i+1)%3]), Math.max(f[i], f[(i+1)%3])], faces: [] });
      edges.get(k).faces.push(fi);
    }
  }

  // Vertex adjacency
  const origVC = mesh.verts.length;
  const adj = Array.from({ length: origVC }, () => ({ nbrs: new Set(), ekeys: [] }));
  for (const [k, e] of edges) {
    adj[e.v[0]].nbrs.add(e.v[1]); adj[e.v[1]].nbrs.add(e.v[0]);
    adj[e.v[0]].ekeys.push(k);    adj[e.v[1]].ekeys.push(k);
  }

  // Helper: weighted average of two vec3 arrays
  function lerp3(a, b, t) {
    return [a[0] + (b[0] - a[0]) * t,
            a[1] + (b[1] - a[1]) * t,
            a[2] + (b[2] - a[2]) * t];
  }
  function add3(a, b) { return [a[0] + b[0], a[1] + b[1], a[2] + b[2]]; }
  function scale3(a, s) { return [a[0] * s, a[1] * s, a[2] * s]; }
  function mid3(a, b) { return scale3(add3(a, b), 0.5); }

  // 1) Edge points
  const edgeMid = new Map();
  for (const [k, e] of edges) {
    const a = mesh.verts[e.v[0]], b = mesh.verts[e.v[1]];
    let p;
    if (e.faces.length === 2) {
      // Interior edge: 3/8 * (a + b) + 1/8 * (c + d)
      const c = mesh.verts[mesh.faces[e.faces[0]].find(v => v !== e.v[0] && v !== e.v[1])];
      const d = mesh.verts[mesh.faces[e.faces[1]].find(v => v !== e.v[0] && v !== e.v[1])];
      p = add3(scale3(add3(a, b), 3 / 8), scale3(add3(c, d), 1 / 8));
    } else {
      p = mid3(a, b);
    }
    edgeMid.set(k, mesh.verts.length);
    mesh.verts.push(p);
  }

  // 2) Reposition original vertices
  const newPos = [];
  for (let i = 0; i < origVC; i++) {
    const v = mesh.verts[i], nbrs = [...adj[i].nbrs], n = nbrs.length;
    if (n < 2) { newPos.push([...v]); continue; }

    const boundary = adj[i].ekeys.some(k => edges.get(k).faces.length < 2);
    if (boundary) {
      // Boundary: 3/4 * v + 1/8 * each boundary neighbor
      const bn = [];
      for (const k of adj[i].ekeys) {
        const e = edges.get(k);
        if (e.faces.length < 2) bn.push(e.v[0] === i ? e.v[1] : e.v[0]);
      }
      let p = scale3(v, 3 / 4);
      for (const ni of bn) p = add3(p, scale3(mesh.verts[ni], 1 / 8));
      newPos.push(p);
    } else {
      // Interior: Warren's beta formula
      let beta = 3 + 2 * Math.cos(2 * Math.PI / n);
      beta = 5 / 8 - (beta * beta) / 64;
      let p = scale3(v, 1 - beta);
      for (const ni of nbrs) p = add3(p, scale3(mesh.verts[ni], beta / n));
      newPos.push(p);
    }
  }
  for (let i = 0; i < origVC; i++) mesh.verts[i] = newPos[i];

  // 3) Split each triangle into 4
  const nf = [];
  for (const [a, b, c] of mesh.faces) {
    const ab = edgeMid.get(edgeKey(a, b));
    const bc = edgeMid.get(edgeKey(b, c));
    const ca = edgeMid.get(edgeKey(c, a));
    nf.push([a, ab, ca], [ab, b, bc], [ca, bc, c], [ab, bc, ca]);
  }
  mesh.faces = nf;
  return mesh;
}

function writeMesh(idx, mesh) {
  clearMesh(idx);
  for (const v of mesh.verts) addMeshVertex(idx, v[0], v[1], v[2]);
  for (const f of mesh.faces) addMeshTriangle(idx, f[0], f[1], f[2]);
  rebuildMesh(idx);
}

// ── Main ───────────────────────────────────────────────────────────
let found = -1;
for (let i = 0; i < getItemCount(); i++) {
  if (isItemSelected(i)) { found = i; break; }
}

if (found < 0) {
  log('Select an item first.');
} else {
  let mesh = readMesh(found);
  log('Start: ' + mesh.verts.length + ' verts, ' + mesh.faces.length + ' faces');
  for (let i = 0; i < iterations; i++) {
    mesh = loopSubdivide(mesh);
    log('Pass ' + (i+1) + ': ' + mesh.verts.length + ' verts, ' + mesh.faces.length + ' faces');
  }
  writeMesh(found, mesh);
  log('Done!');
}
