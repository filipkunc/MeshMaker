interface PropertiesPanelProps {
  selectionCount?: number;
}

export function PropertiesPanel({ selectionCount = 0 }: PropertiesPanelProps) {
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
              Transform
            </span>
            
            {/* Position */}
            <div className="grid grid-cols-3 gap-1">
              <label className="text-xs text-zinc-500">X</label>
              <label className="text-xs text-zinc-500">Y</label>
              <label className="text-xs text-zinc-500">Z</label>
              <input
                type="number"
                step="0.1"
                className="bg-zinc-700 text-zinc-200 text-sm px-2 py-1 rounded border border-zinc-600 focus:border-blue-500 focus:outline-none"
                placeholder="0"
              />
              <input
                type="number"
                step="0.1"
                className="bg-zinc-700 text-zinc-200 text-sm px-2 py-1 rounded border border-zinc-600 focus:border-blue-500 focus:outline-none"
                placeholder="0"
              />
              <input
                type="number"
                step="0.1"
                className="bg-zinc-700 text-zinc-200 text-sm px-2 py-1 rounded border border-zinc-600 focus:border-blue-500 focus:outline-none"
                placeholder="0"
              />
            </div>
          </div>

          {/* Actions */}
          <div className="space-y-2 pt-2 border-t border-zinc-700">
            <span className="text-xs text-zinc-400 uppercase tracking-wide">
              Actions
            </span>
            <div className="flex flex-col gap-1">
              <button className="px-3 py-1.5 text-sm bg-zinc-700 text-zinc-200 hover:bg-zinc-600 rounded transition-colors">
                Duplicate
              </button>
              <button className="px-3 py-1.5 text-sm bg-red-900 text-red-200 hover:bg-red-800 rounded transition-colors">
                Delete
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
