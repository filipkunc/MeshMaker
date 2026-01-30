import { useState, useEffect } from 'react';

type TransformMode = 'select' | 'translate' | 'rotate' | 'scale';
type ViewMode = 'solid' | 'wireframe' | 'solidWireframe';

interface BottomPanelProps {
  // Selection info
  selectionCount: number;
  transformMode: TransformMode;
  // Transform values
  selectionX: number;
  selectionY: number;
  selectionZ: number;
  onSelectionXChange: (value: number) => void;
  onSelectionYChange: (value: number) => void;
  onSelectionZChange: (value: number) => void;
  // Actions
  onDuplicate: () => void;
  onDelete: () => void;
  onFlip: () => void;
  onTriangulate: () => void;
  onExtrude: () => void;
  // View
  viewMode: ViewMode;
  onViewModeChange: (mode: ViewMode) => void;
  showGrid: boolean;
  onShowGridChange: (show: boolean) => void;
  // Mesh steps
  meshSteps: number;
  onMeshStepsChange: (steps: number) => void;
}

export function BottomPanel({
  selectionCount,
  transformMode,
  selectionX,
  selectionY,
  selectionZ,
  onSelectionXChange,
  onSelectionYChange,
  onSelectionZChange,
  onDuplicate,
  onDelete,
  onFlip,
  onTriangulate,
  onExtrude,
  viewMode,
  onViewModeChange,
  showGrid,
  onShowGridChange,
  meshSteps,
  onMeshStepsChange,
}: BottomPanelProps) {
  const [localX, setLocalX] = useState('0.00');
  const [localY, setLocalY] = useState('0.00');
  const [localZ, setLocalZ] = useState('0.00');

  useEffect(() => {
    setLocalX(selectionX.toFixed(2));
    setLocalY(selectionY.toFixed(2));
    setLocalZ(selectionZ.toFixed(2));
  }, [selectionX, selectionY, selectionZ]);

  const handleCommit = (axis: 'x' | 'y' | 'z', value: string) => {
    const num = parseFloat(value);
    if (isNaN(num)) return;
    
    if (axis === 'x') onSelectionXChange(num);
    if (axis === 'y') onSelectionYChange(num);
    if (axis === 'z') onSelectionZChange(num);
  };

  const getTransformLabel = () => {
    switch (transformMode) {
      case 'translate': return 'Position';
      case 'rotate': return 'Rotation°';
      case 'scale': return 'Scale';
      default: return 'Pos';
    }
  };

  const inputClass = "w-16 px-1 py-0.5 text-xs bg-zinc-900 border border-zinc-600 rounded text-zinc-200 text-center focus:outline-none focus:border-blue-500";

  return (
    <div className="flex items-center gap-4 px-3 py-1.5 bg-zinc-800 border-t border-zinc-700 text-xs">
      {/* Selection Info */}
      <div className="flex items-center gap-2">
        <span className="text-zinc-400">Selection:</span>
        <span className="text-zinc-200 min-w-[3rem]">
          {selectionCount > 0 ? `${selectionCount} item${selectionCount !== 1 ? 's' : ''}` : 'None'}
        </span>
      </div>
      
      <div className="w-px h-5 bg-zinc-600" />
      
      {/* Transform Values */}
      {selectionCount > 0 && (
        <>
          <div className="flex items-center gap-2">
            <span className="text-zinc-400">{getTransformLabel()}:</span>
            <label className="flex items-center gap-1">
              <span className="text-red-400">X</span>
              <input
                type="number"
                step={transformMode === 'rotate' ? '15' : '0.1'}
                value={localX}
                onChange={(e) => setLocalX(e.target.value)}
                onBlur={() => handleCommit('x', localX)}
                onKeyDown={(e) => e.key === 'Enter' && handleCommit('x', localX)}
                className={inputClass}
              />
            </label>
            <label className="flex items-center gap-1">
              <span className="text-green-400">Y</span>
              <input
                type="number"
                step={transformMode === 'rotate' ? '15' : '0.1'}
                value={localY}
                onChange={(e) => setLocalY(e.target.value)}
                onBlur={() => handleCommit('y', localY)}
                onKeyDown={(e) => e.key === 'Enter' && handleCommit('y', localY)}
                className={inputClass}
              />
            </label>
            <label className="flex items-center gap-1">
              <span className="text-blue-400">Z</span>
              <input
                type="number"
                step={transformMode === 'rotate' ? '15' : '0.1'}
                value={localZ}
                onChange={(e) => setLocalZ(e.target.value)}
                onBlur={() => handleCommit('z', localZ)}
                onKeyDown={(e) => e.key === 'Enter' && handleCommit('z', localZ)}
                className={inputClass}
              />
            </label>
          </div>
          
          <div className="w-px h-5 bg-zinc-600" />
          
          {/* Quick Actions */}
          <div className="flex items-center gap-1">
            <button
              onClick={onDuplicate}
              title="Duplicate (D)"
              className="px-2 py-0.5 text-zinc-300 hover:bg-zinc-600 rounded"
            >
              Dup
            </button>
            <button
              onClick={onDelete}
              title="Delete (Del)"
              className="px-2 py-0.5 text-red-400 hover:bg-zinc-600 rounded"
            >
              Del
            </button>
            <button
              onClick={onFlip}
              title="Flip Normals (F)"
              className="px-2 py-0.5 text-zinc-300 hover:bg-zinc-600 rounded"
            >
              Flip
            </button>
            <button
              onClick={onTriangulate}
              title="Triangulate (T)"
              className="px-2 py-0.5 text-zinc-300 hover:bg-zinc-600 rounded"
            >
              Tri
            </button>
            <button
              onClick={onExtrude}
              title="Extrude (X)"
              className="px-2 py-0.5 text-zinc-300 hover:bg-zinc-600 rounded"
            >
              Ext
            </button>
          </div>
        </>
      )}
      
      {/* Spacer */}
      <div className="flex-1" />
      
      {/* Mesh Steps */}
      <div className="flex items-center gap-1">
        <span className="text-zinc-400">Steps:</span>
        <input
          type="number"
          min="3"
          max="100"
          value={meshSteps}
          onChange={(e) => onMeshStepsChange(parseInt(e.target.value) || 20)}
          className="w-12 px-1 py-0.5 text-xs bg-zinc-900 border border-zinc-600 rounded text-zinc-200 text-center"
        />
      </div>
      
      <div className="w-px h-5 bg-zinc-600" />
      
      {/* View Mode */}
      <div className="flex items-center gap-1">
        <span className="text-zinc-400">View:</span>
        <select
          value={viewMode}
          onChange={(e) => onViewModeChange(e.target.value as ViewMode)}
          className="px-1 py-0.5 text-xs bg-zinc-900 border border-zinc-600 rounded text-zinc-200"
        >
          <option value="solid">Solid</option>
          <option value="wireframe">Wire</option>
          <option value="solidWireframe">Both</option>
        </select>
      </div>
      
      {/* Grid Toggle */}
      <label className="flex items-center gap-1 cursor-pointer">
        <input
          type="checkbox"
          checked={showGrid}
          onChange={(e) => onShowGridChange(e.target.checked)}
          className="accent-blue-500"
        />
        <span className="text-zinc-400">Grid</span>
      </label>
    </div>
  );
}
