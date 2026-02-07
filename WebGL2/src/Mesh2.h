#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <memory>
#include <glm/glm.hpp>
#include "Vertex.h"

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

enum class SelectionMode {
    Triangles,
    Edges,
    Vertices
};

// A face can be either a triangle (3 vertices) or a quad (4 vertices)
struct Face {
    uint32_t vertices[4];   // Indices into vertex array
    uint32_t edges[4];      // Indices into edge array
    glm::vec2 uvs[4];       // UV coordinates per face corner (allows UV seams)
    uint8_t vertexCount;    // 3 for triangle, 4 for quad
    bool selected;
    
    Face() : vertexCount(3), selected(false) {
        for (int i = 0; i < 4; i++) {
            vertices[i] = UINT32_MAX;
            edges[i] = UINT32_MAX;
            uvs[i] = glm::vec2(0.0f);
        }
    }
    
    bool isQuad() const { return vertexCount == 4; }
    uint8_t count() const { return vertexCount; }
};

// An edge connects two vertices and can be shared by up to 2 faces
struct Edge {
    uint32_t vertices[2];   // The two vertex indices
    uint32_t faces[2];      // Up to 2 adjacent faces (UINT32_MAX if none)
    bool selected;
    bool isSeam;            // Marked as UV seam for unwrapping
    
    Edge() : selected(false), isSeam(false) {
        vertices[0] = vertices[1] = UINT32_MAX;
        faces[0] = faces[1] = UINT32_MAX;
    }
    
    Edge(uint32_t v0, uint32_t v1) : selected(false), isSeam(false) {
        // Always store vertices in sorted order for consistent lookup
        if (v0 < v1) {
            vertices[0] = v0;
            vertices[1] = v1;
        } else {
            vertices[0] = v1;
            vertices[1] = v0;
        }
        faces[0] = faces[1] = UINT32_MAX;
    }
    
    bool containsVertex(uint32_t v) const {
        return vertices[0] == v || vertices[1] == v;
    }
    
    uint32_t otherVertex(uint32_t v) const {
        return vertices[0] == v ? vertices[1] : vertices[0];
    }
    
    bool isBoundary() const {
        return faces[1] == UINT32_MAX;
    }
};

// Vertex with position and selection state
struct MeshVertex {
    glm::vec3 position;
    glm::vec3 normal;
    bool selected;
    
    MeshVertex() : position(0.0f), normal(0.0f, 1.0f, 0.0f), selected(false) {}
    MeshVertex(const glm::vec3& pos) : position(pos), normal(0.0f, 1.0f, 0.0f), selected(false) {}
};

class Mesh2 {
public:
    // Testing support - disable GPU operations when true
    static bool s_disableGPU;
    
    Mesh2();
    ~Mesh2();
    
    // Generate primitive meshes
    void makePlane();
    void makeCube();
    void makeCylinder(uint32_t steps = 20);
    void makeSphere(uint32_t steps = 20);
    void makeIcosahedron();
    void make(MeshType type, uint32_t steps = 20);
    
    // Mesh building
    uint32_t addVertex(const glm::vec3& position);
    uint32_t addTriangle(uint32_t v0, uint32_t v1, uint32_t v2);
    uint32_t addQuad(uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3);
    
    // Set UVs for a face (must be called after addTriangle/addQuad)
    void setFaceUVs(uint32_t faceIndex, const glm::vec2& uv0, const glm::vec2& uv1, 
                    const glm::vec2& uv2, const glm::vec2& uv3 = glm::vec2(0.0f));
    
    void buildConnectivity();  // Build edge connectivity after adding faces
    void computeNormals();
    void clear();
    
    // Access
    size_t getVertexCount() const { return m_vertices.size(); }
    size_t getFaceCount() const { return m_faces.size(); }
    size_t getEdgeCount() const { return m_edges.size(); }
    size_t getTriangleCount() const;  // For rendering (quads = 2 triangles)
    
    const MeshVertex& getVertex(uint32_t index) const { return m_vertices[index]; }
    const Face& getFace(uint32_t index) const { return m_faces[index]; }
    const Edge& getEdge(uint32_t index) const { return m_edges[index]; }
    
    // Full collection access (for testing)
    const std::vector<MeshVertex>& getVertices() const { return m_vertices; }
    std::vector<MeshVertex>& mutVertices() { return m_vertices; }
    const std::vector<Face>& getFaces() const { return m_faces; }
    const std::vector<Edge>& getEdges() const { return m_edges; }
    
    // Transform all vertices by a matrix (for baking transforms during import)
    void transformAllVertices(const glm::mat4& transform);
    
    // Selection
    SelectionMode getSelectionMode() const { return m_selectionMode; }
    void setSelectionMode(SelectionMode mode);
    
    void selectFace(size_t index, bool addToSelection = false);
    void selectEdge(size_t index, bool addToSelection = false);
    void selectVertex(size_t index, bool addToSelection = false);
    void deselectFace(size_t index);
    void deselectEdge(size_t index);
    void deselectVertex(size_t index);
    void selectAll();
    void deselectAll();
    void invertSelection();
    size_t getSelectedCount() const;
    bool isFaceSelected(size_t index) const { return index < m_faces.size() && m_faces[index].selected; }
    bool isEdgeSelected(size_t index) const { return index < m_edges.size() && m_edges[index].selected; }
    bool isVertexSelected(size_t index) const { return index < m_vertices.size() && m_vertices[index].selected; }
    
    // Compatibility aliases for triangle-based API
    void selectTriangle(size_t index, bool addToSelection = false) { selectFace(index, addToSelection); }
    void deselectTriangle(size_t index) { deselectFace(index); }
    bool isTriangleSelected(size_t index) const { return isFaceSelected(index); }
    
