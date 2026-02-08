#ifdef EMSCRIPTEN_BUILD
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#endif

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>

#include "Camera.h"
#include "Mesh2.h"
#include "Item.h"
#include "ItemCollection.h"
#include "Grid.h"
#include "Shader.h"
#include "Manipulator.h"
#include "UndoManager.h"
#include "Texture.h"
#include "DebugLog.h"

// Captures the transform state of selected items for undo
struct ItemManipulationState {
    size_t index;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

// Captures the full mesh state for undo (vertices, faces, edges, selection)
struct MeshState {
    size_t itemIndex;  // Which item this mesh belongs to
    std::vector<MeshVertex> vertices;
    std::vector<Face> faces;
    std::vector<Edge> edges;
    SelectionMode selectionMode;
};

// Captures a full item state for scene undo (includes mesh)
struct ItemState {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    bool selected;
    bool visible;
    std::vector<MeshVertex> meshVertices;
    std::vector<Face> meshFaces;
    std::vector<Edge> meshEdges;
    SelectionMode meshSelectionMode;
    std::shared_ptr<Texture> texture;  // Preserve texture through undo/redo
};

// Captures the full scene state (all items)
struct SceneState {
    std::vector<ItemState> items;
};

// Transform mode enum
enum class TransformMode {
    None,
    Translate,
    Rotate,
    Scale
};

// Global state for main loop
struct AppState {
    GLFWwindow* window = nullptr;
    int windowWidth = 1280;
    int windowHeight = 720;
    int framebufferWidth = 1280;   // Actual framebuffer size (may differ on High DPI)
    int framebufferHeight = 720;
    float contentScaleX = 1.0f;    // Scale for mouse coord → framebuffer coord conversion
    float contentScaleY = 1.0f;
    float dpiScale = 1.0f;         // Actual DPI scale for ImGui font/style sizing
    bool fontNeedsRebuild = false; // Flag to rebuild font at proper scale
    
    Camera camera;
    std::unique_ptr<ItemCollection> items;
    std::unique_ptr<Grid> grid;
    std::unique_ptr<Shader> meshShader;
    std::unique_ptr<Shader> meshWireShader;  // Single-pass solid+wireframe shader
    std::unique_ptr<Shader> gridShader;
    std::unique_ptr<Shader> selectionShader;
    std::unique_ptr<Shader> rectShader;
    std::unique_ptr<Shader> coloredShader;  // For vertices and edges with per-vertex colors
    
    // Rectangle VAO/VBO for selection rectangle drawing
    uint32_t rectVao = 0;
    uint32_t rectVbo = 0;
    
    // Mouse state
    bool mousePressed = false;
    bool middleMousePressed = false;
    bool rightMousePressed = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    double clickMouseX = 0.0;  // Position at mouse press for selection
    double clickMouseY = 0.0;
    
    // Rectangle selection state
    bool isSelecting = false;
    double selectStartX = 0.0;
    double selectStartY = 0.0;
    double selectEndX = 0.0;
    double selectEndY = 0.0;
    
    // Primitive steps for cylinder/sphere
    int meshSteps = 20;
    glm::vec3 meshColor = glm::vec3(0.7f, 0.7f, 0.7f);
    glm::vec3 wireframeColor = glm::vec3(0.1f, 0.1f, 0.1f);
    
    // View settings
    int viewMode = 2;  // SolidWireframe
    bool showGrid = true;
    bool showNormals = false;
#ifdef EMSCRIPTEN_BUILD
    bool showImGui = false;  // Hidden by default in React/web builds
#else
    bool showImGui = true;   // Shown by default in desktop builds
#endif
    
    // Transform settings
    TransformMode transformMode = TransformMode::None;
    float translateStep = 0.1f;
    float rotateStep = 15.0f;  // degrees
    float scaleStep = 0.1f;
    
    // Manipulator (transform gizmo)
    std::unique_ptr<Shader> manipulatorShader;
    std::unique_ptr<Shader> thickLineShader;
    std::unique_ptr<Shader> thickLineColoredShader;  // For screen-space thick lines with per-vertex color
    std::unique_ptr<Manipulator> translateManipulator;
    std::unique_ptr<Manipulator> rotateManipulator;
    std::unique_ptr<Manipulator> scaleManipulator;
    
    // Manipulator dragging state
    bool isDraggingManipulator = false;
    glm::vec3 dragStartPoint;
    glm::vec3 dragAxis;
    
    // Undo/Redo
    UndoManager undoManager;
    std::vector<ItemManipulationState> oldManipulations;  // Captured at manipulation start
    std::unique_ptr<MeshState> oldMeshState;  // Captured at mesh manipulation start
    
    // UV Editor state
    bool isUVViewMode = false;        // When true, render in UV space
    float uvZoom = 1.0f;              // UV view zoom
    glm::vec2 uvOffset = glm::vec2(0.0f);  // UV view pan offset
    std::unique_ptr<Shader> uvShader;       // UV space mesh shader
    std::unique_ptr<Shader> uvGridShader;   // UV grid shader
    std::unique_ptr<Shader> uvColoredShader; // UV vertices/edges shader
    std::unique_ptr<Shader> uvBackgroundShader; // UV background texture shader
    std::unique_ptr<Shader> uvSelectionShader; // UV selection picking shader
    uint32_t uvGridVao = 0;
    uint32_t uvGridVbo = 0;
    uint32_t uvBackgroundVao = 0;
    uint32_t uvBackgroundVbo = 0;
    
    // Split-screen viewport
    bool splitViewEnabled = false;    // When true, show 3D and UV side by side
    float splitRatio = 0.5f;          // 0.0 to 1.0: ratio of 3D viewport width
    int activeViewport = 0;           // 0 = 3D viewport, 1 = UV viewport
};

static AppState g_app;

// Convert screen position to UV coordinates
glm::vec2 screenToUV(double screenX, double screenY) {
    float uvStartX = g_app.windowWidth * (1.0f - g_app.splitRatio);
    float uvWidth = g_app.windowWidth * g_app.splitRatio;
    float uvHeight = static_cast<float>(g_app.windowHeight);
    
    float viewportAspect = uvWidth / uvHeight;
    glm::vec2 aspectAdjust(1.0f, 1.0f);
    if (viewportAspect > 1.0f) {
        aspectAdjust.x = 1.0f / viewportAspect;
    } else {
        aspectAdjust.y = viewportAspect;
    }
    
    float normX = ((static_cast<float>(screenX) - uvStartX) / uvWidth) * 2.0f - 1.0f;
    float normY = 1.0f - (static_cast<float>(screenY) / uvHeight) * 2.0f;
    
    normX /= aspectAdjust.x;
    normY /= aspectAdjust.y;
    
    float u = (normX + 1.0f) / 2.0f;
    float v = (normY + 1.0f) / 2.0f;
    
    u = u / g_app.uvZoom - g_app.uvOffset.x;
    v = v / g_app.uvZoom - g_app.uvOffset.y;
    
    return glm::vec2(u, v);
}

// Get current manipulator based on transform mode
Manipulator* getCurrentManipulator() {
    switch (g_app.transformMode) {
        case TransformMode::Translate:
            return g_app.translateManipulator.get();
        case TransformMode::Rotate:
            return g_app.rotateManipulator.get();
        case TransformMode::Scale:
            return g_app.scaleManipulator.get();
        default:
            return nullptr;
    }
}

// Helper: Get selection count based on current EditMode
size_t getSelectionCount() {
    if (!g_app.items) return 0;
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        return g_app.items->getSelectedItemCount();
    } else {
        return g_app.items->getSelectedComponentCount();
    }
}

// Helper: Get selection center based on current EditMode
glm::vec3 getSelectionCenter() {
    if (!g_app.items) return glm::vec3(0.0f);
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        return g_app.items->getSelectedItemsCenter();
    } else {
        return g_app.items->getSelectedComponentsCenter();
    }
}

// Helper: Translate selection based on current EditMode
void translateSelection(const glm::vec3& offset) {
    if (!g_app.items) return;
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        g_app.items->translateSelectedItems(offset);
    } else {
        g_app.items->translateSelectedComponents(offset);
    }
}

// Helper: Rotate selection based on current EditMode
void rotateSelection(const glm::vec3& axis, float angleRadians) {
    if (!g_app.items) return;
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        g_app.items->rotateSelectedItems(glm::angleAxis(angleRadians, glm::normalize(axis)));
    } else {
        glm::vec3 center = getSelectionCenter();
        glm::quat rotationOffset = glm::angleAxis(angleRadians, glm::normalize(axis));
        g_app.items->rotateSelectedComponents(center, rotationOffset);
    }
}

// Helper: Scale selection based on current EditMode
void scaleSelection(const glm::vec3& center, float factor) {
    if (!g_app.items) return;
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        float scaleFactor = factor > 1.0f ? factor - 1.0f : -(1.0f - factor);
        g_app.items->scaleSelectedItems(glm::vec3(scaleFactor));
    } else {
        g_app.items->scaleSelectedComponents(center, factor);
    }
}

// Helper: Scale selection by offset based on current EditMode
void scaleSelectionByOffset(const glm::vec3& center, const glm::vec3& offset) {
    if (!g_app.items) return;
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        // For items, pass the offset directly for per-axis scaling
        g_app.items->scaleSelectedItemsByOffset(center, offset);
    } else {
        g_app.items->scaleSelectedComponentsByOffset(center, offset);
    }
}

// Helper: Select all based on current EditMode
void selectAll() {
    if (!g_app.items) return;
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        g_app.items->selectAllItems();
    } else {
        g_app.items->selectAllComponents();
    }
}

// Helper: Deselect all based on current EditMode
void deselectAll() {
    if (!g_app.items) return;
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        g_app.items->deselectAllItems();
    } else {
        g_app.items->deselectAllComponents();
    }
}

// Capture current manipulation state of selected items
std::vector<ItemManipulationState> captureItemManipulations() {
    std::vector<ItemManipulationState> states;
    if (!g_app.items) return states;
    
    for (size_t i = 0; i < g_app.items->getItemCount(); ++i) {
        Item* item = g_app.items->getItemAtIndex(i);
        if (item && item->selected) {
            ItemManipulationState state;
            state.index = i;
            state.position = item->position;
            state.rotation = item->rotation;
            state.scale = item->scale;
            states.push_back(state);
        }
    }
    return states;
}

// Apply a captured manipulation state and register undo for the swap
void applyItemManipulations(const std::vector<ItemManipulationState>& states,
                            const std::vector<ItemManipulationState>& currentStates) {
    if (!g_app.items) return;
    
    // Deselect all and apply the old state
    g_app.items->deselectAllItems();
    
    for (const auto& state : states) {
        if (state.index < g_app.items->getItemCount()) {
            Item* item = g_app.items->getItemAtIndex(state.index);
            if (item) {
                item->position = state.position;
                item->rotation = state.rotation;
                item->scale = state.scale;
                item->selected = true;
            }
        }
    }
    
    // Register the reverse operation for undo/redo
    g_app.undoManager.prepareUndo("Manipulations", [states, currentStates]() {
        applyItemManipulations(currentStates, states);
    });
}

// Forward declarations for mesh undo functions
std::unique_ptr<MeshState> captureMeshState();
void applyMeshState(const MeshState& state);
void meshManipulationEnded(const std::string& actionName);

// Called when manipulation starts (mouse press on manipulator)
void manipulationStarted() {
    if (!g_app.items) return;
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        g_app.oldManipulations = captureItemManipulations();
    } else {
        // Component mode - capture mesh state
        g_app.oldMeshState = captureMeshState();
    }
}

// Called when manipulation ends (mouse release)
void manipulationEnded() {
    if (g_app.items && g_app.items->getEditMode() == EditMode::Items) {
        if (!g_app.oldManipulations.empty()) {
            auto currentStates = captureItemManipulations();
            auto oldStates = g_app.oldManipulations;
            
            // Register undo action
            g_app.undoManager.prepareUndo("Manipulations", [oldStates, currentStates]() {
                applyItemManipulations(oldStates, currentStates);
            });
            
            g_app.oldManipulations.clear();
        }
    } else if (g_app.items && g_app.oldMeshState) {
        // Mesh manipulation ended
        meshManipulationEnded("Mesh Manipulation");
    }
}

// Capture current mesh state (for the first selected item's mesh)
std::unique_ptr<MeshState> captureMeshState() {
    if (!g_app.items) return nullptr;
    
    for (size_t i = 0; i < g_app.items->getItemCount(); ++i) {
        Item* item = g_app.items->getItemAtIndex(i);
        if (item && item->selected && item->mesh) {
            auto state = std::make_unique<MeshState>();
            state->itemIndex = i;
            state->selectionMode = item->mesh->getSelectionMode();
            item->mesh->getState(state->vertices, state->faces, state->edges);
            return state;
        }
    }
    return nullptr;
}

// Apply a captured mesh state
void applyMeshState(const MeshState& state) {
    if (!g_app.items) return;
    
    if (state.itemIndex < g_app.items->getItemCount()) {
        Item* item = g_app.items->getItemAtIndex(state.itemIndex);
        if (item && item->mesh) {
            item->mesh->setState(state.vertices, state.faces, state.edges);
            item->mesh->setSelectionMode(state.selectionMode);
            item->selected = true;
        }
    }
}

// Called when mesh manipulation starts
void meshManipulationStarted() {
    if (g_app.items && g_app.items->getEditMode() != EditMode::Items) {
        g_app.oldMeshState = captureMeshState();
    }
}

// Forward declaration for mesh undo helper
void pushMeshUndoAction(const std::string& actionName, const MeshState& fromState, const MeshState& toState);

// Called when mesh manipulation ends - registers the undo action
void meshManipulationEnded(const std::string& actionName) {
    if (!g_app.oldMeshState) return;
    
    auto currentState = captureMeshState();
    if (!currentState) {
        g_app.oldMeshState.reset();
        return;
    }
    
    // Capture by value for the lambda
    MeshState oldState = *g_app.oldMeshState;
    MeshState newState = *currentState;
    
    // Use symmetric undo pattern - each undo/redo prepares its inverse
    pushMeshUndoAction(actionName, oldState, newState);
    
    g_app.oldMeshState.reset();
}

// Push a mesh undo action that, when executed, applies fromState and prepares its inverse
void pushMeshUndoAction(const std::string& actionName, const MeshState& fromState, const MeshState& toState) {
    g_app.undoManager.prepareUndo(actionName, [actionName, fromState, toState]() {
        applyMeshState(fromState);
        // Prepare the inverse action
        pushMeshUndoAction(actionName, toState, fromState);
    });
}

// Perform a mesh action with undo support
void meshActionWithUndo(const std::string& actionName, std::function<void()> action) {
    if (!g_app.items || g_app.items->getEditMode() == EditMode::Items) {
        action();
        return;
    }
    
    auto oldState = captureMeshState();
    if (!oldState) {
        action();
        return;
    }
    
    action();
    
    auto currentState = captureMeshState();
    if (!currentState) return;
    
    // Capture by value for the lambda
    MeshState old = *oldState;
    MeshState current = *currentState;
    
    // Use symmetric undo pattern
    pushMeshUndoAction(actionName, old, current);
}

// Capture full scene state (all items)
SceneState captureSceneState() {
    SceneState state;
    if (!g_app.items) return state;
    
    for (size_t i = 0; i < g_app.items->getItemCount(); ++i) {
        Item* item = g_app.items->getItemAtIndex(i);
        if (item) {
            ItemState itemState;
            itemState.position = item->position;
            itemState.rotation = item->rotation;
            itemState.scale = item->scale;
            itemState.selected = item->selected;
            itemState.visible = item->visible;
            
            itemState.texture = item->texture;
            
            if (item->mesh) {
                itemState.meshSelectionMode = item->mesh->getSelectionMode();
                item->mesh->getState(itemState.meshVertices, itemState.meshFaces, itemState.meshEdges);
            }
            
            state.items.push_back(std::move(itemState));
        }
    }
    return state;
}

// Apply scene state (restores all items)
void applySceneState(const SceneState& state) {
    if (!g_app.items) return;
    
    // Clear current items and recreate from state
    while (g_app.items->getItemCount() > 0) {
        g_app.items->removeItemAtIndex(0);
    }
    
    for (const auto& itemState : state.items) {
        auto item = std::make_unique<Item>();
        item->position = itemState.position;
        item->rotation = itemState.rotation;
        item->scale = itemState.scale;
        item->selected = itemState.selected;
        item->visible = itemState.visible;
        
        item->mesh->setState(itemState.meshVertices, itemState.meshFaces, itemState.meshEdges);
        item->mesh->setSelectionMode(itemState.meshSelectionMode);
        item->texture = itemState.texture;
        
        g_app.items->addItem(std::move(item));
    }
}

// Forward declaration for recursive undo helper
void pushSceneUndoAction(const std::string& actionName, const SceneState& fromState, const SceneState& toState);

// Push a scene undo action that, when executed, applies fromState and prepares its inverse
void pushSceneUndoAction(const std::string& actionName, const SceneState& fromState, const SceneState& toState) {
    g_app.undoManager.prepareUndo(actionName, [actionName, fromState, toState]() {
        applySceneState(fromState);
        // Prepare the inverse action (which will be pushed to redo or undo depending on context)
        pushSceneUndoAction(actionName, toState, fromState);
    });
}

// Perform a scene action with undo support (for add/remove/duplicate items)
void sceneActionWithUndo(const std::string& actionName, std::function<void()> action) {
    SceneState oldState = captureSceneState();
    
    action();
    
    SceneState newState = captureSceneState();
    
    // Register the initial undo action using the symmetric helper
    pushSceneUndoAction(actionName, oldState, newState);
}

// Forward declarations
glm::vec3 screenToWorldRay(double mouseX, double mouseY, glm::vec3& rayOrigin);
int selectAtPoint(int x, int y);
int selectAtPointUV(int x, int y);  // UV viewport selection
std::vector<bool> selectInRect(int x, int y, int width, int height);
std::vector<bool> selectInRectUV(int x, int y, int width, int height);  // UV viewport rect selection
int selectManipulatorAtPoint(int x, int y);
glm::vec3 unprojectPoint(double mouseX, double mouseY, float depth);
glm::vec3 intersectAxisPlane(double mouseX, double mouseY, glm::vec3 planePoint, glm::vec3 planeNormal);
glm::vec3 positionFromDepthBuffer(double mouseX, double mouseY);
void drawSelectionPlaneAtCenter(const glm::vec3& center);
glm::vec3 positionFromAxisPointForTranslation(double mouseX, double mouseY, Axis axis);
glm::vec3 positionFromAxisPoint(double mouseX, double mouseY, Axis axis);
void drawAxisAlignedPlane(Axis planeAxis);
glm::vec3 getManipulatorTranslation(double mouseX, double mouseY, Axis axis);
glm::vec3 getManipulatorPlaneTranslation(double mouseX, double mouseY, Axis planeAxis);
glm::quat getManipulatorRotation(double mouseX, double mouseY, Axis axis, glm::vec3& lastPosition);
void initManipulatorScale(double mouseX, double mouseY, Axis axis, glm::vec3& lastPosition);
glm::vec3 getManipulatorScale(double mouseX, double mouseY, Axis axis, glm::vec3& lastPosition);
void drawSelectionRect();

