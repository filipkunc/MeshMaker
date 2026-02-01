import { useRef } from 'react';

type Tool = 'select' | 'translate' | 'rotate' | 'scale';
type EditMode = 'items' | 'vertices' | 'triangles' | 'edges';

interface TopToolbarProps {
  // Tools
  activeTool: Tool;
  onToolChange: (tool: Tool) => void;
  // Edit mode
  editMode: EditMode;
  onEditModeChange: (mode: EditMode) => void;
  // Primitives
  onAddPrimitive: (type: string) => void;
  // Undo/Redo
  canUndo: boolean;
  canRedo: boolean;
  onUndo: () => void;
  onRedo: () => void;
  // File
  onImportFile: (file: File) => void;
  onExportOBJ: () => void;
  onExportGLB: () => void;
  onClearScene: () => void;
  // Mesh operations
  onSubdivide: () => void;
  onMerge: () => void;
  onSplit: () => void;
  // Edge selection
  onSelectEdgeLoop?: () => void;
  onSelectEdgeRing?: () => void;
  onGrowEdgeSelection?: () => void;
}

interface IconButtonProps {
  icon: string;
  title: string;
  onClick: () => void;
  active?: boolean;
  disabled?: boolean;
  size?: 'sm' | 'md';
}

function IconButton({ icon, title, onClick, active = false, disabled = false, size = 'md' }: IconButtonProps) {
  const sizeClass = size === 'sm' ? 'w-7 h-7' : 'w-9 h-9';
  const imgSize = size === 'sm' ? 'w-5 h-5' : 'w-7 h-7';
  
  return (
    <button
      onClick={onClick}
      disabled={disabled}
      title={title}
      className={`${sizeClass} flex items-center justify-center rounded transition-colors ${
        disabled
          ? 'opacity-30 cursor-not-allowed'
          : active
          ? 'bg-blue-600 hover:bg-blue-500'
          : 'hover:bg-zinc-600'
      }`}
    >
      <img 
        src={icon} 
        alt={title} 
        className={`${imgSize} object-contain`}
        style={{ filter: 'invert(1) brightness(0.9)' }}
      />
    </button>
  );
}

interface TextButtonProps {
  label: string;
  title: string;
  onClick: () => void;
  active?: boolean;
  disabled?: boolean;
}

function TextButton({ label, title, onClick, active = false, disabled = false }: TextButtonProps) {
  return (
    <button
      onClick={onClick}
      disabled={disabled}
      title={title}
      className={`px-2 py-1 text-xs font-medium rounded transition-colors ${
        disabled
          ? 'text-zinc-600 cursor-not-allowed'
          : active
          ? 'bg-blue-600 text-white'
          : 'text-zinc-300 hover:bg-zinc-600'
      }`}
    >
      {label}
    </button>
  );
}

function EmojiButton({ label, title, onClick, disabled = false }: TextButtonProps) {
  return (
    <button
      onClick={onClick}
      disabled={disabled}
      title={title}
      className={`w-9 h-9 flex items-center justify-center text-lg rounded transition-colors ${
        disabled
          ? 'opacity-30 cursor-not-allowed'
          : 'hover:bg-zinc-600'
      }`}
    >
      {label}
    </button>
  );
}

function Separator() {
  return <div className="w-px h-6 bg-zinc-600 mx-1" />;
}

