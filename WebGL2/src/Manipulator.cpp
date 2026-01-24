//
//  Manipulator.cpp
//  MeshMaker WebGL2 Port
//
//  Ported from original MeshMaker by Filip Kunc
//

#include "Manipulator.h"
#include "Shader.h"

#ifdef EMSCRIPTEN_BUILD
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#endif

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ManipulatorWidget implementation

glm::vec4 ManipulatorWidget::getColor(bool isSelected, bool isGray) const {
    float alpha = (widget == Widget::Plane) ? 0.5f : 0.8f;
    
    if (isSelected) {
        return glm::vec4(1.0f, 1.0f, 0.0f, alpha);  // Yellow for selected
    }
    
    if (isGray) {
        return glm::vec4(0.3f, 0.3f, 0.3f, 0.8f);
    }
    
    switch (axis) {
        case Axis::X:
            return glm::vec4(1.0f, 0.0f, 0.0f, alpha);  // Red
        case Axis::Y:
            return glm::vec4(0.0f, 1.0f, 0.0f, alpha);  // Green
        case Axis::Z:
            return glm::vec4(0.0f, 0.0f, 1.0f, alpha);  // Blue
        case Axis::Center:
            return glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);   // White
        default:
            return glm::vec4(1.0f, 1.0f, 1.0f, alpha);
    }
}

glm::mat4 ManipulatorWidget::getAxisTransform() const {
    float angle = (widget == Widget::Plane) ? -90.0f : 90.0f;
    
    switch (axis) {
        case Axis::X:
            return glm::rotate(glm::mat4(1.0f), glm::radians(-angle), glm::vec3(0, 0, 1));
        case Axis::Y:
            return glm::mat4(1.0f);  // Y is default orientation
        case Axis::Z:
            return glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1, 0, 0));
        case Axis::Center:
        default:
            return glm::mat4(1.0f);
    }
}

// Manipulator implementation

Manipulator::Manipulator() 
    : position(0.0f)
    , rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    , size(1.0f)
    , selectedIndex(UINT32_MAX)
{
}

Manipulator::Manipulator(ManipulatorType type)
    : position(0.0f)
    , rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    , size(1.0f)
    , selectedIndex(UINT32_MAX)
{
    switch (type) {
        case ManipulatorType::Default:
            addWidgetWithAxis(Widget::Line, Axis::X);
            addWidgetWithAxis(Widget::Line, Axis::Y);
            addWidgetWithAxis(Widget::Line, Axis::Z);
            break;
            
        case ManipulatorType::Translation:
            addWidgetWithAxis(Widget::Arrow, Axis::X);
            addWidgetWithAxis(Widget::Arrow, Axis::Y);
            addWidgetWithAxis(Widget::Arrow, Axis::Z);
            addWidgetWithAxis(Widget::Plane, Axis::X);
            addWidgetWithAxis(Widget::Plane, Axis::Y);
            addWidgetWithAxis(Widget::Plane, Axis::Z);
            break;
            
        case ManipulatorType::Rotation:
            addWidgetWithAxis(Widget::Circle, Axis::X);
            addWidgetWithAxis(Widget::Circle, Axis::Y);
            addWidgetWithAxis(Widget::Circle, Axis::Z);
            break;
            
        case ManipulatorType::Scale:
            addWidgetWithAxis(Widget::Cube, Axis::Center);
            addWidgetWithAxis(Widget::Cube, Axis::X);
            addWidgetWithAxis(Widget::Cube, Axis::Y);
            addWidgetWithAxis(Widget::Cube, Axis::Z);
            break;
            
        default:
            break;
    }
}

Manipulator::~Manipulator() {
    cleanupGPUResources();
}

void Manipulator::addWidget(const ManipulatorWidget& widget) {
    m_widgets.push_back(widget);
}

void Manipulator::addWidgetWithAxis(Widget widget, Axis axis) {
    m_widgets.push_back(ManipulatorWidget(widget, axis));
}

ManipulatorWidget& Manipulator::widgetAtIndex(uint32_t index) {
    return m_widgets.at(index);
}

