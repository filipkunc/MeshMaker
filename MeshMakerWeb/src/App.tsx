import { useState, useCallback, useEffect } from 'react';
import { Viewport } from './components';
import { TopToolbar } from './components/TopToolbar';
import { BottomPanel } from './components/BottomPanel';
import { useMeshMaker } from './hooks/useMeshMaker';

type EditMode = 'items' | 'vertices' | 'triangles' | 'edges';
type ViewMode = 'solid' | 'wireframe' | 'solidWireframe';

type TransformMode = 'select' | 'translate' | 'rotate' | 'scale';

function App() {
  const { canvasRef, containerRef, isLoading, error, module } = useMeshMaker();
  const [meshSteps, setMeshSteps] = useState(20);
  const [selectionState, setSelectionState] = useState({ count: 0, x: 0, y: 0, z: 0 });
  const [transformMode, setTransformMode] = useState<TransformMode>('select');
  const [editMode, setEditMode] = useState<EditMode>('items');
  const [viewMode, setViewMode] = useState<ViewMode>('solidWireframe');
  const [showGrid, setShowGrid] = useState(true);
  const [canUndo, setCanUndo] = useState(false);
  const [canRedo, setCanRedo] = useState(false);
  const [hasTexture, setHasTexture] = useState(false);
  const [isImporting, setIsImporting] = useState(false);
  const [importProgress, setImportProgress] = useState('');
  
  // Poll selection state and undo/redo state from WASM module
  useEffect(() => {
    if (!module) return;
    
    const updateState = () => {
      // Update selection state
      const count = module.getSelectionCount();
      if (count > 0) {
        setSelectionState({
          count,
          x: module.getSelectionX(),
          y: module.getSelectionY(),
          z: module.getSelectionZ(),
        });
        // Check if selection has texture
        setHasTexture(module.selectionHasTexture());
      } else {
        setSelectionState({ count: 0, x: 0, y: 0, z: 0 });
        setHasTexture(false);
      }
      
      // Update undo/redo state
      setCanUndo(module.canUndo());
      setCanRedo(module.canRedo());
    };
    
    // Update initially
    updateState();
    
    // Poll every 100ms to catch changes from viewport interaction
    const interval = setInterval(updateState, 100);
    return () => clearInterval(interval);
  }, [module, transformMode]); // Re-poll when transform mode changes
  
  // Force re-render to update selection info
  const triggerUpdate = useCallback(() => {
    if (!module) return;
    const count = module.getSelectionCount();
    if (count > 0) {
      setSelectionState({
        count,
        x: module.getSelectionX(),
        y: module.getSelectionY(),
        z: module.getSelectionZ(),
      });
    } else {
      setSelectionState({ count: 0, x: 0, y: 0, z: 0 });
    }
  }, [module]);

  const handleToolChange = (tool: string) => {
    if (!module) return;
    
    // Map tool names to transform modes (0=None, 1=Translate, 2=Rotate, 3=Scale)
    const toolToMode: Record<string, number> = {
      'select': 0,    // None - just selection
      'translate': 1,
      'rotate': 2,
      'scale': 3,
    };
    
    const mode = toolToMode[tool];
    if (mode !== undefined) {
      module.setTransformMode(mode);
      setTransformMode(tool as TransformMode);
    }
  };

  const handleEditModeChange = (mode: string) => {
    if (!module) return;
    
    // Map edit mode names to enum values (0=Items, 1=Vertices, 2=Triangles, 3=Edges)
    const modeToValue: Record<string, number> = {
      'items': 0,
      'vertices': 1,
      'triangles': 2,
      'edges': 3,
    };
    
    const value = modeToValue[mode];
    if (value !== undefined) {
      module.setEditMode(value);
      setEditMode(mode as EditMode);
    }
  };

  const handleAddPrimitive = (type: string, steps?: number) => {
    if (!module) return;
    
    const stepsToUse = steps ?? meshSteps;
    
    switch (type) {
      case 'cube':
        module.addCube();
        break;
      case 'cylinder':
        module.addCylinder(stepsToUse);
        break;
      case 'sphere':
        module.addSphere(stepsToUse);
        break;
      case 'plane':
        module.addPlane();
        break;
      case 'icosahedron':
        module.addIcosahedron();
        break;
    }
    triggerUpdate();
  };

  // Unified selection value handlers (work with current transform mode)
  const handleSelectionXChange = (value: number) => {
    if (!module) return;
    module.setSelectionX(value);
    triggerUpdate();
  };

  const handleSelectionYChange = (value: number) => {
    if (!module) return;
    module.setSelectionY(value);
    triggerUpdate();
  };

  const handleSelectionZChange = (value: number) => {
    if (!module) return;
    module.setSelectionZ(value);
    triggerUpdate();
  };

  const handleDuplicate = () => {
    if (!module) return;
    module.duplicateSelection();
    triggerUpdate();
  };

  const handleDelete = () => {
    if (!module) return;
    module.deleteSelection();
    triggerUpdate();
  };

  // File operations
  const handleExportOBJ = useCallback(() => {
    if (!module) return;
    
    const objData = module.exportToOBJ();
    if (!objData) {
      console.error('Failed to export OBJ');
      return;
    }
    
    // Create download
    const blob = new Blob([objData], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'meshmaker-export.obj';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  }, [module]);

  const handleExportGLB = useCallback(() => {
    if (!module) return;
    
    setIsImporting(true);
    setImportProgress('Exporting GLB...');
    
    // Use setTimeout to allow UI to update before blocking export
    setTimeout(() => {
      const glbData = module.exportToGLB();
      setIsImporting(false);
      setImportProgress('');
      
      if (!glbData) {
        console.error('Failed to export GLB');
        return;
      }
      
      // Copy to a proper ArrayBuffer (Emscripten may return a view on shared memory)
      const buffer = new ArrayBuffer(glbData.length);
      new Uint8Array(buffer).set(glbData);
      
      // Create download
      const blob = new Blob([buffer], { type: 'model/gltf-binary' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = 'meshmaker-export.glb';
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
    }, 50);
  }, [module]);

  const handleImportFile = useCallback((file: File) => {
    if (!module) return;
    
    const extension = file.name.split('.').pop()?.toLowerCase();
    const fileSize = file.size;
    const isLargeFile = fileSize > 1024 * 1024; // > 1MB
    
    if (isLargeFile) {
      setIsImporting(true);
      setImportProgress(`Reading ${file.name} (${(fileSize / 1024 / 1024).toFixed(1)} MB)...`);
    }
    
    if (extension === 'obj') {
      const reader = new FileReader();
      reader.onload = (e) => {
        const text = e.target?.result as string;
        if (text) {
          if (isLargeFile) {
            setImportProgress('Importing OBJ...');
          }
          // Use setTimeout to allow UI to update before blocking import
          setTimeout(() => {
            const success = module.importFromOBJ(text);
            if (!success) {
              console.error('Failed to import OBJ file');
            }
            setIsImporting(false);
            setImportProgress('');
            triggerUpdate();
          }, 50);
        }
      };
      reader.readAsText(file);
    } else if (extension === 'glb') {
      const reader = new FileReader();
      reader.onload = (e) => {
        const buffer = e.target?.result as ArrayBuffer;
        if (buffer) {
          const data = new Uint8Array(buffer);
          if (isLargeFile) {
            setImportProgress('Importing GLB...');
          }
          // Use setTimeout to allow UI to update before blocking import
          setTimeout(() => {
            const success = module.importFromGLB(data);
            if (!success) {
              console.error('Failed to import GLB file');
            }
            setIsImporting(false);
            setImportProgress('');
            triggerUpdate();
          }, 50);
        }
      };
      reader.readAsArrayBuffer(file);
    } else {
      setIsImporting(false);
      setImportProgress('');
      console.error('Unsupported file format:', extension);
    }
  }, [module, triggerUpdate]);

  const handleClearScene = useCallback(() => {
    if (!module) return;
    module.clearScene();
    triggerUpdate();
  }, [module, triggerUpdate]);

  // Undo/Redo
  const handleUndo = useCallback(() => {
    if (!module) return;
    module.undo();
    triggerUpdate();
  }, [module, triggerUpdate]);

  const handleRedo = useCallback(() => {
    if (!module) return;
    module.redo();
    triggerUpdate();
  }, [module, triggerUpdate]);

  // Selection operations
  const handleSelectAll = useCallback(() => {
    if (!module) return;
    module.selectAll();
    triggerUpdate();
  }, [module, triggerUpdate]);

  const handleDeselectAll = useCallback(() => {
    if (!module) return;
    module.deselectAll();
    triggerUpdate();
  }, [module, triggerUpdate]);

  // Mesh operations
  const handleFlip = useCallback(() => {
    if (!module) return;
    module.flipSelectedFaces();
    triggerUpdate();
  }, [module, triggerUpdate]);

  const handleSubdivide = useCallback(() => {
    if (!module) return;
    module.subdivideSelectedFaces();
    triggerUpdate();
  }, [module, triggerUpdate]);

  const handleTriangulate = useCallback(() => {
    if (!module) return;
    module.triangulateSelectedFaces();
    triggerUpdate();
  }, [module, triggerUpdate]);

  const handleExtrude = useCallback(() => {
    if (!module) return;
    module.extrudeSelectedFaces();
    triggerUpdate();
  }, [module, triggerUpdate]);

  const handleSplitEdges = useCallback(() => {
    if (!module) return;
    module.splitSelectedEdges();
    triggerUpdate();
  }, [module, triggerUpdate]);

  const handleMergeVertices = useCallback(() => {
    if (!module) return;
    module.mergeSelectedVertices();
    triggerUpdate();
  }, [module, triggerUpdate]);

  // Texture handlers
  const handleLoadTexture = useCallback((data: Uint8Array) => {
    if (!module) return;
    const success = module.loadTextureFromFileData(data);
    if (success) {
      setHasTexture(true);
    } else {
      console.error('Failed to load texture');
    }
  }, [module]);

  const handleRemoveTexture = useCallback(() => {
    if (!module) return;
    module.removeTexture();
    setHasTexture(false);
  }, [module]);

  // View settings
  const handleViewModeChange = useCallback((mode: 'solid' | 'wireframe' | 'solidWireframe') => {
    if (!module) return;
    // Map to WASM enum: 0=Solid, 1=Wireframe, 2=SolidWireframe
    const modeMap: Record<string, number> = {
      'solid': 0,
      'wireframe': 1,
      'solidWireframe': 2,
    };
    module.setViewMode(modeMap[mode]);
    setViewMode(mode);
  }, [module]);

  const handleShowGridChange = useCallback((show: boolean) => {
    if (!module) return;
    module.setShowGrid(show);
    setShowGrid(show);
  }, [module]);

  // Keyboard shortcuts
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      // Ignore if typing in an input
      if (e.target instanceof HTMLInputElement || e.target instanceof HTMLTextAreaElement) {
        return;
      }

      // Edit modes
      if (e.key === 'q' || e.key === 'Q') {
        e.preventDefault();
        handleEditModeChange('items');
        return;
      }
      if (e.key === 'w' || e.key === 'W') {
        e.preventDefault();
        handleEditModeChange('vertices');
        return;
      }
      if (e.key === 'e' || e.key === 'E') {
        e.preventDefault();
        handleEditModeChange('triangles');
        return;
      }
      if (e.key === 'r' || e.key === 'R') {
        e.preventDefault();
        handleEditModeChange('edges');
        return;
      }

      // Mesh operations
      if (e.key === 'f' || e.key === 'F') {
        e.preventDefault();
        handleFlip();
        return;
      }
      if (e.key === 's' || e.key === 'S') {
        e.preventDefault();
        handleSubdivide();
        return;
      }
      if (e.key === 't' || e.key === 'T') {
        e.preventDefault();
        handleTriangulate();
        return;
      }
      if (e.key === 'x' || e.key === 'X') {
        e.preventDefault();
        handleExtrude();
        return;
      }
      if (e.key === 'm' || e.key === 'M') {
        e.preventDefault();
        handleMergeVertices();
        return;
      }

      // Selection
      if (e.key === 'a' || e.key === 'A') {
        e.preventDefault();
        if (e.shiftKey) {
          handleDeselectAll();
        } else {
          handleSelectAll();
        }
        return;
      }

      // Delete
      if (e.key === 'Delete' || e.key === 'Backspace') {
        e.preventDefault();
        handleDelete();
        return;
      }

      // Duplicate
      if (e.key === 'd' || e.key === 'D') {
        e.preventDefault();
        handleDuplicate();
        return;
      }

      // Undo/Redo
      if ((e.ctrlKey || e.metaKey) && e.key === 'z') {
        e.preventDefault();
        if (e.shiftKey) {
          handleRedo();
        } else {
          handleUndo();
        }
        return;
      }
      if ((e.ctrlKey || e.metaKey) && e.key === 'y') {
        e.preventDefault();
        handleRedo();
        return;
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [handleFlip, handleSubdivide, handleTriangulate, handleExtrude, handleMergeVertices, 
      handleSelectAll, handleDeselectAll, handleDelete, handleDuplicate, handleUndo, handleRedo]);

  return (
    <div className="flex flex-col h-screen w-screen overflow-hidden">
      {/* Top Toolbar */}
      <TopToolbar
        activeTool={transformMode}
        onToolChange={handleToolChange}
        editMode={editMode}
        onEditModeChange={handleEditModeChange}
        onAddPrimitive={handleAddPrimitive}
        canUndo={canUndo}
        canRedo={canRedo}
        onUndo={handleUndo}
        onRedo={handleRedo}
        onImportFile={handleImportFile}
        onExportOBJ={handleExportOBJ}
        onExportGLB={handleExportGLB}
        onClearScene={handleClearScene}
        onSubdivide={handleSubdivide}
        onMerge={handleMergeVertices}
        onSplit={handleSplitEdges}
      />

      {/* Main Viewport */}
      <div className="flex-1 relative">
        <Viewport
          canvasRef={canvasRef}
          containerRef={containerRef}
          isLoading={isLoading}
          error={error}
        />
        
        {/* Import Loading Overlay */}
        {isImporting && (
          <div className="absolute inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50">
            <div className="bg-gray-800 rounded-lg p-6 shadow-xl flex flex-col items-center gap-4">
              <div className="animate-spin rounded-full h-12 w-12 border-4 border-blue-500 border-t-transparent"></div>
              <div className="text-white text-lg font-medium">{importProgress}</div>
              <div className="text-gray-400 text-sm">Please wait...</div>
            </div>
          </div>
        )}
      </div>

      {/* Bottom Panel */}
      <BottomPanel
        selectionCount={selectionState.count}
        transformMode={transformMode}
        selectionX={selectionState.x}
        selectionY={selectionState.y}
        selectionZ={selectionState.z}
        onSelectionXChange={handleSelectionXChange}
        onSelectionYChange={handleSelectionYChange}
        onSelectionZChange={handleSelectionZChange}
        onDuplicate={handleDuplicate}
        onDelete={handleDelete}
        onFlip={handleFlip}
        onTriangulate={handleTriangulate}
        onExtrude={handleExtrude}
        hasTexture={hasTexture}
        onLoadTexture={handleLoadTexture}
        onRemoveTexture={handleRemoveTexture}
        viewMode={viewMode}
        onViewModeChange={handleViewModeChange}
        showGrid={showGrid}
        onShowGridChange={handleShowGridChange}
        meshSteps={meshSteps}
        onMeshStepsChange={setMeshSteps}
      />
    </div>
  );
}

export default App;