// Callbacks
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
#ifdef EMSCRIPTEN_BUILD
    // For WebGL High DPI: get the CSS size and scale to physical pixels
    double cssWidth, cssHeight;
    emscripten_get_element_css_size("#canvas", &cssWidth, &cssHeight);
    
    double devicePixelRatio = emscripten_get_device_pixel_ratio();
    int physicalWidth = static_cast<int>(cssWidth * devicePixelRatio);
    int physicalHeight = static_cast<int>(cssHeight * devicePixelRatio);
    
    // Set the canvas backing buffer to physical pixel size for sharp rendering
    emscripten_set_canvas_element_size("#canvas", physicalWidth, physicalHeight);
    
    g_app.framebufferWidth = physicalWidth;
    g_app.framebufferHeight = physicalHeight;
    
    // After setting canvas element size, GLFW reports mouse coordinates
    // in canvas element coordinates (physical pixels). So use physical size.
    g_app.windowWidth = physicalWidth;
    g_app.windowHeight = physicalHeight;
    g_app.contentScaleX = 1.0f;  // Mouse coords already in physical pixels
    g_app.contentScaleY = 1.0f;
    g_app.dpiScale = static_cast<float>(devicePixelRatio);  // For ImGui font scaling
    
    // Don't set io.DisplaySize here - let ImGui_ImplGlfw_NewFrame handle it
    // It will query GLFW and set DisplaySize/DisplayFramebufferScale correctly
    
    DEBUG_COUT("[WebGL DPI] CSS size: " << cssWidth << "x" << cssHeight 
              << ", devicePixelRatio: " << devicePixelRatio
              << ", physical size: " << physicalWidth << "x" << physicalHeight
              << ", contentScale: " << g_app.contentScaleX << std::endl);
    
    glViewport(0, 0, physicalWidth, physicalHeight);
#else
    g_app.framebufferWidth = width;
    g_app.framebufferHeight = height;
    
    // Get window size (logical size, may differ from framebuffer on High DPI)
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    g_app.windowWidth = windowWidth;
    g_app.windowHeight = windowHeight;
    
    // Calculate content scale from actual framebuffer/window ratio
    if (windowWidth > 0 && windowHeight > 0) {
        g_app.contentScaleX = static_cast<float>(width) / static_cast<float>(windowWidth);
        g_app.contentScaleY = static_cast<float>(height) / static_cast<float>(windowHeight);
    } else {
        g_app.contentScaleX = 1.0f;
        g_app.contentScaleY = 1.0f;
    }
    
    // Get GLFW content scale for DPI-aware font/UI sizing
    float glfwScaleX, glfwScaleY;
    glfwGetWindowContentScale(window, &glfwScaleX, &glfwScaleY);
    g_app.dpiScale = glfwScaleX;  // Use GLFW's reported scale for ImGui
    
    DEBUG_COUT("[Desktop DPI] window: " << windowWidth << "x" << windowHeight 
              << ", framebuffer: " << width << "x" << height
              << ", contentScale: " << g_app.contentScaleX 
              << ", dpiScale: " << g_app.dpiScale << std::endl);
    
    glViewport(0, 0, width, height);
#endif
}

// Convert logical mouse coordinates to framebuffer (physical) coordinates for High DPI
inline int toFramebufferX(double mouseX) {
    return static_cast<int>(mouseX * g_app.contentScaleX);
}

inline int toFramebufferY(double mouseY) {
    return static_cast<int>(mouseY * g_app.contentScaleY);
}

// Get framebuffer Y coordinate (flipped for OpenGL) from logical mouse Y
inline int toFramebufferGLY(double mouseY) {
    int fbY = toFramebufferY(mouseY);
    return g_app.framebufferHeight - fbY;
}

// Get the 3D viewport bounds (in framebuffer coordinates)
struct ViewportBounds {
    int x, y, width, height;
};

ViewportBounds get3DViewportBounds() {
    if (g_app.splitViewEnabled) {
        int splitX = static_cast<int>(g_app.framebufferWidth * g_app.splitRatio);
        return { 0, 0, splitX, g_app.framebufferHeight };
    }
    return { 0, 0, g_app.framebufferWidth, g_app.framebufferHeight };
}

// Check if a point (in framebuffer coords) is inside the 3D viewport
bool isPointIn3DViewport(int fbX, int fbY) {
    ViewportBounds vp = get3DViewportBounds();
    return fbX >= vp.x && fbX < vp.x + vp.width && 
           fbY >= vp.y && fbY < vp.y + vp.height;
}

// Check if a point (in window coords) is inside the UV viewport
bool isPointInUVViewport(double x, double y) {
    if (!g_app.splitViewEnabled) return false;
    float splitX = g_app.windowWidth * (1.0f - g_app.splitRatio);
    return x >= splitX && x < g_app.windowWidth;
}

void scrollCallback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    // Don't handle if ImGui wants it
    if (ImGui::GetIO().WantCaptureMouse) return;
    
    // Get current mouse position
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
#ifdef EMSCRIPTEN_BUILD
    double dpr = emscripten_get_device_pixel_ratio();
    mouseX *= dpr;
    mouseY *= dpr;
#endif
    
    // Check if scrolling over UV viewport
    if (isPointInUVViewport(mouseX, mouseY)) {
        // Zoom UV viewport towards mouse position
        float zoomDelta = static_cast<float>(yoffset) * 0.1f;
        float oldZoom = g_app.uvZoom;
        float newZoom = glm::clamp(oldZoom + zoomDelta * oldZoom, 0.1f, 10.0f);
        
        // Get UV position under mouse before zoom
        glm::vec2 uvUnderMouse = screenToUV(mouseX, mouseY);
        
        // Apply new zoom
        g_app.uvZoom = newZoom;
        
        // Get UV position under mouse after zoom (with old offset)
        glm::vec2 uvAfterZoom = screenToUV(mouseX, mouseY);
        
        // Adjust offset to keep the same UV point under mouse
        g_app.uvOffset += uvAfterZoom - uvUnderMouse;
    } else {
        // Zoom 3D viewport
        g_app.camera.zoom(static_cast<float>(yoffset) * 0.5f);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Don't handle if ImGui wants it
    if (ImGui::GetIO().WantCaptureMouse) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            g_app.mousePressed = true;
            // Save click position for selection
            glfwGetCursorPos(window, &g_app.clickMouseX, &g_app.clickMouseY);
#ifdef EMSCRIPTEN_BUILD
            // Emscripten GLFW with GLFW_SCALE_TO_MONITOR returns CSS pixels,
            // but we work in physical pixels. Scale by devicePixelRatio.
            double dpr = emscripten_get_device_pixel_ratio();
            g_app.clickMouseX *= dpr;
            g_app.clickMouseY *= dpr;
#endif
            
            // Check if Alt is pressed (camera control mode)
            bool altPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || 
                              glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
            
            if (!altPressed) {
                // Check if click is in UV viewport (for UV selection)
                if (isPointInUVViewport(g_app.clickMouseX, g_app.clickMouseY)) {
                    g_app.activeViewport = 1;  // UV viewport
                    
                    // UV viewport click - will be handled on release for selection
                    g_app.isSelecting = true;
                    g_app.selectStartX = g_app.clickMouseX;
                    g_app.selectStartY = g_app.clickMouseY;
                    g_app.selectEndX = g_app.clickMouseX;
                    g_app.selectEndY = g_app.clickMouseY;
                    g_app.activeViewport = 1;  // UV viewport
                    return;
                }
                
                g_app.activeViewport = 0;  // 3D viewport
                
                // First, check if we clicked on a manipulator widget
                Manipulator* manipulator = getCurrentManipulator();
                if (manipulator && getSelectionCount() > 0) {
                    int widgetIndex = selectManipulatorAtPoint(
                        static_cast<int>(g_app.clickMouseX), 
                        static_cast<int>(g_app.clickMouseY)
                    );
                    
                    if (widgetIndex >= 0) {
                        // Start manipulator drag
                        manipulator->selectAtIndex(static_cast<uint32_t>(widgetIndex));
                        g_app.isDraggingManipulator = true;
                        manipulationStarted();  // Capture state for undo
                        
                        // Initialize drag state
                        Axis selectedAxis = manipulator->getSelectedAxis();
                        Widget selectedWidget = manipulator->getSelectedWidget();
                        g_app.dragAxis = manipulator->getSelectedAxisDirection();
                        
                        if (g_app.transformMode == TransformMode::Translate) {
                            if (selectedWidget == Widget::Plane) {
                                g_app.dragStartPoint = getManipulatorPlaneTranslation(
                                    g_app.clickMouseX, g_app.clickMouseY, selectedAxis);
                            } else {
                                g_app.dragStartPoint = getManipulatorTranslation(
                                    g_app.clickMouseX, g_app.clickMouseY, selectedAxis);
                            }
                        } else if (g_app.transformMode == TransformMode::Rotate) {
                            // Initialize lastPosition for rotation
                            glm::vec3 selCenter = getSelectionCenter();
                            glm::vec3 planeNormal;
                            switch (selectedAxis) {
                                case Axis::X: planeNormal = glm::vec3(1, 0, 0); break;
                                case Axis::Y: planeNormal = glm::vec3(0, 1, 0); break;
                                case Axis::Z: planeNormal = glm::vec3(0, 0, 1); break;
                                default: planeNormal = glm::vec3(0, 1, 0); break;
                            }
                            g_app.dragStartPoint = intersectAxisPlane(
                                g_app.clickMouseX, g_app.clickMouseY, selCenter, planeNormal);
                            g_app.dragStartPoint -= selCenter;
                        } else if (g_app.transformMode == TransformMode::Scale) {
                            // Initialize lastPosition for scale using the same method as drag
                            initManipulatorScale(g_app.clickMouseX, g_app.clickMouseY, 
                                                 selectedAxis, g_app.dragStartPoint);
                        }
                        
                        return;  // Don't start rectangle selection
                    }
                }
                
                // Start rectangle selection (no manipulator hit)
                g_app.isSelecting = true;
                g_app.selectStartX = g_app.clickMouseX;
                g_app.selectStartY = g_app.clickMouseY;
                g_app.selectEndX = g_app.clickMouseX;
                g_app.selectEndY = g_app.clickMouseY;
            }
        } else if (action == GLFW_RELEASE) {
            // End manipulator drag
            if (g_app.isDraggingManipulator) {
                manipulationEnded();  // Register undo action
                g_app.isDraggingManipulator = false;
                Manipulator* manipulator = getCurrentManipulator();
                if (manipulator) {
                    manipulator->clearSelection();
                }
                g_app.mousePressed = false;
                return;
            }
            
            double currentX, currentY;
            glfwGetCursorPos(window, &currentX, &currentY);
#ifdef EMSCRIPTEN_BUILD
            // Scale to physical pixels
            double dpr = emscripten_get_device_pixel_ratio();
            currentX *= dpr;
            currentY *= dpr;
#endif
            
            if (g_app.isSelecting) {
                g_app.isSelecting = false;
                
                // Calculate rectangle dimensions
                double minX = std::min(g_app.selectStartX, currentX);
                double minY = std::min(g_app.selectStartY, currentY);
                double maxX = std::max(g_app.selectStartX, currentX);
                double maxY = std::max(g_app.selectStartY, currentY);
                double rectWidth = maxX - minX;
                double rectHeight = maxY - minY;
                
                bool addToSelection = (mods & GLFW_MOD_SHIFT) != 0;
                bool invertSelection = (mods & GLFW_MOD_CONTROL) != 0;
                
                // Check if this was a UV viewport selection
                if (g_app.activeViewport == 1 && g_app.splitViewEnabled) {
                    Mesh2* mesh = g_app.items->getCurrentMesh();
                    
                    if (mesh) {
                        // UV face selection
                        if (rectWidth <= 5.0 && rectHeight <= 5.0) {
                            // Point selection in UV
                            int faceIndex = selectAtPointUV(
                                static_cast<int>(currentX), static_cast<int>(currentY));
                            
                            if (faceIndex >= 0) {
                                if (invertSelection) {
                                    if (mesh->isFaceSelected(static_cast<size_t>(faceIndex))) {
                                        mesh->deselectFace(static_cast<size_t>(faceIndex));
                                    } else {
                                        mesh->selectFace(static_cast<size_t>(faceIndex), true);
                                    }
                                } else {
                                    mesh->selectFace(static_cast<size_t>(faceIndex), addToSelection);
                                }
                            } else if (!addToSelection && !invertSelection) {
                                mesh->deselectAll();
                            }
                        } else {
                            // Rectangle selection in UV
                            if (!addToSelection && !invertSelection) {
                                mesh->deselectAll();
                            }
                            
                            std::vector<bool> selected = selectInRectUV(
                                static_cast<int>(minX), static_cast<int>(minY),
                                static_cast<int>(rectWidth), static_cast<int>(rectHeight)
                            );
                            
                            for (size_t i = 0; i < selected.size(); i++) {
                                if (selected[i]) {
                                    if (invertSelection) {
                                        if (mesh->isFaceSelected(i)) {
                                            mesh->deselectFace(i);
                                        } else {
                                            mesh->selectFace(i, true);
                                        }
                                    } else {
                                        mesh->selectFace(i, true);
                                    }
                                }
                            }
                        }
                    }
                    g_app.mousePressed = false;
                    return;
                }
                
                // Selection behaves differently based on EditMode
                EditMode editMode = g_app.items->getEditMode();
                
                if (editMode == EditMode::Items) {
                    // Item selection
                    if (rectWidth > 5.0 && rectHeight > 5.0) {
                        // Rectangle selection for items
                        if (!addToSelection && !invertSelection) {
                            g_app.items->deselectAllItems();
                        }
                        
                        std::vector<bool> selected = selectInRect(
                            static_cast<int>(minX), static_cast<int>(minY),
                            static_cast<int>(rectWidth), static_cast<int>(rectHeight)
                        );
                        
                        for (size_t i = 0; i < selected.size(); i++) {
                            if (selected[i]) {
                                if (invertSelection) {
                                    // Toggle selection
                                    Item* item = g_app.items->getItemAtIndex(i);
                                    if (item) {
                                        if (item->selected) {
                                            g_app.items->deselectItemAtIndex(i);
                                        } else {
                                            g_app.items->selectItemAtIndex(i, true);
                                        }
                                    }
                                } else {
                                    g_app.items->selectItemAtIndex(i, true);
                                }
                            }
                        }
                    } else {
                        // Point selection (click)
                        int itemIndex = selectAtPoint(static_cast<int>(currentX), static_cast<int>(currentY));
                        
                        if (itemIndex >= 0) {
                            if (invertSelection) {
                                // Toggle selection
                                Item* item = g_app.items->getItemAtIndex(static_cast<size_t>(itemIndex));
                                if (item) {
                                    if (item->selected) {
                                        g_app.items->deselectItemAtIndex(static_cast<size_t>(itemIndex));
                                    } else {
                                        g_app.items->selectItemAtIndex(static_cast<size_t>(itemIndex), true);
                                    }
                                }
                            } else {
                                g_app.items->selectItemAtIndex(static_cast<size_t>(itemIndex), addToSelection);
                            }
                        } else if (!addToSelection && !invertSelection) {
                            // Clicked empty space - deselect all
                            g_app.items->deselectAllItems();
                        }
                    }
                } else {
                    // Component selection (faces/edges/vertices)
                    Mesh2* mesh = g_app.items->getCurrentMesh();
                    if (!mesh) {
                        // No selected item to edit - do nothing
                        g_app.mousePressed = false;
                        return;
                    }
                    
                    if (rectWidth > 5.0 && rectHeight > 5.0) {
                        // Rectangle selection
                        if (!addToSelection && !invertSelection) {
                            mesh->deselectAll();
                        }
                        
                        std::vector<bool> selected = selectInRect(
                            static_cast<int>(minX), static_cast<int>(minY),
                            static_cast<int>(rectWidth), static_cast<int>(rectHeight)
                        );
                        
                        for (size_t i = 0; i < selected.size(); i++) {
                            if (selected[i]) {
                                if (editMode == EditMode::Vertices) {
                                    if (invertSelection) {
                                        if (mesh->isVertexSelected(i)) {
                                            mesh->deselectVertex(i);
                                        } else {
                                            mesh->selectVertex(i, true);
                                        }
                                    } else {
                                        mesh->selectVertex(i, true);
                                    }
                                } else if (editMode == EditMode::Edges) {
                                    if (invertSelection) {
                                        if (mesh->isEdgeSelected(i)) {
                                            mesh->deselectEdge(i);
                                        } else {
                                            mesh->selectEdge(i, true);
                                        }
                                    } else {
                                        mesh->selectEdge(i, true);
                                    }
                                } else {
                                    if (invertSelection) {
                                        if (mesh->isFaceSelected(i)) {
                                            mesh->deselectFace(i);
                                        } else {
                                            mesh->selectFace(i, true);
                                        }
                                    } else {
                                        mesh->selectFace(i, true);
                                    }
                                }
                            }
                        }
                        mesh->createGPUBuffers();
                    } else {
                        // Point selection (click)
                        int selectedIndex = selectAtPoint(static_cast<int>(currentX), static_cast<int>(currentY));
                        
                        if (selectedIndex >= 0) {
                            if (editMode == EditMode::Vertices) {
                                if (invertSelection) {
                                    if (mesh->isVertexSelected(static_cast<size_t>(selectedIndex))) {
                                        mesh->deselectVertex(static_cast<size_t>(selectedIndex));
                                    } else {
                                        mesh->selectVertex(static_cast<size_t>(selectedIndex), true);
                                    }
                                } else {
                                    mesh->selectVertex(static_cast<size_t>(selectedIndex), addToSelection);
                                }
                            } else if (editMode == EditMode::Edges) {
                                if (invertSelection) {
                                    if (mesh->isEdgeSelected(static_cast<size_t>(selectedIndex))) {
                                        mesh->deselectEdge(static_cast<size_t>(selectedIndex));
                                    } else {
                                        mesh->selectEdge(static_cast<size_t>(selectedIndex), true);
                                    }
                                } else {
                                    mesh->selectEdge(static_cast<size_t>(selectedIndex), addToSelection);
                                }
                            } else {
                                if (invertSelection) {
                                    if (mesh->isFaceSelected(static_cast<size_t>(selectedIndex))) {
                                        mesh->deselectFace(static_cast<size_t>(selectedIndex));
                                    } else {
                                        mesh->selectFace(static_cast<size_t>(selectedIndex), true);
                                    }
                                } else {
                                    mesh->selectFace(static_cast<size_t>(selectedIndex), addToSelection);
                                }
                            }
                            mesh->createGPUBuffers();
                        } else if (!addToSelection && !invertSelection) {
                            // Clicked empty space - deselect all
                            mesh->deselectAll();
                            mesh->createGPUBuffers();
                        }
                    }
                }
            }
            
            g_app.mousePressed = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        g_app.middleMousePressed = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        g_app.rightMousePressed = (action == GLFW_PRESS);
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
#ifdef EMSCRIPTEN_BUILD
    // Emscripten GLFW with GLFW_SCALE_TO_MONITOR returns CSS pixels,
    // but we work in physical pixels. Scale by devicePixelRatio.
    double dpr = emscripten_get_device_pixel_ratio();
    xpos *= dpr;
    ypos *= dpr;
#endif
    
    // Don't handle if ImGui wants it
    if (ImGui::GetIO().WantCaptureMouse) {
        g_app.lastMouseX = xpos;
        g_app.lastMouseY = ypos;
        return;
    }
    
    // Handle manipulator dragging
    if (g_app.isDraggingManipulator && g_app.mousePressed) {
        Manipulator* manipulator = getCurrentManipulator();
        if (manipulator && manipulator->hasSelection()) {
            Axis selectedAxis = manipulator->getSelectedAxis();
            Widget selectedWidget = manipulator->getSelectedWidget();
            
            if (g_app.transformMode == TransformMode::Translate) {
                glm::vec3 newPosition;
                glm::vec3 offset;
                
                if (selectedWidget == Widget::Plane) {
                    // Plane widget - translate on two axes
                    newPosition = getManipulatorPlaneTranslation(xpos, ypos, selectedAxis);
                    offset = newPosition - g_app.dragStartPoint;
                    // For planes, we allow movement on two axes (the ones not perpendicular to the plane)
                    // The perpendicular axis is already constrained in getManipulatorPlaneTranslation
                } else {
                    // Arrow widget - translate on single axis
                    newPosition = getManipulatorTranslation(xpos, ypos, selectedAxis);
                    offset = newPosition - g_app.dragStartPoint;
                    
                    // Only move along selected axis (or all axes for center)
                    if (selectedAxis != Axis::Center) {
                        glm::vec3 constrainedOffset(0.0f);
                        constrainedOffset[static_cast<int>(selectedAxis)] = offset[static_cast<int>(selectedAxis)];
                        offset = constrainedOffset;
                    }
                }
                
                translateSelection(offset);
                g_app.dragStartPoint = newPosition;
            }
            else if (g_app.transformMode == TransformMode::Rotate) {
                glm::quat rotation = getManipulatorRotation(xpos, ypos, selectedAxis, g_app.dragStartPoint);
                glm::vec3 axisVec;
                switch (selectedAxis) {
                    case Axis::X: axisVec = glm::vec3(1, 0, 0); break;
                    case Axis::Y: axisVec = glm::vec3(0, 1, 0); break;
                    case Axis::Z: axisVec = glm::vec3(0, 0, 1); break;
                    default: axisVec = glm::vec3(0, 1, 0); break;
                }
                float angle = glm::angle(rotation);
                if (std::abs(angle) > 0.0001f) {
                    rotateSelection(glm::axis(rotation), angle);
                }
            }
            else if (g_app.transformMode == TransformMode::Scale) {
                glm::vec3 scale = getManipulatorScale(xpos, ypos, selectedAxis, g_app.dragStartPoint);
                glm::vec3 center = getSelectionCenter();
                
                // Apply non-uniform scale based on axis
                scaleSelectionByOffset(center, scale);
            }
        }
        
        g_app.lastMouseX = xpos;
        g_app.lastMouseY = ypos;
        return;
    }
    
    // Update selection rectangle end point
    if (g_app.isSelecting) {
        g_app.selectEndX = xpos;
        g_app.selectEndY = ypos;
    }
    
    // Highlight manipulator widget on hover
    Manipulator* manipulator = getCurrentManipulator();
    if (manipulator && getSelectionCount() > 0 && !g_app.mousePressed) {
        int widgetIndex = selectManipulatorAtPoint(static_cast<int>(xpos), static_cast<int>(ypos));
        if (widgetIndex >= 0) {
            manipulator->selectAtIndex(static_cast<uint32_t>(widgetIndex));
        } else {
            manipulator->clearSelection();
        }
    }
    
    float deltaX = static_cast<float>(xpos - g_app.lastMouseX);
    float deltaY = static_cast<float>(ypos - g_app.lastMouseY);
    
    bool altPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || 
                      glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
    bool cmdPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
                      glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
    
    // Handle UV viewport camera controls (same shortcuts as 3D viewport)
    if (isPointInUVViewport(xpos, ypos) && altPressed) {
        float uvPanSensitivity = 1.0f / (g_app.windowWidth * g_app.splitRatio * g_app.uvZoom);
        
        if (g_app.mousePressed || g_app.middleMousePressed) {
            // Alt+Left or Alt+Middle = Pan UV view
            g_app.uvOffset.x += deltaX * uvPanSensitivity;
            g_app.uvOffset.y -= deltaY * uvPanSensitivity;  // Flip Y
        }
        else if (g_app.rightMousePressed) {
            // Alt+Right = Zoom UV view towards center of UV viewport
            float zoomDelta = deltaY * 0.01f;
            float oldZoom = g_app.uvZoom;
            float newZoom = glm::clamp(oldZoom - zoomDelta * oldZoom, 0.1f, 10.0f);
            
            // Get center of UV viewport in screen coords
            float uvViewportCenterX = g_app.windowWidth * (1.0f - g_app.splitRatio * 0.5f);
            float uvViewportCenterY = g_app.windowHeight * 0.5f;
            
            // Get UV position at center before zoom
            glm::vec2 uvCenter = screenToUV(uvViewportCenterX, uvViewportCenterY);
            
            // Apply new zoom
            g_app.uvZoom = newZoom;
            
            // Get UV position at center after zoom
            glm::vec2 uvAfterZoom = screenToUV(uvViewportCenterX, uvViewportCenterY);
            
            // Adjust offset to keep the same UV point at center
            g_app.uvOffset += uvAfterZoom - uvCenter;
        }
        
        g_app.lastMouseX = xpos;
        g_app.lastMouseY = ypos;
        return;
    }
    
    // Match original MeshMaker behavior (3D viewport)
    if (altPressed && cmdPressed && g_app.mousePressed) {
        // Alt+Cmd+Left mouse = Pan
        float sensitivity = 1.0f / ((g_app.windowWidth + g_app.windowHeight) / 2.0f);
        sensitivity *= g_app.camera.getZoom() * 1.12f;
        g_app.camera.leftRight(-deltaX * sensitivity);
        g_app.camera.upDown(-deltaY * sensitivity);  // Negate: GLFW Y is inverted vs macOS
    }
    else if (altPressed && g_app.mousePressed) {
        // Alt+Left mouse = Orbit
        const float sensitivity = 0.005f;
        g_app.camera.rotateLeftRight(deltaX * sensitivity);
        g_app.camera.rotateUpDown(deltaY * sensitivity);
    }
    
    if (altPressed && g_app.middleMousePressed) {
        // Alt+Middle mouse = Pan
        float sensitivity = 1.0f / ((g_app.windowWidth + g_app.windowHeight) / 2.0f);
        g_app.camera.leftRight(-deltaX * g_app.camera.getZoom() * sensitivity);
        g_app.camera.upDown(-deltaY * g_app.camera.getZoom() * sensitivity);  // Negate: GLFW Y is inverted
    }
    
    if (altPressed && g_app.rightMousePressed) {
        // Alt+Right mouse = Zoom
        float sensitivity = g_app.camera.getZoom() * 0.02f;
        g_app.camera.zoom(deltaY * sensitivity);
    }
    
    g_app.lastMouseX = xpos;
    g_app.lastMouseY = ypos;
}

void keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods) {
    // Don't handle if ImGui wants keyboard
    if (ImGui::GetIO().WantCaptureKeyboard) return;
    
#ifdef EMSCRIPTEN_BUILD
    // Don't handle if an HTML input element is focused (React UI textboxes)
    bool htmlInputFocused = EM_ASM_INT({
        var activeEl = document.activeElement;
        if (!activeEl) return 0;
        var tag = activeEl.tagName;
        return (tag === 'INPUT' || tag === 'TEXTAREA' || activeEl.isContentEditable) ? 1 : 0;
    });
    if (htmlInputFocused) return;
#endif
    
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    
    // Handle Ctrl+Z (Undo) and Ctrl+Y (Redo) - only in desktop builds
    // In WASM builds, React handles these shortcuts to avoid double-triggering
#ifndef EMSCRIPTEN_BUILD
    if (mods & GLFW_MOD_CONTROL) {
        if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
            g_app.undoManager.undo();
            return;
        }
        if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
            g_app.undoManager.redo();
            return;
        }
    }
#endif
    
    bool hasSelection = getSelectionCount() > 0;
    
    // Mode switching keys (matching original MeshMaker: 1=Select, 2=Translate, 3=Rotate, 4=Scale)
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_1:  // Select mode (default)
                g_app.transformMode = TransformMode::None;
                break;
            case GLFW_KEY_2:  // Translate mode
                g_app.transformMode = TransformMode::Translate;
                break;
            case GLFW_KEY_3:  // Rotate mode
                g_app.transformMode = TransformMode::Rotate;
                break;
            case GLFW_KEY_4:  // Scale mode
                g_app.transformMode = TransformMode::Scale;
                break;
            case GLFW_KEY_ESCAPE:
                g_app.transformMode = TransformMode::None;
                break;
            case GLFW_KEY_A:  // Ctrl+A = Select all (desktop only, React handles in WASM)
#ifndef EMSCRIPTEN_BUILD
                if (mods & GLFW_MOD_CONTROL) {
                    selectAll();
                }
#endif
                break;
            case GLFW_KEY_I:  // Ctrl+I = Invert selection
                if (mods & GLFW_MOD_CONTROL) {
                    if (g_app.items->getEditMode() == EditMode::Items) {
                        for (size_t i = 0; i < g_app.items->getItemCount(); ++i) {
                            Item* item = g_app.items->getItemAtIndex(i);
                            if (item) item->selected = !item->selected;
                        }
                    } else {
                        Mesh2* mesh = g_app.items->getCurrentMesh();
                        if (mesh) {
                            mesh->invertSelection();
                            mesh->createGPUBuffers();
                        }
                    }
                }
                break;
            case GLFW_KEY_D:  // Ctrl+D = Duplicate (desktop only, React handles in WASM)
#ifndef EMSCRIPTEN_BUILD
                if (mods & GLFW_MOD_CONTROL) {
                    if (hasSelection) {
                        if (g_app.items->getEditMode() == EditMode::Items) {
                            sceneActionWithUndo("Duplicate Items", []() {
                                g_app.items->duplicateSelectedItems();
                            });
                        } else {
                            meshActionWithUndo("Duplicate", []() {
                                g_app.items->duplicateSelectedFaces();
                            });
                        }
                    }
                }
#endif
                break;
            case GLFW_KEY_F:  // Ctrl+Shift+F = Flip normals
                if ((mods & GLFW_MOD_CONTROL) && (mods & GLFW_MOD_SHIFT)) {
                    if (hasSelection && g_app.items->getEditMode() != EditMode::Items) {
                        meshActionWithUndo("Flip Normals", []() {
                            g_app.items->flipSelectedFaces();
                        });
                    }
                }
                break;
            case GLFW_KEY_S:  // Ctrl+Shift+S = Split
                if ((mods & GLFW_MOD_CONTROL) && (mods & GLFW_MOD_SHIFT)) {
                    if (hasSelection && g_app.items->getEditMode() != EditMode::Items) {
                        meshActionWithUndo("Split", []() {
                            g_app.items->splitSelected();
                        });
                    }
                }
                break;
            case GLFW_KEY_M:  // Ctrl+Shift+M = Merge
                if ((mods & GLFW_MOD_CONTROL) && (mods & GLFW_MOD_SHIFT)) {
                    if (hasSelection && g_app.items->getEditMode() == EditMode::Vertices) {
                        meshActionWithUndo("Merge Vertices", []() {
                            g_app.items->mergeSelectedVertices();
                        });
                    }
                }
                break;
            case GLFW_KEY_U:  // Ctrl+Shift+U = Subdivide (Catmull-Clark)
                if ((mods & GLFW_MOD_CONTROL) && (mods & GLFW_MOD_SHIFT)) {
                    if (hasSelection && g_app.items->getEditMode() != EditMode::Items) {
                        meshActionWithUndo("Subdivide", []() {
                            g_app.items->catmullClarkSubdivide(1);
                        });
                    }
                }
                break;
            case GLFW_KEY_E:  // Ctrl+Shift+E = Extrude
                if ((mods & GLFW_MOD_CONTROL) && (mods & GLFW_MOD_SHIFT)) {
                    if (hasSelection && g_app.items->getEditMode() == EditMode::Triangles) {
                        meshActionWithUndo("Extrude", []() {
                            g_app.items->extrudeSelectedFaces();
                        });
                    }
                }
                break;
            case GLFW_KEY_T:  // Ctrl+Shift+T = Triangulate
                if ((mods & GLFW_MOD_CONTROL) && (mods & GLFW_MOD_SHIFT)) {
                    if (hasSelection && g_app.items->getEditMode() == EditMode::Triangles) {
                        meshActionWithUndo("Triangulate", []() {
                            g_app.items->triangulateSelectedFaces();
                        });
                    }
                }
                break;
            case GLFW_KEY_5:  // Vertices mode
                g_app.items->setEditMode(EditMode::Vertices);
                break;
            case GLFW_KEY_6:  // Edges mode
                g_app.items->setEditMode(EditMode::Edges);
                break;
            case GLFW_KEY_7:  // Triangles mode
                g_app.items->setEditMode(EditMode::Triangles);
                break;
            case GLFW_KEY_0:  // Items mode
                g_app.items->setEditMode(EditMode::Items);
                break;
            case GLFW_KEY_DELETE:  // Delete selected (desktop only, React handles in WASM)
            case GLFW_KEY_BACKSPACE:
#ifndef EMSCRIPTEN_BUILD
                if (hasSelection) {
                    if (g_app.items->getEditMode() == EditMode::Items) {
                        sceneActionWithUndo("Delete Items", []() {
                            g_app.items->deleteSelectedItems();
                        });
                    } else {
                        meshActionWithUndo("Delete", []() {
                            g_app.items->deleteSelectedFaces();
                        });
                    }
                }
#endif
                break;
        }
    }
    
    // Transform keys (work with selection)
    if (hasSelection) {
        switch (g_app.transformMode) {
            case TransformMode::Translate:
                switch (key) {
                    case GLFW_KEY_RIGHT:
                        translateSelection(glm::vec3(g_app.translateStep, 0.0f, 0.0f));
                        break;
                    case GLFW_KEY_LEFT:
                        translateSelection(glm::vec3(-g_app.translateStep, 0.0f, 0.0f));
                        break;
                    case GLFW_KEY_UP:
                        translateSelection(glm::vec3(0.0f, g_app.translateStep, 0.0f));
                        break;
                    case GLFW_KEY_DOWN:
                        translateSelection(glm::vec3(0.0f, -g_app.translateStep, 0.0f));
                        break;
                }
                break;
                
            case TransformMode::Rotate:
                switch (key) {
                    case GLFW_KEY_RIGHT:
                        rotateSelection(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(g_app.rotateStep));
                        break;
                    case GLFW_KEY_LEFT:
                        rotateSelection(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-g_app.rotateStep));
                        break;
                    case GLFW_KEY_UP:
                        rotateSelection(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(g_app.rotateStep));
                        break;
                    case GLFW_KEY_DOWN:
                        rotateSelection(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(-g_app.rotateStep));
                        break;
                }
                break;
                
            case TransformMode::Scale:
                switch (key) {
                    case GLFW_KEY_EQUAL:  // Plus key
                    case GLFW_KEY_UP:
                        scaleSelection(getSelectionCenter(), 1.0f + g_app.scaleStep);
                        break;
                    case GLFW_KEY_MINUS:
                    case GLFW_KEY_DOWN:
                        scaleSelection(getSelectionCenter(), 1.0f - g_app.scaleStep);
                        break;
                }
                break;
                
            default:
                break;
        }
    }
}

// Convert screen coordinates to world-space ray
glm::vec3 screenToWorldRay(double mouseX, double mouseY, glm::vec3& rayOrigin) {
    float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    
    // Convert to normalized device coordinates (-1 to 1)
    float x = (2.0f * static_cast<float>(mouseX)) / g_app.windowWidth - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / g_app.windowHeight;
    
    // Create ray in clip space
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    
    // Transform to eye space
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    
    // Transform to world space
    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
    rayWorld = glm::normalize(rayWorld);
    
    // Ray origin is camera position
    rayOrigin = g_app.camera.getPosition();
    
    return rayWorld;
}

// Color-buffer picking for selection (matching original MeshMaker approach)
// Renders each triangle with a unique color and reads back the pixel at click position
// x, y are in logical (window) coordinates
int selectAtPoint(int x, int y) {
    // Convert to framebuffer coordinates for High DPI
    int fbX = toFramebufferX(x);
    int fbY = toFramebufferGLY(y);  // Already flipped for OpenGL
    
    // Get 3D viewport bounds (may be partial when in split view)
    ViewportBounds vp = get3DViewportBounds();
    
    // Check if click is outside the 3D viewport
    int fbYNonFlipped = toFramebufferY(y);
    if (!isPointIn3DViewport(fbX, fbYNonFlipped)) {
        return -1;  // Click is in UV viewport or divider
    }
    
    // Set viewport and scissor to 3D viewport area
    glViewport(vp.x, vp.y, vp.width, vp.height);
    glScissor(vp.x, vp.y, vp.width, vp.height);
    glEnable(GL_SCISSOR_TEST);
    
    // Save current clear color
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    
    // Clear to black (index 0 = no selection)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Setup matrices using the 3D viewport aspect ratio
    float aspectRatio = static_cast<float>(vp.width) / static_cast<float>(vp.height);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    
    // Draw based on edit mode
    g_app.selectionShader->use();
    g_app.selectionShader->setMat4("uView", view);
    g_app.selectionShader->setMat4("uProjection", projection);
    
    EditMode editMode = g_app.items->getEditMode();
    
    if (editMode == EditMode::Items) {
        // Draw each item with a unique color (item index)
        for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
            Item* item = g_app.items->getItemAtIndex(i);
            if (!item || !item->visible) continue;
            
            glm::mat4 model = item->getTransformMatrix();
            g_app.selectionShader->setMat4("uModel", model);
            
            // Draw all faces with same color (item index + 1)
            uint32_t colorIndex = static_cast<uint32_t>(i) + 1;
            uint8_t r = colorIndex & 0xFF;
            uint8_t g = (colorIndex >> 8) & 0xFF;
            uint8_t b = (colorIndex >> 16) & 0xFF;
            
            // Set color uniform and draw all triangles
            g_app.selectionShader->setVec4("uColorOverride", 
                glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f));
            g_app.selectionShader->setBool("uUseColorOverride", true);
            
            item->mesh->draw(ViewMode::Solid);
        }
        g_app.selectionShader->setBool("uUseColorOverride", false);
    } else {
        // Component mode - only draw currently selected item's mesh
        Mesh2* mesh = g_app.items->getCurrentMesh();
        if (mesh) {
            Item* selectedItem = g_app.items->getFirstSelectedItem();
            glm::mat4 model = selectedItem ? selectedItem->getTransformMatrix() : glm::mat4(1.0f);
            g_app.selectionShader->setMat4("uModel", model);
            
            // Draw based on selection mode (Vertices/Edges/Triangles)
            if (editMode == EditMode::Vertices) {
                mesh->drawVerticesForSelection(*g_app.selectionShader);
            } else if (editMode == EditMode::Edges) {
                // Pre-set thick line colored shader uniforms for edge selection rendering
                g_app.thickLineColoredShader->use();
                g_app.thickLineColoredShader->setMat4("uModel", model);
                g_app.thickLineColoredShader->setMat4("uView", view);
                g_app.thickLineColoredShader->setMat4("uProjection", projection);
                g_app.selectionShader->use();  // Switch back
                mesh->drawEdgesForSelection(*g_app.selectionShader, *g_app.thickLineColoredShader,
                                             static_cast<float>(vp.width), static_cast<float>(vp.height));
            } else {
                mesh->drawForSelection(*g_app.selectionShader);
            }
        }
    }
    
    glFinish();
    glDisable(GL_SCISSOR_TEST);
    
    // Read pixel at click position (sample 5x5 area for easier clicking, scaled for DPI)
    int sampleSize = static_cast<int>(5 * g_app.contentScaleX);
    if (sampleSize < 3) sampleSize = 3;
    const int halfSize = sampleSize / 2;
    std::vector<uint8_t> pixels(sampleSize * sampleSize * 4);
    
    int readX = std::max(0, fbX - halfSize);
    int readY = std::max(0, fbY - halfSize);
    
    glReadPixels(readX, readY, sampleSize, sampleSize, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    
    // Find first non-zero index in sample
    int selectedIndex = -1;
    for (int i = 0; i < sampleSize * sampleSize && selectedIndex < 0; i++) {
        uint32_t r = pixels[i * 4 + 0];
        uint32_t g = pixels[i * 4 + 1];
        uint32_t b = pixels[i * 4 + 2];
        // Use RGB only (alpha is always 255 for visible triangles)
        uint32_t colorIndex = r | (g << 8) | (b << 16);
        if (colorIndex > 0) {
            selectedIndex = static_cast<int>(colorIndex - 1);  // Index 1+ = element 0+
        }
    }
    
    // Restore clear color
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    
    return selectedIndex;
}

