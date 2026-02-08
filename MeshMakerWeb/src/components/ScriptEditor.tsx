import { useState, useRef, useCallback, useEffect } from 'react';
import Editor, { type OnMount } from '@monaco-editor/react';
import type { MeshMakerModule } from '../types/meshmaker';
import { runScript, type ConsoleEntry } from '../scripting/runScript';
import { generateTypeDeclarations } from '../scripting/scriptingApi';

const DEFAULT_SCRIPT = `// MeshMaker Script
// Use log() to print to the console below.
// All API functions are available as globals.
// Press Ctrl+Enter or click Run to execute.

addCube();
log("Items in scene:", getItemCount());
`;

interface ScriptEditorProps {
  module: MeshMakerModule | null;
}

export function ScriptEditor({ module }: ScriptEditorProps) {
  const [consoleEntries, setConsoleEntries] = useState<ConsoleEntry[]>([]);
  const [code, setCode] = useState(DEFAULT_SCRIPT);
  const consoleEndRef = useRef<HTMLDivElement>(null);
  const fileInputRef = useRef<HTMLInputElement>(null);
  const editorRef = useRef<Parameters<OnMount>[0] | null>(null);

  // Expose editor for E2E tests
  useEffect(() => {
    (window as any).__meshmakerScriptEditor = {
      setValue: (value: string) => {
        setCode(value);
        editorRef.current?.setValue(value);
      },
      getValue: () => editorRef.current?.getValue() ?? code,
      getEditor: () => editorRef.current,
    };
    return () => { delete (window as any).__meshmakerScriptEditor; };
  }, [code]);

  // Auto-scroll console to bottom
  useEffect(() => {
    consoleEndRef.current?.scrollIntoView({ behavior: 'smooth' });
  }, [consoleEntries]);

  const handleRun = useCallback(() => {
    if (!module) return;
    runScript(code, module, (entry) => {
      setConsoleEntries(prev => [...prev, entry]);
    });
  }, [code, module]);

  const handleClearConsole = useCallback(() => {
    setConsoleEntries([]);
  }, []);

  const handleSave = useCallback(() => {
    const blob = new Blob([code], { type: 'text/javascript' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'meshmaker-script.js';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  }, [code]);

  const handleLoad = useCallback(() => {
    fileInputRef.current?.click();
  }, []);

  const handleFileChange = useCallback((e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = (ev) => {
      const text = ev.target?.result as string;
      if (text) {
        setCode(text);
        editorRef.current?.setValue(text);
      }
    };
    reader.readAsText(file);
    // Reset so the same file can be loaded again
    e.target.value = '';
  }, []);

  const handleEditorMount: OnMount = useCallback((editor, monaco) => {
    editorRef.current = editor;

    // Configure JavaScript defaults for autocompletion
    const jsDefaults = monaco.languages.typescript.javascriptDefaults;
    jsDefaults.setDiagnosticsOptions({
      noSemanticValidation: false,
      noSyntaxValidation: false,
    });
    jsDefaults.setCompilerOptions({
      target: monaco.languages.typescript.ScriptTarget.ESNext,
      allowNonTsExtensions: true,
      allowJs: true,
      checkJs: true,
    });

    // Add the MeshMaker scripting type declarations
    const typeDecls = generateTypeDeclarations();
    jsDefaults.addExtraLib(typeDecls, 'meshmaker-scripting.d.ts');

    // Ctrl+Enter to run
    editor.addAction({
      id: 'meshmaker-run-script',
      label: 'Run Script',
      keybindings: [monaco.KeyMod.CtrlCmd | monaco.KeyCode.Enter],
      run: () => {
        // Get current code from editor directly
        const currentCode = editor.getValue();
        if (!module) return;
        runScript(currentCode, module, (entry) => {
          setConsoleEntries(prev => [...prev, entry]);
        });
      },
    });
  }, [module]);

  const btnClass = 'px-2 py-1 text-xs rounded transition-colors';

  return (
    <div className="flex flex-col h-full bg-zinc-900 border-l border-zinc-700">
      {/* Toolbar */}
      <div className="flex items-center gap-1 px-2 py-1 bg-zinc-800 border-b border-zinc-700 shrink-0">
        <button
          onClick={handleRun}
          disabled={!module}
          className={`${btnClass} bg-green-700 hover:bg-green-600 text-white disabled:opacity-40 disabled:cursor-not-allowed font-medium`}
          title="Run script (Ctrl+Enter)"
        >
          ▶ Run
        </button>
        <button
          onClick={handleClearConsole}
          className={`${btnClass} text-zinc-300 hover:bg-zinc-600`}
          title="Clear console output"
        >
          Clear
        </button>
        <div className="flex-1" />
        <button
          onClick={handleLoad}
          className={`${btnClass} text-zinc-300 hover:bg-zinc-600`}
          title="Load script from file"
        >
          Open
        </button>
        <button
          onClick={handleSave}
          className={`${btnClass} text-zinc-300 hover:bg-zinc-600`}
          title="Save script to file"
        >
          Save
        </button>
        <input
          ref={fileInputRef}
          type="file"
          accept=".js,.ts,.txt"
          className="hidden"
          onChange={handleFileChange}
        />
      </div>

      {/* Editor */}
      <div className="flex-1 min-h-0">
        <Editor
          defaultLanguage="javascript"
          defaultValue={DEFAULT_SCRIPT}
          value={code}
          onChange={(value) => setCode(value ?? '')}
          onMount={handleEditorMount}
          theme="vs-dark"
          options={{
            minimap: { enabled: false },
            fontSize: 13,
            lineNumbers: 'on',
            scrollBeyondLastLine: false,
            wordWrap: 'on',
            tabSize: 2,
            automaticLayout: true,
            fixedOverflowWidgets: true,
          }}
        />
      </div>

      {/* Console Output */}
      <div className="h-32 border-t border-zinc-700 bg-zinc-950 overflow-y-auto font-mono text-xs shrink-0">
        <div className="px-2 py-1 text-zinc-500 border-b border-zinc-800 text-[10px] uppercase tracking-wider sticky top-0 bg-zinc-950">
          Console
        </div>
        <div className="p-2 space-y-0.5">
          {consoleEntries.length === 0 && (
            <div className="text-zinc-600 italic">Run a script to see output here</div>
          )}
          {consoleEntries.map((entry, i) => (
            <div
              key={i}
              className={entry.type === 'error' ? 'text-red-400' : 'text-zinc-300'}
            >
              {entry.type === 'error' && <span className="text-red-500 mr-1">✕</span>}
              {entry.text}
            </div>
          ))}
          <div ref={consoleEndRef} />
        </div>
      </div>
    </div>
  );
}
