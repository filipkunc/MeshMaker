import { useState, useCallback, useEffect } from 'react';
import { Toolbar, PropertiesPanel, Viewport } from './components';
import { useMeshMaker } from './hooks/useMeshMaker';

type TransformMode = 'select' | 'translate' | 'rotate' | 'scale';

function App() {
  const { canvasRef, containerRef, isLoading, error, module } = useMeshMaker();
  const [meshSteps, setMeshSteps] = useState(20);
  const [selectionState, setSelectionState] = useState({ count: 0, x: 0, y: 0, z: 0 });
  const [transformMode, setTransformMode] = useState<TransformMode>('select');
  
  // Poll selection state from WASM module (uses unified API that respects transform mode)
  useEffect(() => {
    if (!module) return;
    
    const updateSelectionState = () => {
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
    };
    
    // Update initially
    updateSelectionState();
    
    // Poll every 100ms to catch changes from viewport interaction
    const interval = setInterval(updateSelectionState, 100);
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

  return (
    <div className="flex h-screen w-screen overflow-hidden">
      {/* Left Toolbar */}
      <Toolbar
        onToolChange={handleToolChange}
        onEditModeChange={handleEditModeChange}
        onAddPrimitive={handleAddPrimitive}
        meshSteps={meshSteps}
        onMeshStepsChange={setMeshSteps}
      />

      {/* Main Viewport */}
      <Viewport
        canvasRef={canvasRef}
        containerRef={containerRef}
        isLoading={isLoading}
        error={error}
      />

      {/* Right Properties Panel */}
      <PropertiesPanel 
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
      />
    </div>
  );
}

export default App;