void Manipulator::initGPUResources() {
    if (m_gpuResourcesInitialized) return;
    
    createArrowGeometry();
    createCircleGeometry();
    createPlaneGeometry();
    createCubeGeometry();
    createLineGeometry();
    
    m_gpuResourcesInitialized = true;
}

void Manipulator::cleanupGPUResources() {
    if (!m_gpuResourcesInitialized) return;
    
    if (m_arrowVao) { glDeleteVertexArrays(1, &m_arrowVao); m_arrowVao = 0; }
    if (m_arrowVbo) { glDeleteBuffers(1, &m_arrowVbo); m_arrowVbo = 0; }
    
    if (m_circleVao) { glDeleteVertexArrays(1, &m_circleVao); m_circleVao = 0; }
    if (m_circleVbo) { glDeleteBuffers(1, &m_circleVbo); m_circleVbo = 0; }
    
    if (m_planeVao) { glDeleteVertexArrays(1, &m_planeVao); m_planeVao = 0; }
    if (m_planeVbo) { glDeleteBuffers(1, &m_planeVbo); m_planeVbo = 0; }
    
    if (m_cubeVao) { glDeleteVertexArrays(1, &m_cubeVao); m_cubeVao = 0; }
    if (m_cubeVbo) { glDeleteBuffers(1, &m_cubeVbo); m_cubeVbo = 0; }
    if (m_cubeIbo) { glDeleteBuffers(1, &m_cubeIbo); m_cubeIbo = 0; }
    
    if (m_lineVao) { glDeleteVertexArrays(1, &m_lineVao); m_lineVao = 0; }
    if (m_lineVbo) { glDeleteBuffers(1, &m_lineVbo); m_lineVbo = 0; }
    
    m_gpuResourcesInitialized = false;
}