// UV viewport color-buffer picking for face selection
// x, y are in logical (window) coordinates
int selectAtPointUV(int x, int y) {
    if (!g_app.splitViewEnabled) return -1;
    if (!g_app.uvSelectionShader) return -1;
    
    // Get UV viewport bounds
    int uvX = static_cast<int>(g_app.framebufferWidth * (1.0f - g_app.splitRatio));
    int uvWidth = g_app.framebufferWidth - uvX;
    int uvHeight = g_app.framebufferHeight;
    
    // Convert click to framebuffer coordinates
    int fbX = toFramebufferX(x);
    int fbY = toFramebufferY(y);
    
    // Check if click is in UV viewport
    if (fbX < uvX || fbX >= g_app.framebufferWidth) return -1;
    
    // Set viewport and scissor to UV viewport area
    glViewport(uvX, 0, uvWidth, uvHeight);
    glScissor(uvX, 0, uvWidth, uvHeight);
    glEnable(GL_SCISSOR_TEST);
    
    // Save current clear color
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    
    // Clear to black (index 0 = no selection)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    // Calculate UV transform
    float viewportAspect = static_cast<float>(uvWidth) / static_cast<float>(uvHeight);
    float scale = g_app.uvZoom;
    
    glm::vec2 aspectAdjust(1.0f, 1.0f);
    if (viewportAspect > 1.0f) {
        aspectAdjust.x = 1.0f / viewportAspect;
    } else {
        aspectAdjust.y = viewportAspect;
    }
    
    // Setup shader
    g_app.uvSelectionShader->use();
    g_app.uvSelectionShader->setVec2("uOffset", g_app.uvOffset);
    g_app.uvSelectionShader->setFloat("uZoom", scale);
    g_app.uvSelectionShader->setVec2("uAspectAdjust", aspectAdjust);
    
    // Draw faces for selection
    g_app.items->drawUVForSelection(*g_app.uvSelectionShader, g_app.uvOffset, scale, aspectAdjust);
    
    // Read pixel at click position (flip Y for OpenGL)
    int glY = g_app.framebufferHeight - fbY - 1;
    
    const int sampleSize = 3;
    uint8_t pixels[sampleSize * sampleSize * 4];
    int readX = fbX - sampleSize / 2;
    int readY = glY - sampleSize / 2;
    
    glReadPixels(readX, readY, sampleSize, sampleSize, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    // Find first non-zero index
    int selectedIndex = -1;
    for (int i = 0; i < sampleSize * sampleSize && selectedIndex < 0; i++) {
        uint32_t r = pixels[i * 4 + 0];
        uint32_t g = pixels[i * 4 + 1];
        uint32_t b = pixels[i * 4 + 2];
        uint32_t colorIndex = r | (g << 8) | (b << 16);
        if (colorIndex > 0) {
            selectedIndex = static_cast<int>(colorIndex - 1);
        }
    }
    
    // Restore state
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    
    return selectedIndex;
}

// UV viewport rectangle selection using color-buffer picking
// x, y, width, height are in logical (window) coordinates
std::vector<bool> selectInRectUV(int x, int y, int width, int height) {
    Mesh2* mesh = g_app.items->getCurrentMesh();
    size_t faceCount = mesh ? mesh->getFaceCount() : 0;
    std::vector<bool> selected(faceCount, false);
    
    if (!g_app.splitViewEnabled || !g_app.uvSelectionShader || faceCount == 0) {
        return selected;
    }
    if (width <= 0 || height <= 0) return selected;
    
    // Get UV viewport bounds
    int uvX = static_cast<int>(g_app.framebufferWidth * (1.0f - g_app.splitRatio));
    int uvWidth = g_app.framebufferWidth - uvX;
    int uvHeight = g_app.framebufferHeight;
    
    // Set viewport and scissor to UV viewport area
    glViewport(uvX, 0, uvWidth, uvHeight);
    glScissor(uvX, 0, uvWidth, uvHeight);
    glEnable(GL_SCISSOR_TEST);
    
    // Save current clear color
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    
    // Clear to black
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    // Calculate UV transform
    float viewportAspect = static_cast<float>(uvWidth) / static_cast<float>(uvHeight);
    float scale = g_app.uvZoom;
    
    glm::vec2 aspectAdjust(1.0f, 1.0f);
    if (viewportAspect > 1.0f) {
        aspectAdjust.x = 1.0f / viewportAspect;
    } else {
        aspectAdjust.y = viewportAspect;
    }
    
    // Setup shader and draw
    g_app.uvSelectionShader->use();
    g_app.uvSelectionShader->setVec2("uOffset", g_app.uvOffset);
    g_app.uvSelectionShader->setFloat("uZoom", scale);
    g_app.uvSelectionShader->setVec2("uAspectAdjust", aspectAdjust);
    
    g_app.items->drawUVForSelection(*g_app.uvSelectionShader, g_app.uvOffset, scale, aspectAdjust);
    
    // Coordinates are already in framebuffer space (DPR-scaled by caller)
    int fbX = x;
    int fbY = y;
    int fbWidth = width;
    int fbHeight = height;
    
    // Flip Y for OpenGL
    int glY = g_app.framebufferHeight - fbY - fbHeight;
    
    // Clamp to UV viewport bounds
    int readX = std::max(fbX, uvX);
    int readY = std::max(0, glY);
    int readWidth = std::min(fbX + fbWidth, g_app.framebufferWidth) - readX;
    int readHeight = std::min(glY + fbHeight, g_app.framebufferHeight) - readY;
    
    if (readWidth <= 0 || readHeight <= 0) {
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);
        return selected;
    }
    
    // Limit max pixels
    const int kMaxPixels = 512 * 512;
    if (readWidth * readHeight > kMaxPixels) {
        float ratio = std::sqrt(static_cast<float>(kMaxPixels) / (readWidth * readHeight));
        readWidth = static_cast<int>(readWidth * ratio);
        readHeight = static_cast<int>(readHeight * ratio);
    }
    
    // Read pixels
    std::vector<uint8_t> pixels(readWidth * readHeight * 4);
    glReadPixels(readX, readY, readWidth, readHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    
    // Find all unique face indices in the rect
    for (int i = 0; i < readWidth * readHeight; i++) {
        uint32_t r = pixels[i * 4 + 0];
        uint32_t g = pixels[i * 4 + 1];
        uint32_t b = pixels[i * 4 + 2];
        uint32_t colorIndex = r | (g << 8) | (b << 16);
        if (colorIndex > 0 && colorIndex <= faceCount) {
            selected[colorIndex - 1] = true;
        }
    }
    
    // Restore state
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    
    return selected;
}

// Rectangle selection using color-buffer picking
// Returns a vector indicating which items/faces/vertices/edges are within the rectangle
// x, y, width, height are in logical (window) coordinates
std::vector<bool> selectInRect(int x, int y, int width, int height) {
    EditMode editMode = g_app.items->getEditMode();
    
    size_t count = 0;
    if (editMode == EditMode::Items) {
        count = g_app.items->getItemCount();
    } else {
        Mesh2* mesh = g_app.items->getCurrentMesh();
        if (mesh) {
            if (editMode == EditMode::Vertices) {
                count = mesh->getVertexCount();
            } else if (editMode == EditMode::Edges) {
                count = mesh->getEdgeCount();
            } else {
                count = mesh->getFaceCount();
            }
        }
    }
    
    std::vector<bool> selected(count, false);
    
    if (width <= 0 || height <= 0 || count == 0) return selected;
    
    // Get 3D viewport bounds
    ViewportBounds vp = get3DViewportBounds();
    
    // Convert to framebuffer coordinates for High DPI
    int fbX = toFramebufferX(x);
    int fbY = toFramebufferY(y);
    int fbWidth = toFramebufferX(width);
    int fbHeight = toFramebufferY(height);
    
    // Flip Y for OpenGL coordinate system (origin at bottom-left)
    int glY = g_app.framebufferHeight - fbY - fbHeight;
    
    // Clamp to framebuffer bounds (and 3D viewport bounds in split mode)
    int readX = std::max(0, fbX);
    int readY = std::max(0, glY);
    int readWidth = std::min(fbWidth, vp.width - readX);
    int readHeight = std::min(fbHeight, g_app.framebufferHeight - readY);
    
    if (readWidth <= 0 || readHeight <= 0) return selected;
    
    // Limit max pixels to prevent memory issues
    const int kMaxPixels = 1024 * 1024;  // 1M pixels max
    if (readWidth * readHeight > kMaxPixels) {
        // Sample at reduced resolution
        readWidth = std::min(readWidth, 1024);
        readHeight = std::min(readHeight, 1024);
    }
    
    // Set viewport and scissor to 3D viewport area
    glViewport(vp.x, vp.y, vp.width, vp.height);
    glScissor(vp.x, vp.y, vp.width, vp.height);
    glEnable(GL_SCISSOR_TEST);
    
    // Save current clear color
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    
    // Clear to black (index 0 = no selection)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Setup matrices using 3D viewport aspect ratio
    float aspectRatio = static_cast<float>(vp.width) / static_cast<float>(vp.height);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    
    g_app.selectionShader->use();
    g_app.selectionShader->setMat4("uView", view);
    g_app.selectionShader->setMat4("uProjection", projection);
    
    if (editMode == EditMode::Items) {
        // Draw each item with a unique color (item index)
        for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
            Item* item = g_app.items->getItemAtIndex(i);
            if (!item || !item->visible) continue;
            
            glm::mat4 model = item->getTransformMatrix();
            g_app.selectionShader->setMat4("uModel", model);
            
            // Draw all faces with same color (item index + 1)
            uint32_t colorIndex = static_cast<uint32_t>(i) + 1;
            uint8_t r = colorIndex & 0xFF;
            uint8_t g = (colorIndex >> 8) & 0xFF;
            uint8_t b = (colorIndex >> 16) & 0xFF;
            
            g_app.selectionShader->setVec4("uColorOverride", 
                glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f));
            g_app.selectionShader->setBool("uUseColorOverride", true);
            
            item->mesh->draw(ViewMode::Solid);
        }
        g_app.selectionShader->setBool("uUseColorOverride", false);
    } else {
        // Component mode - draw current mesh for selection
        Mesh2* mesh = g_app.items->getCurrentMesh();
        if (mesh) {
            Item* selectedItem = g_app.items->getFirstSelectedItem();
            glm::mat4 model = selectedItem ? selectedItem->getTransformMatrix() : glm::mat4(1.0f);
            g_app.selectionShader->setMat4("uModel", model);
            
            // Draw based on selection mode
            if (editMode == EditMode::Vertices) {
                mesh->drawVerticesForSelection(*g_app.selectionShader);
            } else if (editMode == EditMode::Edges) {
                // Pre-set thick line colored shader uniforms for edge selection rendering
                g_app.thickLineColoredShader->use();
                g_app.thickLineColoredShader->setMat4("uModel", model);
                g_app.thickLineColoredShader->setMat4("uView", view);
                g_app.thickLineColoredShader->setMat4("uProjection", projection);
                g_app.selectionShader->use();  // Switch back
                mesh->drawEdgesForSelection(*g_app.selectionShader, *g_app.thickLineColoredShader,
                                             static_cast<float>(vp.width), static_cast<float>(vp.height));
            } else {
                mesh->drawForSelection(*g_app.selectionShader);
            }
        }
    }
    
    glFinish();
    glDisable(GL_SCISSOR_TEST);
    
    // Read pixels from the rectangle
    std::vector<uint8_t> pixels(readWidth * readHeight * 4);
    glReadPixels(readX, readY, readWidth, readHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    
    // Find all unique non-zero indices in the rectangle
    for (int i = 0; i < readWidth * readHeight; i++) {
        uint32_t r = pixels[i * 4 + 0];
        uint32_t g = pixels[i * 4 + 1];
        uint32_t b = pixels[i * 4 + 2];
        uint32_t colorIndex = r | (g << 8) | (b << 16);
        if (colorIndex > 0) {
            size_t index = colorIndex - 1;
            if (index < count) {
                selected[index] = true;
            }
        }
    }
    
    // Restore clear color
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    
    return selected;
}

