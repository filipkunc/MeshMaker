#include "Mesh.h"

#ifdef EMSCRIPTEN_BUILD
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#endif

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Mesh::Mesh()
    : m_color(0.7f, 0.7f, 0.7f)
    , m_wireframeColor(0.1f, 0.1f, 0.1f)
    , m_vao(0)
    , m_vbo(0)
    , m_edgeVao(0)
    , m_edgeVbo(0)
    , m_gpuBuffersCreated(false)
{
}

Mesh::~Mesh() {
    deleteGPUBuffers();
}

void Mesh::clear() {
    m_vertices.clear();
    m_edgeVertices.clear();
}

void Mesh::addTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& color) {
    // Calculate face normal
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    
    m_vertices.emplace_back(v0, normal, color);
    m_vertices.emplace_back(v1, normal, color);
    m_vertices.emplace_back(v2, normal, color);
}

void Mesh::addQuad(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& color) {
    // Split quad into two triangles
    addTriangle(v0, v1, v2, color);
    addTriangle(v0, v2, v3, color);
}

void Mesh::setColor(const glm::vec3& color) {
    m_color = color;
    for (auto& vertex : m_vertices) {
        vertex.color = color;
    }
}

void Mesh::setWireframeColor(const glm::vec3& color) {
    m_wireframeColor = color;
    for (auto& vertex : m_edgeVertices) {
        vertex.color = color;
    }
}

void Mesh::buildEdges() {
    m_edgeVertices.clear();
    
    // Extract edges from triangles (3 edges per triangle)
    for (size_t i = 0; i < m_vertices.size(); i += 3) {
        const glm::vec3& v0 = m_vertices[i].position;
        const glm::vec3& v1 = m_vertices[i + 1].position;
        const glm::vec3& v2 = m_vertices[i + 2].position;
        
        // Edge 0-1
        m_edgeVertices.emplace_back(v0, m_wireframeColor);
        m_edgeVertices.emplace_back(v1, m_wireframeColor);
        
        // Edge 1-2
        m_edgeVertices.emplace_back(v1, m_wireframeColor);
        m_edgeVertices.emplace_back(v2, m_wireframeColor);
        
        // Edge 2-0
        m_edgeVertices.emplace_back(v2, m_wireframeColor);
        m_edgeVertices.emplace_back(v0, m_wireframeColor);
    }
}

void Mesh::makePlane() {
    clear();
    
    glm::vec3 v0(-1.0f, 0.0f, -1.0f);
    glm::vec3 v1(-1.0f, 0.0f,  1.0f);
    glm::vec3 v2( 1.0f, 0.0f,  1.0f);
    glm::vec3 v3( 1.0f, 0.0f, -1.0f);
    
    addQuad(v0, v1, v2, v3, m_color);
}

void Mesh::makeCube() {
    clear();
    
    // Back vertices
    glm::vec3 v0(-1.0f, -1.0f, -1.0f);
    glm::vec3 v1( 1.0f, -1.0f, -1.0f);
    glm::vec3 v2( 1.0f,  1.0f, -1.0f);
    glm::vec3 v3(-1.0f,  1.0f, -1.0f);
    
    // Front vertices
    glm::vec3 v4(-1.0f, -1.0f,  1.0f);
    glm::vec3 v5( 1.0f, -1.0f,  1.0f);
    glm::vec3 v6( 1.0f,  1.0f,  1.0f);
    glm::vec3 v7(-1.0f,  1.0f,  1.0f);
    
    // Back face
    addQuad(v0, v1, v2, v3, m_color);
    
    // Front face
    addQuad(v7, v6, v5, v4, m_color);
    
    // Bottom face
    addQuad(v1, v0, v4, v5, m_color);
    
    // Top face
    addQuad(v3, v2, v6, v7, m_color);
    
    // Left face
    addQuad(v7, v4, v0, v3, m_color);
    
    // Right face
    addQuad(v2, v1, v5, v6, m_color);
}

void Mesh::makeCylinder(uint32_t steps) {
    clear();
    
    glm::vec3 bottom(0.0f, -1.0f, 0.0f);
    glm::vec3 top(0.0f, 1.0f, 0.0f);
    
    float step = static_cast<float>(2.0 * M_PI) / steps;
    
    for (uint32_t i = 0; i < steps; i++) {
        float angle1 = i * step;
        float angle2 = (i + 1) * step;
        
        glm::vec3 b1(cos(angle1), -1.0f, sin(angle1));
        glm::vec3 b2(cos(angle2), -1.0f, sin(angle2));
        glm::vec3 t1(cos(angle1),  1.0f, sin(angle1));
        glm::vec3 t2(cos(angle2),  1.0f, sin(angle2));
        
        // Side quad
        addQuad(b1, t1, t2, b2, m_color);
        
        // Bottom triangle
        addTriangle(bottom, b2, b1, m_color);
        
        // Top triangle
        addTriangle(top, t1, t2, m_color);
    }
}

