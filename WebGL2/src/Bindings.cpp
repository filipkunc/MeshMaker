// Emscripten bindings for exposing C++ functions to JavaScript
// This file is only compiled when building with Emscripten

#ifdef EMSCRIPTEN_BUILD

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "Serialization.h"
#include "ItemCollection.h"

// External declarations for API functions defined in main.cpp
extern void api_addCube();
extern void api_addPlane();
extern void api_addCylinder(int steps);
extern void api_addSphere(int steps);
extern void api_addIcosahedron();

extern int api_getEditMode();
extern void api_setEditMode(int mode);

extern int api_getTransformMode();
extern void api_setTransformMode(int mode);

extern int api_getSelectionCount();
extern void api_selectAll();
extern void api_deselectAll();
extern void api_deleteSelection();
extern void api_duplicateSelection();

extern bool api_canUndo();
extern bool api_canRedo();
extern void api_undo();
extern void api_redo();

extern void api_flipSelectedFaces();
extern void api_subdivideSelectedFaces();
extern void api_splitSelected();
extern void api_triangulateSelectedFaces();
extern void api_extrudeSelectedFaces();
extern void api_mergeSelectedVertices();

extern void api_selectEdgeLoop();
extern void api_selectEdgeRing();
extern void api_growEdgeSelection();

extern int api_getViewMode();
extern void api_setViewMode(int mode);
extern bool api_getShowGrid();
extern void api_setShowGrid(bool show);
extern bool api_getShowNormals();
extern void api_setShowNormals(bool show);

extern int api_getItemCount();

extern bool api_getShowImGui();
extern void api_setShowImGui(bool show);

// Unified selection value API (respects current transform mode)
extern float api_getSelectionX();
extern float api_getSelectionY();
extern float api_getSelectionZ();
extern void api_setSelectionX(float value);
extern void api_setSelectionY(float value);
extern void api_setSelectionZ(float value);

// Legacy position-only API
extern float api_getSelectionCenterX();
extern float api_getSelectionCenterY();
extern float api_getSelectionCenterZ();
extern void api_setSelectionPosition(float x, float y, float z);
extern void api_rotateSelection(float xDegrees, float yDegrees, float zDegrees);
extern void api_scaleSelection(float xOffset, float yOffset, float zOffset);

extern int api_getMeshSteps();
extern void api_setMeshSteps(int steps);

// Serialization API
extern std::string api_exportToOBJ();
extern bool api_importFromOBJ(const std::string& objData);
extern emscripten::val api_exportToGLB();
extern bool api_importFromGLBArray(emscripten::val data);
extern emscripten::val api_importGLBParse(emscripten::val data);
extern emscripten::val api_importGLBStepInfo(int stepIndex);
extern bool api_importGLBStep(int stepIndex);
extern bool api_importGLBFinalize();
extern void api_clearScene();
extern int api_getItemCount_export();

// Texture API
extern bool api_loadTextureFromRGBA(emscripten::val data, int width, int height);
extern bool api_loadTextureFromFileData(emscripten::val data);
extern void api_removeTexture();
extern bool api_selectionHasTexture();

// UV Mapping API
extern void api_unwrapSelectedUVs(int projectionType);
extern void api_unwrapAllUVs(int projectionType);
extern void api_markSelectedEdgesAsSeam(bool isSeam);
extern void api_clearAllSeams();

// UV View Mode API
extern bool api_getUVViewMode();
extern void api_setUVViewMode(bool enabled);
extern float api_getUVZoom();
extern void api_setUVZoom(float zoom);
extern emscripten::val api_getUVOffset();
extern void api_setUVOffset(float x, float y);
extern void api_renderUV(int width, int height);

// Split View API
extern bool api_getSplitViewEnabled();
extern void api_setSplitViewEnabled(bool enabled);
extern float api_getSplitRatio();
extern void api_setSplitRatio(float ratio);
extern int api_getActiveViewport();
extern void api_setActiveViewport(int viewport);
extern int api_getViewportAtPosition(float x, float y);

// Per-Item Iteration API
extern float api_getItemPositionX(int index);
extern float api_getItemPositionY(int index);
extern float api_getItemPositionZ(int index);
extern void api_setItemPosition(int index, float x, float y, float z);
extern float api_getItemRotationX(int index);
extern float api_getItemRotationY(int index);
extern float api_getItemRotationZ(int index);
extern void api_setItemRotation(int index, float xDeg, float yDeg, float zDeg);
extern float api_getItemScaleX(int index);
extern float api_getItemScaleY(int index);
extern float api_getItemScaleZ(int index);
extern void api_setItemScale(int index, float x, float y, float z);
extern bool api_isItemSelected(int index);
extern void api_selectItemAtIndex(int index);
extern void api_deselectItemAtIndex(int index);
extern bool api_isItemVisible(int index);
extern void api_setItemVisible(int index, bool visible);
extern int api_getItemVertexCount(int index);
extern int api_getItemFaceCount(int index);
extern int api_getItemEdgeCount(int index);

