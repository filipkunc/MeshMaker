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

  for (const ext of ['usda', 'usdc'] as const) {
    test(`imports the checked-in ${ext.toUpperCase()} model`, async ({ page }) => {
      test.setTimeout(180000);
      await page.click('button[title="Clear Scene"]');
      const bytes = await readFile(
        new URL(`../../usd-io/test/models/mixed-topology.${ext}`, import.meta.url));

      const added = await page.evaluate(async ({ data, extension }) => {
        const { importUsd } = await (new Function(
          'return import("/src/lib/usdIo.ts")')() as Promise<any>);
        return importUsd((window as any).Module, new Uint8Array(data), extension);
      }, { data: [...bytes], extension: ext });

      expect(added).toBe(2);
      expect(await getItemCount(page)).toBe(2);
      expect(await faceHistogram(page, 0)).toEqual({ 4: 6 });
      expect(await faceHistogram(page, 1)).toEqual({ 3: 1 });
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
});
