import { test, expect, type Page } from '@playwright/test';

// Helper: wait for WASM module to load
async function waitForWASMReady(page: Page) {
  await page.waitForSelector('#canvas', { timeout: 30000 });
  await page.waitForSelector('button[title="Add Cube"]', { timeout: 30000 });
  await page.waitForTimeout(500);
}

// Helper: add a cube and select it in translate mode
async function setupCubeInTranslateMode(page: Page) {
  await page.goto('/');
  await waitForWASMReady(page);

  // Add a cube (it is auto-selected)
  await page.click('button[title="Add Cube"]');
  await page.waitForTimeout(200);

  // Switch to translate mode
  await page.click('button[title="Translate (2)"]');
  await page.waitForTimeout(200);
}

// Helper: get the X/Y/Z inputs in the bottom panel
function getBottomPanelXInput(page: Page) {
  return page.locator('label').filter({ hasText: /^X$/ }).locator('input[type="text"]').first();
}

function getBottomPanelYInput(page: Page) {
  return page.locator('label').filter({ hasText: /^Y$/ }).locator('input[type="text"]').first();
}

function getBottomPanelZInput(page: Page) {
  return page.locator('label').filter({ hasText: /^Z$/ }).locator('input[type="text"]').first();
}

// Helper: fill an input and wait for React to settle before pressing keys.
// Playwright fill() + CDP key dispatch can race with React's controlled input
// reconciliation. An intermediate assertion ensures the DOM is stable.
async function fillAndSettle(input: ReturnType<typeof getBottomPanelXInput>, value: string) {
  await input.fill(value);
  await expect(input).toHaveValue(value);
}

