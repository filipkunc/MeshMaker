#pragma once

#include <vector>
#include <cstdint>
#include "Vertex.h"

// Forward declaration
class Shader;

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
    size_t getTriangleCount() const { return m_vertices.size() / 3; }
    
    // GPU resources
    void createGPUBuffers();
    void deleteGPUBuffers();
    void draw(ViewMode mode = ViewMode::SolidWireframe) const;
    void drawSolid() const;
    void drawWireframe() const;
    void drawForSelection(Shader& selectionShader) const;  // Color-buffer picking
    
    // Selection
    bool raycast(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
                 float& outDistance, size_t& outTriangleIndex) const;
    void selectTriangle(size_t triangleIndex, bool addToSelection = false);
    void deselectTriangle(size_t triangleIndex);
    void selectAll();
    void deselectAll();
    bool isTriangleSelected(size_t triangleIndex) const;
    size_t getSelectedCount() const;
    const std::vector<bool>& getSelection() const { return m_triangleSelection; }
    
    // Highlight selected triangles
    void setSelectionColor(const glm::vec3& color);
    void updateSelectionColors();
    
    // Transform operations on selected triangles
    void translateSelected(const glm::vec3& delta);
    void rotateSelected(const glm::vec3& axis, float angleRadians);
    void scaleSelected(const glm::vec3& center, float factor);
    void scaleSelectedByOffset(const glm::vec3& center, const glm::vec3& offset);
    glm::vec3 getSelectionCenter() const;
    
    // Mesh operations on selected triangles
    void flipSelected();           // Flip normals (reverse winding order)
    void duplicateSelected();      // Duplicate selected triangles
    void deleteSelected();         // Delete selected triangles
    
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
    std::vector<bool> m_triangleSelection;
    glm::vec3 m_color;
    glm::vec3 m_wireframeColor;
    glm::vec3 m_selectionColor;
    
    // OpenGL handles for solid mesh
    uint32_t m_vao;
    uint32_t m_vbo;
    
    // OpenGL handles for wireframe
    uint32_t m_edgeVao;
    uint32_t m_edgeVbo;
    
    bool m_gpuBuffersCreated;
};
