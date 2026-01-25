// Emscripten bindings for exposing C++ functions to JavaScript
// This file is only compiled when building with Emscripten

#ifdef EMSCRIPTEN_BUILD

#include <emscripten/bind.h>

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
}

#endif // EMSCRIPTEN_BUILD
