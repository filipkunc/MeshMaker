import type { RefObject } from 'react';

interface ViewportProps {
  canvasRef: RefObject<HTMLCanvasElement | null>;
  containerRef: RefObject<HTMLDivElement | null>;
  isLoading?: boolean;
  error?: string | null;
}

export function Viewport({ canvasRef, containerRef, isLoading, error }: ViewportProps) {
  return (
    <div ref={containerRef} id="canvas-container" className="flex-1 relative bg-zinc-900 overflow-hidden">
      <canvas
        ref={canvasRef}
        id="canvas"
        className="absolute top-0 left-0"
        onContextMenu={(e) => e.preventDefault()}
      />
        
        {/* Loading overlay */}
        {isLoading && (
          <div className="absolute inset-0 flex items-center justify-center bg-zinc-900/80">
            <div className="text-center">
              <div className="w-8 h-8 border-2 border-blue-500 border-t-transparent rounded-full animate-spin mx-auto mb-3" />
              <p className="text-zinc-400">Loading MeshMaker...</p>
            </div>
          </div>
        )}
        
        {/* Error overlay */}
        {error && (
          <div className="absolute inset-0 flex items-center justify-center bg-zinc-900/90">
            <div className="text-center max-w-md p-4">
              <div className="text-red-500 text-4xl mb-3">⚠</div>
              <p className="text-red-400 font-medium mb-2">Failed to load MeshMaker</p>
              <p className="text-zinc-500 text-sm">{error}</p>
            </div>
          </div>
        )}
      </div>
    );
}
