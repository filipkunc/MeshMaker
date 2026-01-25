//
//  Manipulator.h
//  MeshMaker WebGL2 Port
//
//  Ported from original MeshMaker by Filip Kunc
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <cstdint>

// Widget types matching original
enum class Widget {
    Line,
    Arrow,
    Plane,
    Circle,
    Cube
};

// Axis types matching original
enum class Axis {
    Center = -1,
    X = 0,
    Y = 1,
    Z = 2
};

// Manipulator types matching original
enum class ManipulatorType {
    Default = 0,
    Translation = 1,
    Rotation = 2,
    Scale = 3
};

class Shader;

// ManipulatorWidget - represents a single widget (arrow, circle, etc.) on an axis
class ManipulatorWidget {
public:
    Widget widget;
    Axis axis;
    
    ManipulatorWidget(Widget aWidget, Axis anAxis) : widget(aWidget), axis(anAxis) {}
    
    glm::vec4 getColor(bool isSelected, bool isGray) const;
    glm::mat4 getAxisTransform() const;
};

// Manipulator - the complete transform gizmo
class Manipulator {
public:
    glm::vec3 position;
    glm::quat rotation;
    float size;
    uint32_t selectedIndex;
    
    Manipulator();
    explicit Manipulator(ManipulatorType type);
    ~Manipulator();
    
    void addWidget(const ManipulatorWidget& widget);
    void addWidgetWithAxis(Widget widget, Axis axis);
    ManipulatorWidget& widgetAtIndex(uint32_t index);
    uint32_t widgetCount() const { return static_cast<uint32_t>(m_widgets.size()); }
    
    // Initialize GPU resources
    void initGPUResources();
    void cleanupGPUResources();
    
    // Drawing
    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection, 
              const glm::vec3& axisZ, const glm::vec3& center, bool highlightAll = false);
    
    // Check if this is a rotation manipulator
    bool isRotationManipulator() const;
    
    // Selection
    uint32_t selectableCount() const;
    void drawForSelection(Shader& shader, const glm::mat4& view, const glm::mat4& projection, uint32_t index);
    void selectAtIndex(uint32_t index);
    void clearSelection() { selectedIndex = UINT32_MAX; }
    bool hasSelection() const { return selectedIndex != UINT32_MAX; }
    
    // Get the axis direction for the selected widget (in world space)
    glm::vec3 getSelectedAxisDirection() const;
    Axis getSelectedAxis() const;
    Widget getSelectedWidget() const;

private:
    std::vector<ManipulatorWidget> m_widgets;
    
    // GPU resources for drawing primitives
    uint32_t m_arrowVao = 0;
    uint32_t m_arrowVbo = 0;
    uint32_t m_arrowVertexCount = 0;
    
    uint32_t m_circleVao = 0;
    uint32_t m_circleVbo = 0;
    uint32_t m_circleVertexCount = 0;
    
    uint32_t m_sphereVao = 0;
    uint32_t m_sphereVbo = 0;
    uint32_t m_sphereVertexCount = 0;
    
    uint32_t m_planeVao = 0;
    uint32_t m_planeVbo = 0;
    
    uint32_t m_cubeVao = 0;
    uint32_t m_cubeVbo = 0;
    uint32_t m_cubeIbo = 0;
    
    uint32_t m_lineVao = 0;
    uint32_t m_lineVbo = 0;
    
    bool m_gpuResourcesInitialized = false;
    
    void createArrowGeometry();
    void createCircleGeometry();
    void createSphereGeometry();
    void createPlaneGeometry();
    void createCubeGeometry();
    void createLineGeometry();
    
    void drawWidget(Shader& shader, const ManipulatorWidget& widget, bool isSelected, bool isGray);
};
