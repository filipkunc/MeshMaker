import { useState, useRef } from 'react';

type Tool = 'select' | 'translate' | 'rotate' | 'scale';
type EditMode = 'items' | 'vertices' | 'triangles' | 'edges';
type ViewMode = 'solid' | 'wireframe' | 'solidWireframe';

interface ToolbarProps {
  onToolChange?: (tool: Tool) => void;
  onEditModeChange?: (mode: EditMode) => void;
  onAddPrimitive?: (type: string, steps?: number) => void;
  meshSteps?: number;
  onMeshStepsChange?: (steps: number) => void;
  onExportOBJ?: () => void;
  onExportGLB?: () => void;
  onImportFile?: (file: File) => void;
  onClearScene?: () => void;
  // View
  viewMode?: ViewMode;
  onViewModeChange?: (mode: ViewMode) => void;
  showGrid?: boolean;
  onShowGridChange?: (show: boolean) => void;
  showNormals?: boolean;
  onShowNormalsChange?: (show: boolean) => void;
  // Undo/Redo
  canUndo?: boolean;
  canRedo?: boolean;
  onUndo?: () => void;
  onRedo?: () => void;
  // Selection
  onSelectAll?: () => void;
  onDeselectAll?: () => void;
  onSelectEdgeLoop?: () => void;
  onSelectEdgeRing?: () => void;
  // Mesh operations
  onFlip?: () => void;
  onSubdivide?: () => void;
  onTriangulate?: () => void;
  onExtrude?: () => void;
  onSplit?: () => void;
  onMergeVertices?: () => void;
  // State sync
  activeTool?: Tool;
  editMode?: EditMode;
}