// Per-Vertex Iteration API
extern float api_getVertexX(int itemIndex, int vertexIndex);
extern float api_getVertexY(int itemIndex, int vertexIndex);
extern float api_getVertexZ(int itemIndex, int vertexIndex);
extern void api_setVertexPosition(int itemIndex, int vertexIndex, float x, float y, float z);
extern float api_getVertexNormalX(int itemIndex, int vertexIndex);
extern float api_getVertexNormalY(int itemIndex, int vertexIndex);
extern float api_getVertexNormalZ(int itemIndex, int vertexIndex);
extern bool api_isVertexSelected(int itemIndex, int vertexIndex);
extern void api_setVertexSelected(int itemIndex, int vertexIndex, bool selected);
extern bool api_isFaceSelected(int itemIndex, int faceIndex);
extern void api_setFaceSelected(int itemIndex, int faceIndex, bool selected);
extern int api_getFaceVertexCount(int itemIndex, int faceIndex);
extern int api_getFaceVertexIndex(int itemIndex, int faceIndex, int cornerIndex);

// Mesh Mutation API
extern void api_clearMesh(int itemIndex);
extern int api_addMeshVertex(int itemIndex, float x, float y, float z);
extern int api_addMeshTriangle(int itemIndex, int v0, int v1, int v2);
extern int api_addMeshQuad(int itemIndex, int v0, int v1, int v2, int v3);
extern void api_rebuildMesh(int itemIndex);

// ============================================================================
// Embind Bindings
// ============================================================================

