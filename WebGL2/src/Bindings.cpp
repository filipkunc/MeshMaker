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
extern void api_triangulateSelectedFaces();
extern void api_extrudeSelectedFaces();
extern void api_splitSelectedEdges();
extern void api_mergeSelectedVertices();

extern int api_getViewMode();
extern void api_setViewMode(int mode);
extern bool api_getShowGrid();
extern void api_setShowGrid(bool show);

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
extern void api_clearScene();
extern int api_getItemCount_export();

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
    function("triangulateSelectedFaces", &api_triangulateSelectedFaces);
    function("extrudeSelectedFaces", &api_extrudeSelectedFaces);
    function("splitSelectedEdges", &api_splitSelectedEdges);
    function("mergeSelectedVertices", &api_mergeSelectedVertices);
    
    // View settings
    function("getViewMode", &api_getViewMode);
    function("setViewMode", &api_setViewMode);
    function("getShowGrid", &api_getShowGrid);
    function("setShowGrid", &api_setShowGrid);
    
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
    function("clearScene", &api_clearScene);
}

#endif // EMSCRIPTEN_BUILD