export function Toolbar({ 
  onToolChange, 
  onEditModeChange, 
  onAddPrimitive,
  meshSteps = 20,
  onMeshStepsChange,
  onExportOBJ,
  onExportGLB,
  onImportFile,
  onClearScene,
  viewMode = 'solidWireframe',
  onViewModeChange,
  showGrid = true,
  onShowGridChange,
  showNormals = false,
  onShowNormalsChange,
  canUndo = false,
  canRedo = false,
  onUndo,
  onRedo,
  onSelectAll,
  onDeselectAll,
  onSelectEdgeLoop,
  onSelectEdgeRing,
  onFlip,
  onSubdivide,
  onTriangulate,
  onExtrude,
  onSplit,
  onMergeVertices,
  activeTool: controlledTool,
  editMode: controlledEditMode
}: ToolbarProps) {
  const [internalTool, setInternalTool] = useState<Tool>('select');
  const [internalEditMode, setInternalEditMode] = useState<EditMode>('items');
  const fileInputRef = useRef<HTMLInputElement>(null);
  
  // Use controlled values if provided, otherwise use internal state
  const activeTool = controlledTool ?? internalTool;
  const editMode = controlledEditMode ?? internalEditMode;

  const handleToolClick = (tool: Tool) => {
    setInternalTool(tool);
    onToolChange?.(tool);
  };

  const handleEditModeClick = (mode: EditMode) => {
    setInternalEditMode(mode);
    onEditModeChange?.(mode);
  };

  const tools: { id: Tool; label: string; shortcut: string }[] = [
    { id: 'select', label: 'Select', shortcut: '1' },
    { id: 'translate', label: 'Move', shortcut: '2' },
    { id: 'rotate', label: 'Rotate', shortcut: '3' },
    { id: 'scale', label: 'Scale', shortcut: '4' },
  ];

  const editModes: { id: EditMode; label: string; shortcut: string }[] = [
    { id: 'items', label: 'Items', shortcut: 'Q' },
    { id: 'vertices', label: 'Vertices', shortcut: 'W' },
    { id: 'triangles', label: 'Triangles', shortcut: 'E' },
    { id: 'edges', label: 'Edges', shortcut: 'R' },
  ];

  const viewModes: { id: ViewMode; label: string }[] = [
    { id: 'solid', label: 'Solid' },
    { id: 'wireframe', label: 'Wireframe' },
    { id: 'solidWireframe', label: 'Solid + Wire' },
  ];

  const handleFileImport = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (file) {
      onImportFile?.(file);
      // Reset input so the same file can be imported again
      e.target.value = '';
    }
  };

  return (
    <div className="flex flex-col gap-4 p-3 bg-zinc-800 border-r border-zinc-700 overflow-y-auto">
      {/* File Operations */}
      <div className="flex flex-col gap-1">
        <span className="text-xs text-zinc-400 uppercase tracking-wide mb-1">File</span>
        <input
          ref={fileInputRef}
          type="file"
          accept=".obj,.glb"
          onChange={handleFileImport}
          className="hidden"
        />
        <button
          onClick={() => fileInputRef.current?.click()}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
        >
          📂 Import...
        </button>
        <button
          onClick={onExportOBJ}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
        >
          💾 Export OBJ
        </button>
        <button
          onClick={onExportGLB}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
        >
          💾 Export GLB
        </button>
        <button
          onClick={onClearScene}
          className="px-3 py-2 text-sm text-left text-red-400 hover:bg-zinc-700 rounded transition-colors"
        >
          🗑️ Clear Scene
        </button>
      </div>

      {/* Undo/Redo */}
      <div className="flex flex-col gap-1">
        <span className="text-xs text-zinc-400 uppercase tracking-wide mb-1">Edit</span>
        <div className="flex gap-1">
          <button
            onClick={onUndo}
            disabled={!canUndo}
            className={`flex-1 px-3 py-2 text-sm rounded transition-colors ${
              canUndo 
                ? 'text-zinc-300 hover:bg-zinc-700' 
                : 'text-zinc-600 cursor-not-allowed'
            }`}
            title="Undo (Ctrl+Z)"
          >
            ↩ Undo
          </button>
          <button
            onClick={onRedo}
            disabled={!canRedo}
            className={`flex-1 px-3 py-2 text-sm rounded transition-colors ${
              canRedo 
                ? 'text-zinc-300 hover:bg-zinc-700' 
                : 'text-zinc-600 cursor-not-allowed'
            }`}
            title="Redo (Ctrl+Y)"
          >
            ↪ Redo
          </button>
        </div>
        <button
          onClick={onSelectAll}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
          title="Select All (Ctrl+A)"
        >
          ☑ Select All
        </button>
        <button
          onClick={onDeselectAll}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
          title="Deselect All (Ctrl+D)"
        >
          ☐ Deselect All
        </button>
        {editMode === 'edges' && (
          <>
            <button
              onClick={onSelectEdgeLoop}
              className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
              title="Select Edge Loop (L)"
            >
              ⟲ Edge Loop
            </button>
            <button
              onClick={onSelectEdgeRing}
              className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
              title="Select Edge Ring (Shift+L)"
            >
              ⟳ Edge Ring
            </button>
          </>
        )}
      </div>

      {/* Transform Tools */}
      <div className="flex flex-col gap-1">
        <span className="text-xs text-zinc-400 uppercase tracking-wide mb-1">Tools</span>
        {tools.map((tool) => (
          <button
            key={tool.id}
            onClick={() => handleToolClick(tool.id)}
            className={`px-3 py-2 text-sm text-left rounded transition-colors ${
              activeTool === tool.id
                ? 'bg-blue-600 text-white'
                : 'text-zinc-300 hover:bg-zinc-700'
            }`}
          >
            <span>{tool.label}</span>
            <span className="ml-2 text-xs text-zinc-500">{tool.shortcut}</span>
          </button>
        ))}
      </div>

      {/* Edit Mode */}
      <div className="flex flex-col gap-1">
        <span className="text-xs text-zinc-400 uppercase tracking-wide mb-1">Edit Mode</span>
        {editModes.map((mode) => (
          <button
            key={mode.id}
            onClick={() => handleEditModeClick(mode.id)}
            className={`px-3 py-2 text-sm text-left rounded transition-colors ${
              editMode === mode.id
                ? 'bg-green-600 text-white'
                : 'text-zinc-300 hover:bg-zinc-700'
            }`}
          >
            <span>{mode.label}</span>
            <span className="ml-2 text-xs text-zinc-500">{mode.shortcut}</span>
          </button>
        ))}
      </div>

      {/* Mesh Operations */}
      <div className="flex flex-col gap-1">
        <span className="text-xs text-zinc-400 uppercase tracking-wide mb-1">Mesh</span>
        <button
          onClick={onFlip}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
          title="Flip Selected Faces (F)"
        >
          🔄 Flip
        </button>
        <button
          onClick={onSubdivide}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
          title="Subdivide Selected"
        >
          ◫ Subdivide
        </button>
        <button
          onClick={onTriangulate}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
          title="Triangulate Selected (T)"
        >
          △ Triangulate
        </button>
        <button
          onClick={onExtrude}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
          title="Extrude Selected (X)"
        >
          ⬆ Extrude
        </button>
        <button
          onClick={onSplit}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
          title="Split Selected (S)"
        >
          ✂ Split
        </button>
        <button
          onClick={onMergeVertices}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
          title="Merge Selected Vertices (M)"
        >
          ⊕ Merge Vertices
        </button>
      </div>

      {/* View Mode */}
      <div className="flex flex-col gap-1">
        <span className="text-xs text-zinc-400 uppercase tracking-wide mb-1">View</span>
        {viewModes.map((mode) => (
          <button
            key={mode.id}
            onClick={() => onViewModeChange?.(mode.id)}
            className={`px-3 py-2 text-sm text-left rounded transition-colors ${
              viewMode === mode.id
                ? 'bg-purple-600 text-white'
                : 'text-zinc-300 hover:bg-zinc-700'
            }`}
          >
            {mode.label}
          </button>
        ))}
        <label className="flex items-center gap-2 px-3 py-2 text-sm text-zinc-300 hover:bg-zinc-700 rounded cursor-pointer">
          <input
            type="checkbox"
            checked={showGrid}
            onChange={(e) => onShowGridChange?.(e.target.checked)}
            className="accent-purple-500"
          />
          Show Grid
        </label>
        <label className="flex items-center gap-2 px-3 py-2 text-sm text-zinc-300 hover:bg-zinc-700 rounded cursor-pointer">
          <input
            type="checkbox"
            checked={showNormals}
            onChange={(e) => onShowNormalsChange?.(e.target.checked)}
            className="accent-cyan-500"
          />
          Show Normals
        </label>
      </div>

      {/* Add Primitives */}
      <div className="flex flex-col gap-1">
        <span className="text-xs text-zinc-400 uppercase tracking-wide mb-1">Add</span>
        <button
          onClick={() => onAddPrimitive?.('cube')}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
        >
          + Cube
        </button>
        <button
          onClick={() => onAddPrimitive?.('plane')}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
        >
          + Plane
        </button>
        
        {/* Steps input for Cylinder/Sphere */}
        <div className="mt-2 mb-1">
          <label className="flex items-center gap-2 text-xs text-zinc-400">
            <span>Steps:</span>
            <input
              type="number"
              min="3"
              max="100"
              value={meshSteps}
              onChange={(e) => {
                const val = parseInt(e.target.value);
                if (!isNaN(val) && val >= 3 && val <= 100) {
                  onMeshStepsChange?.(val);
                }
              }}
              className="w-14 bg-zinc-700 text-zinc-200 text-sm px-2 py-1 rounded border border-zinc-600 focus:border-blue-500 focus:outline-none"
            />
          </label>
        </div>
        
        <button
          onClick={() => onAddPrimitive?.('cylinder', meshSteps)}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
        >
          + Cylinder
        </button>
        <button
          onClick={() => onAddPrimitive?.('sphere', meshSteps)}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
        >
          + Sphere
        </button>
        <button
          onClick={() => onAddPrimitive?.('icosahedron')}
          className="px-3 py-2 text-sm text-left text-zinc-300 hover:bg-zinc-700 rounded transition-colors"
        >
          + Icosahedron
        </button>
      </div>
    </div>
  );
}