// Select manipulator widget at point using color-buffer picking
// x, y are in logical (window) coordinates
int selectManipulatorAtPoint(int x, int y) {
    Manipulator* manipulator = getCurrentManipulator();
    if (!manipulator) return -1;
    if (getSelectionCount() == 0) return -1;
    
    // Convert to framebuffer coordinates for High DPI
    int fbX = toFramebufferX(x);
    int fbY = toFramebufferGLY(y);  // Already flipped for OpenGL
    
    // Get 3D viewport bounds
    ViewportBounds vp = get3DViewportBounds();
    
    // Check if click is outside the 3D viewport
    int fbYNonFlipped = toFramebufferY(y);
    if (!isPointIn3DViewport(fbX, fbYNonFlipped)) {
        return -1;  // Click is in UV viewport or divider
    }
    
    // Set viewport and scissor to 3D viewport area
    glViewport(vp.x, vp.y, vp.width, vp.height);
    glScissor(vp.x, vp.y, vp.width, vp.height);
    glEnable(GL_SCISSOR_TEST);
    
    // Save current clear color
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    
    // Clear to black (index 0 = no selection)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Setup matrices using 3D viewport aspect ratio
    float aspectRatio = static_cast<float>(vp.width) / static_cast<float>(vp.height);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    
    // Position manipulator at selection center
    manipulator->position = getSelectionCenter();
    manipulator->size = g_app.camera.getDistance() * 0.15f;
    
    // Use a simple shader that outputs color based on uColorIndex
    // We need to create a selection shader variant for the manipulator
    // For now, we'll use the selection shader with uAxisTransform
    g_app.manipulatorShader->use();
    g_app.manipulatorShader->setMat4("uView", view);
    g_app.manipulatorShader->setMat4("uProjection", projection);
    
    // Build model matrix
    glm::mat4 model = glm::translate(glm::mat4(1.0f), manipulator->position);
    model = model * glm::mat4_cast(manipulator->rotation);
    model = glm::scale(model, glm::vec3(manipulator->size));
    g_app.manipulatorShader->setMat4("uModel", model);
    
    // Draw each widget with its index as color
    for (uint32_t i = 0; i < manipulator->widgetCount(); i++) {
        ManipulatorWidget& widget = manipulator->widgetAtIndex(i);
        
        // Skip lines - too thin to click
        if (widget.widget == Widget::Line) continue;
        
        // Encode index as color (index + 1 to distinguish from background)
        uint32_t colorIndex = i + 1;
        float r = static_cast<float>(colorIndex & 0xFF) / 255.0f;
        float g = static_cast<float>((colorIndex >> 8) & 0xFF) / 255.0f;
        float b = static_cast<float>((colorIndex >> 16) & 0xFF) / 255.0f;
        glm::vec4 selectionColor(r, g, b, 1.0f);
        
        manipulator->drawForSelection(*g_app.manipulatorShader, *g_app.thickLineShader, view, projection, i,
                                       static_cast<float>(vp.width), static_cast<float>(vp.height),
                                       selectionColor);
    }
    
    glFinish();
    glDisable(GL_SCISSOR_TEST);
    
    // Read a tolerance region for better selection on thin widgets (scaled for DPI)
    int tolerance = static_cast<int>(5 * g_app.contentScaleX);
    if (tolerance < 3) tolerance = 3;
    int startX = std::max(0, fbX - tolerance);
    int startY = std::max(0, fbY - tolerance);
    int readWidth = std::min(tolerance * 2, g_app.framebufferWidth - startX);
    int readHeight = std::min(tolerance * 2, g_app.framebufferHeight - startY);
    
    // Check bounds - can happen if mouse is outside framebuffer
    if (readWidth <= 0 || readHeight <= 0) {
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        return -1;
    }
    
    std::vector<uint8_t> pixels(readWidth * readHeight * 4);
    glReadPixels(startX, startY, readWidth, readHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    
    // Restore clear color
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    
    // Find the closest hit to the center of the region
    // We prefer pixels closer to the click point
    int centerOffsetX = fbX - startX;
    int centerOffsetY = fbY - startY;
    int bestDistance = INT_MAX;
    uint32_t bestColorIndex = 0;
    
    for (int py = 0; py < readHeight; py++) {
        for (int px = 0; px < readWidth; px++) {
            int idx = (py * readWidth + px) * 4;
            uint32_t colorIndex = pixels[idx] | (pixels[idx + 1] << 8) | (pixels[idx + 2] << 16);
            if (colorIndex > 0) {
                int dx = px - centerOffsetX;
                int dy = py - centerOffsetY;
                int dist = dx * dx + dy * dy;
                if (dist < bestDistance) {
                    bestDistance = dist;
                    bestColorIndex = colorIndex;
                }
            }
        }
    }
    
    if (bestColorIndex > 0) {
        return static_cast<int>(bestColorIndex - 1);
    }
    
    return -1;
}

// Unproject screen point to world coordinates at given depth
glm::vec3 unprojectPoint(double mouseX, double mouseY, float depth) {
    float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    glm::vec4 viewport(0, 0, g_app.windowWidth, g_app.windowHeight);
    
    // Flip Y for OpenGL coordinate system
    float glY = static_cast<float>(g_app.windowHeight) - static_cast<float>(mouseY);
    
    glm::vec3 winCoord(static_cast<float>(mouseX), glY, depth);
    return glm::unProject(winCoord, view, projection, viewport);
}

// Calculate intersection with axis-aligned plane
glm::vec3 intersectAxisPlane(double mouseX, double mouseY, glm::vec3 planePoint, glm::vec3 planeNormal) {
    float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    glm::vec4 viewport(0, 0, g_app.windowWidth, g_app.windowHeight);
    
    float glY = static_cast<float>(g_app.windowHeight) - static_cast<float>(mouseY);
    
    // Get ray from camera through mouse position
    glm::vec3 nearPoint = glm::unProject(glm::vec3(mouseX, glY, 0.0f), view, projection, viewport);
    glm::vec3 farPoint = glm::unProject(glm::vec3(mouseX, glY, 1.0f), view, projection, viewport);
    glm::vec3 rayDir = glm::normalize(farPoint - nearPoint);
    
    // Intersect ray with plane
    float denom = glm::dot(planeNormal, rayDir);
    if (std::abs(denom) < 0.0001f) {
        // Ray is parallel to plane, return plane point
        return planePoint;
    }
    
    float t = glm::dot(planePoint - nearPoint, planeNormal) / denom;
    return nearPoint + rayDir * t;
}

// Get translation based on mouse position and axis
// Uses ray-plane intersection with camera-facing plane through selection center
glm::vec3 getManipulatorTranslation(double mouseX, double mouseY, Axis axis) {
    glm::vec3 selCenter = getSelectionCenter();
    
    // Get camera forward direction as plane normal (plane faces camera)
    glm::vec3 cameraForward = -g_app.camera.getAxisZ();  // Camera looks along -Z
    
    // Intersect ray with camera-facing plane through selection center
    glm::vec3 intersection = intersectAxisPlane(mouseX, mouseY, selCenter, cameraForward);
    
    // Return selection center with only the selected axis from intersection
    glm::vec3 result = selCenter;
    int axisIndex = static_cast<int>(axis);
    if (axisIndex >= 0 && axisIndex <= 2) {
        result[axisIndex] = intersection[axisIndex];
    }
    return result;
}

// Draw an axis-aligned plane for plane widget translation
// Matches original DrawSelectionPlane
void drawAxisAlignedPlane(Axis planeAxis) {
    glm::vec3 selCenter = getSelectionCenter();
    const float planeSize = 4000.0f;
    
    float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    
    if (!g_app.coloredShader || !g_app.coloredShader->isValid()) return;
    
    g_app.coloredShader->use();
    g_app.coloredShader->setMat4("modelViewProjection", projection * view);
    g_app.coloredShader->setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    std::vector<float> vertices;
    
    // Plane at selection center, perpendicular to the specified axis
    switch (planeAxis) {
        case Axis::X: {
            // YZ plane at selCenter.x
            float x = selCenter.x;
            vertices = {
                x, -planeSize, -planeSize,
                x, -planeSize,  planeSize,
                x,  planeSize,  planeSize,
                x, -planeSize, -planeSize,
                x,  planeSize,  planeSize,
                x,  planeSize, -planeSize
            };
            break;
        }
        case Axis::Y: {
            // XZ plane at selCenter.y
            float y = selCenter.y;
            vertices = {
                -planeSize, y, -planeSize,
                -planeSize, y,  planeSize,
                 planeSize, y,  planeSize,
                -planeSize, y, -planeSize,
                 planeSize, y,  planeSize,
                 planeSize, y, -planeSize
            };
            break;
        }
        case Axis::Z: {
            // XY plane at selCenter.z
            float z = selCenter.z;
            vertices = {
                -planeSize, -planeSize, z,
                -planeSize,  planeSize, z,
                 planeSize,  planeSize, z,
                -planeSize, -planeSize, z,
                 planeSize,  planeSize, z,
                 planeSize, -planeSize, z
            };
            break;
        }
        default:
            return;
    }
    
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

// Get translation for plane widget (moves on two axes, constrained perpendicular to plane)
// Uses ray-plane intersection with axis-aligned plane through selection center
glm::vec3 getManipulatorPlaneTranslation(double mouseX, double mouseY, Axis planeAxis) {
    glm::vec3 selCenter = getSelectionCenter();
    
    // Plane normal is the axis perpendicular to the movement plane
    glm::vec3 planeNormal;
    switch (planeAxis) {
        case Axis::X: planeNormal = glm::vec3(1, 0, 0); break;  // YZ plane
        case Axis::Y: planeNormal = glm::vec3(0, 1, 0); break;  // XZ plane  
        case Axis::Z: planeNormal = glm::vec3(0, 0, 1); break;  // XY plane
        default: planeNormal = glm::vec3(0, 1, 0); break;
    }
    
    // Intersect ray with plane through selection center
    glm::vec3 position = intersectAxisPlane(mouseX, mouseY, selCenter, planeNormal);
    
    // Keep the axis perpendicular to the plane fixed at selection center
    int axisIndex = static_cast<int>(planeAxis);
    if (axisIndex >= 0 && axisIndex <= 2) {
        position[axisIndex] = selCenter[axisIndex];
    }
    
    return position;
}

// Get rotation based on mouse position and axis
glm::quat getManipulatorRotation(double mouseX, double mouseY, Axis axis, glm::vec3& lastPosition) {
    glm::vec3 selCenter = getSelectionCenter();
    
    // Determine which plane to use based on rotation axis
    glm::vec3 planeNormal;
    switch (axis) {
        case Axis::X: planeNormal = glm::vec3(1, 0, 0); break;
        case Axis::Y: planeNormal = glm::vec3(0, 1, 0); break;
        case Axis::Z: planeNormal = glm::vec3(0, 0, 1); break;
        default: return glm::quat(1, 0, 0, 0);
    }
    
    glm::vec3 position = intersectAxisPlane(mouseX, mouseY, selCenter, planeNormal);
    position -= selCenter;
    
    float angle = 0.0f;
    glm::vec3 rotationAxis;
    
    switch (axis) {
        case Axis::X:
            angle = atan2f(position.y, position.z) - atan2f(lastPosition.y, lastPosition.z);
            rotationAxis = glm::vec3(1, 0, 0);
            angle = -angle;
            break;
        case Axis::Y:
            angle = atan2f(position.x, position.z) - atan2f(lastPosition.x, lastPosition.z);
            rotationAxis = glm::vec3(0, 1, 0);
            break;
        case Axis::Z:
            angle = atan2f(position.x, position.y) - atan2f(lastPosition.x, lastPosition.y);
            rotationAxis = glm::vec3(0, 0, 1);
            angle = -angle;
            break;
        default:
            return glm::quat(1, 0, 0, 0);
    }
    
    lastPosition = position;
    return glm::angleAxis(angle, rotationAxis);
}

// Draw a large camera-facing plane for depth buffer picking
void drawSelectionPlaneForPicking() {
    float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    
    // Original DrawPlane draws at ORIGIN, not at selection center
    // Using camera's right and up vectors to create camera-facing plane
    glm::vec3 cameraRight = g_app.camera.getAxisX();
    glm::vec3 cameraUp = g_app.camera.getAxisY();
    
    // Create a large quad at ORIGIN facing the camera (like original)
    const float planeSize = 4000.0f;  // Match original planeSize
    glm::vec3 v0 = -cameraRight * planeSize - cameraUp * planeSize;
    glm::vec3 v1 =  cameraRight * planeSize - cameraUp * planeSize;
    glm::vec3 v2 =  cameraRight * planeSize + cameraUp * planeSize;
    glm::vec3 v3 = -cameraRight * planeSize + cameraUp * planeSize;
    
    // Draw with a simple shader
    if (!g_app.coloredShader || !g_app.coloredShader->isValid()) return;
    
    g_app.coloredShader->use();
    g_app.coloredShader->setMat4("modelViewProjection", projection * view);
    g_app.coloredShader->setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    float vertices[] = {
        v0.x, v0.y, v0.z,
        v1.x, v1.y, v1.z,
        v2.x, v2.y, v2.z,
        v0.x, v0.y, v0.z,
        v2.x, v2.y, v2.z,
        v3.x, v3.y, v3.z
    };
    
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

// Get 3D position from mouse using depth buffer readback
// mouseX, mouseY are in logical (window) coordinates
glm::vec3 positionFromDepthBuffer(double mouseX, double mouseY) {
    float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    // Note: viewport uses logical window coordinates for unProject
    glm::vec4 viewport(0, 0, g_app.windowWidth, g_app.windowHeight);
    
    // Convert to framebuffer coordinates for glReadPixels
    int fbX = toFramebufferX(mouseX);
    int fbY = toFramebufferGLY(mouseY);  // Already flipped for OpenGL
    
    // Read depth at mouse position (framebuffer coordinates)
    float depth;
    glReadPixels(fbX, fbY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    
    // Unproject to world space (using logical window coordinates)
    float glY = static_cast<float>(g_app.windowHeight) - static_cast<float>(mouseY);
    glm::vec3 winCoord(static_cast<float>(mouseX), glY, depth);
    return glm::unProject(winCoord, view, projection, viewport);
}

// Draw camera-facing plane at a specific center point for translation
void drawSelectionPlaneAtCenter(const glm::vec3& center) {
    float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    
    glm::vec3 cameraRight = g_app.camera.getAxisX();
    glm::vec3 cameraUp = g_app.camera.getAxisY();
    
    // Create a large quad at CENTER facing the camera
    const float planeSize = 4000.0f;
    glm::vec3 v0 = center - cameraRight * planeSize - cameraUp * planeSize;
    glm::vec3 v1 = center + cameraRight * planeSize - cameraUp * planeSize;
    glm::vec3 v2 = center + cameraRight * planeSize + cameraUp * planeSize;
    glm::vec3 v3 = center - cameraRight * planeSize + cameraUp * planeSize;
    
    if (!g_app.coloredShader || !g_app.coloredShader->isValid()) return;
    
    g_app.coloredShader->use();
    g_app.coloredShader->setMat4("modelViewProjection", projection * view);
    g_app.coloredShader->setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    float vertices[] = {
        v0.x, v0.y, v0.z,
        v1.x, v1.y, v1.z,
        v2.x, v2.y, v2.z,
        v0.x, v0.y, v0.z,
        v2.x, v2.y, v2.z,
        v3.x, v3.y, v3.z
    };
    
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

// Get position for translation - plane at selection center so mouse follows object
glm::vec3 positionFromAxisPointForTranslation(double mouseX, double mouseY, Axis axis) {
    glm::vec3 selCenter = getSelectionCenter();
    
    // Draw camera-facing plane AT SELECTION CENTER and read position
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawSelectionPlaneAtCenter(selCenter);
    glm::vec3 position = positionFromDepthBuffer(mouseX, mouseY);
    
    // Return selection center with only the selected axis component from mouse position
    glm::vec3 result = selCenter;
    int axisIndex = static_cast<int>(axis);
    result[axisIndex] = position[axisIndex];
    return result;
}

// Get position from axis point - returns selectionCenter with only the specified axis from mouse position
// This matches the original positionFromRotatedAxisPoint - plane at ORIGIN for scale
glm::vec3 positionFromAxisPoint(double mouseX, double mouseY, Axis axis) {
    glm::vec3 selCenter = getSelectionCenter();
    
    // Draw camera-facing plane at ORIGIN and read position
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawSelectionPlaneForPicking();
    glm::vec3 position = positionFromDepthBuffer(mouseX, mouseY);
    
    // Return selection center with only the selected axis component from mouse position
    glm::vec3 result = selCenter;
    int axisIndex = static_cast<int>(axis);
    result[axisIndex] = position[axisIndex];
    return result;
}

// Get position from plane axis - for Center widget, uses Y plane (horizontal)
// Returns position on the plane with Y fixed to selection center
glm::vec3 positionFromPlaneAxisY(double mouseX, double mouseY) {
    glm::vec3 selCenter = getSelectionCenter();
    
    // Draw a horizontal plane at selection center height
    float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    
    const float planeSize = 1000.0f;
    glm::vec3 v0 = selCenter + glm::vec3(-planeSize, 0, -planeSize);
    glm::vec3 v1 = selCenter + glm::vec3( planeSize, 0, -planeSize);
    glm::vec3 v2 = selCenter + glm::vec3( planeSize, 0,  planeSize);
    glm::vec3 v3 = selCenter + glm::vec3(-planeSize, 0,  planeSize);
    
    if (!g_app.coloredShader || !g_app.coloredShader->isValid()) return selCenter;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    g_app.coloredShader->use();
    g_app.coloredShader->setMat4("modelViewProjection", projection * view);
    g_app.coloredShader->setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    float vertices[] = {
        v0.x, v0.y, v0.z,
        v1.x, v1.y, v1.z,
        v2.x, v2.y, v2.z,
        v0.x, v0.y, v0.z,
        v2.x, v2.y, v2.z,
        v3.x, v3.y, v3.z
    };
    
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    
    glm::vec3 position = positionFromDepthBuffer(mouseX, mouseY);
    
    // Return position with Y fixed to selection center (movement on X and Z)
    glm::vec3 result = position;
    result.y = selCenter.y;
    return result;
}

// Get position for scale using ray-plane intersection (cross-platform, works in WebGL2)
// Uses camera-facing plane through selection center for consistent scale behavior
glm::vec3 positionFromAxisPointForScale(double mouseX, double mouseY, Axis axis) {
    glm::vec3 selCenter = getSelectionCenter();
    
    // Get camera forward direction as plane normal (plane faces camera at selection center)
    glm::vec3 cameraForward = -g_app.camera.getAxisZ();
    
    // Intersect ray with camera-facing plane through selection center
    glm::vec3 intersection = intersectAxisPlane(mouseX, mouseY, selCenter, cameraForward);
    
    // Return selection center with only the selected axis from intersection
    glm::vec3 result = selCenter;
    int axisIndex = static_cast<int>(axis);
    if (axisIndex >= 0 && axisIndex <= 2) {
        result[axisIndex] = intersection[axisIndex];
    }
    return result;
}

// Get position for uniform scale (Center widget) using horizontal Y plane
glm::vec3 positionFromPlaneAxisYForScale(double mouseX, double mouseY) {
    glm::vec3 selCenter = getSelectionCenter();
    
    // Intersect ray with horizontal (Y) plane through selection center
    glm::vec3 planeNormal(0.0f, 1.0f, 0.0f);
    glm::vec3 intersection = intersectAxisPlane(mouseX, mouseY, selCenter, planeNormal);
    
    // Return position with Y fixed to selection center
    intersection.y = selCenter.y;
    return intersection;
}

// Initialize scale position - same as what getManipulatorScale uses but doesn't compute scale
// This matches the original scaleFromPoint call in mouseDown that just sets lastPosition
void initManipulatorScale(double mouseX, double mouseY, Axis axis, glm::vec3& lastPosition) {
    switch (axis) {
        case Axis::X:
        case Axis::Y:
        case Axis::Z:
            lastPosition = positionFromAxisPointForScale(mouseX, mouseY, axis);
            break;
        case Axis::Center:
            lastPosition = positionFromPlaneAxisYForScale(mouseX, mouseY);
            break;
    }
}

// Get scale based on mouse position and axis (matching original scaleFromPoint)
// Uses ray-plane intersection for cross-platform compatibility (WebGL2 + Desktop)
glm::vec3 getManipulatorScale(double mouseX, double mouseY, Axis axis, glm::vec3& lastPosition) {
    glm::vec3 position;
    glm::vec3 scale(0.0f);
    
    switch (axis) {
        case Axis::X:
        case Axis::Y:
        case Axis::Z:
            // Get position with only this axis from mouse, rest from selection center
            position = positionFromAxisPointForScale(mouseX, mouseY, axis);
            scale = position - lastPosition;
            break;
        case Axis::Center:
            // Use horizontal Y plane for uniform scale
            position = positionFromPlaneAxisYForScale(mouseX, mouseY);
            scale = position - lastPosition;
            // Uniform scale: use X delta for all axes
            scale.y = scale.x;
            scale.z = scale.x;
            break;
    }
    
    lastPosition = position;
    scale *= 2.0f;
    return scale;
}

// Draw selection rectangle overlay using shaders
void drawSelectionRect() {
    if (!g_app.isSelecting) return;
    if (!g_app.rectShader || !g_app.rectShader->isValid()) return;
    
    float minX = static_cast<float>(std::min(g_app.selectStartX, g_app.selectEndX));
    float minY = static_cast<float>(std::min(g_app.selectStartY, g_app.selectEndY));
    float maxX = static_cast<float>(std::max(g_app.selectStartX, g_app.selectEndX));
    float maxY = static_cast<float>(std::max(g_app.selectStartY, g_app.selectEndY));
    
    // Get viewport dimensions for NDC conversion
    float vpX = 0.0f;
    float vpWidth = static_cast<float>(g_app.windowWidth);
    float vpHeight = static_cast<float>(g_app.windowHeight);
    
    if (g_app.splitViewEnabled) {
        float splitX = static_cast<float>(g_app.windowWidth) * g_app.splitRatio;
        
        if (g_app.activeViewport == 1) {
            // UV viewport - right side
            vpX = splitX;
            vpWidth = static_cast<float>(g_app.windowWidth) - splitX;
            // Clamp selection rect to UV viewport
            minX = std::max(minX, vpX);
            maxX = std::min(maxX, static_cast<float>(g_app.windowWidth));
        } else {
            // 3D viewport - left side
            vpWidth = splitX;
            // Clamp selection rect to 3D viewport
            minX = std::max(minX, 0.0f);
            maxX = std::min(maxX, vpWidth);
        }
    }
    
    // Convert to normalized device coordinates relative to the active viewport
    float ndcMinX = (2.0f * (minX - vpX)) / vpWidth - 1.0f;
    float ndcMaxX = (2.0f * (maxX - vpX)) / vpWidth - 1.0f;
    float ndcMinY = 1.0f - (2.0f * maxY) / vpHeight;  // Flip Y
    float ndcMaxY = 1.0f - (2.0f * minY) / vpHeight;  // Flip Y
    
    // Create rectangle vertices (for both filled quad and line loop)
    float vertices[] = {
        // Filled quad (two triangles)
        ndcMinX, ndcMinY,
        ndcMaxX, ndcMinY,
        ndcMaxX, ndcMaxY,
        ndcMinX, ndcMinY,
        ndcMaxX, ndcMaxY,
        ndcMinX, ndcMaxY,
        // Line loop
        ndcMinX, ndcMinY,
        ndcMaxX, ndcMinY,
        ndcMaxX, ndcMaxY,
        ndcMinX, ndcMaxY,
    };
    
    // Create VAO/VBO if needed
    if (g_app.rectVao == 0) {
        glGenVertexArrays(1, &g_app.rectVao);
        glGenBuffers(1, &g_app.rectVbo);
        
        glBindVertexArray(g_app.rectVao);
        glBindBuffer(GL_ARRAY_BUFFER, g_app.rectVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);
    } else {
        // Update vertex data
        glBindBuffer(GL_ARRAY_BUFFER, g_app.rectVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    }
    
    // Set viewport for the active viewport
    if (g_app.splitViewEnabled && g_app.activeViewport == 1) {
        // UV viewport
        int uvX = static_cast<int>(g_app.framebufferWidth * g_app.splitRatio);
        int uvWidth = g_app.framebufferWidth - uvX;
        glViewport(uvX, 0, uvWidth, g_app.framebufferHeight);
    } else if (g_app.splitViewEnabled) {
        // 3D viewport
        int splitX = static_cast<int>(g_app.framebufferWidth * g_app.splitRatio);
        glViewport(0, 0, splitX, g_app.framebufferHeight);
    } else {
        glViewport(0, 0, g_app.framebufferWidth, g_app.framebufferHeight);
    }
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    g_app.rectShader->use();
    glBindVertexArray(g_app.rectVao);
    
    // Draw filled rectangle (semi-transparent blue)
    g_app.rectShader->setVec4("uColor", glm::vec4(0.2f, 0.4f, 1.0f, 0.2f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Draw rectangle outline (more opaque blue)
    g_app.rectShader->setVec4("uColor", glm::vec4(0.2f, 0.4f, 1.0f, 0.9f));
    glDrawArrays(GL_LINE_LOOP, 6, 4);
    
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

bool initShaders() {
#ifdef EMSCRIPTEN_BUILD
    std::string shaderPath = "/shaders/";
#else
    std::string shaderPath = "shaders/";
#endif
    
    g_app.meshShader = std::make_unique<Shader>();
    if (!g_app.meshShader->loadFromFiles(shaderPath + "mesh.vert", shaderPath + "mesh.frag")) {
        std::cerr << "Failed to load mesh shaders" << std::endl;
        return false;
    }
    
    g_app.meshWireShader = std::make_unique<Shader>();
    if (!g_app.meshWireShader->loadFromFiles(shaderPath + "mesh_wire.vert", shaderPath + "mesh_wire.frag")) {
        std::cerr << "Failed to load mesh_wire shaders" << std::endl;
        return false;
    }
    
    g_app.gridShader = std::make_unique<Shader>();
    if (!g_app.gridShader->loadFromFiles(shaderPath + "grid.vert", shaderPath + "grid.frag")) {
        std::cerr << "Failed to load grid shaders" << std::endl;
        return false;
    }
    
    g_app.selectionShader = std::make_unique<Shader>();
    if (!g_app.selectionShader->loadFromFiles(shaderPath + "selection.vert", shaderPath + "selection.frag")) {
        std::cerr << "Failed to load selection shaders" << std::endl;
        return false;
    }
    
    g_app.rectShader = std::make_unique<Shader>();
    if (!g_app.rectShader->loadFromFiles(shaderPath + "rect.vert", shaderPath + "rect.frag")) {
        std::cerr << "Failed to load rect shaders" << std::endl;
        return false;
    }
    
    g_app.coloredShader = std::make_unique<Shader>();
    if (!g_app.coloredShader->loadFromFiles(shaderPath + "colored.vert", shaderPath + "colored.frag")) {
        std::cerr << "Failed to load colored shaders" << std::endl;
        return false;
    }
    
    g_app.manipulatorShader = std::make_unique<Shader>();
    if (!g_app.manipulatorShader->loadFromFiles(shaderPath + "manipulator.vert", shaderPath + "manipulator.frag")) {
        std::cerr << "Failed to load manipulator shaders" << std::endl;
        return false;
    }
    
    g_app.thickLineShader = std::make_unique<Shader>();
    if (!g_app.thickLineShader->loadFromFiles(shaderPath + "thickline.vert", shaderPath + "thickline.frag")) {
        std::cerr << "Failed to load thick line shaders" << std::endl;
        return false;
    }
    
    g_app.thickLineColoredShader = std::make_unique<Shader>();
    if (!g_app.thickLineColoredShader->loadFromFiles(shaderPath + "thickline_colored.vert", shaderPath + "thickline_colored.frag")) {
        std::cerr << "Failed to load thick line colored shaders" << std::endl;
        return false;
    }
    
    // UV Editor shaders
    g_app.uvShader = std::make_unique<Shader>();
    if (!g_app.uvShader->loadFromFiles(shaderPath + "uv.vert", shaderPath + "uv.frag")) {
        std::cerr << "Failed to load UV shaders" << std::endl;
        return false;
    }
    
    g_app.uvGridShader = std::make_unique<Shader>();
    if (!g_app.uvGridShader->loadFromFiles(shaderPath + "uvgrid.vert", shaderPath + "uvgrid.frag")) {
        std::cerr << "Failed to load UV grid shaders" << std::endl;
        return false;
    }
    
    g_app.uvColoredShader = std::make_unique<Shader>();
    if (!g_app.uvColoredShader->loadFromFiles(shaderPath + "uvcolored.vert", shaderPath + "uvcolored.frag")) {
        std::cerr << "Failed to load UV colored shaders" << std::endl;
        return false;
    }
    
    g_app.uvBackgroundShader = std::make_unique<Shader>();
    if (!g_app.uvBackgroundShader->loadFromFiles(shaderPath + "uvbackground.vert", shaderPath + "uvbackground.frag")) {
        std::cerr << "Failed to load UV background shaders" << std::endl;
        return false;
    }
    
    g_app.uvSelectionShader = std::make_unique<Shader>();
    if (!g_app.uvSelectionShader->loadFromFiles(shaderPath + "uvselection.vert", shaderPath + "uvselection.frag")) {
        std::cerr << "Failed to load UV selection shaders" << std::endl;
        return false;
    }
    
    return true;
}

void initScene() {
    // Create item collection with one cube item to start
    g_app.items = std::make_unique<ItemCollection>();
    g_app.items->addCube();
    
    // Create grid
    g_app.grid = std::make_unique<Grid>(10, 20);
    g_app.grid->createGPUBuffers();
    
    // Create manipulators for each transform mode
    g_app.translateManipulator = std::make_unique<Manipulator>(ManipulatorType::Translation);
    g_app.rotateManipulator = std::make_unique<Manipulator>(ManipulatorType::Rotation);
    g_app.scaleManipulator = std::make_unique<Manipulator>(ManipulatorType::Scale);
    
    // Create UV grid (0-1 square with subdivisions)
    std::vector<float> uvGridLines;
    // Main UV square border
    uvGridLines.insert(uvGridLines.end(), {0.0f, 0.0f, 1.0f, 0.0f}); // bottom
    uvGridLines.insert(uvGridLines.end(), {1.0f, 0.0f, 1.0f, 1.0f}); // right
    uvGridLines.insert(uvGridLines.end(), {1.0f, 1.0f, 0.0f, 1.0f}); // top
    uvGridLines.insert(uvGridLines.end(), {0.0f, 1.0f, 0.0f, 0.0f}); // left
    // Grid subdivisions (0.1 step)
    for (float i = 0.1f; i < 1.0f; i += 0.1f) {
        uvGridLines.insert(uvGridLines.end(), {i, 0.0f, i, 1.0f}); // vertical
        uvGridLines.insert(uvGridLines.end(), {0.0f, i, 1.0f, i}); // horizontal
    }
    
    glGenVertexArrays(1, &g_app.uvGridVao);
    glGenBuffers(1, &g_app.uvGridVbo);
    glBindVertexArray(g_app.uvGridVao);
    glBindBuffer(GL_ARRAY_BUFFER, g_app.uvGridVbo);
    glBufferData(GL_ARRAY_BUFFER, uvGridLines.size() * sizeof(float), uvGridLines.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    // Create UV background quad (0-1 range)
    float uvBackgroundQuad[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    
    glGenVertexArrays(1, &g_app.uvBackgroundVao);
    glGenBuffers(1, &g_app.uvBackgroundVbo);
    glBindVertexArray(g_app.uvBackgroundVao);
    glBindBuffer(GL_ARRAY_BUFFER, g_app.uvBackgroundVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvBackgroundQuad), uvBackgroundQuad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void renderImGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    
#ifdef EMSCRIPTEN_BUILD
    // Override ImGui display size to match our physical framebuffer
    // GLFW in Emscripten doesn't properly report the scaled framebuffer size
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(g_app.framebufferWidth), 
                            static_cast<float>(g_app.framebufferHeight));
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    
    static bool loggedOnce = false;
    if (!loggedOnce) {
        DEBUG_COUT("[ImGui Override] DisplaySize: " << io.DisplaySize.x << "x" << io.DisplaySize.y
                  << ", DisplayFramebufferScale: " << io.DisplayFramebufferScale.x 
                  << std::endl);
        loggedOnce = true;
    }
#endif
    
    ImGui::NewFrame();
    
    // Enable docking
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    
    // Settings window
    ImGui::Begin("MeshMaker WebGL2");
    
    ImGui::Text("Camera Controls:");
    ImGui::BulletText("Alt+Left Mouse: Orbit");
    ImGui::BulletText("Alt+Middle Mouse: Pan");
    ImGui::BulletText("Scroll: Zoom");
    
    if (ImGui::Button("Reset Camera")) {
        g_app.camera.reset();
    }
    
    ImGui::Separator();
    ImGui::Text("Edit:");
    
    // Undo/Redo buttons
    bool canUndo = g_app.undoManager.canUndo();
    bool canRedo = g_app.undoManager.canRedo();
    
    if (!canUndo) ImGui::BeginDisabled();
    if (ImGui::Button("Undo (Ctrl+Z)")) {
        g_app.undoManager.undo();
    }
    if (!canUndo) ImGui::EndDisabled();
    
    ImGui::SameLine();
    
    if (!canRedo) ImGui::BeginDisabled();
    if (ImGui::Button("Redo (Ctrl+Y)")) {
        g_app.undoManager.redo();
    }
    if (!canRedo) ImGui::EndDisabled();
    
    // Show undo/redo counts
    ImGui::Text("Undo: %zu  Redo: %zu", g_app.undoManager.getUndoCount(), g_app.undoManager.getRedoCount());
    
    ImGui::Separator();
    ImGui::Text("Add Primitives:");
    
    if (ImGui::Button("Cube")) { 
        sceneActionWithUndo("Add Cube", []() { g_app.items->addCube(); });
    }
    ImGui::SameLine();
    if (ImGui::Button("Plane")) { 
        sceneActionWithUndo("Add Plane", []() { g_app.items->addPlane(); });
    }
    ImGui::SameLine();
    if (ImGui::Button("Cylinder")) { 
        sceneActionWithUndo("Add Cylinder", [steps = g_app.meshSteps]() { 
            g_app.items->addCylinder(static_cast<uint32_t>(steps)); 
        });
    }
    ImGui::SameLine();
    if (ImGui::Button("Sphere")) { 
        sceneActionWithUndo("Add Sphere", [steps = g_app.meshSteps]() { 
            g_app.items->addSphere(static_cast<uint32_t>(steps)); 
        });
    }
    ImGui::SameLine();
    if (ImGui::Button("Icosahedron")) { 
        sceneActionWithUndo("Add Icosahedron", []() { g_app.items->addIcosahedron(); });
    }
    
    ImGui::SliderInt("Steps (Cyl/Sphere)", &g_app.meshSteps, 4, 64);
    
    ImGui::Separator();
    ImGui::Text("Edit Mode:");
    
    // Edit mode selector
    const char* editModes[] = { "Items", "Vertices", "Triangles", "Edges" };
    int currentEditMode = static_cast<int>(g_app.items->getEditMode());
    if (ImGui::Combo("Mode", &currentEditMode, editModes, 4)) {
        g_app.items->setEditMode(static_cast<EditMode>(currentEditMode));
    }
    
    ImGui::Separator();
    ImGui::Text("Selection:");
    ImGui::BulletText("Click: Select");
    ImGui::BulletText("Shift+Click: Add to selection");
    ImGui::BulletText("A: Select all, D: Deselect all");
    
    size_t selectedCount = getSelectionCount();
    EditMode editMode = g_app.items->getEditMode();
    
    if (editMode == EditMode::Items) {
        ImGui::Text("Selected: %zu / %zu items", selectedCount, g_app.items->getItemCount());
    } else {
        Mesh2* mesh = g_app.items->getCurrentMesh();
        size_t totalCount = mesh ? mesh->getFaceCount() : 0;
        ImGui::Text("Selected: %zu / %zu faces", selectedCount, totalCount);
    }
    
    ImGui::SameLine();
    if (ImGui::Button("All")) {
        selectAll();
    }
    ImGui::SameLine();
    if (ImGui::Button("None")) {
        deselectAll();
    }
    
    ImGui::Separator();
    ImGui::Text("Transform (requires selection):");
    
    // Transform mode display and buttons
    const char* modeNames[] = { "Select (1)", "Translate (2)", "Rotate (3)", "Scale (4)" };
    int currentMode = static_cast<int>(g_app.transformMode);
    ImGui::Text("Mode: %s", modeNames[currentMode]);
    
    if (ImGui::Button("Select (1)")) g_app.transformMode = TransformMode::None;
    ImGui::SameLine();
    if (ImGui::Button("Translate (2)")) g_app.transformMode = TransformMode::Translate;
    ImGui::SameLine();
    if (ImGui::Button("Rotate (3)")) g_app.transformMode = TransformMode::Rotate;
    ImGui::SameLine();
    if (ImGui::Button("Scale (4)")) g_app.transformMode = TransformMode::Scale;
    
    if (selectedCount > 0) {
        // Show relevant controls based on mode
        switch (g_app.transformMode) {
            case TransformMode::Translate:
                ImGui::Text("Use X/Y/Z or Arrow keys to move");
                ImGui::SliderFloat("Step", &g_app.translateStep, 0.01f, 1.0f);
                if (ImGui::Button("+X")) { translateSelection(glm::vec3(g_app.translateStep, 0, 0)); }
                ImGui::SameLine();
                if (ImGui::Button("-X")) { translateSelection(glm::vec3(-g_app.translateStep, 0, 0)); }
                ImGui::SameLine();
                if (ImGui::Button("+Y")) { translateSelection(glm::vec3(0, g_app.translateStep, 0)); }
                ImGui::SameLine();
                if (ImGui::Button("-Y")) { translateSelection(glm::vec3(0, -g_app.translateStep, 0)); }
                ImGui::SameLine();
                if (ImGui::Button("+Z")) { translateSelection(glm::vec3(0, 0, g_app.translateStep)); }
                ImGui::SameLine();
                if (ImGui::Button("-Z")) { translateSelection(glm::vec3(0, 0, -g_app.translateStep)); }
                break;
                
            case TransformMode::Rotate:
                ImGui::Text("Use X/Y/Z or Arrow keys to rotate");
                ImGui::SliderFloat("Degrees", &g_app.rotateStep, 1.0f, 90.0f);
                if (ImGui::Button("X+")) { rotateSelection(glm::vec3(1,0,0), glm::radians(g_app.rotateStep)); }
                ImGui::SameLine();
                if (ImGui::Button("X-")) { rotateSelection(glm::vec3(1,0,0), glm::radians(-g_app.rotateStep)); }
                ImGui::SameLine();
                if (ImGui::Button("Y+")) { rotateSelection(glm::vec3(0,1,0), glm::radians(g_app.rotateStep)); }
                ImGui::SameLine();
                if (ImGui::Button("Y-")) { rotateSelection(glm::vec3(0,1,0), glm::radians(-g_app.rotateStep)); }
                ImGui::SameLine();
                if (ImGui::Button("Z+")) { rotateSelection(glm::vec3(0,0,1), glm::radians(g_app.rotateStep)); }
                ImGui::SameLine();
                if (ImGui::Button("Z-")) { rotateSelection(glm::vec3(0,0,1), glm::radians(-g_app.rotateStep)); }
                break;
                
            case TransformMode::Scale:
                ImGui::Text("Use +/- or Up/Down arrows to scale");
                ImGui::SliderFloat("Factor", &g_app.scaleStep, 0.01f, 0.5f);
                if (ImGui::Button("Scale Up")) { 
                    scaleSelection(getSelectionCenter(), 1.0f + g_app.scaleStep);
                }
                ImGui::SameLine();
                if (ImGui::Button("Scale Down")) { 
                    scaleSelection(getSelectionCenter(), 1.0f - g_app.scaleStep);
                }
                break;
                
            default:
                ImGui::Text("Press 2/3/4 to enter transform mode");
                break;
        }
        
        ImGui::Separator();
        if (editMode == EditMode::Items) {
            ImGui::Text("Item Operations:");
            if (ImGui::Button("Duplicate")) { 
                sceneActionWithUndo("Duplicate Items", []() { g_app.items->duplicateSelectedItems(); });
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete")) { 
                sceneActionWithUndo("Delete Items", []() { g_app.items->deleteSelectedItems(); });
            }
        } else {
            ImGui::Text("Mesh Operations:");
            if (ImGui::Button("Flip (F)")) { 
                meshActionWithUndo("Flip Normals", []() { g_app.items->flipSelectedFaces(); });
            }
            ImGui::SameLine();
            if (ImGui::Button("Duplicate")) { 
                meshActionWithUndo("Duplicate", []() { g_app.items->duplicateSelectedFaces(); });
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete")) { 
                meshActionWithUndo("Delete", []() { g_app.items->deleteSelectedFaces(); });
            }
            
            // Additional mesh operations
            if (ImGui::Button("Subdivide")) { 
                meshActionWithUndo("Subdivide", []() { g_app.items->catmullClarkSubdivide(1); });
            }
            if (editMode == EditMode::Triangles) {
                ImGui::SameLine();
                if (ImGui::Button("Triangulate (Ctrl+Shift+T)")) { 
                    meshActionWithUndo("Triangulate", []() { g_app.items->triangulateSelectedFaces(); });
                }
                ImGui::SameLine();
                if (ImGui::Button("Extrude (Ctrl+Shift+E)")) { 
                    meshActionWithUndo("Extrude", []() { g_app.items->extrudeSelectedFaces(); });
                }
                ImGui::SameLine();
                if (ImGui::Button("Split (S)")) { 
                    meshActionWithUndo("Split", []() { g_app.items->splitSelected(); });
                }
            }
            if (editMode == EditMode::Edges) {
                ImGui::SameLine();
                if (ImGui::Button("Split (S)")) { 
                    meshActionWithUndo("Split", []() { g_app.items->splitSelected(); });
                }
            }
            if (editMode == EditMode::Vertices) {
                if (ImGui::Button("Merge (Ctrl+Shift+M)")) { 
                    meshActionWithUndo("Merge Vertices", []() { g_app.items->mergeSelectedVertices(); });
                }
            }
        }
    } else {
        if (editMode == EditMode::Items) {
            ImGui::TextDisabled("Select items first");
        } else {
            ImGui::TextDisabled("Select faces first");
        }
    }
    
    ImGui::Separator();
    ImGui::Text("View Settings:");
    
    const char* viewModes[] = { "Solid", "Wireframe", "Solid + Wireframe" };
    ImGui::Combo("View Mode", &g_app.viewMode, viewModes, 3);
    
    ImGui::Checkbox("Show Grid", &g_app.showGrid);
    ImGui::Checkbox("Show Normals", &g_app.showNormals);
    
    ImGui::Separator();
    ImGui::Text("Stats:");
    size_t totalVerts = 0, totalFaces = 0;
    g_app.items->getVertexAndFaceCount(totalVerts, totalFaces);
    ImGui::Text("Items: %zu", g_app.items->getItemCount());
    ImGui::Text("Total Vertices: %zu", totalVerts);
    ImGui::Text("Total Faces: %zu", totalFaces);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    
    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Forward declaration
void renderUVViewport(int x, int y, int width, int height);

// Render UV view (2D UV space rendering) - standalone full-screen version
void renderUV() {
    // Just call the viewport version with full screen dimensions
    renderUVViewport(0, 0, g_app.framebufferWidth, g_app.framebufferHeight);
}

// Render 3D viewport to a specific region of the screen
void render3DViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    glScissor(x, y, width, height);
    glEnable(GL_SCISSOR_TEST);
    
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    
    // Draw grid
    if (g_app.showGrid) {
        glDisable(GL_DEPTH_TEST);
        g_app.gridShader->use();
        g_app.gridShader->setMat4("uModel", glm::mat4(1.0f));
        g_app.gridShader->setMat4("uView", view);
        g_app.gridShader->setMat4("uProjection", projection);
        g_app.grid->draw();
        glEnable(GL_DEPTH_TEST);
    }
    
    ViewMode viewMode = static_cast<ViewMode>(g_app.viewMode);
    
    // Draw all items
    g_app.meshWireShader->use();
    g_app.meshWireShader->setBool("uShowSeams", g_app.splitViewEnabled);
    g_app.items->draw(*g_app.meshShader, *g_app.meshWireShader, *g_app.thickLineColoredShader, viewMode, view, projection,
                      static_cast<float>(width), static_cast<float>(height));
    
    // Draw component overlay (vertices/edges) for component editing modes
    g_app.items->drawComponentOverlay(*g_app.coloredShader, *g_app.thickLineColoredShader, view, projection,
                                      static_cast<float>(width), static_cast<float>(height));
    
    // Draw face normals if enabled
    if (g_app.showNormals) {
        glDisable(GL_DEPTH_TEST);
        g_app.items->drawNormals(*g_app.coloredShader, view, projection);
        glEnable(GL_DEPTH_TEST);
    }
    
    // Draw manipulator if there's a selection and we're in a transform mode
    Manipulator* manipulator = getCurrentManipulator();
    if (manipulator && getSelectionCount() > 0) {
        manipulator->position = getSelectionCenter();
        manipulator->size = g_app.camera.getDistance() * 0.15f;
        glm::vec3 cameraForward = g_app.camera.getForwardDirection();
        
        manipulator->draw(*g_app.manipulatorShader, *g_app.thickLineShader, view, projection, 
                          cameraForward, manipulator->position, 
                          static_cast<float>(width), static_cast<float>(height));
    }
    
    // Draw selection rectangle overlay (only if active viewport is 3D)
    if (g_app.activeViewport == 0) {
        drawSelectionRect();
    }
    
    glDisable(GL_SCISSOR_TEST);
}

// Render UV viewport to a specific region of the screen
void renderUVViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    glScissor(x, y, width, height);
    glEnable(GL_SCISSOR_TEST);
    
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);  // Slightly different background for UV
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    // Calculate UV transform - keep UV space square and centered
    float viewportAspect = static_cast<float>(width) / static_cast<float>(height);
    float scale = g_app.uvZoom;
    
    // Adjust for aspect ratio to keep UV space square
    glm::vec2 aspectAdjust(1.0f, 1.0f);
    if (viewportAspect > 1.0f) {
        aspectAdjust.x = 1.0f / viewportAspect;
    } else {
        aspectAdjust.y = viewportAspect;
    }
    
    glm::vec2 offset = g_app.uvOffset;
    
    // Draw texture background (before grid so grid shows on top)
    if (g_app.uvBackgroundShader && g_app.uvBackgroundVao) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        g_app.uvBackgroundShader->use();
        g_app.uvBackgroundShader->setVec2("uOffset", offset);
        g_app.uvBackgroundShader->setFloat("uZoom", scale);
        g_app.uvBackgroundShader->setVec2("uAspectAdjust", aspectAdjust);
        g_app.uvBackgroundShader->setFloat("uAlpha", 0.8f);
        
        // Get texture from first selected item
        auto texture = g_app.items->getFirstSelectedTexture();
        if (texture) {
            texture->bind(0);
            g_app.uvBackgroundShader->setInt("uTexture", 0);
            g_app.uvBackgroundShader->setBool("uHasTexture", true);
        } else {
            g_app.uvBackgroundShader->setBool("uHasTexture", false);
        }
        
        glBindVertexArray(g_app.uvBackgroundVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        if (texture) {
            Texture::unbind(0);
        }
        
        glDisable(GL_BLEND);
    }
    
    // Draw UV grid (0-1 square with subdivisions)
    if (g_app.uvGridShader && g_app.uvGridVao) {
        g_app.uvGridShader->use();
        g_app.uvGridShader->setVec2("uOffset", offset);
        g_app.uvGridShader->setFloat("uZoom", scale);
        g_app.uvGridShader->setVec2("uAspectAdjust", aspectAdjust);
        
        glBindVertexArray(g_app.uvGridVao);
        glDrawArrays(GL_LINES, 0, 84);  // 21*2 horizontal + 21*2 vertical lines
        glBindVertexArray(0);
    }
    
    // Enable blending for UV mesh rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw UV faces from selected items
    g_app.items->drawUV(*g_app.uvShader, *g_app.uvColoredShader, offset, scale, aspectAdjust);
    
    // Draw selection rectangle overlay (only if active viewport is UV)
    if (g_app.activeViewport == 1) {
        drawSelectionRect();
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
}

// Draw a vertical divider line between viewports
void drawViewportDivider(int x, int height) {
    glViewport(0, 0, g_app.framebufferWidth, g_app.framebufferHeight);
    glDisable(GL_DEPTH_TEST);
    
    // Simple line at the split position
    if (g_app.rectShader) {
        g_app.rectShader->use();
        
        // Convert pixel position to NDC
        float ndcX = (2.0f * x / g_app.framebufferWidth) - 1.0f;
        float lineWidth = 2.0f / g_app.framebufferWidth;  // 2 pixels wide
        
        float vertices[] = {
            ndcX - lineWidth, -1.0f,
            ndcX + lineWidth, -1.0f,
            ndcX + lineWidth,  1.0f,
            ndcX - lineWidth, -1.0f,
            ndcX + lineWidth,  1.0f,
            ndcX - lineWidth,  1.0f,
        };
        
        g_app.rectShader->setVec4("uColor", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
        
        GLuint tempVao, tempVbo;
        glGenVertexArrays(1, &tempVao);
        glGenBuffers(1, &tempVbo);
        glBindVertexArray(tempVao);
        glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDeleteBuffers(1, &tempVbo);
        glDeleteVertexArrays(1, &tempVao);
        glBindVertexArray(0);
    }
    
    glEnable(GL_DEPTH_TEST);
}

void render() {
    if (g_app.splitViewEnabled) {
        // Split-screen mode: 3D on left, UV on right
        int splitX = static_cast<int>(g_app.framebufferWidth * g_app.splitRatio);
        int leftWidth = splitX;
        int rightWidth = g_app.framebufferWidth - splitX;
        int height = g_app.framebufferHeight;
        
        // Render 3D viewport on left
        render3DViewport(0, 0, leftWidth, height);
        
        // Render UV viewport on right
        renderUVViewport(splitX, 0, rightWidth, height);
        
        // Draw divider
        drawViewportDivider(splitX, height);
    } else {
        // Single viewport mode: full screen 3D
        glViewport(0, 0, g_app.framebufferWidth, g_app.framebufferHeight);
        
        glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
        glm::mat4 view = g_app.camera.getViewMatrix();
        glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
        
        // Draw grid
        if (g_app.showGrid) {
            glDisable(GL_DEPTH_TEST);
            g_app.gridShader->use();
            g_app.gridShader->setMat4("uModel", glm::mat4(1.0f));
            g_app.gridShader->setMat4("uView", view);
            g_app.gridShader->setMat4("uProjection", projection);
            g_app.grid->draw();
            glEnable(GL_DEPTH_TEST);
        }
        
        ViewMode viewMode = static_cast<ViewMode>(g_app.viewMode);
        
        // Draw all items
        g_app.meshWireShader->use();
        g_app.meshWireShader->setBool("uShowSeams", g_app.splitViewEnabled);
        g_app.items->draw(*g_app.meshShader, *g_app.meshWireShader, *g_app.thickLineColoredShader, viewMode, view, projection,
                          static_cast<float>(g_app.framebufferWidth), static_cast<float>(g_app.framebufferHeight));
        
        // Draw component overlay
        g_app.items->drawComponentOverlay(*g_app.coloredShader, *g_app.thickLineColoredShader, view, projection,
                                          static_cast<float>(g_app.framebufferWidth), static_cast<float>(g_app.framebufferHeight));
        
        // Draw face normals if enabled
        if (g_app.showNormals) {
            glDisable(GL_DEPTH_TEST);
            g_app.items->drawNormals(*g_app.coloredShader, view, projection);
            glEnable(GL_DEPTH_TEST);
        }
        
        // Draw manipulator if there's a selection and we're in a transform mode
        Manipulator* manipulator = getCurrentManipulator();
        if (manipulator && getSelectionCount() > 0) {
            manipulator->position = getSelectionCenter();
            manipulator->size = g_app.camera.getDistance() * 0.15f;
            glm::vec3 cameraForward = g_app.camera.getForwardDirection();
            
            manipulator->draw(*g_app.manipulatorShader, *g_app.thickLineShader, view, projection, 
                              cameraForward, manipulator->position, 
                              static_cast<float>(g_app.framebufferWidth), static_cast<float>(g_app.framebufferHeight));
        }
        
        // Draw selection rectangle overlay
        drawSelectionRect();
    }
    
    // Draw ImGui (can be hidden when using external UI like React)
    if (g_app.showImGui) {
        renderImGui();
    }
}

void mainLoop() {
    glfwPollEvents();
    render();
    glfwSwapBuffers(g_app.window);
}

#ifdef EMSCRIPTEN_BUILD
// Helper to get the container's size and update canvas to match
EM_JS(void, getContainerSize, (double* outWidth, double* outHeight), {
    var container = document.getElementById('canvas-container');
    if (container) {
        var rect = container.getBoundingClientRect();
        setValue(outWidth, rect.width, 'double');
        setValue(outHeight, rect.height, 'double');
    } else {
        // Fallback to window size if container not found
        setValue(outWidth, window.innerWidth, 'double');
        setValue(outHeight, window.innerHeight, 'double');
    }
});

// Helper to update canvas CSS size (overrides Emscripten's !important styles)
EM_JS(void, updateCanvasCssSize, (double width, double height), {
    var canvas = document.getElementById('canvas');
    if (canvas) {
        canvas.style.setProperty('width', width + 'px', 'important');
        canvas.style.setProperty('height', height + 'px', 'important');
    }
});

void emscriptenMainLoop() {
    // Poll for CONTAINER size changes (not canvas - Emscripten sets canvas with !important)
    double cssWidth, cssHeight;
    getContainerSize(&cssWidth, &cssHeight);
    
    double devicePixelRatio = emscripten_get_device_pixel_ratio();
    int physicalWidth = static_cast<int>(cssWidth * devicePixelRatio);
    int physicalHeight = static_cast<int>(cssHeight * devicePixelRatio);
    
    // Check if size changed
    if (physicalWidth != g_app.framebufferWidth || physicalHeight != g_app.framebufferHeight) {
        // Update canvas buffer size
        emscripten_set_canvas_element_size("#canvas", physicalWidth, physicalHeight);
        
        // Update CSS size to match container (override Emscripten's !important)
        updateCanvasCssSize(cssWidth, cssHeight);
        
        g_app.framebufferWidth = physicalWidth;
        g_app.framebufferHeight = physicalHeight;
        g_app.windowWidth = physicalWidth;
        g_app.windowHeight = physicalHeight;
        g_app.dpiScale = static_cast<float>(devicePixelRatio);
        
        DEBUG_COUT("[WebGL Resize] CSS: " << cssWidth << "x" << cssHeight 
                  << ", physical: " << physicalWidth << "x" << physicalHeight << std::endl);
        
        glViewport(0, 0, physicalWidth, physicalHeight);
    }
    
    mainLoop();
}
#endif

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW
#ifdef EMSCRIPTEN_BUILD
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    // Tell Emscripten's GLFW we're HiDPI-aware - this affects mouse coordinate scaling
    // Emscripten uses GLFW_SCALE_TO_MONITOR (0x0002200C) to determine if app handles HiDPI
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#endif
    
    // Create window
    g_app.window = glfwCreateWindow(g_app.windowWidth, g_app.windowHeight, 
                                     "MeshMaker WebGL2", nullptr, nullptr);
    if (!g_app.window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(g_app.window);
    glfwSwapInterval(1);  // VSync
    
#ifndef EMSCRIPTEN_BUILD
    // Initialize GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#endif
    
    // Get initial framebuffer size and content scale for High DPI support
#ifdef EMSCRIPTEN_BUILD
    // For WebGL High DPI: get the CONTAINER size (not canvas - Emscripten overrides that)
    double cssWidth, cssHeight;
    getContainerSize(&cssWidth, &cssHeight);
    
    double devicePixelRatio = emscripten_get_device_pixel_ratio();
    int physicalWidth = static_cast<int>(cssWidth * devicePixelRatio);
    int physicalHeight = static_cast<int>(cssHeight * devicePixelRatio);
    
    // Set the canvas backing buffer to physical pixel size for sharp rendering
    emscripten_set_canvas_element_size("#canvas", physicalWidth, physicalHeight);
    
    // Override Emscripten's !important CSS with the actual container size
    updateCanvasCssSize(cssWidth, cssHeight);
    
    g_app.framebufferWidth = physicalWidth;
    g_app.framebufferHeight = physicalHeight;
    // After setting canvas size, GLFW mouse coords are in physical pixels
    g_app.windowWidth = physicalWidth;
    g_app.windowHeight = physicalHeight;
    g_app.contentScaleX = 1.0f;  // Mouse coords already in physical pixels
    g_app.contentScaleY = 1.0f;
    g_app.dpiScale = static_cast<float>(devicePixelRatio);  // For ImGui font scaling
    
    DEBUG_COUT("[WebGL DPI Init] Container: " << cssWidth << "x" << cssHeight 
              << ", physical: " << physicalWidth << "x" << physicalHeight
              << ", dpiScale: " << g_app.dpiScale << std::endl);
#else
    glfwGetFramebufferSize(g_app.window, &g_app.framebufferWidth, &g_app.framebufferHeight);
    glfwGetWindowSize(g_app.window, &g_app.windowWidth, &g_app.windowHeight);
    
    // Calculate content scale from actual framebuffer/window ratio
    if (g_app.windowWidth > 0 && g_app.windowHeight > 0) {
        g_app.contentScaleX = static_cast<float>(g_app.framebufferWidth) / static_cast<float>(g_app.windowWidth);
        g_app.contentScaleY = static_cast<float>(g_app.framebufferHeight) / static_cast<float>(g_app.windowHeight);
    } else {
        g_app.contentScaleX = 1.0f;
        g_app.contentScaleY = 1.0f;
    }
    
    // Get GLFW content scale for DPI-aware font/UI sizing
    float glfwScaleX, glfwScaleY;
    glfwGetWindowContentScale(g_app.window, &glfwScaleX, &glfwScaleY);
    g_app.dpiScale = glfwScaleX;
    
    DEBUG_COUT("[Desktop DPI Init] window: " << g_app.windowWidth << "x" << g_app.windowHeight
              << ", framebuffer: " << g_app.framebufferWidth << "x" << g_app.framebufferHeight
              << ", dpiScale: " << g_app.dpiScale << std::endl);
#endif
    
    // Set callbacks
    glfwSetFramebufferSizeCallback(g_app.window, framebufferSizeCallback);
    glfwSetScrollCallback(g_app.window, scrollCallback);
    glfwSetMouseButtonCallback(g_app.window, mouseButtonCallback);
    glfwSetCursorPosCallback(g_app.window, cursorPosCallback);
    glfwSetKeyCallback(g_app.window, keyCallback);
    
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
#ifdef EMSCRIPTEN_BUILD
    // WebGL: No font/style scaling needed since we render at physical resolution
    // The dpiScale is handled by setting canvas size to physical pixels
    float baseFontSize = 13.0f;
    io.Fonts->AddFontDefault();
    DEBUG_COUT("[ImGui DPI] Font size: " << baseFontSize 
              << "px (WebGL renders at physical resolution)" << std::endl);
#else
    // Desktop: Scale ImGui for High DPI using actual DPI scale
    ImGui::GetStyle().ScaleAllSizes(g_app.dpiScale);
    
    // Load font at scaled size for High DPI
    float baseFontSize = 13.0f;
    io.Fonts->AddFontDefault();
    ImFontConfig fontConfig;
    fontConfig.SizePixels = baseFontSize * g_app.dpiScale;
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;
    io.FontDefault = io.Fonts->AddFontDefault(&fontConfig);
    io.FontGlobalScale = 1.0f;  // Don't double-scale
    
    DEBUG_COUT("[ImGui DPI] Font size: " << (baseFontSize * g_app.dpiScale) 
              << "px (base " << baseFontSize << " * scale " << g_app.dpiScale << ")" << std::endl);
#endif
    
    ImGui::StyleColorsDark();
    
#ifdef EMSCRIPTEN_BUILD
    ImGui_ImplGlfw_InitForOpenGL(g_app.window, true);
    ImGui_ImplOpenGL3_Init("#version 300 es");
#else
    ImGui_ImplGlfw_InitForOpenGL(g_app.window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
#endif
    
    // Initialize shaders
    if (!initShaders()) {
        return -1;
    }
    
    // Initialize scene
    initScene();
    
    // OpenGL state
    glEnable(GL_DEPTH_TEST);
#ifndef EMSCRIPTEN_BUILD
    glEnable(GL_PROGRAM_POINT_SIZE);  // Allow vertex shader to set gl_PointSize
#endif
    glViewport(0, 0, g_app.framebufferWidth, g_app.framebufferHeight);
    
    // Main loop
#ifdef EMSCRIPTEN_BUILD
    emscripten_set_main_loop(emscriptenMainLoop, 0, 1);
#else
    while (!glfwWindowShouldClose(g_app.window)) {
        mainLoop();
    }
#endif
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(g_app.window);
    glfwTerminate();
    
    return 0;
}

// ============================================================================
// JavaScript API Functions (called from Bindings.cpp via Embind)
// ============================================================================

#ifdef EMSCRIPTEN_BUILD

// Primitives
void api_addCube() {
    if (!g_app.items) return;
    sceneActionWithUndo("Add Cube", []() { g_app.items->addCube(); });
}

void api_addPlane() {
    if (!g_app.items) return;
    sceneActionWithUndo("Add Plane", []() { g_app.items->addPlane(); });
}

void api_addCylinder(int steps) {
    if (!g_app.items) return;
    int s = steps > 0 ? steps : g_app.meshSteps;
    sceneActionWithUndo("Add Cylinder", [s]() { 
        g_app.items->addCylinder(static_cast<uint32_t>(s)); 
    });
}

void api_addSphere(int steps) {
    if (!g_app.items) return;
    int s = steps > 0 ? steps : g_app.meshSteps;
    sceneActionWithUndo("Add Sphere", [s]() { 
        g_app.items->addSphere(static_cast<uint32_t>(s)); 
    });
}

void api_addIcosahedron() {
    if (!g_app.items) return;
    sceneActionWithUndo("Add Icosahedron", []() { g_app.items->addIcosahedron(); });
}

// Edit mode
int api_getEditMode() {
    if (!g_app.items) return 0;
    return static_cast<int>(g_app.items->getEditMode());
}

void api_setEditMode(int mode) {
    if (!g_app.items) return;
    if (mode < 0 || mode > 3) return;
    g_app.items->setEditMode(static_cast<EditMode>(mode));
}

// Transform mode
int api_getTransformMode() {
    return static_cast<int>(g_app.transformMode);
}

void api_setTransformMode(int mode) {
    if (mode < 0 || mode > 3) return;
    g_app.transformMode = static_cast<TransformMode>(mode);
}

// UV View Mode
bool api_getUVViewMode() {
    return g_app.isUVViewMode;
}

void api_setUVViewMode(bool enabled) {
    g_app.isUVViewMode = enabled;
}

float api_getUVZoom() {
    return g_app.uvZoom;
}

void api_setUVZoom(float zoom) {
    g_app.uvZoom = std::max(0.1f, std::min(10.0f, zoom));
}

emscripten::val api_getUVOffset() {
    return emscripten::val::array(std::vector<float>{g_app.uvOffset.x, g_app.uvOffset.y});
}

void api_setUVOffset(float x, float y) {
    g_app.uvOffset = glm::vec2(x, y);
}

void api_renderUV(int width, int height) {
    // Render UV view to the same framebuffer
    glViewport(0, 0, width, height);
    renderUV();
}

// Split View Mode
bool api_getSplitViewEnabled() {
    return g_app.splitViewEnabled;
}

void api_setSplitViewEnabled(bool enabled) {
    g_app.splitViewEnabled = enabled;
}

float api_getSplitRatio() {
    return g_app.splitRatio;
}

void api_setSplitRatio(float ratio) {
    g_app.splitRatio = std::max(0.2f, std::min(0.8f, ratio));
}

int api_getActiveViewport() {
    return g_app.activeViewport;
}

void api_setActiveViewport(int viewport) {
    if (viewport == 0 || viewport == 1) {
        g_app.activeViewport = viewport;
    }
}

// Get which viewport contains the given mouse position (0 = 3D, 1 = UV, -1 = divider)
int api_getViewportAtPosition(float x, float y) {
    if (!g_app.splitViewEnabled) {
        return 0;  // Only 3D viewport
    }
    
    float splitX = g_app.framebufferWidth * g_app.splitRatio;
    float dividerWidth = 4.0f;  // Divider click area
    
    if (x < splitX - dividerWidth / 2) {
        return 0;  // 3D viewport
    } else if (x > splitX + dividerWidth / 2) {
        return 1;  // UV viewport
    } else {
        return -1;  // On the divider
    }
}

// Selection
int api_getSelectionCount() {
    if (!g_app.items) return 0;
    if (g_app.items->getEditMode() == EditMode::Items) {
        return static_cast<int>(g_app.items->getSelectedItemCount());
    } else {
        return static_cast<int>(g_app.items->getSelectedComponentCount());
    }
}

void api_selectAll() {
    selectAll();
}

void api_deselectAll() {
    deselectAll();
}

void api_deleteSelection() {
    if (!g_app.items) return;
    if (g_app.items->getEditMode() == EditMode::Items) {
        sceneActionWithUndo("Delete Items", []() {
            g_app.items->deleteSelectedItems();
        });
    } else {
        sceneActionWithUndo("Delete Faces", []() {
            g_app.items->deleteSelectedFaces();
        });
    }
}

void api_duplicateSelection() {
    if (!g_app.items) return;
    if (g_app.items->getEditMode() == EditMode::Items) {
        sceneActionWithUndo("Duplicate Items", []() {
            g_app.items->duplicateSelectedItems();
        });
    } else {
        sceneActionWithUndo("Duplicate Faces", []() {
            g_app.items->duplicateSelectedFaces();
        });
    }
}

// Undo/Redo
bool api_canUndo() {
    return g_app.undoManager.canUndo();
}

bool api_canRedo() {
    return g_app.undoManager.canRedo();
}

void api_undo() {
    g_app.undoManager.undo();
}

void api_redo() {
    g_app.undoManager.redo();
}

// Mesh operations
void api_flipSelectedFaces() {
    if (!g_app.items || g_app.items->getEditMode() == EditMode::Items) return;
    sceneActionWithUndo("Flip Faces", []() { g_app.items->flipSelectedFaces(); });
}

void api_subdivideSelectedFaces() {
    if (!g_app.items) return;
    DEBUG_LOG("[Subdivide] Calling catmullClarkSubdivide(1), editMode=%d\n", 
              static_cast<int>(g_app.items->getEditMode()));
    sceneActionWithUndo("Subdivide", []() { g_app.items->catmullClarkSubdivide(1); });
}

void api_splitSelected() {
    if (!g_app.items || g_app.items->getEditMode() == EditMode::Items) return;
    sceneActionWithUndo("Split", []() { g_app.items->splitSelected(); });
}

void api_triangulateSelectedFaces() {
    if (!g_app.items || g_app.items->getEditMode() == EditMode::Items) return;
    sceneActionWithUndo("Triangulate", []() { g_app.items->triangulateSelectedFaces(); });
}

void api_extrudeSelectedFaces() {
    if (!g_app.items || g_app.items->getEditMode() == EditMode::Items) return;
    sceneActionWithUndo("Extrude", []() { g_app.items->extrudeSelectedFaces(); });
}

void api_mergeSelectedVertices() {
    if (!g_app.items || g_app.items->getEditMode() != EditMode::Vertices) return;
    sceneActionWithUndo("Merge Vertices", []() { g_app.items->mergeSelectedVertices(); });
}

// Edge loop/ring selection
void api_selectEdgeLoop() {
    if (!g_app.items || g_app.items->getEditMode() != EditMode::Edges) return;
    g_app.items->selectEdgeLoopFromSelected();
}

void api_selectEdgeRing() {
    if (!g_app.items || g_app.items->getEditMode() != EditMode::Edges) return;
    g_app.items->selectEdgeRingFromSelected();
}

void api_growEdgeSelection() {
    if (!g_app.items || g_app.items->getEditMode() != EditMode::Edges) return;
    g_app.items->growEdgeSelection();
}

// View settings
int api_getViewMode() {
    return g_app.viewMode;
}

void api_setViewMode(int mode) {
    if (mode < 0 || mode > 2) return;
    g_app.viewMode = mode;
}

bool api_getShowGrid() {
    return g_app.showGrid;
}

void api_setShowGrid(bool show) {
    g_app.showGrid = show;
}

bool api_getShowNormals() {
    return g_app.showNormals;
}

void api_setShowNormals(bool show) {
    g_app.showNormals = show;
}

// Info
int api_getItemCount() {
    if (!g_app.items) return 0;
    return static_cast<int>(g_app.items->getItemCount());
}

// ImGui visibility
bool api_getShowImGui() {
    return g_app.showImGui;
}

void api_setShowImGui(bool show) {
    g_app.showImGui = show;
}

// Helper: Get euler angles from quaternion (in degrees)
glm::vec3 quaternionToEulerDegrees(const glm::quat& q) {
    glm::vec3 euler = glm::eulerAngles(q);
    return glm::degrees(euler);
}

// Get the last selected item (for single selection display)
Item* getLastSelectedItem() {
    if (!g_app.items) return nullptr;
    
    if (g_app.items->getEditMode() == EditMode::Items) {
        for (int i = static_cast<int>(g_app.items->getItemCount()) - 1; i >= 0; --i) {
            Item* item = g_app.items->getItemAtIndex(i);
            if (item->selected) return item;
        }
    }
    return nullptr;
}

// Get selection value based on current transform mode and axis (0=X, 1=Y, 2=Z)
float api_getSelectionValue(int axis) {
    if (!g_app.items || getSelectionCount() == 0) return 0.0f;
    if (axis < 0 || axis > 2) return 0.0f;
    
    switch (g_app.transformMode) {
        case TransformMode::Translate:
        case TransformMode::None: {
            glm::vec3 center = getSelectionCenter();
            return center[axis];
        }
        case TransformMode::Rotate: {
            // For rotation, show euler angles of last selected item
            Item* item = getLastSelectedItem();
            if (item) {
                glm::vec3 euler = quaternionToEulerDegrees(item->rotation);
                return euler[axis];
            }
            return 0.0f;
        }
        case TransformMode::Scale: {
            // For scale, show scale of last selected item
            Item* item = getLastSelectedItem();
            if (item) {
                return item->scale[axis];
            }
            return 1.0f;
        }
    }
    return 0.0f;
}

// Set selection value based on current transform mode and axis
void api_setSelectionValue(int axis, float value) {
    if (!g_app.items || getSelectionCount() == 0) return;
    if (axis < 0 || axis > 2) return;
    
    switch (g_app.transformMode) {
        case TransformMode::Translate:
        case TransformMode::None: {
            glm::vec3 currentCenter = getSelectionCenter();
            glm::vec3 offset(0.0f);
            offset[axis] = value - currentCenter[axis];
            
            sceneActionWithUndo("Move Selection", [offset]() {
                translateSelection(offset);
            });
            break;
        }
        case TransformMode::Rotate: {
            // Set absolute rotation for the last selected item
            Item* item = getLastSelectedItem();
            if (item) {
                glm::vec3 currentEuler = quaternionToEulerDegrees(item->rotation);
                float delta = value - currentEuler[axis];
                
                glm::vec3 axisVec(0.0f);
                axisVec[axis] = 1.0f;
                
                sceneActionWithUndo("Rotate Selection", [axisVec, delta]() {
                    rotateSelection(axisVec, glm::radians(delta));
                });
            }
            break;
        }
        case TransformMode::Scale: {
            // Set absolute scale for the last selected item
            Item* item = getLastSelectedItem();
            if (item) {
                glm::vec3 scaleOffset(0.0f);
                scaleOffset[axis] = value - item->scale[axis];
                glm::vec3 center = getSelectionCenter();
                
                sceneActionWithUndo("Scale Selection", [center, scaleOffset]() {
                    scaleSelectionByOffset(center, scaleOffset);
                });
            }
            break;
        }
    }
}

// Convenience wrappers for X, Y, Z
float api_getSelectionX() { return api_getSelectionValue(0); }
float api_getSelectionY() { return api_getSelectionValue(1); }
float api_getSelectionZ() { return api_getSelectionValue(2); }
void api_setSelectionX(float value) { api_setSelectionValue(0, value); }
void api_setSelectionY(float value) { api_setSelectionValue(1, value); }
void api_setSelectionZ(float value) { api_setSelectionValue(2, value); }

// Legacy functions kept for compatibility
float api_getSelectionCenterX() {
    if (!g_app.items || getSelectionCount() == 0) return 0.0f;
    return getSelectionCenter().x;
}

float api_getSelectionCenterY() {
    if (!g_app.items || getSelectionCount() == 0) return 0.0f;
    return getSelectionCenter().y;
}

float api_getSelectionCenterZ() {
    if (!g_app.items || getSelectionCount() == 0) return 0.0f;
    return getSelectionCenter().z;
}

void api_setSelectionPosition(float x, float y, float z) {
    if (!g_app.items || getSelectionCount() == 0) return;
    
    glm::vec3 currentCenter = getSelectionCenter();
    glm::vec3 newPosition(x, y, z);
    glm::vec3 offset = newPosition - currentCenter;
    
    sceneActionWithUndo("Move Selection", [offset]() {
        translateSelection(offset);
    });
}

// Apply rotation to selection (incremental, in degrees)
void api_rotateSelection(float xDegrees, float yDegrees, float zDegrees) {
    if (!g_app.items || getSelectionCount() == 0) return;
    
    sceneActionWithUndo("Rotate Selection", [xDegrees, yDegrees, zDegrees]() {
        if (xDegrees != 0.0f) {
            rotateSelection(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(xDegrees));
        }
        if (yDegrees != 0.0f) {
            rotateSelection(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(yDegrees));
        }
        if (zDegrees != 0.0f) {
            rotateSelection(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(zDegrees));
        }
    });
}

// Apply scale to selection (as offset from current)
void api_scaleSelection(float xOffset, float yOffset, float zOffset) {
    if (!g_app.items || getSelectionCount() == 0) return;
    
    glm::vec3 center = getSelectionCenter();
    glm::vec3 scaleOffset(xOffset, yOffset, zOffset);
    
    sceneActionWithUndo("Scale Selection", [center, scaleOffset]() {
        scaleSelectionByOffset(center, scaleOffset);
    });
}

// Mesh steps for primitives
int api_getMeshSteps() {
    return g_app.meshSteps;
}

void api_setMeshSteps(int steps) {
    if (steps < 3) steps = 3;
    if (steps > 100) steps = 100;
    g_app.meshSteps = steps;
}

// ============================================================================
// Serialization API
// ============================================================================

#include "Serialization.h"
#include <emscripten/val.h>

std::string api_exportToOBJ() {
    if (!g_app.items) return "";
    return Serialization::exportToOBJ(*g_app.items);
}

bool api_importFromOBJ(const std::string& objData) {
    if (!g_app.items) return false;
    return Serialization::importFromOBJ(*g_app.items, objData);
}

emscripten::val api_exportToGLB() {
    if (!g_app.items) {
        return emscripten::val::null();
    }
    
    std::vector<uint8_t> glbData = Serialization::exportToGLB(*g_app.items);
    if (glbData.empty()) {
        return emscripten::val::null();
    }
    
    // Convert to JavaScript Uint8Array
    return emscripten::val(emscripten::typed_memory_view(glbData.size(), glbData.data())).call<emscripten::val>("slice");
}

bool api_importFromGLB(const std::string& glbDataStr) {
    if (!g_app.items) return false;
    
    // Convert string to binary data
    std::vector<uint8_t> glbData(glbDataStr.begin(), glbDataStr.end());
    return Serialization::importFromGLB(*g_app.items, glbData);
}

// Import from binary Uint8Array (for GLB)
bool api_importFromGLBArray(emscripten::val data) {
    if (!g_app.items) return false;
    
    // Get array length
    unsigned int length = data["length"].as<unsigned int>();
    if (length == 0) return false;
    
    // Allocate buffer
    std::vector<uint8_t> glbData(length);
    
    // Fast bulk copy: create a typed view into the WASM heap at our buffer, then set from JS array
    emscripten::val destView = emscripten::val(emscripten::typed_memory_view(length, glbData.data()));
    destView.call<void>("set", data);
    
    return Serialization::importFromGLB(*g_app.items, glbData);
}

// Clear all items
void api_clearScene() {
    if (!g_app.items) return;
    
    // Remove all items by removing from end
    while (g_app.items->getItemCount() > 0) {
        g_app.items->removeItemAtIndex(g_app.items->getItemCount() - 1);
    }
}

// Get item count
int api_getItemCount_export() {
    if (!g_app.items) return 0;
    return static_cast<int>(g_app.items->getItemCount());
}

// Load texture from image data (RGBA format)
bool api_loadTextureFromRGBA(emscripten::val data, int width, int height) {
    if (!g_app.items) return false;
    
    // Get selected items
    std::vector<Item*> selectedItems;
    for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
        Item* item = g_app.items->getItemAtIndex(i);
        if (item && item->selected) {
            selectedItems.push_back(item);
        }
    }
    
    if (selectedItems.empty()) return false;
    
    // Convert JavaScript Uint8Array to C++ vector
    std::vector<uint8_t> pixels;
    unsigned int length = data["length"].as<unsigned int>();
    pixels.resize(length);
    for (unsigned int i = 0; i < length; i++) {
        pixels[i] = data[i].as<uint8_t>();
    }
    
    // Create texture
    auto texture = TextureManager::instance().createFromRGBA(
        "loaded_texture", pixels.data(), width, height
    );
    
    if (!texture) return false;
    
    // Apply texture to all selected items
    for (Item* item : selectedItems) {
        item->setTexture(texture);
    }
    
    return true;
}

// Load texture from file data (PNG, JPG, etc.)
bool api_loadTextureFromFileData(emscripten::val data) {
    if (!g_app.items) return false;
    
    // Get selected items
    std::vector<Item*> selectedItems;
    for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
        Item* item = g_app.items->getItemAtIndex(i);
        if (item && item->selected) {
            selectedItems.push_back(item);
        }
    }
    
    if (selectedItems.empty()) return false;
    
    // Convert JavaScript Uint8Array to C++ vector
    std::vector<uint8_t> fileData;
    unsigned int length = data["length"].as<unsigned int>();
    fileData.resize(length);
    for (unsigned int i = 0; i < length; i++) {
        fileData[i] = data[i].as<uint8_t>();
    }
    
    // Create texture from file data
    auto texture = TextureManager::instance().createFromFileData(
        "loaded_texture", fileData.data(), static_cast<int>(fileData.size())
    );
    
    if (!texture) return false;
    
    // Apply texture to all selected items
    for (Item* item : selectedItems) {
        item->setTexture(texture);
    }
    
    return true;
}

// Remove texture from selected items
void api_removeTexture() {
    if (!g_app.items) return;
    
    for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
        Item* item = g_app.items->getItemAtIndex(i);
        if (item && item->selected) {
            item->setTexture(nullptr);
        }
    }
}

// Check if selection has texture
bool api_selectionHasTexture() {
    if (!g_app.items) return false;
    
    for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
        Item* item = g_app.items->getItemAtIndex(i);
        if (item && item->selected && item->hasTexture()) {
            return true;
        }
    }
    return false;
}

// ============================================================================
// UV Mapping API
// ============================================================================

// UV unwrap projection types (matches Mesh2::UVProjection enum)
// 0=Box, 1=Planar, 2=Cylindrical, 3=Spherical
void api_unwrapSelectedUVs(int projectionType) {
    if (!g_app.items) return;
    
    Mesh2::UVProjection projection = static_cast<Mesh2::UVProjection>(projectionType);
    
    sceneActionWithUndo("UV Unwrap", [projection]() {
        for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
            Item* item = g_app.items->getItemAtIndex(i);
            if (item && item->selected && item->mesh) {
                item->mesh->unwrapSelectedUVs(projection);
                item->mesh->updateGPUBuffers();  // Immediate update
            }
        }
    });
}

void api_unwrapAllUVs(int projectionType) {
    if (!g_app.items) return;
    
    Mesh2::UVProjection projection = static_cast<Mesh2::UVProjection>(projectionType);
    
    sceneActionWithUndo("UV Unwrap All", [projection]() {
        for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
            Item* item = g_app.items->getItemAtIndex(i);
            if (item && item->selected && item->mesh) {
                item->mesh->unwrapAllUVs(projection);
                item->mesh->updateGPUBuffers();  // Immediate update
            }
        }
    });
}

// Mark selected edges as seams
void api_markSelectedEdgesAsSeam(bool isSeam) {
    if (!g_app.items) return;
    
    sceneActionWithUndo(isSeam ? "Mark Seams" : "Clear Seams", [isSeam]() {
        for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
            Item* item = g_app.items->getItemAtIndex(i);
            if (item && item->selected && item->mesh) {
                item->mesh->markSelectedEdgesAsSeam(isSeam);
            }
        }
    });
}

// Clear all seams
void api_clearAllSeams() {
    if (!g_app.items) return;
    
    sceneActionWithUndo("Clear All Seams", []() {
        for (size_t i = 0; i < g_app.items->getItemCount(); i++) {
            Item* item = g_app.items->getItemAtIndex(i);
            if (item && item->selected && item->mesh) {
                item->mesh->clearAllSeams();
            }
        }
    });
}

#endif // EMSCRIPTEN_BUILD
