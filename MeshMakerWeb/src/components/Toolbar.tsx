import { useState } from 'react';

type Tool = 'select' | 'translate' | 'rotate' | 'scale';
type EditMode = 'items' | 'vertices' | 'triangles' | 'edges';

interface ToolbarProps {
  onToolChange?: (tool: Tool) => void;
  onEditModeChange?: (mode: EditMode) => void;
  onAddPrimitive?: (type: string, steps?: number) => void;
  meshSteps?: number;
  onMeshStepsChange?: (steps: number) => void;
}

export function Toolbar({ 
  onToolChange, 
  onEditModeChange, 
  onAddPrimitive,
  meshSteps = 20,
  onMeshStepsChange 
}: ToolbarProps) {
  const [activeTool, setActiveTool] = useState<Tool>('select');
  const [editMode, setEditMode] = useState<EditMode>('items');

  const handleToolClick = (tool: Tool) => {
    setActiveTool(tool);
    onToolChange?.(tool);
  };

  const handleEditModeClick = (mode: EditMode) => {
    setEditMode(mode);
    onEditModeChange?.(mode);
  };

  const tools: { id: Tool; label: string; shortcut: string }[] = [
    { id: 'select', label: 'Select', shortcut: '1' },
    { id: 'translate', label: 'Move', shortcut: '2' },
    { id: 'rotate', label: 'Rotate', shortcut: '3' },
    { id: 'scale', label: 'Scale', shortcut: '4' },
  ];

  const editModes: { id: EditMode; label: string }[] = [
    { id: 'items', label: 'Items' },
    { id: 'vertices', label: 'Vertices' },
    { id: 'triangles', label: 'Triangles' },
    { id: 'edges', label: 'Edges' },
  ];

  return (
    <div className="flex flex-col gap-4 p-3 bg-zinc-800 border-r border-zinc-700">
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
            {mode.label}
          </button>
        ))}
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
