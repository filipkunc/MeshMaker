import { test, expect, type Page } from '@playwright/test';

// Helper: wait for WASM module to load
async function waitForWASMReady(page: Page) {
  await page.waitForSelector('#canvas', { timeout: 30000 });
  await page.waitForSelector('button[title="Add Cube"]', { timeout: 30000 });
  await page.waitForTimeout(500);
}

// Helper: check if a toolbar button has the active (blue) background
async function isButtonActive(page: Page, title: string): Promise<boolean> {
  const button = page.locator(`button[title="${title}"]`);
  const classes = await button.getAttribute('class');
  return classes?.includes('bg-blue-600') ?? false;
}

// Helper: press a key on the canvas so GLFW receives it (not an HTML input)
async function pressKeyOnCanvas(page: Page, key: string) {
  await page.locator('#canvas').press(key);
  // Wait for React to sync state from WASM
  await page.waitForTimeout(300);
}

test.describe('Transform Mode Keyboard Shortcuts Update Toolbar', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('Select button is active by default', async ({ page }) => {
    await expect(page.locator('button[title="Select (1)"]')).toBeVisible();
    expect(await isButtonActive(page, 'Select (1)')).toBe(true);
    expect(await isButtonActive(page, 'Translate (2)')).toBe(false);
    expect(await isButtonActive(page, 'Rotate (3)')).toBe(false);
    expect(await isButtonActive(page, 'Scale (4)')).toBe(false);
  });

  test('pressing 2 activates Translate button', async ({ page }) => {
    await pressKeyOnCanvas(page, '2');

    expect(await isButtonActive(page, 'Select (1)')).toBe(false);
    expect(await isButtonActive(page, 'Translate (2)')).toBe(true);
    expect(await isButtonActive(page, 'Rotate (3)')).toBe(false);
    expect(await isButtonActive(page, 'Scale (4)')).toBe(false);
  });

  test('pressing 3 activates Rotate button', async ({ page }) => {
    await pressKeyOnCanvas(page, '3');

    expect(await isButtonActive(page, 'Select (1)')).toBe(false);
    expect(await isButtonActive(page, 'Translate (2)')).toBe(false);
    expect(await isButtonActive(page, 'Rotate (3)')).toBe(true);
    expect(await isButtonActive(page, 'Scale (4)')).toBe(false);
  });

  test('pressing 4 activates Scale button', async ({ page }) => {
    await pressKeyOnCanvas(page, '4');

    expect(await isButtonActive(page, 'Select (1)')).toBe(false);
    expect(await isButtonActive(page, 'Translate (2)')).toBe(false);
    expect(await isButtonActive(page, 'Rotate (3)')).toBe(false);
    expect(await isButtonActive(page, 'Scale (4)')).toBe(true);
  });

  test('pressing 1 returns to Select after switching', async ({ page }) => {
    await pressKeyOnCanvas(page, '2');
    expect(await isButtonActive(page, 'Translate (2)')).toBe(true);

    await pressKeyOnCanvas(page, '1');
    expect(await isButtonActive(page, 'Select (1)')).toBe(true);
    expect(await isButtonActive(page, 'Translate (2)')).toBe(false);
  });

  test('cycling through all modes with keyboard', async ({ page }) => {
    // Start at Select
    expect(await isButtonActive(page, 'Select (1)')).toBe(true);

    // Cycle: 2 -> 3 -> 4 -> 1
    await pressKeyOnCanvas(page, '2');
    expect(await isButtonActive(page, 'Translate (2)')).toBe(true);

    await pressKeyOnCanvas(page, '3');
    expect(await isButtonActive(page, 'Rotate (3)')).toBe(true);

    await pressKeyOnCanvas(page, '4');
    expect(await isButtonActive(page, 'Scale (4)')).toBe(true);

    await pressKeyOnCanvas(page, '1');
    expect(await isButtonActive(page, 'Select (1)')).toBe(true);
  });
});
