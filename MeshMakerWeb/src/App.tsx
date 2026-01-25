import { Toolbar, PropertiesPanel, Viewport } from './components';
import { useMeshMaker } from './hooks/useMeshMaker';

function App() {
  const { canvasRef, containerRef, isLoading, error } = useMeshMaker();

  const handleToolChange = (tool: string) => {
    console.log('Tool changed:', tool);
    // TODO: Call WASM function to change tool
  };

  const handleEditModeChange = (mode: string) => {
    console.log('Edit mode changed:', mode);
    // TODO: Call WASM function to change edit mode
  };

  const handleAddPrimitive = (type: string) => {
    console.log('Add primitive:', type);
    // TODO: Call WASM function to add primitive
  };

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
      <PropertiesPanel selectionCount={0} />
    </div>
  );
}

export default App;
