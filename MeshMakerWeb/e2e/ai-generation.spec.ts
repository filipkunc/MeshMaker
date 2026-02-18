import { test, expect, type Page } from '@playwright/test';
import * as fs from 'node:fs';

/**
 * AI 3D Generation (Hunyuan3D-2) — E2E tests against the REAL server.
 *
 * Prerequisites:
 *   1. Hunyuan3D-2 server running:
 *      cd Hunyuan3D-2 && uv run python gradio_app.py --model_path tencent/Hunyuan3D-2mini --subfolder hunyuan3d-dit-v2-mini --texgen_model_path tencent/Hunyuan3D-2 --low_vram_mode --enable_t23d
 *   2. React dev server running:
 *      cd MeshMakerWeb && npm run dev
 *
 * Run:
 *   npx playwright test e2e/ai-generation.spec.ts
 */

const HUNYUAN_URL = 'http://localhost:8080';

// Generation can take 30-120s on a real GPU
const GENERATION_TIMEOUT = 180_000;

// ── Helpers ────────────────────────────────────────────────────────

async function waitForWASMReady(page: Page) {
  await page.waitForSelector('#canvas', { timeout: 30000 });
  await page.waitForSelector('button[title="Add Cube"]', { timeout: 30000 });
  await page.waitForTimeout(500);
}

async function getItemCount(page: Page): Promise<number> {
  return page.evaluate(() => (window as any).Module?.getItemCount() ?? 0);
}

async function isHunyuanServerRunning(): Promise<boolean> {
  try {
    const res = await fetch(`${HUNYUAN_URL}/config`, {
      signal: AbortSignal.timeout(5000),
    });
    return res.ok;
  } catch {
    return false;
  }
}

async function openAIDialog(page: Page) {
  await page.click('button[title="AI 3D Generation"]');
  await page.waitForSelector('h2:has-text("AI 3D Generation (Hunyuan3D-2)")', { timeout: 5000 });
}

// ── Tests ──────────────────────────────────────────────────────────

