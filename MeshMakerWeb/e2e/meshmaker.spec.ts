import { test, expect, type Page } from '@playwright/test';

// Helper: wait for WASM module to load by checking for canvas and module readiness
async function waitForWASMReady(page: Page) {
  // Wait for canvas to be present
  await page.waitForSelector('#canvas', { timeout: 30000 });
  
  // Wait for WASM module to initialize — the app shows toolbar buttons 
  // only after module loads. Wait for "Add Cube" button as readiness signal.
  await page.waitForSelector('button[title="Add Cube"]', { timeout: 30000 });
  
  // Small delay to let WebGL context fully initialize
  await page.waitForTimeout(500);
}

test.describe('App Loading', () => {
  test('canvas renders and WASM loads', async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
    
    const canvas = page.locator('#canvas');
    await expect(canvas).toBeVisible();
  });

  test('toolbar buttons are present', async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
    
    // Transform tools
    await expect(page.locator('button[title="Select (1)"]')).toBeVisible();
    await expect(page.locator('button[title="Translate (2)"]')).toBeVisible();
    await expect(page.locator('button[title="Rotate (3)"]')).toBeVisible();
    await expect(page.locator('button[title="Scale (4)"]')).toBeVisible();
    
    // Edit modes
    await expect(page.locator('button[title="Edit Items (Q)"]')).toBeVisible();
    await expect(page.locator('button[title="Edit Vertices (W)"]')).toBeVisible();
    await expect(page.locator('button[title="Edit Triangles (E)"]')).toBeVisible();
    await expect(page.locator('button[title="Edit Edges (R)"]')).toBeVisible();
  });
});

test.describe('Primitives', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('add cube', async ({ page }) => {
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
    
    // Selection count should show in bottom panel
    const selectionText = page.locator('text=Selection:');
    await expect(selectionText).toBeVisible();
  });

  test('add multiple primitives', async ({ page }) => {
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
    await page.click('button[title="Add Sphere"]');
    await page.waitForTimeout(200);
    await page.click('button[title="Add Cylinder"]');
    await page.waitForTimeout(200);
    
    // Should have items in scene
    const selectionText = page.locator('text=Selection:');
    await expect(selectionText).toBeVisible();
  });
});

test.describe('Edit Modes', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
  });

  test('switch to edge mode', async ({ page }) => {
    await page.click('button[title="Edit Edges (R)"]');
    await page.waitForTimeout(100);
    
    // Edge-specific tools should appear
    await expect(page.locator('button[title="Select Edge Loop (L)"]')).toBeVisible();
    await expect(page.locator('button[title="Select Edge Ring (Shift+L)"]')).toBeVisible();
    await expect(page.locator('button[title="Grow Edge Selection (G)"]')).toBeVisible();
  });

  test('switch to vertex mode', async ({ page }) => {
    await page.click('button[title="Edit Vertices (W)"]');
    await page.waitForTimeout(100);
    
    // Edge tools should NOT be visible
    await expect(page.locator('button[title="Select Edge Loop (L)"]')).not.toBeVisible();
  });

  test('keyboard shortcuts for edit modes', async ({ page }) => {
    // Press R for edge mode
    await page.keyboard.press('r');
    await page.waitForTimeout(100);
    await expect(page.locator('button[title="Select Edge Loop (L)"]')).toBeVisible();
    
    // Press Q for items mode  
    await page.keyboard.press('q');
    await page.waitForTimeout(100);
    await expect(page.locator('button[title="Select Edge Loop (L)"]')).not.toBeVisible();
  });
});

test.describe('UV Editor', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
  });

  test('toggle UV editor visibility', async ({ page }) => {
    // UV Editor button should be visible
    const uvButton = page.locator('button', { hasText: 'UV Editor' });
    await expect(uvButton).toBeVisible();
    
    // Click to show UV editor
    await uvButton.click();
    await page.waitForTimeout(200);
    
    // UV controls should now be visible
    await expect(page.locator('button[title="Reset UV View"]')).toBeVisible();
    await expect(page.locator('button[title*="Box projection"]')).toBeVisible();
    
    // Click again to hide
    await uvButton.click();
    await page.waitForTimeout(200);
    
    // UV controls should be hidden
    await expect(page.locator('button[title="Reset UV View"]')).not.toBeVisible();
  });

  test('UV projection buttons visible when editor open', async ({ page }) => {
    const uvButton = page.locator('button', { hasText: 'UV Editor' });
    await uvButton.click();
    await page.waitForTimeout(200);
    
    await expect(page.locator('button[title*="Box projection"]')).toBeVisible();
    await expect(page.locator('button[title="Planar projection from Z axis"]')).toBeVisible();
    await expect(page.locator('button[title="Cylindrical projection around Y axis"]')).toBeVisible();
    await expect(page.locator('button[title="Spherical projection"]')).toBeVisible();
  });

  test('seam buttons visible when editor open', async ({ page }) => {
    const uvButton = page.locator('button', { hasText: 'UV Editor' });
    await uvButton.click();
    await page.waitForTimeout(200);
    
    await expect(page.locator('button[title*="Mark selected edges as UV seams"]')).toBeVisible();
    await expect(page.locator('button[title="Unmark selected edges as seams"]')).toBeVisible();
  });

  test('click box projection unwrap', async ({ page }) => {
    const uvButton = page.locator('button', { hasText: 'UV Editor' });
    await uvButton.click();
    await page.waitForTimeout(200);
    
    // Click box projection
    const boxButton = page.locator('button[title*="Box projection"]');
    await boxButton.click();
    await page.waitForTimeout(300);
    
    // No crash = success, UV view should still be open
    await expect(page.locator('button[title="Reset UV View"]')).toBeVisible();
  });

  test('click all projection types without crash', async ({ page }) => {
    const uvButton = page.locator('button', { hasText: 'UV Editor' });
    await uvButton.click();
    await page.waitForTimeout(200);
    
    const projections = [
      'button[title*="Box projection"]',
      'button[title="Planar projection from Z axis"]',
      'button[title="Cylindrical projection around Y axis"]',
      'button[title="Spherical projection"]',
      'button[title="Conformal unwrap using seam edges to define UV islands"]',
    ];
    
    for (const selector of projections) {
      await page.click(selector);
      await page.waitForTimeout(200);
    }
    
    // Still alive
    await expect(page.locator('#canvas')).toBeVisible();
  });
});