export function TopToolbar({
  activeTool,
  onToolChange,
  editMode,
  onEditModeChange,
  onAddPrimitive,
  canUndo,
  canRedo,
  onUndo,
  onRedo,
  onImportFile,
  onExportOBJ,
  onExportGLB,
  onClearScene,
  onSubdivide,
  onMerge,
  onSplit,
  onSelectEdgeLoop,
  onSelectEdgeRing,
  onGrowEdgeSelection,
}: TopToolbarProps) {
  const fileInputRef = useRef<HTMLInputElement>(null);

  const handleFileImport = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (file) {
      onImportFile(file);
      e.target.value = '';
    }
  };

  return (
    <div className="flex items-center gap-1 px-2 py-1 bg-zinc-800 border-b border-zinc-700">
      {/* File Operations */}
      <input
        ref={fileInputRef}
        type="file"
        accept=".obj,.glb"
        onChange={handleFileImport}
        className="hidden"
      />
      <EmojiButton label="📂" title="Import (OBJ/GLB)" onClick={() => fileInputRef.current?.click()} />
      <EmojiButton label="💾" title="Export OBJ" onClick={onExportOBJ} />
      <EmojiButton label="📦" title="Export GLB" onClick={onExportGLB} />
      
      <Separator />
      
      {/* Undo/Redo */}
      <EmojiButton label="↩" title="Undo (Ctrl+Z)" onClick={onUndo} disabled={!canUndo} />
      <EmojiButton label="↪" title="Redo (Ctrl+Y)" onClick={onRedo} disabled={!canRedo} />
      
      <Separator />
      
      {/* Transform Tools */}
      <IconButton
        icon="/icons/SelectTemplate.png"
        title="Select (1)"
        onClick={() => onToolChange('select')}
        active={activeTool === 'select'}
      />
      <IconButton
        icon="/icons/TranslateTemplate.png"
        title="Translate (2)"
        onClick={() => onToolChange('translate')}
        active={activeTool === 'translate'}
      />
      <IconButton
        icon="/icons/RotateTemplate.png"
        title="Rotate (3)"
        onClick={() => onToolChange('rotate')}
        active={activeTool === 'rotate'}
      />
      <IconButton
        icon="/icons/ScaleTemplate.png"
        title="Scale (4)"
        onClick={() => onToolChange('scale')}
        active={activeTool === 'scale'}
      />
      
      <Separator />
      
      {/* Edit Mode */}
      <TextButton
        label="Items"
        title="Edit Items (Q)"
        onClick={() => onEditModeChange('items')}
        active={editMode === 'items'}
      />
      <TextButton
        label="Verts"
        title="Edit Vertices (W)"
        onClick={() => onEditModeChange('vertices')}
        active={editMode === 'vertices'}
      />
      <TextButton
        label="Tris"
        title="Edit Triangles (E)"
        onClick={() => onEditModeChange('triangles')}
        active={editMode === 'triangles'}
      />
      <TextButton
        label="Edges"
        title="Edit Edges (R)"
        onClick={() => onEditModeChange('edges')}
        active={editMode === 'edges'}
      />
      
      {/* Edge Loop/Ring Selection (only in Edges mode) */}
      {editMode === 'edges' && (
        <>
          <Separator />
          <TextButton
            label="Loop"
            title="Select Edge Loop (L)"
            onClick={() => onSelectEdgeLoop?.()}
          />
          <TextButton
            label="Ring"
            title="Select Edge Ring (Shift+L)"
            onClick={() => onSelectEdgeRing?.()}
          />
          <TextButton
            label="Grow"
            title="Grow Edge Selection (G)"
            onClick={() => onGrowEdgeSelection?.()}
          />
        </>
      )}
      
      <Separator />
      
      {/* Mesh Operations */}
      <IconButton
        icon="/icons/SubdivisionTemplate.png"
        title="Subdivide (S)"
        onClick={onSubdivide}
        size="sm"
      />
      <IconButton
        icon="/icons/MergeTemplate.png"
        title="Merge Vertices (M)"
        onClick={onMerge}
        size="sm"
      />
      <IconButton
        icon="/icons/SplitTemplate.png"
        title="Split Edges"
        onClick={onSplit}
        size="sm"
      />
      
      <Separator />
      
      {/* Add Primitives */}
      <IconButton
        icon="/icons/CubeTemplate.png"
        title="Add Cube"
        onClick={() => onAddPrimitive('cube')}
      />
      <IconButton
        icon="/icons/PlaneTemplate.png"
        title="Add Plane"
        onClick={() => onAddPrimitive('plane')}
      />
      <IconButton
        icon="/icons/CylinderTemplate.png"
        title="Add Cylinder"
        onClick={() => onAddPrimitive('cylinder')}
      />
      <IconButton
        icon="/icons/SphereTemplate.png"
        title="Add Sphere"
        onClick={() => onAddPrimitive('sphere')}
      />
      <IconButton
        icon="/icons/IcosahedronTemplate.png"
        title="Add Icosahedron"
        onClick={() => onAddPrimitive('icosahedron')}
      />
      
      <Separator />
      
      {/* Clear */}
      <EmojiButton label="🗑️" title="Clear Scene" onClick={onClearScene} />
    </div>
  );
}
