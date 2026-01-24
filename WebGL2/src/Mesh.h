#pragma once

#include <vector>
#include <cstdint>
#include "Vertex.h"

enum class MeshType {
    Plane,
    Cube,
    Cylinder,
    Sphere,
    Icosahedron
};

enum class ViewMode {
    Solid,
    Wireframe,
    SolidWireframe
};

class Mesh {
public:
    Mesh();
    ~Mesh();
    
    // Generate primitive meshes
    void makePlane();
    void makeCube();
    void makeCylinder(uint32_t steps = 20);
    void makeSphere(uint32_t steps = 20);
    void makeIcosahedron();
    
    // Factory method
    void make(MeshType type, uint32_t steps = 20);
    
    // Access vertex data for GPU upload
    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    const std::vector<LineVertex>& getEdgeVertices() const { return m_edgeVertices; }
    size_t getVertexCount() const { return m_vertices.size(); }
    size_t getEdgeVertexCount() const { return m_edgeVertices.size(); }
    
    // GPU resources
    void createGPUBuffers();
    void deleteGPUBuffers();
    void draw(ViewMode mode = ViewMode::SolidWireframe) const;
    void drawSolid() const;
    void drawWireframe() const;
    
    // Transform
    void setColor(const glm::vec3& color);
    void setWireframeColor(const glm::vec3& color);
    
private:
    void addTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& color);
    void addQuad(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& color);
    void buildEdges();
    void clear();
    
    std::vector<Vertex> m_vertices;
    std::vector<LineVertex> m_edgeVertices;
    glm::vec3 m_color;
    glm::vec3 m_wireframeColor;
    
    // OpenGL handles for solid mesh
    uint32_t m_vao;
    uint32_t m_vbo;
    
    // OpenGL handles for wireframe
    uint32_t m_edgeVao;
    uint32_t m_edgeVbo;
    
    bool m_gpuBuffersCreated;
};
