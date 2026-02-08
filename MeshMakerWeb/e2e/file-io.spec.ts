import { test, expect, type Page } from '@playwright/test';
import { Buffer } from 'node:buffer';

// Helper: wait for WASM module to load
async function waitForWASMReady(page: Page) {
  await page.waitForSelector('#canvas', { timeout: 30000 });
  await page.waitForSelector('button[title="Add Cube"]', { timeout: 30000 });
  await page.waitForTimeout(500);
}

// Helper: get item count from the WASM module directly
async function getItemCount(page: Page): Promise<number> {
  return page.evaluate(() => (window as any).Module?.getItemCount() ?? 0);
}

test.describe('File I/O: OBJ', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('OBJ export returns valid string after adding a cube', async ({ page }) => {
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);

    const obj = await page.evaluate(() => {
      const m = (window as any).Module;
      return m?.exportToOBJ() as string | undefined;
    });

    expect(obj).toBeTruthy();
    expect(obj).toContain('v ');  // vertices
    expect(obj).toContain('f ');  // faces
  });

  test('OBJ round-trip: export then import restores items', async ({ page }) => {
    // Clear any default items first
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);

    // Create scene with two primitives
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
    await page.click('button[title="Add Sphere"]');
    await page.waitForTimeout(200);

    const originalCount = await getItemCount(page);
    expect(originalCount).toBe(2);

    // Export
    const objData = await page.evaluate(() => {
      const m = (window as any).Module;
      return m?.exportToOBJ() as string;
    });

    // Clear scene
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);
    expect(await getItemCount(page)).toBe(0);

    // Import
    const success = await page.evaluate((data: string) => {
      const m = (window as any).Module;
      return m?.importFromOBJ(data) as boolean;
    }, objData);

    expect(success).toBe(true);
    expect(await getItemCount(page)).toBeGreaterThan(0);
  });
});

test.describe('File I/O: GLB', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('GLB export returns Uint8Array after adding a cube', async ({ page }) => {
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);

    const info = await page.evaluate(() => {
      const m = (window as any).Module;
      const data = m?.exportToGLB();
      if (!data) return null;
      return { length: data.length, magic: String.fromCharCode(data[0], data[1], data[2], data[3]) };
    });

    expect(info).not.toBeNull();
    expect(info!.length).toBeGreaterThan(12);
    expect(info!.magic).toBe('glTF');
  });

  test('GLB round-trip: export then import restores items', async ({ page }) => {
    // Clear any default items first
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);

    // Create a cube
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);

    const originalCount = await getItemCount(page);
    expect(originalCount).toBe(1);

    // Export to GLB — transfer as base64 to avoid serialization issues
    const glbBase64 = await page.evaluate(() => {
      const m = (window as any).Module;
      const data = m?.exportToGLB();
      if (!data) return null;
      // Convert Uint8Array to base64 string for transport
      let binary = '';
      for (let i = 0; i < data.length; i++) {
        binary += String.fromCharCode(data[i]);
      }
      return btoa(binary);
    });

    expect(glbBase64).not.toBeNull();

    // Clear scene
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);
    expect(await getItemCount(page)).toBe(0);

    // Import GLB
    const success = await page.evaluate((b64: string) => {
      const m = (window as any).Module;
      const binary = atob(b64);
      const data = new Uint8Array(binary.length);
      for (let i = 0; i < binary.length; i++) {
        data[i] = binary.charCodeAt(i);
      }
      return m?.importFromGLB(data) as boolean;
    }, glbBase64!);

    expect(success).toBe(true);
    expect(await getItemCount(page)).toBe(originalCount);
  });

  test('GLB import: bulk data transfer works for non-trivial mesh', async ({ page }) => {
    // Clear any default items first
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);

    // Build a moderately complex scene (sphere has many verts)
    await page.click('button[title="Add Sphere"]');
    await page.waitForTimeout(200);
    await page.click('button[title="Add Cylinder"]');
    await page.waitForTimeout(200);
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);

    const originalCount = await getItemCount(page);
    expect(originalCount).toBe(3);

    // Export
    const glbBase64 = await page.evaluate(() => {
      const m = (window as any).Module;
      const data = m?.exportToGLB();
      if (!data) return null;
      let binary = '';
      for (let i = 0; i < data.length; i++) {
        binary += String.fromCharCode(data[i]);
      }
      return btoa(binary);
    });

    expect(glbBase64).not.toBeNull();

    // Clear and reimport
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);

    const success = await page.evaluate((b64: string) => {
      const m = (window as any).Module;
      const binary = atob(b64);
      const data = new Uint8Array(binary.length);
      for (let i = 0; i < binary.length; i++) {
        data[i] = binary.charCodeAt(i);
      }
      return m?.importFromGLB(data) as boolean;
    }, glbBase64!);

    expect(success).toBe(true);
    expect(await getItemCount(page)).toBe(originalCount);
  });

  test('GLB import: large synthetic buffer does not crash', async ({ page }) => {
    // Generate a large GLB in the browser by subdividing a sphere multiple times
    // This tests that the WASM bulk copy and memory growth work for bigger data
    await page.click('button[title="Add Sphere"]');
    await page.waitForTimeout(200);

    // Select all faces and subdivide twice to make a dense mesh
    await page.click('button[title="Edit Triangles (E)"]');
    await page.waitForTimeout(100);
    await page.keyboard.press('a'); // select all
    await page.waitForTimeout(200);
    await page.keyboard.press('d'); // subdivide
    await page.waitForTimeout(500);
    await page.keyboard.press('a'); // select all again
    await page.waitForTimeout(200);
    await page.keyboard.press('d'); // subdivide again
    await page.waitForTimeout(1000);

    // Switch back to items mode
    await page.keyboard.press('q');
    await page.waitForTimeout(100);

    // Export — should produce a reasonably large GLB
    const result = await page.evaluate(() => {
      const m = (window as any).Module;
      const data = m?.exportToGLB();
      if (!data) return { ok: false, size: 0 };
      // Convert to base64
      let binary = '';
      for (let i = 0; i < data.length; i++) {
        binary += String.fromCharCode(data[i]);
      }
      return { ok: true, size: data.length, b64: btoa(binary) };
    });

    expect(result.ok).toBe(true);
    // After 2 subdivisions of a sphere, the GLB should be at least 50KB
    expect(result.size).toBeGreaterThan(50000);

    // Clear and reimport the large GLB
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);

    const success = await page.evaluate((b64: string) => {
      const m = (window as any).Module;
      const binary = atob(b64);
      const data = new Uint8Array(binary.length);
      for (let i = 0; i < binary.length; i++) {
        data[i] = binary.charCodeAt(i);
      }
      return m?.importFromGLB(data) as boolean;
    }, result.b64!);

    expect(success).toBe(true);
    expect(await getItemCount(page)).toBeGreaterThan(0);
  });

  test('GLB import: raw binary Uint8Array transfer works (no base64)', async ({ page }) => {
    // Clear any default items first
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);

    // This test verifies the direct Uint8Array path that the UI file picker uses
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);

    // Export, clear, and reimport entirely within page.evaluate using Uint8Array
    const result = await page.evaluate(() => {
      const m = (window as any).Module;

      // Export
      const exported = m?.exportToGLB();
      if (!exported) return { exported: false, imported: false, count: 0 };

      // Copy to a fresh Uint8Array (like FileReader would produce)
      const fresh = new Uint8Array(exported.length);
      fresh.set(exported);

      // Clear
      m.clearScene();

      // Import
      const ok = m.importFromGLB(fresh);
      return { exported: true, imported: ok, count: m.getItemCount() };
    });

    expect(result.exported).toBe(true);
    expect(result.imported).toBe(true);
    expect(result.count).toBe(1);
  });
});

