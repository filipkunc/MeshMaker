#include "Grid.h"

#ifdef EMSCRIPTEN_BUILD
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

Grid::Grid(int size, int divisions)
    : m_size(size)
    , m_divisions(divisions)
    , m_vao(0)
    , m_vbo(0)
    , m_gpuBuffersCreated(false)
{
    generateGrid();
}

Grid::~Grid() {
    deleteGPUBuffers();
}

void Grid::generateGrid() {
    m_vertices.clear();
    
    float halfSize = static_cast<float>(m_size);
    float step = (2.0f * halfSize) / m_divisions;
    
    glm::vec3 gridColor(0.5f, 0.5f, 0.5f);
    glm::vec3 axisColorX(0.8f, 0.2f, 0.2f);  // Red for X
    glm::vec3 axisColorZ(0.2f, 0.2f, 0.8f);  // Blue for Z
    glm::vec3 centerColor(0.3f, 0.3f, 0.3f);
    
    // Generate grid lines parallel to X axis
    for (int i = 0; i <= m_divisions; i++) {
        float z = -halfSize + i * step;
        glm::vec3 color = (i == m_divisions / 2) ? axisColorX : gridColor;
        
        m_vertices.emplace_back(glm::vec3(-halfSize, 0.0f, z), color);
        m_vertices.emplace_back(glm::vec3( halfSize, 0.0f, z), color);
    }
    
    // Generate grid lines parallel to Z axis
    for (int i = 0; i <= m_divisions; i++) {
        float x = -halfSize + i * step;
        glm::vec3 color = (i == m_divisions / 2) ? axisColorZ : gridColor;
        
        m_vertices.emplace_back(glm::vec3(x, 0.0f, -halfSize), color);
        m_vertices.emplace_back(glm::vec3(x, 0.0f,  halfSize), color);
    }
    
    // Add Y axis indicator (green)
    glm::vec3 axisColorY(0.2f, 0.8f, 0.2f);
    m_vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), axisColorY);
    m_vertices.emplace_back(glm::vec3(0.0f, 2.0f, 0.0f), axisColorY);
}

void Grid::createGPUBuffers() {
    if (m_gpuBuffersCreated) {
        deleteGPUBuffers();
    }
    
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 m_vertices.size() * sizeof(LineVertex),
                 m_vertices.data(),
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

void Grid::deleteGPUBuffers() {
    if (m_gpuBuffersCreated) {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        m_vao = 0;
        m_vbo = 0;
        m_gpuBuffersCreated = false;
    }
}

void Grid::draw() const {
    if (!m_gpuBuffersCreated || m_vertices.empty()) {
        return;
    }
    
    glBindVertexArray(m_vao);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_vertices.size()));
    glBindVertexArray(0);
}