void Mesh::makeSphere(uint32_t steps) {
    clear();
    
    float stepPhi = static_cast<float>(M_PI) / steps;
    float stepTheta = static_cast<float>(2.0 * M_PI) / steps;
    
    for (uint32_t i = 0; i < steps; i++) {
        float phi1 = i * stepPhi;
        float phi2 = (i + 1) * stepPhi;
        
        for (uint32_t j = 0; j < steps; j++) {
            float theta1 = j * stepTheta;
            float theta2 = (j + 1) * stepTheta;
            
            // Spherical to Cartesian
            glm::vec3 v0(sin(phi1) * cos(theta1), cos(phi1), sin(phi1) * sin(theta1));
            glm::vec3 v1(sin(phi2) * cos(theta1), cos(phi2), sin(phi2) * sin(theta1));
            glm::vec3 v2(sin(phi2) * cos(theta2), cos(phi2), sin(phi2) * sin(theta2));
            glm::vec3 v3(sin(phi1) * cos(theta2), cos(phi1), sin(phi1) * sin(theta2));
            
            if (i == 0) {
                // Top cap - triangle
                addTriangle(v0, v1, v2, m_color);
            } else if (i == steps - 1) {
                // Bottom cap - triangle
                addTriangle(v0, v1, v3, m_color);
            } else {
                // Regular quad
                addQuad(v0, v1, v2, v3, m_color);
            }
        }
    }
}

void Mesh::makeIcosahedron() {
    clear();
    
    const float X = 0.525731112119133606f;
    const float Z = 0.850650808352039932f;
    
    glm::vec3 vdata[12] = {
        {-X, 0.0f, Z}, {X, 0.0f, Z}, {-X, 0.0f, -Z}, {X, 0.0f, -Z},
        {0.0f, Z, X}, {0.0f, Z, -X}, {0.0f, -Z, X}, {0.0f, -Z, -X},
        {Z, X, 0.0f}, {-Z, X, 0.0f}, {Z, -X, 0.0f}, {-Z, -X, 0.0f}
    };
    
    uint32_t tindices[20][3] = {
        {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
        {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
        {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
        {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}
    };
    
    for (uint32_t i = 0; i < 20; i++) {
        addTriangle(
            vdata[tindices[i][0]],
            vdata[tindices[i][1]],
            vdata[tindices[i][2]],
            m_color
        );
    }
}

void Mesh::make(MeshType type, uint32_t steps) {
    switch (type) {
        case MeshType::Plane:
            makePlane();
            break;
        case MeshType::Cube:
            makeCube();
            break;
        case MeshType::Cylinder:
            makeCylinder(steps);
            break;
        case MeshType::Sphere:
            makeSphere(steps);
            break;
        case MeshType::Icosahedron:
            makeIcosahedron();
            break;
    }
    
    // Build edge data for wireframe rendering
    buildEdges();
}

void Mesh::createGPUBuffers() {
    if (m_gpuBuffersCreated) {
        deleteGPUBuffers();
    }
    
    // Build edges if not already built
    if (m_edgeVertices.empty() && !m_vertices.empty()) {
        buildEdges();
    }
    
    // Create solid mesh buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                 m_vertices.size() * sizeof(Vertex), 
                 m_vertices.data(), 
                 GL_STATIC_DRAW);
    
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                          (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    
    // Normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    // Color attribute (location = 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                          (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    // Create wireframe buffers
    glGenVertexArrays(1, &m_edgeVao);
    glGenBuffers(1, &m_edgeVbo);
    
    glBindVertexArray(m_edgeVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_edgeVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 m_edgeVertices.size() * sizeof(LineVertex),
                 m_edgeVertices.data(),
                 GL_STATIC_DRAW);
    
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                          (void*)offsetof(LineVertex, position));
    glEnableVertexAttribArray(0);
    
    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                          (void*)offsetof(LineVertex, color));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    m_gpuBuffersCreated = true;
}

void Mesh::deleteGPUBuffers() {
    if (m_gpuBuffersCreated) {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_edgeVao);
        glDeleteBuffers(1, &m_edgeVbo);
        m_vao = 0;
        m_vbo = 0;
        m_edgeVao = 0;
        m_edgeVbo = 0;
        m_gpuBuffersCreated = false;
    }
}

void Mesh::drawSolid() const {
    if (!m_gpuBuffersCreated || m_vertices.empty()) {
        return;
    }
    
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertices.size()));
    glBindVertexArray(0);
}

void Mesh::drawWireframe() const {
    if (!m_gpuBuffersCreated || m_edgeVertices.empty()) {
        return;
    }
    
    glBindVertexArray(m_edgeVao);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_edgeVertices.size()));
    glBindVertexArray(0);
}

void Mesh::draw(ViewMode mode) const {
    switch (mode) {
        case ViewMode::Solid:
            drawSolid();
            break;
        case ViewMode::Wireframe:
            drawWireframe();
            break;
        case ViewMode::SolidWireframe:
            drawSolid();
            // Wireframe is drawn separately with different shader
            break;
    }
}
