import { test, expect, type Page } from '@playwright/test';
import { readFile } from 'node:fs/promises';

// USD import/export goes through the lazy-loaded OpenUSD wasm module
// (public/usd-io/, ~10 MB), which needs cross-origin isolation — these tests
// also guard the COOP/COEP headers in vite.config.ts.

async function waitForWASMReady(page: Page) {
  await page.waitForSelector('#canvas', { timeout: 30000 });
  await page.waitForSelector('button[title="Add Cube"]', { timeout: 30000 });
  await page.waitForTimeout(500);
}

async function getItemCount(page: Page): Promise<number> {
  return page.evaluate(() => (window as any).Module?.getItemCount() ?? 0);
}

// Corner-count histogram of one item's faces, e.g. a cube -> { 4: 6 }
async function faceHistogram(page: Page, item: number): Promise<Record<number, number>> {
  return page.evaluate((itemIndex: number) => {
    const m = (window as any).Module;
    const hist: Record<number, number> = {};
    const faces = m.getItemFaceCount(itemIndex);
    for (let f = 0; f < faces; f++) {
      const c = m.getFaceVertexCount(itemIndex, f);
      hist[c] = (hist[c] ?? 0) + 1;
    }
    return hist;
  }, item);
}

test.describe('File I/O: USD', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('page is cross-origin isolated (COOP/COEP headers present)', async ({ page }) => {
    expect(await page.evaluate(() => crossOriginIsolated)).toBe(true);
  });

  test('USD round-trip: quads and triangles survive, GLB triangulates', async ({ page }) => {
    test.setTimeout(180000); // first call fetches + instantiates the 10 MB module

    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
    await page.click('button[title="Add Cylinder"]');
    await page.waitForTimeout(200);
    expect(await getItemCount(page)).toBe(2);

    const cubeBefore = await faceHistogram(page, 0);
    const cylBefore = await faceHistogram(page, 1);
    expect(cubeBefore).toEqual({ 4: 6 }); // a cube is six quads
    expect(cylBefore[4]).toBeGreaterThan(0); // cylinder sides are quads

    // Export through the real OpenUSD module (string-form evaluate: the
    // dynamic import must reach the browser untranspiled).
    const usda: string = await page.evaluate(`(async () => {
      const { exportUsd } = await import('/src/lib/usdIo.ts');
      const bytes = await exportUsd(window.Module, 'usda');
      return new TextDecoder().decode(bytes);
    })()`) as string;

    expect(usda).toContain('#usda 1.0');
    expect(usda).toContain('def Mesh "Item_0');
    expect(usda).toContain('def Mesh "Item_1');
    expect(usda).toContain('subdivisionScheme = "none"');

    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);
    expect(await getItemCount(page)).toBe(0);

    const added: number = await page.evaluate(`(async (text) => {
      const { importUsd } = await import('/src/lib/usdIo.ts');
      return importUsd(window.Module, new TextEncoder().encode(text), 'usda');
    })(${JSON.stringify(usda)})`) as number;

    expect(added).toBe(2);
    expect(await getItemCount(page)).toBe(2);

    // The headline claim: topology survives USD exactly.
    expect(await faceHistogram(page, 0)).toEqual(cubeBefore);
    expect(await faceHistogram(page, 1)).toEqual(cylBefore);
  });

  for (const ext of ['usda', 'usdc', 'usdz'] as const) {
    test(`imports the checked-in ${ext.toUpperCase()} model`, async ({ page }) => {
      test.setTimeout(180000);
      await page.click('button[title="Clear Scene"]');
      const bytes = await readFile(
        new URL(`../../usd-io/test/models/${ext === 'usdz' ? 'textured' : 'mixed-topology'}.${ext}`, import.meta.url));

      const added = await page.evaluate(async ({ data, extension }) => {
        const { importUsd } = await (new Function(
          'return import("/src/lib/usdIo.ts")')() as Promise<any>);
        return importUsd((window as any).Module, new Uint8Array(data), extension);
      }, { data: [...bytes], extension: ext });

      expect(added).toBe(2);
      expect(await getItemCount(page)).toBe(2);
      expect(await faceHistogram(page, 0)).toEqual({ 4: 6 });
      expect(await faceHistogram(page, 1)).toEqual({ 3: 1 });
      expect(await page.evaluate(() => (window as any).Module.itemHasTexture(0)))
        .toBe(ext === 'usdz');
    });
  }

  test('writes valid USDA and USDC and reads both back', async ({ page }) => {
    test.setTimeout(180000);
    await page.click('button[title="Clear Scene"]');
    await page.click('button[title="Add Cube"]');

    const result = await page.evaluate(async () => {
      const { exportUsd, importUsd } = await (new Function(
        'return import("/src/lib/usdIo.ts")')() as Promise<any>);
      const mm = (window as any).Module;
      const usda = await exportUsd(mm, 'usda');
      const usdc = await exportUsd(mm, 'usdc');
      mm.clearScene();
      const fromUsda = await importUsd(mm, usda, 'usda');
      mm.clearScene();
      const fromUsdc = await importUsd(mm, usdc, 'usdc');
      return {
        usdaHeader: new TextDecoder().decode(usda.subarray(0, 9)),
        usdcHeader: new TextDecoder().decode(usdc.subarray(0, 8)),
        fromUsda,
        fromUsdc,
      };
    });

    expect(result).toEqual({
      usdaHeader: '#usda 1.0',
      usdcHeader: 'PXR-USDC',
      fromUsda: 1,
      fromUsdc: 1,
    });
    expect(await faceHistogram(page, 0)).toEqual({ 4: 6 });
  });

  test('USDZ round-trip preserves face-varying UVs and bitmap texture', async ({ page }) => {
    test.setTimeout(180000);
    await page.click('button[title="Clear Scene"]');
    await page.click('button[title="Add Cube"]');

    const result = await page.evaluate(async ({ pngBase64, normalBase64 }) => {
      const mm = (window as any).Module;
      const png = Uint8Array.from(atob(pngBase64), (char) => char.charCodeAt(0));
      const normalPng = Uint8Array.from(atob(normalBase64), (char) => char.charCodeAt(0));
      mm.setFaceUV(0, 0, 0, 0.125, 0.875);
      mm.setItemMaterial(0, 0.8, 0.2, 0.1, 0.75, 0.6, 0.3);
      mm.setItemAdvancedMaterial(0, 0.05, 0.1, 0.2, 0.7, 0.15, 1.45);
      if (!mm.setItemTextureFromFileData(0, png)) throw new Error('fixture PNG decode failed');
      if (!mm.setItemNormalTextureFromFileData(0, normalPng)) throw new Error('normal PNG decode failed');
      const { exportUsd, importUsd } = await (new Function(
        'return import("/src/lib/usdIo.ts")')() as Promise<any>);
      const usdz = await exportUsd(mm, 'usdz');
      mm.clearScene();
      const added = await importUsd(mm, usdz, 'usdz');
      return {
        magic: new TextDecoder().decode(usdz.subarray(0, 2)),
        added,
        textured: mm.itemHasTexture(0),
        normalMapped: mm.itemHasNormalTexture(0),
        width: mm.getItemTextureWidth(0),
        height: mm.getItemTextureHeight(0),
        u: mm.getFaceUVX(0, 0, 0),
        v: mm.getFaceUVY(0, 0, 0),
        material: [mm.getItemBaseColorR(0), mm.getItemBaseColorG(0),
          mm.getItemBaseColorB(0), mm.getItemOpacity(0),
          mm.getItemMetallic(0), mm.getItemRoughness(0),
          mm.getItemEmissiveR(0), mm.getItemEmissiveG(0), mm.getItemEmissiveB(0),
          mm.getItemClearcoat(0), mm.getItemClearcoatRoughness(0), mm.getItemIor(0)],
      };
    }, {
      pngBase64: 'iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAYAAABytg0kAAAAIGNIUk0AAHomAACAhAAA+gAAAIDoAAB1MAAA6mAAADqYAAAXcJy6UTwAAAAGYktHRAD/AP8A/6C9p5MAAAAHdElNRQfqCA0TNyrszXWTAAAAJXRFWHRkYXRlOmNyZWF0ZQAyMDI2LTA4LTEzVDE5OjU1OjQyKzAwOjAwR5PNDQAAACV0RVh0ZGF0ZTptb2RpZnkAMjAyNi0wOC0xM1QxOTo1NTo0MiswMDowMDbOdbEAAAAodEVYdGRhdGU6dGltZXN0YW1wADIwMjYtMDgtMTNUMTk6NTU6NDIrMDA6MDBh21RuAAAAFElEQVQI12P8z8Dwn4GBgYGJAQoAHxcCAmep4oIAAAAASUVORK5CYII=',
      normalBase64: 'iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAYAAABytg0kAAAAIGNIUk0AAHomAACAhAAA+gAAAIDoAAB1MAAA6mAAADqYAAAXcJy6UTwAAAAGYktHRAD/AP8A/6C9p5MAAAAHdElNRQfqCA8UMgYMJDM7AAAAJXRFWHRkYXRlOmNyZWF0ZQAyMDI2LTA4LTE1VDIwOjUwOjA2KzAwOjAwedpqdwAAACV0RVh0ZGF0ZTptb2RpZnkAMjAyNi0wOC0xNVQyMDo1MDowNiswMDowMAiH0ssAAAAodEVYdGRhdGU6dGltZXN0YW1wADIwMjYtMDgtMTVUMjA6NTA6MDYrMDA6MDBfkvMUAAAAFElEQVQI12NsaPj/n4GBgYGJAQoALZkDAoFnMKoAAAAASUVORK5CYII=',
    });

    expect(result.magic).toBe('PK');
    expect(result.added).toBe(1);
    expect(result.textured).toBe(true);
    expect(result.normalMapped).toBe(true);
    expect(result.width).toBe(2);
    expect(result.height).toBe(2);
    expect(result.u).toBeCloseTo(0.125);
    expect(result.v).toBeCloseTo(0.875);
    expect(result.material).toEqual([
      expect.closeTo(0.8), expect.closeTo(0.2), expect.closeTo(0.1),
      expect.closeTo(0.75), expect.closeTo(0.6), expect.closeTo(0.3),
      expect.closeTo(0.05), expect.closeTo(0.1), expect.closeTo(0.2),
      expect.closeTo(0.7), expect.closeTo(0.15), expect.closeTo(1.45),
    ]);
  });
});
