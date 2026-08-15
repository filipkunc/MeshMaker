#pragma once

#include <glm/glm.hpp>

// GPU-ready vertex structure for mesh rendering
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 uv;
    glm::vec4 tangent;
    
    Vertex() : position(0.0f), normal(0.0f), color(1.0f), uv(0.0f), tangent(1, 0, 0, 1) {}
    
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& col)
        : position(pos), normal(norm), color(col), uv(0.0f), tangent(1, 0, 0, 1) {}
    
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& col,
           const glm::vec2& texCoord, const glm::vec4& tan = glm::vec4(1, 0, 0, 1))
        : position(pos), normal(norm), color(col), uv(texCoord), tangent(tan) {}
};

// Simple vertex for grid/line rendering
struct LineVertex {
    glm::vec3 position;
    glm::vec3 color;
    
    LineVertex() : position(0.0f), color(1.0f) {}
    
    LineVertex(const glm::vec3& pos, const glm::vec3& col)
        : position(pos), color(col) {}
};

// Vertex for single-pass solid+wireframe rendering (barycentric coordinate approach)
struct WireVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 uv;
    glm::vec3 bary;       // Barycentric coordinate (1,0,0), (0,1,0), or (0,0,1)
    glm::vec3 edgeMask;   // 1.0 = real edge, 0.0 = quad diagonal (hide it)
    glm::vec3 edgeState;  // Per-edge state: 0=normal, 1=selected, 2=seam (x,y,z map to bary edges)
    glm::vec4 tangent;
    
    WireVertex() : position(0.0f), normal(0.0f), color(1.0f), uv(0.0f), bary(0.0f), edgeMask(1.0f), edgeState(0.0f), tangent(1, 0, 0, 1) {}
    
    WireVertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& col,
               const glm::vec2& texCoord, const glm::vec3& b, const glm::vec3& mask,
               const glm::vec3& state, const glm::vec4& tan = glm::vec4(1, 0, 0, 1))
        : position(pos), normal(norm), color(col), uv(texCoord), bary(b), edgeMask(mask), edgeState(state), tangent(tan) {}
};

// Vertex for screen-space thick line rendering (expanded to quads in vertex shader)
struct ThickLineVertex {
    glm::vec3 position;      // Current endpoint position
    glm::vec3 nextPosition;  // Other endpoint position
    float side;              // -1.0 or +1.0 (which side of the line)
    glm::vec3 color;         // Per-vertex color
    
    ThickLineVertex() : position(0.0f), nextPosition(0.0f), side(-1.0f), color(1.0f) {}
    
    ThickLineVertex(const glm::vec3& pos, const glm::vec3& next, float s, const glm::vec3& col)
        : position(pos), nextPosition(next), side(s), color(col) {}
};
