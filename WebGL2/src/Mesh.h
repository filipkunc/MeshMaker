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
    size_t getVertexCount() const { return m_vertices.size(); }
    
    // GPU resources
    void createGPUBuffers();
    void deleteGPUBuffers();
    void draw() const;
    
    // Transform
    void setColor(const glm::vec3& color);
    
private:
    void addTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& color);
    void addQuad(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& color);
    void clear();
    
    std::vector<Vertex> m_vertices;
    glm::vec3 m_color;
    
    // OpenGL handles
    uint32_t m_vao;
    uint32_t m_vbo;
    bool m_gpuBuffersCreated;
};
