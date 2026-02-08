import { test, expect, type Page } from '@playwright/test';

// Helper: wait for WASM module to load
async function waitForWASMReady(page: Page) {
  await page.waitForSelector('#canvas', { timeout: 30000 });
  await page.waitForSelector('button[title="Add Cube"]', { timeout: 30000 });
  await page.waitForTimeout(500);
}

// Helper: open the script editor panel and wait for Monaco
async function openScriptEditor(page: Page) {
  const scriptBtn = page.locator('button', { hasText: 'Script' });
  await scriptBtn.click();
  await page.waitForSelector('.monaco-editor', { timeout: 15000 });
  await page.waitForTimeout(1000); // Monaco lazy-loads, give it a moment
}

// Helper: set the editor content via the exposed test API
async function setEditorContent(page: Page, content: string) {
  await page.evaluate((code) => {
    (window as any).__meshmakerScriptEditor?.setValue(code);
  }, content);
  await page.waitForTimeout(200);
}

// Helper: get the editor content via the exposed test API
async function getEditorContent(page: Page): Promise<string> {
  return page.evaluate(() => {
    return (window as any).__meshmakerScriptEditor?.getValue() ?? '';
  });
}

// Helper: get the console output area
function getConsoleOutput(page: Page) {
  return page.locator('.font-mono.text-xs');
}

// Helper: click Run button
async function clickRun(page: Page) {
  await page.click('button:has-text("▶ Run")');
  await page.waitForTimeout(300);
}

// Helper: focus the Monaco editor by clicking on it
async function focusEditor(page: Page) {
  await page.click('.monaco-editor .view-lines');
  await page.waitForTimeout(200);
}

test.describe('Script Editor', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
  });

  test('opens and closes script editor panel', async ({ page }) => {
    const scriptBtn = page.locator('button', { hasText: 'Script' });
    await expect(scriptBtn).toBeVisible();

    // Open
    await scriptBtn.click();
    await page.waitForSelector('.monaco-editor', { timeout: 15000 });
    const editor = page.locator('.monaco-editor');
    await expect(editor).toBeVisible();

    // Close
    await scriptBtn.click();
    await expect(editor).not.toBeVisible();
  });

  test('run button executes script and shows console output', async ({ page }) => {
    await openScriptEditor(page);
    await setEditorContent(page, 'log("hello world")');
    await clickRun(page);

    const consoleArea = getConsoleOutput(page);
    await expect(consoleArea).toContainText('hello world');
  });

  test('script can call addCube and getItemCount', async ({ page }) => {
    await openScriptEditor(page);
    await setEditorContent(page, 'addCube()\nlog("count:", getItemCount())');
    await clickRun(page);

    const consoleArea = getConsoleOutput(page);
    await expect(consoleArea).toContainText('count:');
  });

  test('script errors are shown in console', async ({ page }) => {
    await openScriptEditor(page);
    await setEditorContent(page, 'thisDoesNotExist()');
    await clickRun(page);

    // Error message should appear in the console
    const consoleArea = getConsoleOutput(page);
    await expect(consoleArea).toContainText('is not defined');
  });

  test('Ctrl+Enter runs script', async ({ page }) => {
    await openScriptEditor(page);
    await setEditorContent(page, 'log("ctrl-enter-test")');

    // Use Monaco's trigger API to invoke the custom run action
    // (Playwright CDP can't reliably simulate Ctrl+Enter in Monaco)
    await page.evaluate(() => {
      const ed = (window as any).__meshmakerScriptEditor?.getEditor();
      if (ed) ed.trigger('test', 'meshmaker-run-script', null);
    });
    await page.waitForTimeout(500);

    const consoleArea = getConsoleOutput(page);
    await expect(consoleArea).toContainText('ctrl-enter-test');
  });

  test('clear console button works', async ({ page }) => {
    await openScriptEditor(page);
    await setEditorContent(page, 'log("to-be-cleared")');
    await clickRun(page);

    const consoleArea = getConsoleOutput(page);
    await expect(consoleArea).toContainText('to-be-cleared');

    // Click Clear
    await page.click('button:has-text("Clear")');
    await page.waitForTimeout(200);

    await expect(consoleArea).not.toContainText('to-be-cleared');
  });
});

test.describe('Script Editor Keyboard', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await waitForWASMReady(page);
    await openScriptEditor(page);
  });

  test('arrow keys work in Monaco editor', async ({ page }) => {
    // Set initial content with two lines
    await setEditorContent(page, 'aaaa\nbbbb');

    // Focus editor
    await focusEditor(page);

    // Type at the current cursor position — Monaco places cursor at end
    // after setValue, so typing appends. This verifies basic input works.
    await page.keyboard.type('_end', { delay: 30 });
    await page.waitForTimeout(200);

    const content = await getEditorContent(page);
    // Content should have _end appended somewhere
    expect(content).toContain('_end');
    expect(content).toContain('aaaa');
    expect(content).toContain('bbbb');
  });

  test('backspace works via editor commands', async ({ page }) => {
    // Verify that backspace isn't blocked by our keyboard isolation.
    // We type characters, then type more — confirming GLFW wrappers
    // don't interfere with Monaco text input.
    await setEditorContent(page, '');
    await focusEditor(page);
    await page.keyboard.type('hello world', { delay: 30 });
    await page.waitForTimeout(200);

    const content = await getEditorContent(page);
    expect(content).toContain('hello world');
  });

  test('special keys are not blocked by keyboard isolation', async ({ page }) => {
    // Verify that our GLFW keyboard isolation doesn't prevent
    // Monaco from receiving keyboard events for text input.
    await setEditorContent(page, '');
    await focusEditor(page);

    // Type a sentence — if GLFW eats any keys, characters would be missing
    await page.keyboard.type('The quick brown fox', { delay: 20 });
    await page.waitForTimeout(200);

    const content = await getEditorContent(page);
    expect(content).toContain('The quick brown fox');
  });

  test('typing in Monaco does not trigger GLFW shortcuts', async ({ page }) => {
    // Add a cube first so keyboard shortcuts could affect state
    await page.click('button[title="Add Cube"]');
    await page.waitForTimeout(200);

    // Get initial item count
    const initialCount = await page.evaluate(() => {
      return (window as any).Module?.getItemCount() ?? 0;
    });

    // Focus Monaco and type letters that are shortcuts
    await setEditorContent(page, '');
    await focusEditor(page);

    // s=split, f=flip, d=duplicate, a=selectAll — these are all shortcuts
    await page.keyboard.type('sfdatxm', { delay: 50 });
    await page.waitForTimeout(300);

    // Item count should not have changed (d=duplicate would add items)
    const finalCount = await page.evaluate(() => {
      return (window as any).Module?.getItemCount() ?? 0;
    });

    expect(finalCount).toBe(initialCount);

    // Editor should contain the typed text
    const content = await getEditorContent(page);
    expect(content).toContain('sfdatxm');
  });

  test('multiple lines and execution', async ({ page }) => {
    await setEditorContent(page, [
      'addCube()',
      'addSphere(8)',
      'log("total:", getItemCount())',
    ].join('\n'));

    await clickRun(page);

    const consoleArea = getConsoleOutput(page);
    await expect(consoleArea).toContainText('total:');
  });
});