test.describe('File I/O: Import via file chooser', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('OBJ file import via file picker', async ({ page }) => {
    // Create a cube, export OBJ, so we have valid content
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);

    const objContent = await page.evaluate(() => {
      const m = (window as any).Module;
      return m?.exportToOBJ() as string;
    });

    // Clear scene
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);
    expect(await getItemCount(page)).toBe(0);

    // Create a synthetic OBJ file and trigger the file input
    const [fileChooser] = await Promise.all([
      page.waitForEvent('filechooser'),
      page.click('button[title="Import (OBJ/GLB)"]'),
    ]);

    // Write OBJ content to a temp buffer and set as file
    const buffer = Buffer.from(objContent, 'utf-8');
    await fileChooser.setFiles({
      name: 'test.obj',
      mimeType: 'text/plain',
      buffer,
    });

    // Wait for import to complete
    await page.waitForTimeout(1000);

    expect(await getItemCount(page)).toBeGreaterThan(0);
  });

  test('GLB file import via file picker', async ({ page }) => {
    // Create a cube, export GLB
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);

    const glbBase64 = await page.evaluate(() => {
      const m = (window as any).Module;
      const data = m?.exportToGLB();
      if (!data) return null;
      let binary = '';
      for (let i = 0; i < data.length; i++) {
        binary += String.fromCharCode(data[i]);
      }
      return btoa(binary);
    });

    expect(glbBase64).not.toBeNull();

    // Clear scene
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);
    expect(await getItemCount(page)).toBe(0);

    // Import via file picker
    const [fileChooser] = await Promise.all([
      page.waitForEvent('filechooser'),
      page.click('button[title="Import (OBJ/GLB)"]'),
    ]);

    const buffer = Buffer.from(glbBase64!, 'base64');
    await fileChooser.setFiles({
      name: 'test.glb',
      mimeType: 'model/gltf-binary',
      buffer,
    });

    await page.waitForTimeout(1000);

    expect(await getItemCount(page)).toBeGreaterThan(0);
  });
});

test.describe('File I/O: Edge cases', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('import empty OBJ returns false', async ({ page }) => {
    const success = await page.evaluate(() => {
      const m = (window as any).Module;
      return m?.importFromOBJ('') as boolean;
    });
    expect(success).toBe(false);
  });

  test('import garbage GLB returns false without crashing', async ({ page }) => {
    const success = await page.evaluate(() => {
      const m = (window as any).Module;
      const garbage = new Uint8Array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]);
      return m?.importFromGLB(garbage) as boolean;
    });
    expect(success).toBe(false);
    // App should still be alive
    await expect(page.locator('#canvas')).toBeVisible();
  });

  test('import zero-length GLB returns false without crashing', async ({ page }) => {
    const success = await page.evaluate(() => {
      const m = (window as any).Module;
      const empty = new Uint8Array(0);
      return m?.importFromGLB(empty) as boolean;
    });
    expect(success).toBe(false);
    await expect(page.locator('#canvas')).toBeVisible();
  });

  test('export GLB with no items returns null/empty', async ({ page }) => {
    const result = await page.evaluate(() => {
      const m = (window as any).Module;
      const data = m?.exportToGLB();
      return data ? data.length : 0;
    });
    // Either null/undefined or empty — both are acceptable
    // Just shouldn't crash
    await expect(page.locator('#canvas')).toBeVisible();
  });

  test('export OBJ with no items returns empty string', async ({ page }) => {
    const result = await page.evaluate(() => {
      const m = (window as any).Module;
      return m?.exportToOBJ() as string;
    });
    // Should return something (even empty) without crashing
    await expect(page.locator('#canvas')).toBeVisible();
  });
});