void Manipulator::createArrowGeometry() {
    // Arrow: line from origin to size, then cone at the end
    // Line part
    std::vector<float> vertices;
    
    // Line from 0 to size along Y axis
    vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
    vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
    
    // Cone at the end (simple triangle fan approximation)
    float coneWidth = 0.05f;
    float coneHeight = 0.15f;
    float coneBase = 1.0f;
    
    // Cone tip
    float tipY = coneBase + coneHeight;
    
    // Create cone triangles
    int segments = 16;
    for (int i = 0; i < segments; i++) {
        float angle1 = (2.0f * static_cast<float>(M_PI) * i) / segments;
        float angle2 = (2.0f * static_cast<float>(M_PI) * (i + 1)) / segments;
        
        float x1 = cosf(angle1) * coneWidth;
        float z1 = sinf(angle1) * coneWidth;
        float x2 = cosf(angle2) * coneWidth;
        float z2 = sinf(angle2) * coneWidth;
        
        // Triangle: tip, base1, base2
        vertices.push_back(0.0f); vertices.push_back(tipY); vertices.push_back(0.0f);
        vertices.push_back(x1); vertices.push_back(coneBase); vertices.push_back(z1);
        vertices.push_back(x2); vertices.push_back(coneBase); vertices.push_back(z2);
    }
    
    m_arrowVertexCount = static_cast<uint32_t>(vertices.size() / 3);
    
    glGenVertexArrays(1, &m_arrowVao);
    glGenBuffers(1, &m_arrowVbo);
    
    glBindVertexArray(m_arrowVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_arrowVbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void Manipulator::createCircleGeometry() {
    std::vector<float> vertices;
    
    float radius = 0.7f;  // Match original: size * 0.7f
    int segments = 64;
    
    for (int i = 0; i <= segments; i++) {
        float angle = (2.0f * static_cast<float>(M_PI) * i) / segments;
        float x = sinf(angle) * radius;
        float z = cosf(angle) * radius;
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
    }
    
    m_circleVertexCount = static_cast<uint32_t>(vertices.size() / 3);
    
    glGenVertexArrays(1, &m_circleVao);
    glGenBuffers(1, &m_circleVbo);
    
    glBindVertexArray(m_circleVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_circleVbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void Manipulator::createPlaneGeometry() {
    // Small plane for translation on two axes
    float planeSize = 0.3f;
    float offset = 0.2f;
    
    float vertices[] = {
        offset, 0.0f, offset,
        planeSize + offset, 0.0f, offset,
        planeSize + offset, 0.0f, planeSize + offset,
        offset, 0.0f, offset,
        planeSize + offset, 0.0f, planeSize + offset,
        offset, 0.0f, planeSize + offset
    };
    
    glGenVertexArrays(1, &m_planeVao);
    glGenBuffers(1, &m_planeVbo);
    
    glBindVertexArray(m_planeVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_planeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void Manipulator::createCubeGeometry() {
    float halfSize = 0.04f;  // Small cube
    
    float vertices[] = {
        // Front face
        -halfSize, -halfSize,  halfSize,
         halfSize, -halfSize,  halfSize,
         halfSize,  halfSize,  halfSize,
        -halfSize,  halfSize,  halfSize,
        // Back face
        -halfSize, -halfSize, -halfSize,
        -halfSize,  halfSize, -halfSize,
         halfSize,  halfSize, -halfSize,
         halfSize, -halfSize, -halfSize
    };
    
    uint32_t indices[] = {
        0, 1, 2,  2, 3, 0,  // Front
        4, 5, 6,  6, 7, 4,  // Back
        3, 2, 6,  6, 5, 3,  // Top
        0, 4, 7,  7, 1, 0,  // Bottom
        0, 3, 5,  5, 4, 0,  // Left
        1, 7, 6,  6, 2, 1   // Right
    };
    
    glGenVertexArrays(1, &m_cubeVao);
    glGenBuffers(1, &m_cubeVbo);
    glGenBuffers(1, &m_cubeIbo);
    
    glBindVertexArray(m_cubeVao);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void Manipulator::createLineGeometry() {
    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    
    glGenVertexArrays(1, &m_lineVao);
    glGenBuffers(1, &m_lineVbo);
    
    glBindVertexArray(m_lineVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void Manipulator::drawWidget(Shader& shader, const ManipulatorWidget& widget, bool isSelected, bool isGray) {
    glm::vec4 color = widget.getColor(isSelected, isGray);
    glm::mat4 axisTransform = widget.getAxisTransform();
    
    shader.setVec4("uColor", color);
    shader.setMat4("uAxisTransform", axisTransform);
    
    switch (widget.widget) {
        case Widget::Line:
            glBindVertexArray(m_lineVao);
            glLineWidth(1.5f);
            glDrawArrays(GL_LINES, 0, 2);
            glLineWidth(1.0f);
            break;
            
        case Widget::Arrow:
            glBindVertexArray(m_arrowVao);
            // Draw line part
            glLineWidth(1.5f);
            glDrawArrays(GL_LINES, 0, 2);
            glLineWidth(1.0f);
            // Draw cone part
            glDrawArrays(GL_TRIANGLES, 2, m_arrowVertexCount - 2);
            break;
            
        case Widget::Circle:
            glBindVertexArray(m_circleVao);
            glLineWidth(2.0f);
            glDrawArrays(GL_LINE_LOOP, 0, m_circleVertexCount);
            glLineWidth(1.0f);
            break;
            
        case Widget::Plane:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindVertexArray(m_planeVao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glDisable(GL_BLEND);
            break;
            
        case Widget::Cube:
            if (widget.axis == Axis::Center) {
                // Draw cube at center
                shader.setMat4("uAxisTransform", glm::mat4(1.0f));
                glBindVertexArray(m_cubeVao);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            } else {
                // Draw cube at end of axis line, plus the line
                glBindVertexArray(m_lineVao);
                glLineWidth(1.5f);
                glDrawArrays(GL_LINES, 0, 2);
                glLineWidth(1.0f);
                
                // Translate cube to end of axis
                glm::mat4 cubeTransform = axisTransform * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                shader.setMat4("uAxisTransform", cubeTransform);
                glBindVertexArray(m_cubeVao);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
            break;
    }
    
    glBindVertexArray(0);
}

void Manipulator::draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection,
                       const glm::vec3& /*axisZ*/, const glm::vec3& /*center*/, bool highlightAll) {
    if (m_widgets.empty()) return;
    if (!m_gpuResourcesInitialized) initGPUResources();
    
    shader.use();
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);
    
    // Build model matrix from position, rotation, and size
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = model * glm::mat4_cast(rotation);
    model = glm::scale(model, glm::vec3(size));
    shader.setMat4("uModel", model);
    
    // Disable depth test so gizmo is always visible
    glDisable(GL_DEPTH_TEST);
    
    for (uint32_t i = 0; i < m_widgets.size(); i++) {
        bool isSelected = highlightAll || (i == selectedIndex);
        drawWidget(shader, m_widgets[i], isSelected, false);
    }
    
    glEnable(GL_DEPTH_TEST);
}

uint32_t Manipulator::selectableCount() const {
    return static_cast<uint32_t>(m_widgets.size());
}

void Manipulator::drawForSelection(Shader& shader, const glm::mat4& view, const glm::mat4& projection, uint32_t index) {
    if (index >= m_widgets.size()) return;
    if (!m_gpuResourcesInitialized) initGPUResources();
    
    ManipulatorWidget& widget = m_widgets[index];
    
    // Skip lines - they're too thin to click
    if (widget.widget == Widget::Line) return;
    
    shader.use();
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);
    
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = model * glm::mat4_cast(rotation);
    model = glm::scale(model, glm::vec3(size));
    shader.setMat4("uModel", model);
    shader.setMat4("uAxisTransform", widget.getAxisTransform());
    
    // Color encoding for selection (will be set by caller via uColorIndex)
    
    switch (widget.widget) {
        case Widget::Arrow:
            glBindVertexArray(m_arrowVao);
            // Draw thicker line for the shaft to make it easier to select
            glLineWidth(8.0f);
            glDrawArrays(GL_LINES, 0, 2);
            glLineWidth(1.0f);
            // Draw cone
            glDrawArrays(GL_TRIANGLES, 2, m_arrowVertexCount - 2);
            break;
            
        case Widget::Circle:
            // Draw thicker line for selection
            glBindVertexArray(m_circleVao);
            glLineWidth(8.0f);
            glDrawArrays(GL_LINE_LOOP, 0, m_circleVertexCount);
            glLineWidth(1.0f);
            break;
            
        case Widget::Plane:
            glBindVertexArray(m_planeVao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            break;
            
        case Widget::Cube:
            if (widget.axis == Axis::Center) {
                shader.setMat4("uAxisTransform", glm::mat4(1.0f));
                glBindVertexArray(m_cubeVao);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            } else {
                // Draw thicker line for the shaft to make it easier to select
                glBindVertexArray(m_lineVao);
                glLineWidth(8.0f);
                glDrawArrays(GL_LINES, 0, 2);
                glLineWidth(1.0f);
                
                // Draw cube at end of axis
                glm::mat4 cubeTransform = widget.getAxisTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                shader.setMat4("uAxisTransform", cubeTransform);
                glBindVertexArray(m_cubeVao);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
            break;
            
        default:
            break;
    }
    
    glBindVertexArray(0);
}

void Manipulator::selectAtIndex(uint32_t index) {
    selectedIndex = index;
}

glm::vec3 Manipulator::getSelectedAxisDirection() const {
    if (selectedIndex >= m_widgets.size()) {
        return glm::vec3(0.0f);
    }
    
    const ManipulatorWidget& widget = m_widgets[selectedIndex];
    glm::vec3 axis(0.0f);
    
    switch (widget.axis) {
        case Axis::X: axis = glm::vec3(1.0f, 0.0f, 0.0f); break;
        case Axis::Y: axis = glm::vec3(0.0f, 1.0f, 0.0f); break;
        case Axis::Z: axis = glm::vec3(0.0f, 0.0f, 1.0f); break;
        case Axis::Center: axis = glm::vec3(1.0f, 1.0f, 1.0f); break;
    }
    
    // Transform by manipulator rotation
    return glm::mat3_cast(rotation) * axis;
}

Axis Manipulator::getSelectedAxis() const {
    if (selectedIndex >= m_widgets.size()) {
        return Axis::Center;
    }
    return m_widgets[selectedIndex].axis;
}

Widget Manipulator::getSelectedWidget() const {
    if (selectedIndex >= m_widgets.size()) {
        return Widget::Line;  // Default
    }
    return m_widgets[selectedIndex].widget;
}