EMSCRIPTEN_BINDINGS(meshmaker) {
    using namespace emscripten;
    
    // Primitives
    function("addCube", &api_addCube);
    function("addPlane", &api_addPlane);
    function("addCylinder", &api_addCylinder);
    function("addSphere", &api_addSphere);
    function("addIcosahedron", &api_addIcosahedron);
    
    // Edit mode (0=Items, 1=Vertices, 2=Triangles, 3=Edges)
    function("getEditMode", &api_getEditMode);
    function("setEditMode", &api_setEditMode);
    
    // Transform mode (0=None, 1=Translate, 2=Rotate, 3=Scale)
    function("getTransformMode", &api_getTransformMode);
    function("setTransformMode", &api_setTransformMode);
    
    // Selection
    function("getSelectionCount", &api_getSelectionCount);
    function("selectAll", &api_selectAll);
    function("deselectAll", &api_deselectAll);
    function("deleteSelection", &api_deleteSelection);
    function("duplicateSelection", &api_duplicateSelection);
    
    // Undo/Redo
    function("canUndo", &api_canUndo);
    function("canRedo", &api_canRedo);
    function("undo", &api_undo);
    function("redo", &api_redo);
    
    // Mesh operations
    function("flipSelectedFaces", &api_flipSelectedFaces);
    function("subdivideSelectedFaces", &api_subdivideSelectedFaces);
    function("splitSelected", &api_splitSelected);
    function("triangulateSelectedFaces", &api_triangulateSelectedFaces);
    function("extrudeSelectedFaces", &api_extrudeSelectedFaces);
    function("mergeSelectedVertices", &api_mergeSelectedVertices);
    
    // Edge selection
    function("selectEdgeLoop", &api_selectEdgeLoop);
    function("selectEdgeRing", &api_selectEdgeRing);
    function("growEdgeSelection", &api_growEdgeSelection);
    
    // View settings
    function("getViewMode", &api_getViewMode);
    function("setViewMode", &api_setViewMode);
    function("getShowGrid", &api_getShowGrid);
    function("setShowGrid", &api_setShowGrid);
    function("getShowNormals", &api_getShowNormals);
    function("setShowNormals", &api_setShowNormals);
    
    // Info
    function("getItemCount", &api_getItemCount);
    
    // ImGui visibility
    function("getShowImGui", &api_getShowImGui);
    function("setShowImGui", &api_setShowImGui);
    
    // Unified selection value API (respects current transform mode)
    function("getSelectionX", &api_getSelectionX);
    function("getSelectionY", &api_getSelectionY);
    function("getSelectionZ", &api_getSelectionZ);
    function("setSelectionX", &api_setSelectionX);
    function("setSelectionY", &api_setSelectionY);
    function("setSelectionZ", &api_setSelectionZ);
    
    // Legacy position-only API
    function("getSelectionCenterX", &api_getSelectionCenterX);
    function("getSelectionCenterY", &api_getSelectionCenterY);
    function("getSelectionCenterZ", &api_getSelectionCenterZ);
    function("setSelectionPosition", &api_setSelectionPosition);
    function("rotateSelection", &api_rotateSelection);
    function("scaleSelection", &api_scaleSelection);
    
    // Mesh steps for primitives
    function("getMeshSteps", &api_getMeshSteps);
    function("setMeshSteps", &api_setMeshSteps);
    
    // Serialization - Import/Export
    function("exportToOBJ", &api_exportToOBJ);
    function("importFromOBJ", &api_importFromOBJ);
    function("exportToGLB", &api_exportToGLB);
    function("importFromGLB", &api_importFromGLBArray);
    function("importGLBParse", &api_importGLBParse);
    function("importGLBStepInfo", &api_importGLBStepInfo);
    function("importGLBStep", &api_importGLBStep);
    function("importGLBFinalize", &api_importGLBFinalize);
    function("clearScene", &api_clearScene);
    
    // Texture API
    function("loadTextureFromRGBA", &api_loadTextureFromRGBA);
    function("loadTextureFromFileData", &api_loadTextureFromFileData);
    function("removeTexture", &api_removeTexture);
    function("selectionHasTexture", &api_selectionHasTexture);
    
    // UV Mapping API
    // Projection types: 0=Box, 1=Planar, 2=Cylindrical, 3=Spherical
    function("unwrapSelectedUVs", &api_unwrapSelectedUVs);
    function("unwrapAllUVs", &api_unwrapAllUVs);
    
    // Seam marking API
    function("markSelectedEdgesAsSeam", &api_markSelectedEdgesAsSeam);
    function("clearAllSeams", &api_clearAllSeams);
    
    // UV View Mode API
    function("getUVViewMode", &api_getUVViewMode);
    function("setUVViewMode", &api_setUVViewMode);
    function("getUVZoom", &api_getUVZoom);
    function("setUVZoom", &api_setUVZoom);
    function("getUVOffset", &api_getUVOffset);
    function("setUVOffset", &api_setUVOffset);
    function("renderUV", &api_renderUV);
    
    // Split View API
    function("getSplitViewEnabled", &api_getSplitViewEnabled);
    function("setSplitViewEnabled", &api_setSplitViewEnabled);
    function("getSplitRatio", &api_getSplitRatio);
    function("setSplitRatio", &api_setSplitRatio);
    function("getActiveViewport", &api_getActiveViewport);
    function("setActiveViewport", &api_setActiveViewport);
    function("getViewportAtPosition", &api_getViewportAtPosition);
    
    // Per-Item Iteration API
    function("getItemPositionX", &api_getItemPositionX);
    function("getItemPositionY", &api_getItemPositionY);
    function("getItemPositionZ", &api_getItemPositionZ);
    function("setItemPosition", &api_setItemPosition);
    function("getItemRotationX", &api_getItemRotationX);
    function("getItemRotationY", &api_getItemRotationY);
    function("getItemRotationZ", &api_getItemRotationZ);
    function("setItemRotation", &api_setItemRotation);
    function("getItemScaleX", &api_getItemScaleX);
    function("getItemScaleY", &api_getItemScaleY);
    function("getItemScaleZ", &api_getItemScaleZ);
    function("setItemScale", &api_setItemScale);
    function("isItemSelected", &api_isItemSelected);
    function("selectItemAtIndex", &api_selectItemAtIndex);
    function("deselectItemAtIndex", &api_deselectItemAtIndex);
    function("isItemVisible", &api_isItemVisible);
    function("setItemVisible", &api_setItemVisible);
    function("getItemVertexCount", &api_getItemVertexCount);
    function("getItemFaceCount", &api_getItemFaceCount);
    function("getItemEdgeCount", &api_getItemEdgeCount);
    
    // Per-Vertex Iteration API
    function("getVertexX", &api_getVertexX);
    function("getVertexY", &api_getVertexY);
    function("getVertexZ", &api_getVertexZ);
    function("setVertexPosition", &api_setVertexPosition);
    function("getVertexNormalX", &api_getVertexNormalX);
    function("getVertexNormalY", &api_getVertexNormalY);
    function("getVertexNormalZ", &api_getVertexNormalZ);
    function("isVertexSelected", &api_isVertexSelected);
    function("setVertexSelected", &api_setVertexSelected);
    function("isFaceSelected", &api_isFaceSelected);
    function("setFaceSelected", &api_setFaceSelected);
    function("getFaceVertexCount", &api_getFaceVertexCount);
    function("getFaceVertexIndex", &api_getFaceVertexIndex);
    
    // Mesh Mutation API
    function("clearMesh", &api_clearMesh);
    function("addMeshVertex", &api_addMeshVertex);
    function("addMeshTriangle", &api_addMeshTriangle);
    function("addMeshQuad", &api_addMeshQuad);
    function("rebuildMesh", &api_rebuildMesh);
}

#endif // EMSCRIPTEN_BUILD
