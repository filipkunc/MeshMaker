#pragma once

#include <glm/glm.hpp>

// GPU-ready vertex structure for mesh rendering
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 uv;
    
    Vertex() : position(0.0f), normal(0.0f), color(1.0f), uv(0.0f) {}
    
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& col)
        : position(pos), normal(norm), color(col), uv(0.0f) {}
    
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& col, const glm::vec2& texCoord)
        : position(pos), normal(norm), color(col), uv(texCoord) {}
};

// Simple vertex for grid/line rendering
struct LineVertex {
    glm::vec3 position;
    glm::vec3 color;
    
    LineVertex() : position(0.0f), color(1.0f) {}
    
    LineVertex(const glm::vec3& pos, const glm::vec3& col)
        : position(pos), color(col) {}
};
