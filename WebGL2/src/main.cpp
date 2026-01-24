#ifdef EMSCRIPTEN_BUILD
#include <emscripten.h>
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

#include "Camera.h"
#include "Mesh.h"
#include "Grid.h"
#include "Shader.h"

// Global state for main loop
struct AppState {
    GLFWwindow* window = nullptr;
    int windowWidth = 1280;
    int windowHeight = 720;
    
    Camera camera;
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Grid> grid;
    std::unique_ptr<Shader> meshShader;
    std::unique_ptr<Shader> gridShader;
    
    // Mouse state
    bool mousePressed = false;
    bool middleMousePressed = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    
    // Mesh settings
    int currentMeshType = 1;  // Cube
    int meshSteps = 20;
    glm::vec3 meshColor = glm::vec3(0.7f, 0.7f, 0.7f);
    glm::vec3 wireframeColor = glm::vec3(0.1f, 0.1f, 0.1f);
    
    // View settings
    int viewMode = 2;  // SolidWireframe
    bool showGrid = true;
};

static AppState g_app;

// Callbacks
void framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height) {
    g_app.windowWidth = width;
    g_app.windowHeight = height;
    glViewport(0, 0, width, height);
}

void scrollCallback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset) {
    // Don't handle if ImGui wants it
    if (ImGui::GetIO().WantCaptureMouse) return;
    
    g_app.camera.zoom(static_cast<float>(yoffset) * 0.5f);
}

void mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
    // Don't handle if ImGui wants it
    if (ImGui::GetIO().WantCaptureMouse) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        g_app.mousePressed = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        g_app.middleMousePressed = (action == GLFW_PRESS);
    }
}

void cursorPosCallback(GLFWwindow* /*window*/, double xpos, double ypos) {
    // Don't handle if ImGui wants it
    if (ImGui::GetIO().WantCaptureMouse) {
        g_app.lastMouseX = xpos;
        g_app.lastMouseY = ypos;
        return;
    }
    
    double deltaX = xpos - g_app.lastMouseX;
    double deltaY = ypos - g_app.lastMouseY;
    
    if (g_app.mousePressed) {
        // Orbit camera
        g_app.camera.rotateLeftRight(static_cast<float>(deltaX) * 0.01f);
        g_app.camera.rotateUpDown(static_cast<float>(deltaY) * 0.01f);
    }
    
    if (g_app.middleMousePressed) {
        // Pan camera
        g_app.camera.pan(static_cast<float>(-deltaX), static_cast<float>(deltaY));
    }
    
    g_app.lastMouseX = xpos;
    g_app.lastMouseY = ypos;
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
    
    g_app.gridShader = std::make_unique<Shader>();
    if (!g_app.gridShader->loadFromFiles(shaderPath + "grid.vert", shaderPath + "grid.frag")) {
        std::cerr << "Failed to load grid shaders" << std::endl;
        return false;
    }
    
    return true;
}

void initScene() {
    // Create mesh
    g_app.mesh = std::make_unique<Mesh>();
    g_app.mesh->makeCube();
    g_app.mesh->createGPUBuffers();
    
    // Create grid
    g_app.grid = std::make_unique<Grid>(10, 20);
    g_app.grid->createGPUBuffers();
}

void renderImGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Enable docking
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    
    // Settings window
    ImGui::Begin("MeshMaker WebGL2");
    
    ImGui::Text("Camera Controls:");
    ImGui::BulletText("Left Mouse: Orbit");
    ImGui::BulletText("Middle Mouse: Pan");
    ImGui::BulletText("Scroll: Zoom");
    
    if (ImGui::Button("Reset Camera")) {
        g_app.camera.reset();
    }
    
    ImGui::Separator();
    ImGui::Text("Mesh Settings:");
    
    const char* meshTypes[] = { "Plane", "Cube", "Cylinder", "Sphere", "Icosahedron" };
    if (ImGui::Combo("Mesh Type", &g_app.currentMeshType, meshTypes, 5)) {
        g_app.mesh->make(static_cast<MeshType>(g_app.currentMeshType), g_app.meshSteps);
        g_app.mesh->createGPUBuffers();
    }
    
    if (g_app.currentMeshType == 2 || g_app.currentMeshType == 3) {  // Cylinder or Sphere
        if (ImGui::SliderInt("Steps", &g_app.meshSteps, 4, 64)) {
            g_app.mesh->make(static_cast<MeshType>(g_app.currentMeshType), g_app.meshSteps);
            g_app.mesh->createGPUBuffers();
        }
    }
    
    if (ImGui::ColorEdit3("Mesh Color", &g_app.meshColor.x)) {
        g_app.mesh->setColor(g_app.meshColor);
        g_app.mesh->createGPUBuffers();
    }
    
    if (ImGui::ColorEdit3("Wireframe Color", &g_app.wireframeColor.x)) {
        g_app.mesh->setWireframeColor(g_app.wireframeColor);
        g_app.mesh->createGPUBuffers();
    }
    
    ImGui::Separator();
    ImGui::Text("View Settings:");
    
    const char* viewModes[] = { "Solid", "Wireframe", "Solid + Wireframe" };
    ImGui::Combo("View Mode", &g_app.viewMode, viewModes, 3);
    
    ImGui::Checkbox("Show Grid", &g_app.showGrid);
    
    ImGui::Separator();
    ImGui::Text("Stats:");
    ImGui::Text("Vertices: %zu", g_app.mesh->getVertexCount());
    ImGui::Text("Edges: %zu", g_app.mesh->getEdgeVertexCount() / 2);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    
    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void render() {
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float aspectRatio = static_cast<float>(g_app.windowWidth) / static_cast<float>(g_app.windowHeight);
    glm::mat4 view = g_app.camera.getViewMatrix();
    glm::mat4 projection = g_app.camera.getProjectionMatrix(aspectRatio);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));
    
    // Draw grid
    if (g_app.showGrid) {
        glDisable(GL_DEPTH_TEST);
        g_app.gridShader->use();
        g_app.gridShader->setMat4("uView", view);
        g_app.gridShader->setMat4("uProjection", projection);
        g_app.grid->draw();
        glEnable(GL_DEPTH_TEST);
    }
    
    ViewMode viewMode = static_cast<ViewMode>(g_app.viewMode);
    
    // Draw solid mesh
    if (viewMode == ViewMode::Solid || viewMode == ViewMode::SolidWireframe) {
        g_app.meshShader->use();
        g_app.meshShader->setMat4("uModel", model);
        g_app.meshShader->setMat4("uView", view);
        g_app.meshShader->setMat4("uProjection", projection);
        g_app.meshShader->setMat3("uNormalMatrix", normalMatrix);
        g_app.meshShader->setVec3("uLightDir", glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f)));
        g_app.meshShader->setVec3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        g_app.meshShader->setVec3("uAmbientColor", glm::vec3(0.2f, 0.2f, 0.2f));
        g_app.meshShader->setVec3("uViewPos", g_app.camera.getPosition());
        g_app.mesh->drawSolid();
    }
    
    // Draw wireframe
    if (viewMode == ViewMode::Wireframe || viewMode == ViewMode::SolidWireframe) {
        // Use polygon offset to prevent z-fighting when drawing wireframe on top of solid
        if (viewMode == ViewMode::SolidWireframe) {
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.0f, -1.0f);
        }
        
        g_app.gridShader->use();  // Reuse grid shader for simple line rendering
        g_app.gridShader->setMat4("uView", view);
        g_app.gridShader->setMat4("uProjection", projection);
        g_app.mesh->drawWireframe();
        
        if (viewMode == ViewMode::SolidWireframe) {
            glDisable(GL_POLYGON_OFFSET_LINE);
        }
    }
    
    // Draw ImGui
    renderImGui();
}

void mainLoop() {
    glfwPollEvents();
    render();
    glfwSwapBuffers(g_app.window);
}

#ifdef EMSCRIPTEN_BUILD
void emscriptenMainLoop() {
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
    
    // Set callbacks
    glfwSetFramebufferSizeCallback(g_app.window, framebufferSizeCallback);
    glfwSetScrollCallback(g_app.window, scrollCallback);
    glfwSetMouseButtonCallback(g_app.window, mouseButtonCallback);
    glfwSetCursorPosCallback(g_app.window, cursorPosCallback);
    
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
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
    glViewport(0, 0, g_app.windowWidth, g_app.windowHeight);
    
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