test.describe('Editor Input: Transform Value Text Fields', () => {
  test.beforeEach(async ({ page }) => {
    await setupCubeInTranslateMode(page);
  });

  test('X input is visible and editable in translate mode', async ({ page }) => {
    const xInput = getBottomPanelXInput(page);
    await expect(xInput).toBeVisible();

    // Clear and type a value
    await xInput.click();
    await fillAndSettle(xInput, '1.23');
  });

  test('typing a value and pressing Enter commits it', async ({ page }) => {
    const xInput = getBottomPanelXInput(page);
    await xInput.click();
    await fillAndSettle(xInput, '2.5');
    await xInput.press('Enter');
    await page.waitForTimeout(200);

    // After commit, the WASM module updates the selection and the polling
    // syncs the formatted value back to the input
    const value = await xInput.inputValue();
    expect(parseFloat(value)).toBeCloseTo(2.5, 1);
  });

  test('Backspace key works inside the X input field', async ({ page }) => {
    const xInput = getBottomPanelXInput(page);
    await xInput.click();

    // Fill and wait for React to settle before pressing keys
    await fillAndSettle(xInput, '1.23');

    // Move cursor to the end and press Backspace to delete '3'
    await xInput.press('End');
    await xInput.press('Backspace');

    await expect(xInput).toHaveValue('1.2');
  });

  test('Backspace key does not trigger delete action while editing input', async ({ page }) => {
    const xInput = getBottomPanelXInput(page);
    await xInput.click();

    // Fill and wait for React to settle
    await fillAndSettle(xInput, '5.67');

    // Press Backspace — should only delete a character, NOT delete the selected cube
    await xInput.press('End');
    await xInput.press('Backspace');

    // The value should have the last character removed
    await expect(xInput).toHaveValue('5.6');

    // The cube should still be selected (selection count > 0)
    const selectionText = page.locator('text=Selection:');
    await expect(selectionText).toBeVisible();
  });

  test('Delete key works inside the X input field', async ({ page }) => {
    const xInput = getBottomPanelXInput(page);
    await xInput.click();

    await fillAndSettle(xInput, '3.45');
    // Move cursor to the beginning and delete first character
    await xInput.press('Home');
    await xInput.press('Delete');

    await expect(xInput).toHaveValue('.45');
  });

  test('Arrow keys work inside the X input field', async ({ page }) => {
    const xInput = getBottomPanelXInput(page);
    await xInput.click();

    await fillAndSettle(xInput, '1.23');

    // Arrow left then Backspace should delete '2' (second-to-last digit)
    await xInput.press('End');
    await xInput.press('ArrowLeft');
    await xInput.press('Backspace');

    await expect(xInput).toHaveValue('1.3');
  });

  test('multiple Backspace presses clear characters correctly', async ({ page }) => {
    const xInput = getBottomPanelXInput(page);
    await xInput.click();

    await fillAndSettle(xInput, '12.34');
    await xInput.press('End');

    // Press Backspace: '12.34' -> '12.3'
    await xInput.press('Backspace');
    await expect(xInput).toHaveValue('12.3');

    // Press Backspace more to get past the decimal point
    await xInput.press('Backspace');
    await expect(xInput).toHaveValue('12.');
    await xInput.press('Backspace');
    await expect(xInput).toHaveValue('12');
  });

  test('Y and Z inputs also support Backspace', async ({ page }) => {
    // Y input
    const yInput = getBottomPanelYInput(page);
    await yInput.click();
    await fillAndSettle(yInput, '4.56');
    await yInput.press('End');
    await yInput.press('Backspace');
    await expect(yInput).toHaveValue('4.5');

    // Z input
    const zInput = getBottomPanelZInput(page);
    await zInput.click();
    await fillAndSettle(zInput, '7.89');
    await zInput.press('End');
    await zInput.press('Backspace');
    await expect(zInput).toHaveValue('7.8');
  });

  test('keyboard shortcuts do not fire while editing input fields', async ({ page }) => {
    const xInput = getBottomPanelXInput(page);
    await xInput.click();
    await fillAndSettle(xInput, '1.00');

    // Press 'd' (duplicate shortcut) — should type 'd' into the text input, NOT duplicate the cube
    await xInput.press('d');
    await page.waitForTimeout(200);

    // Still only 1 item selected (no duplicate was triggered)
    const selectionLabel = page.locator('text=/1 item/');
    await expect(selectionLabel).toBeVisible();
  });

  test('select text with Ctrl+A inside input does not trigger Select All', async ({ page }) => {
    const xInput = getBottomPanelXInput(page);
    await xInput.click();
    await fillAndSettle(xInput, '9.99');

    // Ctrl+A in the input should select the text, not trigger "select all" scene objects
    await xInput.press('Control+a');
    await page.waitForTimeout(200);

    // Value should still be intact
    await expect(xInput).toHaveValue('9.99');

    // Input should still be focused
    await expect(xInput).toBeFocused();
  });
});

test.describe('Editor Input: Transform Mode Switching', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);
  });

  test('switch between transform modes via keyboard', async ({ page }) => {
    // Press 2 for translate
    await page.keyboard.press('2');
    await page.waitForTimeout(100);

    const xInput = getBottomPanelXInput(page);
    await expect(xInput).toBeVisible();

    // Press 3 for rotate
    await page.keyboard.press('3');
    await page.waitForTimeout(100);
    await expect(xInput).toBeVisible();

    // Press 4 for scale
    await page.keyboard.press('4');
    await page.waitForTimeout(100);
    await expect(xInput).toBeVisible();
  });

  test('transform values update when translate mode is selected', async ({ page }) => {
    // Click translate button
    await page.click('button[title="Translate (2)"]');
    await page.waitForTimeout(200);

    const xInput = getBottomPanelXInput(page);
    await expect(xInput).toBeVisible();

    // The default cube position should be 0
    const value = await xInput.inputValue();
    expect(parseFloat(value)).toBeCloseTo(0, 1);
  });

  test('edit X position, commit, and verify value persists', async ({ page }) => {
    await page.click('button[title="Translate (2)"]');
    await page.waitForTimeout(200);

    const xInput = getBottomPanelXInput(page);
    await xInput.click();
    await fillAndSettle(xInput, '3.14');
    await xInput.press('Enter');
    await page.waitForTimeout(300);

    // Value should be committed and formatted
    const value = await xInput.inputValue();
    expect(parseFloat(value)).toBeCloseTo(3.14, 1);
  });
});