test.describe('UV Seam Workflow', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
  });

  test('mark seam workflow: edge mode → select → mark → unwrap', async ({ page }) => {
    // 1. Switch to edge mode
    await page.click('button[title="Edit Edges (R)"]');
    await page.waitForTimeout(100);
    
    // 2. Open UV editor
    const uvButton = page.locator('button', { hasText: 'UV Editor' });
    await uvButton.click();
    await page.waitForTimeout(200);
    
    // 3. Select all edges (A key)
    await page.keyboard.press('a');
    await page.waitForTimeout(200);
    
    // 4. Mark seams
    const markSeamButton = page.locator('button[title*="Mark selected edges as UV seams"]');
    await markSeamButton.click();
    await page.waitForTimeout(200);
    
    // 5. Unwrap with box projection
    await page.click('button[title*="Box projection"]');
    await page.waitForTimeout(300);
    
    // No crash = success
    await expect(page.locator('#canvas')).toBeVisible();
  });

  test('seam unwrap workflow: mark seams → conformal unwrap', async ({ page }) => {
    // 1. Switch to edge mode
    await page.click('button[title="Edit Edges (R)"]');
    await page.waitForTimeout(100);
    
    // 2. Open UV editor
    const uvButton = page.locator('button', { hasText: 'UV Editor' });
    await uvButton.click();
    await page.waitForTimeout(200);
    
    // 3. Select all edges
    await page.keyboard.press('a');
    await page.waitForTimeout(200);
    
    // 4. Mark seams
    await page.click('button[title*="Mark selected edges as UV seams"]');
    await page.waitForTimeout(200);
    
    // 5. Click the Unwrap button (seam-based conformal unwrap)
    await page.click('button[title="Conformal unwrap using seam edges to define UV islands"]');
    await page.waitForTimeout(300);
    
    // Should not crash, UV viewport still visible
    await expect(page.locator('button[title="Reset UV View"]')).toBeVisible();
    await expect(page.locator('#canvas')).toBeVisible();
  });

  test('clear seams', async ({ page }) => {
    // Switch to edge mode
    await page.click('button[title="Edit Edges (R)"]');
    await page.waitForTimeout(100);
    
    // Open UV editor
    const uvButton = page.locator('button', { hasText: 'UV Editor' });
    await uvButton.click();
    await page.waitForTimeout(200);
    
    // Select all and mark seams
    await page.keyboard.press('a');
    await page.waitForTimeout(200);
    await page.click('button[title*="Mark selected edges as UV seams"]');
    await page.waitForTimeout(200);
    
    // Clear seams
    await page.click('button[title="Unmark selected edges as seams"]');
    await page.waitForTimeout(200);
    
    // No crash
    await expect(page.locator('#canvas')).toBeVisible();
  });
});

test.describe('Undo/Redo', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('undo button disabled when no history', async ({ page }) => {
    const undoButton = page.locator('button[title="Undo (Ctrl+Z)"]');
    await expect(undoButton).toBeDisabled();
  });

  test('undo enabled after action', async ({ page }) => {
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
    
    const undoButton = page.locator('button[title="Undo (Ctrl+Z)"]');
    await expect(undoButton).toBeEnabled();
  });

  test('undo add cube', async ({ page }) => {
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
    
    // Undo
    await page.click('button[title="Undo (Ctrl+Z)"]');
    await page.waitForTimeout(200);
    
    // Redo should now be enabled
    const redoButton = page.locator('button[title="Redo (Ctrl+Y)"]');
    await expect(redoButton).toBeEnabled();
  });

  test('undo via keyboard shortcut', async ({ page }) => {
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
    
    await page.keyboard.press('Control+z');
    await page.waitForTimeout(200);
    
    const redoButton = page.locator('button[title="Redo (Ctrl+Y)"]');
    await expect(redoButton).toBeEnabled();
  });

  test('redo via keyboard shortcut', async ({ page }) => {
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
    
    await page.keyboard.press('Control+z');
    await page.waitForTimeout(200);
    
    await page.keyboard.press('Control+y');
    await page.waitForTimeout(200);
    
    // Undo should be enabled again (we re-did the add)
    const undoButton = page.locator('button[title="Undo (Ctrl+Z)"]');
    await expect(undoButton).toBeEnabled();
  });
});

test.describe('Scene Operations', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('clear scene', async ({ page }) => {
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
    
    await page.click('button[title="Clear Scene"]');
    await page.waitForTimeout(200);
    
    // Should still be functional
    await expect(page.locator('#canvas')).toBeVisible();
  });

  test('view mode switching', async ({ page }) => {
    const viewSelect = page.locator('select');
    await expect(viewSelect).toBeVisible();
    
    await viewSelect.selectOption('Wire');
    await page.waitForTimeout(100);
    
    await viewSelect.selectOption('Both');
    await page.waitForTimeout(100);
    
    await viewSelect.selectOption('Solid');
    await page.waitForTimeout(100);
    
    await expect(page.locator('#canvas')).toBeVisible();
  });
});