    // Edge loop selection
    void selectEdgeLoop(uint32_t edgeIndex);
    void selectEdgeRing(uint32_t edgeIndex);
    void growEdgeSelection();  // Expand selection to adjacent edges
    
    // Raycast
    bool raycast(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                 float& outDistance, size_t& outFaceIndex) const;
    
    // Transform operations
    void translateSelected(const glm::vec3& delta);
    void rotateSelected(const glm::vec3& axis, float angleRadians);
    void scaleSelected(const glm::vec3& center, float factor);
    void scaleSelectedByOffset(const glm::vec3& center, const glm::vec3& offset);
    void transformSelectedByMatrix(const glm::mat4& matrix);
    glm::vec3 getSelectionCenter() const;
    
    // Mesh operations
    void flipSelected();
    void duplicateSelected();
    void deleteSelected();
    void splitSelectedEdges();
    void mergeSelectedVertices(float threshold = 0.001f);
    void splitSelected();  // Dispatches to splitSelectedFaces or splitSelectedEdges based on mode
    void catmullClarkSubdivide(int level = 1);  // Catmull-Clark (or Loop for tri-only) via OpenSubdiv
    void triangulateSelected();  // Convert quads to triangles
    void extrudeSelected();
    
    // UV projection unwrapping
    enum class UVProjection {
        Box,        // Project from 6 sides based on face normal
        Planar,     // Project from a single plane
        Cylindrical,// Project onto a cylinder
        Spherical,  // Project onto a sphere
        SeamBased   // Conformal unwrap using seam edges to define UV islands
    };
    
    void unwrapSelectedUVs(UVProjection projection);
    void unwrapAllUVs(UVProjection projection);
    
    // Seam-based conformal unwrap
    void unwrapSeamBased(bool selectedOnly);
    
    // Seam marking (for unwrapping)
    void markEdgeAsSeam(uint32_t edgeIndex, bool isSeam);
    bool isEdgeSeam(uint32_t edgeIndex) const;
    void markSelectedEdgesAsSeam(bool isSeam);
    void clearAllSeams();
    
    // State capture/restore for undo
    void getState(std::vector<MeshVertex>& outVertices, 
                  std::vector<Face>& outFaces, 
                  std::vector<Edge>& outEdges) const;
    void setState(const std::vector<MeshVertex>& vertices,
                  const std::vector<Face>& faces,
                  const std::vector<Edge>& edges);
    
    // Merge another mesh into this one
    void merge(const Mesh2* mesh);
    
    // GPU rendering
    void createGPUBuffers();
    void updateGPUBuffers();  // Re-upload buffer data without recreating VAOs
    void deleteGPUBuffers();
    void draw(ViewMode mode = ViewMode::SolidWireframe) const;
    void drawForSelection(Shader& selectionShader) const;  // Draw faces for selection
    void drawVerticesForSelection(Shader& selectionShader) const;  // Draw vertices for selection
    void drawEdgesForSelection(Shader& selectionShader) const;     // Draw edges for selection
    void drawVertices(Shader& pointShader) const;  // Draw vertices as points (blue=deselected, red=selected)
    void drawEdges(Shader& lineShader) const;      // Draw edges with selection colors
    void drawNormals(Shader& lineShader) const;    // Draw face normals as lines from face centers
    
    // UV Editor rendering
    void drawUV(Shader& uvShader) const;           // Draw faces in UV space
    void drawUVEdges(Shader& uvColoredShader) const;     // Draw UV edges
    void drawUVForSelection(Shader& selectionShader) const;  // UV space face selection picking
    
    // Colors
    void setColor(const glm::vec3& color) { m_color = color; }
    void setWireframeColor(const glm::vec3& color) { m_wireframeColor = color; }
    void setSelectionColor(const glm::vec3& color) { m_selectionColor = color; }

private:
    // Find or create edge between two vertices
    uint32_t findOrCreateEdge(uint32_t v0, uint32_t v1);
    
    // Get vertices that are connected to a vertex via edges
    std::vector<uint32_t> getConnectedVertices(uint32_t vertexIndex) const;
    
    // Get edges connected to a vertex
    std::vector<uint32_t> getVertexEdges(uint32_t vertexIndex) const;
    
    // Get faces connected to a vertex
    std::vector<uint32_t> getVertexFaces(uint32_t vertexIndex) const;
    
    // Build render data from mesh topology
    void buildRenderData();
    
    // Mesh topology
    std::vector<MeshVertex> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Edge> m_edges;
    
    // Edge lookup by vertex pair (for fast edge finding)
    std::unordered_map<uint64_t, uint32_t> m_edgeLookup;
    
    // Vertex to edge/face connectivity (built by buildConnectivity)
    std::vector<std::vector<uint32_t>> m_vertexToEdges;
    std::vector<std::vector<uint32_t>> m_vertexToFaces;
    
    // Render data (generated from topology)
    std::vector<Vertex> m_renderVertices;
    std::vector<LineVertex> m_edgeRenderVertices;
    
    // State
    SelectionMode m_selectionMode;
    glm::vec3 m_color;
    glm::vec3 m_wireframeColor;
    glm::vec3 m_selectionColor;
    
    // GPU handles
    uint32_t m_vao = 0;
    uint32_t m_vbo = 0;
    uint32_t m_edgeVao = 0;
    uint32_t m_edgeVbo = 0;
    
    bool m_gpuBuffersCreated = false;
    bool m_renderDataDirty = true;
    
    // Helper to create edge key for lookup
    static uint64_t makeEdgeKey(uint32_t v0, uint32_t v1) {
        if (v0 > v1) std::swap(v0, v1);
        return (static_cast<uint64_t>(v0) << 32) | v1;
    }
    
    void splitSelectedFaces();  // Linear face split (used by splitSelected)
};
