import { Toolbar, PropertiesPanel, Viewport } from './components';
import { useMeshMaker } from './hooks/useMeshMaker';

function App() {
  const { canvasRef, containerRef, isLoading, error, module } = useMeshMaker();

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

  const handleAddPrimitive = (type: string) => {
    if (!module) return;
    
    switch (type) {
      case 'cube':
        module.addCube();
        break;
      case 'cylinder':
        module.addCylinder(20);
        break;
      case 'sphere':
        module.addSphere(20);
        break;
      case 'plane':
        module.addPlane();
        break;
    }
  };

  const selectionCount = module?.getSelectionCount() ?? 0;

  return (
    <div className="flex h-screen w-screen overflow-hidden">
      {/* Left Toolbar */}
      <Toolbar
        onToolChange={handleToolChange}
        onEditModeChange={handleEditModeChange}
        onAddPrimitive={handleAddPrimitive}
      />

      {/* Main Viewport */}
      <Viewport
        canvasRef={canvasRef}
        containerRef={containerRef}
        isLoading={isLoading}
        error={error}
      />

      {/* Right Properties Panel */}
      <PropertiesPanel selectionCount={selectionCount} />
    </div>
  );
}

export default App;
