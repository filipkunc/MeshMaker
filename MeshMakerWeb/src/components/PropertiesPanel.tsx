import { useState, useEffect } from 'react';

type TransformMode = 'select' | 'translate' | 'rotate' | 'scale';

interface PropertiesPanelProps {
  selectionCount?: number;
  transformMode?: TransformMode;
  // Current X, Y, Z values (meaning depends on transform mode)
  selectionX?: number;
  selectionY?: number;
  selectionZ?: number;
  onSelectionXChange?: (value: number) => void;
  onSelectionYChange?: (value: number) => void;
  onSelectionZChange?: (value: number) => void;
  onDuplicate?: () => void;
  onDelete?: () => void;
}

export function PropertiesPanel({ 
  selectionCount = 0,
  transformMode = 'select',
  selectionX = 0,
  selectionY = 0,
  selectionZ = 0,
  onSelectionXChange,
  onSelectionYChange,
  onSelectionZChange,
  onDuplicate,
  onDelete 
}: PropertiesPanelProps) {
  // Local state for input fields
  const [localX, setLocalX] = useState('0');
  const [localY, setLocalY] = useState('0');
  const [localZ, setLocalZ] = useState('0');

  // Update local state when selection values change
  useEffect(() => {
    setLocalX(selectionX.toFixed(2));
    setLocalY(selectionY.toFixed(2));
    setLocalZ(selectionZ.toFixed(2));
  }, [selectionX, selectionY, selectionZ]);

  const handleCommit = (axis: 'x' | 'y' | 'z', value: string) => {
    const num = parseFloat(value);
    if (isNaN(num)) return;
    
    if (axis === 'x') onSelectionXChange?.(num);
    if (axis === 'y') onSelectionYChange?.(num);
    if (axis === 'z') onSelectionZChange?.(num);
  };

  const getTransformLabel = () => {
    switch (transformMode) {
      case 'translate': return 'Position';
      case 'rotate': return 'Rotation (degrees)';
      case 'scale': return 'Scale';
      default: return 'Position';
    }
  };

  const getStep = () => {
    switch (transformMode) {
      case 'rotate': return '15';
      default: return '0.1';
    }
  };

  return (
    <div className="w-64 p-3 bg-zinc-800 border-l border-zinc-700 overflow-y-auto">
      <h2 className="text-sm font-semibold text-zinc-200 mb-4">Properties</h2>
      
      {selectionCount === 0 ? (
        <p className="text-sm text-zinc-500">No selection</p>
      ) : (
        <div className="space-y-4">
          <div>
            <span className="text-xs text-zinc-400 uppercase tracking-wide">
              Selection
            </span>
            <p className="text-sm text-zinc-200 mt-1">
              {selectionCount} item{selectionCount !== 1 ? 's' : ''} selected
            </p>
          </div>

          {/* Transform Section */}
          <div className="space-y-2">
            <span className="text-xs text-zinc-400 uppercase tracking-wide">
              {getTransformLabel()}
            </span>
            <div className="grid grid-cols-3 gap-1">
              <label className="text-xs text-zinc-500">X</label>
              <label className="text-xs text-zinc-500">Y</label>
              <label className="text-xs text-zinc-500">Z</label>
              <input
                type="number"
                step={getStep()}
                value={localX}
                onChange={(e) => setLocalX(e.target.value)}
                onBlur={() => handleCommit('x', localX)}
                onKeyDown={(e) => e.key === 'Enter' && handleCommit('x', localX)}
                className="bg-zinc-700 text-zinc-200 text-sm px-2 py-1 rounded border border-zinc-600 focus:border-blue-500 focus:outline-none"
              />
              <input
                type="number"
                step={getStep()}
                value={localY}
                onChange={(e) => setLocalY(e.target.value)}
                onBlur={() => handleCommit('y', localY)}
                onKeyDown={(e) => e.key === 'Enter' && handleCommit('y', localY)}
                className="bg-zinc-700 text-zinc-200 text-sm px-2 py-1 rounded border border-zinc-600 focus:border-blue-500 focus:outline-none"
              />
              <input
                type="number"
                step={getStep()}
                value={localZ}
                onChange={(e) => setLocalZ(e.target.value)}
                onBlur={() => handleCommit('z', localZ)}
                onKeyDown={(e) => e.key === 'Enter' && handleCommit('z', localZ)}
                className="bg-zinc-700 text-zinc-200 text-sm px-2 py-1 rounded border border-zinc-600 focus:border-blue-500 focus:outline-none"
              />
            </div>
          </div>

          {/* Actions */}
          <div className="space-y-2 pt-2 border-t border-zinc-700">
            <span className="text-xs text-zinc-400 uppercase tracking-wide">
              Actions
            </span>
            <div className="flex flex-col gap-1">
              <button 
                onClick={onDuplicate}
                className="px-3 py-1.5 text-sm bg-zinc-700 text-zinc-200 hover:bg-zinc-600 rounded transition-colors"
              >
                Duplicate
              </button>
              <button 
                onClick={onDelete}
                className="px-3 py-1.5 text-sm bg-red-900 text-red-200 hover:bg-red-800 rounded transition-colors"
              >
                Delete
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
