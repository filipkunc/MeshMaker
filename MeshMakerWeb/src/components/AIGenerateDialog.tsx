import { useState, useRef, useCallback } from 'react';
import { useAIGeneration, getStoredServerUrl, setServerUrl } from '../hooks/useAIGeneration';

type InputMode = 'image' | 'text';

interface AIGenerateDialogProps {
  open: boolean;
  onClose: () => void;
  onGenerated: (glbData: Uint8Array) => void;
}

export function AIGenerateDialog({ open, onClose, onGenerated }: AIGenerateDialogProps) {
  const [inputMode, setInputMode] = useState<InputMode>('image');
  const [imageFile, setImageFile] = useState<File | null>(null);
  const [imagePreview, setImagePreview] = useState<string | null>(null);
  const [textPrompt, setTextPrompt] = useState('');
  const [seed, setSeed] = useState(42);
  const [showAdvanced, setShowAdvanced] = useState(false);
  const [serverUrlInput, setServerUrlInput] = useState(getStoredServerUrl());
  const [connectionStatus, setConnectionStatus] = useState<'untested' | 'ok' | 'failed'>('untested');

  // Advanced Hunyuan3D-2 parameters (defaults match Gradio UI)
  const [octreeResolution, setOctreeResolution] = useState(256);
  const [numInferenceSteps, setNumInferenceSteps] = useState(30);
  const [guidanceScale, setGuidanceScale] = useState(5.0);
  const [numChunks, setNumChunks] = useState(8000);
  const [texture, setTexture] = useState(false);
  const [noRembg, setNoRembg] = useState(false);
  const [randomizeSeed, setRandomizeSeed] = useState(true);

  const fileInputRef = useRef<HTMLInputElement>(null);
  const { isGenerating, progress, error, generate, cancel, testConnection } =
    useAIGeneration();

  const handleImageSelect = useCallback((e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (!file) return;
    setImageFile(file);

    const reader = new FileReader();
    reader.onload = () => {
      const dataUrl = reader.result as string;
      setImagePreview(dataUrl);
    };
    reader.readAsDataURL(file);
    e.target.value = '';
  }, []);

  const handleDrop = useCallback((e: React.DragEvent) => {
    e.preventDefault();
    const file = e.dataTransfer.files?.[0];
    if (!file || !file.type.startsWith('image/')) return;
    setImageFile(file);

    const reader = new FileReader();
    reader.onload = () => {
      const dataUrl = reader.result as string;
      setImagePreview(dataUrl);
    };
    reader.readAsDataURL(file);
  }, []);

  const canGenerate = inputMode === 'image' ? !!imageFile : textPrompt.trim().length > 0;

  const handleGenerate = async () => {
    if (!canGenerate) return;
    const glbData = await generate({
      imageFile: inputMode === 'image' ? imageFile ?? undefined : undefined,
      text: inputMode === 'text' ? textPrompt.trim() : undefined,
      seed: randomizeSeed ? Math.floor(Math.random() * 10000000) : seed,
      octreeResolution,
      numInferenceSteps,
      guidanceScale,
      numChunks,
      texture,
      noRembg,
    });
    if (glbData) {
      onGenerated(glbData);
      onClose();
    }
  };

  const handleTestConnection = async () => {
    setConnectionStatus('untested');
    const ok = await testConnection();
    setConnectionStatus(ok ? 'ok' : 'failed');
  };

  const handleServerUrlSave = () => {
    const trimmed = serverUrlInput.trim().replace(/\/+$/, '');
    setServerUrl(trimmed);
    setConnectionStatus('untested');
  };

  if (!open) return null;

  return (
    <div className="fixed inset-0 bg-black/60 flex items-center justify-center z-[100]" onClick={onClose}>
      <div
        className="bg-zinc-800 rounded-xl shadow-2xl w-[520px] max-h-[90vh] overflow-y-auto border border-zinc-700"
        onClick={(e) => e.stopPropagation()}
      >
        {/* Header */}
        <div className="flex items-center justify-between px-5 py-4 border-b border-zinc-700">
          <h2 className="text-lg font-semibold text-white">AI 3D Generation (Hunyuan3D-2)</h2>
          <button onClick={onClose} className="text-zinc-400 hover:text-white text-xl leading-none">
            &times;
          </button>
        </div>

        <div className="px-5 py-4 space-y-4">
          {/* Input Mode Toggle */}
          <div className="flex gap-1 bg-zinc-900 rounded-lg p-1">
            <button
              onClick={() => setInputMode('image')}
              className={`flex-1 px-3 py-1.5 text-sm rounded-md transition-colors ${
                inputMode === 'image'
                  ? 'bg-blue-600 text-white'
                  : 'text-zinc-400 hover:text-zinc-300'
              }`}
              disabled={isGenerating}
            >
              &#128247; Image to 3D
            </button>
            <button
              onClick={() => setInputMode('text')}
              className={`flex-1 px-3 py-1.5 text-sm rounded-md transition-colors ${
                inputMode === 'text'
                  ? 'bg-blue-600 text-white'
                  : 'text-zinc-400 hover:text-zinc-300'
              }`}
              disabled={isGenerating}
            >
              &#9997; Text to 3D
            </button>
          </div>

          {/* Image Input */}
          {inputMode === 'image' && (
            <div>
              <label className="block text-sm text-zinc-400 mb-1">Reference image</label>
              <input
                ref={fileInputRef}
                type="file"
                accept="image/png,image/jpeg,image/webp"
                onChange={handleImageSelect}
                className="hidden"
              />
              <div
                onClick={() => !isGenerating && fileInputRef.current?.click()}
                onDrop={handleDrop}
                onDragOver={(e) => e.preventDefault()}
                className={`border-2 border-dashed rounded-lg p-4 text-center cursor-pointer transition-colors ${
                  imagePreview
                    ? 'border-blue-500 bg-zinc-900'
                    : 'border-zinc-600 hover:border-zinc-400 bg-zinc-900'
                } ${isGenerating ? 'opacity-50 cursor-not-allowed' : ''}`}
              >
                {imagePreview ? (
                  <div className="flex flex-col items-center gap-2">
                    <img
                      src={imagePreview}
                      alt="Preview"
                      className="max-h-40 rounded object-contain"
                    />
                    <span className="text-xs text-zinc-400">Click to change</span>
                  </div>
                ) : (
                  <div className="py-4">
                    <div className="text-2xl mb-2">&#128247;</div>
                    <div className="text-sm text-zinc-400">Click or drag an image here</div>
                    <div className="text-xs text-zinc-500 mt-1">PNG, JPG, or WebP</div>
                  </div>
                )}
              </div>
            </div>
          )}

          {/* Text Input */}
          {inputMode === 'text' && (
            <div>
              <label className="block text-sm text-zinc-400 mb-1">Text prompt</label>
              <textarea
                value={textPrompt}
                onChange={(e) => setTextPrompt(e.target.value)}
                placeholder="Describe the 3D object you want to generate..."
                className="w-full bg-zinc-900 border border-zinc-600 rounded-lg px-3 py-2 text-white text-sm focus:outline-none focus:border-blue-500 resize-none"
                rows={3}
                disabled={isGenerating}
              />
              <div className="text-xs text-zinc-500 mt-1">
                Requires server started with --enable_t23d flag
              </div>
            </div>
          )}

          {/* Seed */}
          <div>
            <label className="block text-xs text-zinc-400 mb-1">Seed</label>
            <div className="flex gap-2 items-center">
              <input
                type="number"
                value={seed}
                onChange={(e) => setSeed(Number(e.target.value))}
                className={`flex-1 bg-zinc-900 border border-zinc-600 rounded px-2 py-1 text-white text-xs focus:outline-none focus:border-blue-500 ${randomizeSeed ? 'opacity-50' : ''}`}
                disabled={isGenerating || randomizeSeed}
              />
              <label className="flex items-center gap-1 text-xs text-zinc-300 cursor-pointer whitespace-nowrap">
                <input
                  type="checkbox"
                  checked={randomizeSeed}
                  onChange={(e) => setRandomizeSeed(e.target.checked)}
                  className="rounded border-zinc-600"
                  disabled={isGenerating}
                />
                Randomize
              </label>
            </div>
          </div>

          {/* Advanced Options */}
          <div>
            <button
              onClick={() => setShowAdvanced(!showAdvanced)}
              className="text-sm text-zinc-400 hover:text-zinc-300 flex items-center gap-1"
            >
              <span className={`transition-transform ${showAdvanced ? 'rotate-90' : ''}`}>&#9654;</span>
              Advanced Options
            </button>

            {showAdvanced && (
              <div className="mt-3 space-y-3 bg-zinc-900 rounded-lg p-3 border border-zinc-700">
                {/* Generation Parameters */}
                <div className="grid grid-cols-2 gap-3">
                  <div>
                    <label className="block text-xs text-zinc-400 mb-1">Octree Resolution</label>
                    <input
                      type="number"
                      value={octreeResolution}
                      onChange={(e) => setOctreeResolution(Number(e.target.value))}
                      className="w-full bg-zinc-800 border border-zinc-600 rounded px-2 py-1 text-white text-xs focus:outline-none focus:border-blue-500"
                      disabled={isGenerating}
                      min={64}
                      max={512}
                      step={64}
                    />
                  </div>
                  <div>
                    <label className="block text-xs text-zinc-400 mb-1">Inference Steps</label>
                    <input
                      type="number"
                      value={numInferenceSteps}
                      onChange={(e) => setNumInferenceSteps(Number(e.target.value))}
                      className="w-full bg-zinc-800 border border-zinc-600 rounded px-2 py-1 text-white text-xs focus:outline-none focus:border-blue-500"
                      disabled={isGenerating}
                      min={1}
                      max={50}
                    />
                  </div>
                  <div>
                    <label className="block text-xs text-zinc-400 mb-1">Guidance Scale</label>
                    <input
                      type="number"
                      value={guidanceScale}
                      onChange={(e) => setGuidanceScale(Number(e.target.value))}
                      className="w-full bg-zinc-800 border border-zinc-600 rounded px-2 py-1 text-white text-xs focus:outline-none focus:border-blue-500"
                      disabled={isGenerating}
                      min={1}
                      max={20}
                      step={0.5}
                    />
                  </div>
                  <div>
                    <label className="block text-xs text-zinc-400 mb-1">Number of Chunks</label>
                    <input
                      type="number"
                      value={numChunks}
                      onChange={(e) => setNumChunks(Number(e.target.value))}
                      className="w-full bg-zinc-800 border border-zinc-600 rounded px-2 py-1 text-white text-xs focus:outline-none focus:border-blue-500"
                      disabled={isGenerating}
                      min={1000}
                      max={5000000}
                      step={1000}
                    />
                  </div>
                </div>

                {/* Checkboxes */}
                <div className="space-y-2">
                  <label className="flex items-center gap-2 text-xs text-zinc-300 cursor-pointer">
                    <input
                      type="checkbox"
                      checked={texture}
                      onChange={(e) => setTexture(e.target.checked)}
                      className="rounded border-zinc-600"
                      disabled={isGenerating}
                    />
                    Enable texture painting
                    <span className="text-zinc-500">(requires texture model loaded)</span>
                  </label>
                  <label className="flex items-center gap-2 text-xs text-zinc-300 cursor-pointer">
                    <input
                      type="checkbox"
                      checked={noRembg}
                      onChange={(e) => setNoRembg(e.target.checked)}
                      className="rounded border-zinc-600"
                      disabled={isGenerating}
                    />
                    Skip background removal
                  </label>
                </div>

                {/* Server URL */}
                <div className="pt-2 border-t border-zinc-700">
                  <label className="block text-xs text-zinc-400 mb-1">Hunyuan3D-2 Server URL</label>
                  <div className="flex gap-2">
                    <input
                      type="text"
                      value={serverUrlInput}
                      onChange={(e) => setServerUrlInput(e.target.value)}
                      className="flex-1 bg-zinc-800 border border-zinc-600 rounded px-2 py-1 text-white text-xs focus:outline-none focus:border-blue-500"
                      placeholder="http://localhost:8080"
                      disabled={isGenerating}
                    />
                    <button
                      onClick={handleServerUrlSave}
                      className="px-2 py-1 text-xs bg-zinc-700 hover:bg-zinc-600 rounded text-zinc-300"
                      disabled={isGenerating}
                    >
                      Save
                    </button>
                    <button
                      onClick={handleTestConnection}
                      className="px-2 py-1 text-xs bg-zinc-700 hover:bg-zinc-600 rounded text-zinc-300"
                      disabled={isGenerating}
                    >
                      Test
                    </button>
                  </div>
                  {connectionStatus === 'ok' && (
                    <div className="text-xs text-green-400 mt-1">Connected to Hunyuan3D-2 server</div>
                  )}
                  {connectionStatus === 'failed' && (
                    <div className="text-xs text-red-400 mt-1">
                      Cannot connect. Is the server running? (python gradio_app.py --low_vram_mode)
                    </div>
                  )}
                </div>
              </div>
            )}
          </div>

          {/* Error Display */}
          {error && (
            <div className="bg-red-900/40 border border-red-700 rounded-lg px-3 py-2 text-sm text-red-300">
              {error}
            </div>
          )}

          {/* Progress Display */}
          {isGenerating && (
            <div className="space-y-2">
              <div className="flex items-center gap-3 bg-zinc-900 rounded-lg px-3 py-3 border border-zinc-700">
                <div className="animate-spin rounded-full h-5 w-5 border-2 border-blue-500 border-t-transparent shrink-0" />
                <span className="text-sm text-zinc-300">{progress}</span>
              </div>
            </div>
          )}
        </div>

        {/* Footer */}
        <div className="flex justify-end gap-2 px-5 py-4 border-t border-zinc-700">
          {isGenerating ? (
            <button
              onClick={cancel}
              className="px-4 py-2 text-sm font-medium bg-red-600 hover:bg-red-500 text-white rounded-lg transition-colors"
            >
              Cancel
            </button>
          ) : (
            <>
              <button
                onClick={onClose}
                className="px-4 py-2 text-sm font-medium bg-zinc-700 hover:bg-zinc-600 text-zinc-300 rounded-lg transition-colors"
              >
                Close
              </button>
              <button
                onClick={handleGenerate}
                disabled={!canGenerate}
                className={`px-4 py-2 text-sm font-medium rounded-lg transition-colors ${
                  canGenerate
                    ? 'bg-blue-600 hover:bg-blue-500 text-white'
                    : 'bg-zinc-700 text-zinc-500 cursor-not-allowed'
                }`}
              >
                Generate
              </button>
            </>
          )}
        </div>
      </div>
    </div>
  );
}