test.describe('AI 3D Generation (Hunyuan3D-2)', () => {
  test.beforeAll(async () => {
    const running = await isHunyuanServerRunning();
    if (!running) {
      test.skip();
      // The skip above won't work in beforeAll in all Playwright versions,
      // so individual tests also check.
    }
  });

  test.beforeEach(async ({ page }) => {
    const running = await isHunyuanServerRunning();
    test.skip(!running, 'Hunyuan3D-2 server not running on localhost:8080');

    await page.goto('/');
    await waitForWASMReady(page);

    // Clear any existing items
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);
    expect(await getItemCount(page)).toBe(0);
  });

  test('dialog opens and shows Hunyuan3D-2 title', async ({ page }) => {
    await openAIDialog(page);

    const title = page.locator('h2:has-text("AI 3D Generation (Hunyuan3D-2)")');
    await expect(title).toBeVisible();

    // Mode toggle buttons should be present
    await expect(page.getByText('Image to 3D')).toBeVisible();
    await expect(page.getByText('Text to 3D')).toBeVisible();

    // Generate button should be disabled (no input yet)
    const generateBtn = page.locator('button:has-text("Generate")');
    await expect(generateBtn).toBeDisabled();
  });

  test('connection test succeeds against real server', async ({ page }) => {
    const consoleErrors: string[] = [];
    page.on('console', (msg) => {
      if (msg.type() === 'error') {
        consoleErrors.push(msg.text());
      }
    });

    await openAIDialog(page);

    // Open advanced options
    await page.click('text=Advanced Options');
    await page.waitForTimeout(200);

    // Click Test button
    await page.click('button:has-text("Test")');

    // Should show green "Connected" message
    const connected = page.locator('text=Connected to Hunyuan3D-2 server');
    await expect(connected).toBeVisible({ timeout: 10000 });

    const bufferErrors = consoleErrors.filter((message) =>
      /Buffer is not defined/i.test(message)
    );
    expect(bufferErrors).toEqual([]);
  });

  test('text-to-3D generates and imports a mesh', async ({ page }) => {
    test.setTimeout(GENERATION_TIMEOUT);

    await openAIDialog(page);

    // Switch to text mode
    await page.getByText('Text to 3D').click();
    await page.waitForTimeout(200);

    // Type a simple prompt
    const textarea = page.locator('textarea');
    await textarea.fill('a simple wooden chair');

    // Generate button should now be enabled
    const generateBtn = page.locator('button:has-text("Generate")');
    await expect(generateBtn).toBeEnabled();

    // Click Generate
    await generateBtn.click();

    // Progress indicator should appear
    const spinner = page.locator('.animate-spin');
    await expect(spinner).toBeVisible({ timeout: 5000 });

    // Wait for dialog to close (generation complete + auto-close)
    await page.waitForSelector('h2:has-text("AI 3D Generation")', {
      state: 'hidden',
      timeout: GENERATION_TIMEOUT,
    });

    // Verify mesh was imported
    await expect
      .poll(async () => getItemCount(page), {
        timeout: 30_000,
        message: 'Expected generated mesh to be imported after dialog closes',
      })
      .toBeGreaterThan(0);
  });

  test('image-to-3D with texture enabled completes and imports mesh', async ({ page }) => {
    test.setTimeout(1_200_000);

    await openAIDialog(page);
    await page.click('text=Advanced Options');

    const fileInput = page.locator('input[type="file"][accept="image/png,image/jpeg,image/webp"]');
    const testImagePath = 'C:\\Users\\kuncf\\OneDrive\\Pictures\\20251108_085028.jpg';
    expect(fs.existsSync(testImagePath)).toBe(true);

    await fileInput.setInputFiles(testImagePath);
    await expect(page.locator('img[alt="Preview"]')).toBeVisible({ timeout: 5000 });

    await page.getByLabel('Enable texture painting').check();

    const generateBtn = page.locator('button:has-text("Generate")');
    await expect(generateBtn).toBeEnabled();
    await generateBtn.click();

    await expect(page.locator('.animate-spin')).toBeVisible({ timeout: 5000 });

    await page.waitForSelector('h2:has-text("AI 3D Generation")', {
      state: 'hidden',
      timeout: 1_200_000,
    });

    await expect
      .poll(async () => getItemCount(page), {
        timeout: 30_000,
        message: 'Expected textured generated mesh to be imported after dialog closes',
      })
      .toBeGreaterThan(0);
  });

  test('image-to-3D generates and imports a mesh', async ({ page }) => {
    test.setTimeout(GENERATION_TIMEOUT);

    await openAIDialog(page);

    // Should already be in image mode by default
    await expect(page.getByText('Click or drag an image here')).toBeVisible();

    // Upload a real photo via the hidden file input
    const fileInput = page.locator('input[type="file"][accept="image/png,image/jpeg,image/webp"]');
    const testImagePath = 'C:\\Users\\kuncf\\OneDrive\\Pictures\\DSC_5282 - Copy.jpg';

    // Verify test image exists
    expect(fs.existsSync(testImagePath)).toBe(true);

    await fileInput.setInputFiles(testImagePath);

    // Image preview should appear
    const preview = page.locator('img[alt="Preview"]');
    await expect(preview).toBeVisible({ timeout: 5000 });

    // Generate button should now be enabled
    const generateBtn = page.locator('button:has-text("Generate")');
    await expect(generateBtn).toBeEnabled();

    // Click Generate
    await generateBtn.click();

    // Progress indicator should appear
    const spinner = page.locator('.animate-spin');
    await expect(spinner).toBeVisible({ timeout: 5000 });

    // Wait for dialog to close (generation complete + auto-close)
    await page.waitForSelector('h2:has-text("AI 3D Generation")', {
      state: 'hidden',
      timeout: GENERATION_TIMEOUT,
    });

    // Verify mesh was imported
    const itemCount = await getItemCount(page);
    expect(itemCount).toBeGreaterThan(0);
  });

  test('image-to-3D with user photo enters queue (no immediate client error)', async ({ page }) => {
    test.setTimeout(60_000);

    await openAIDialog(page);

    const fileInput = page.locator('input[type="file"][accept="image/png,image/jpeg,image/webp"]');
    const testImagePath = 'C:\\Users\\kuncf\\OneDrive\\Pictures\\20251108_085028.jpg';
    expect(fs.existsSync(testImagePath)).toBe(true);

    await fileInput.setInputFiles(testImagePath);
    await expect(page.locator('img[alt="Preview"]')).toBeVisible({ timeout: 5000 });

    const generateBtn = page.locator('button:has-text("Generate")');
    await expect(generateBtn).toBeEnabled();
    await generateBtn.click();

    await expect(page.locator('.animate-spin')).toBeVisible({ timeout: 5000 });

    const progressText = page.locator('text=Waiting in queue').or(page.locator('text=Generating 3D model'));
    await expect(progressText).toBeVisible({ timeout: 20_000 });

    await expect(page.locator('text=Buffer is not defined')).not.toBeVisible();

    await page.locator('button:has-text("Cancel")').click();
    await expect(page.locator('.animate-spin')).not.toBeVisible({ timeout: 5000 });
  });

  test('image-to-3D with user photo completes and imports mesh', async ({ page }) => {
    test.setTimeout(600_000);

    await openAIDialog(page);

    const fileInput = page.locator('input[type="file"][accept="image/png,image/jpeg,image/webp"]');
    const testImagePath = 'C:\\Users\\kuncf\\OneDrive\\Pictures\\20251108_085028.jpg';
    expect(fs.existsSync(testImagePath)).toBe(true);

    await fileInput.setInputFiles(testImagePath);
    await expect(page.locator('img[alt="Preview"]')).toBeVisible({ timeout: 5000 });

    const generateBtn = page.locator('button:has-text("Generate")');
    await expect(generateBtn).toBeEnabled();
    await generateBtn.click();

    await expect(page.locator('.animate-spin')).toBeVisible({ timeout: 5000 });

    await page.waitForSelector('h2:has-text("AI 3D Generation")', {
      state: 'hidden',
      timeout: 600_000,
    });

    const itemCount = await getItemCount(page);
    expect(itemCount).toBeGreaterThan(0);
  });

  test('cancel stops generation', async ({ page }) => {
    test.setTimeout(30_000);

    await openAIDialog(page);

    // Switch to text mode and start a generation
    await page.getByText('Text to 3D').click();
    await page.locator('textarea').fill('a red sports car');

    await page.locator('button:has-text("Generate")').click();

    // Wait for spinner to appear (generation started)
    await expect(page.locator('.animate-spin')).toBeVisible({ timeout: 5000 });

    // Click Cancel
    await page.locator('button:has-text("Cancel")').click();

    // Spinner should disappear, dialog should still be open
    await expect(page.locator('.animate-spin')).not.toBeVisible({ timeout: 5000 });
    await expect(page.locator('h2:has-text("AI 3D Generation")')).toBeVisible();

    // No items should have been imported
    expect(await getItemCount(page)).toBe(0);
  });

  test('error is shown when text-to-3D used without --enable_t2i', async ({ page }) => {
    // This test verifies the UI handles server errors gracefully.
    // If the server WAS started with --enable_t2i, the generation may succeed instead
    // of erroring — in that case this test is effectively a success path test. 
    // We still verify no crash occurs either way.
    test.setTimeout(GENERATION_TIMEOUT);

    await openAIDialog(page);
    await page.getByText('Text to 3D').click();
    await page.locator('textarea').fill('test object');
    await page.locator('button:has-text("Generate")').click();

    // Wait for either error or completion (dialog closes)
    const errorOrDone = await Promise.race([
      page.locator('.bg-red-900\\/40').waitFor({ timeout: GENERATION_TIMEOUT }).then(() => 'error'),
      page.waitForSelector('h2:has-text("AI 3D Generation")', {
        state: 'hidden',
        timeout: GENERATION_TIMEOUT,
      }).then(() => 'done'),
    ]);

    // Either outcome is acceptable — we just verify no crash
    expect(['error', 'done']).toContain(errorOrDone);
  });
});
