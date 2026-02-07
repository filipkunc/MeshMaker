#include "Mesh2.h"
#include "Shader.h"
#include "OpenSubdivHelper.h"

#ifdef EMSCRIPTEN_BUILD
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#endif

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Static variable for testing - disables GPU operations
bool Mesh2::s_disableGPU = false;

Mesh2::Mesh2()
    : m_selectionMode(SelectionMode::Triangles)
    , m_color(0.7f, 0.7f, 0.7f)
    , m_wireframeColor(0.0f, 0.0f, 0.0f)
    , m_selectionColor(0.9f, 0.2f, 0.0f)
{
}

Mesh2::~Mesh2() {
    deleteGPUBuffers();
}

void Mesh2::clear() {
    m_vertices.clear();
    m_faces.clear();
    m_edges.clear();
    m_edgeLookup.clear();
    m_vertexToEdges.clear();
    m_vertexToFaces.clear();
    m_renderVertices.clear();
    m_edgeRenderVertices.clear();
    m_renderDataDirty = true;
}

uint32_t Mesh2::addVertex(const glm::vec3& position) {
    uint32_t index = static_cast<uint32_t>(m_vertices.size());
    m_vertices.push_back(MeshVertex(position));
    m_renderDataDirty = true;
    return index;
}

uint32_t Mesh2::findOrCreateEdge(uint32_t v0, uint32_t v1) {
    uint64_t key = makeEdgeKey(v0, v1);
    auto it = m_edgeLookup.find(key);
    if (it != m_edgeLookup.end()) {
        return it->second;
    }
    
    uint32_t edgeIndex = static_cast<uint32_t>(m_edges.size());
    m_edges.push_back(Edge(v0, v1));
    m_edgeLookup[key] = edgeIndex;
    return edgeIndex;
}

uint32_t Mesh2::addTriangle(uint32_t v0, uint32_t v1, uint32_t v2) {
    uint32_t faceIndex = static_cast<uint32_t>(m_faces.size());
    Face face;
    face.vertexCount = 3;
    face.vertices[0] = v0;
    face.vertices[1] = v1;
    face.vertices[2] = v2;
    
    // Create or find edges
    face.edges[0] = findOrCreateEdge(v0, v1);
    face.edges[1] = findOrCreateEdge(v1, v2);
    face.edges[2] = findOrCreateEdge(v2, v0);
    
    // Link edges to this face
    for (int i = 0; i < 3; i++) {
        Edge& edge = m_edges[face.edges[i]];
        if (edge.faces[0] == UINT32_MAX) {
            edge.faces[0] = faceIndex;
        } else {
            edge.faces[1] = faceIndex;
        }
    }
    
    m_faces.push_back(face);
    m_renderDataDirty = true;
    return faceIndex;
}

uint32_t Mesh2::addQuad(uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3) {
    uint32_t faceIndex = static_cast<uint32_t>(m_faces.size());
    Face face;
    face.vertexCount = 4;
    face.vertices[0] = v0;
    face.vertices[1] = v1;
    face.vertices[2] = v2;
    face.vertices[3] = v3;
    
    // Create or find edges
    face.edges[0] = findOrCreateEdge(v0, v1);
    face.edges[1] = findOrCreateEdge(v1, v2);
    face.edges[2] = findOrCreateEdge(v2, v3);
    face.edges[3] = findOrCreateEdge(v3, v0);
    
    // Link edges to this face
    for (int i = 0; i < 4; i++) {
        Edge& edge = m_edges[face.edges[i]];
        if (edge.faces[0] == UINT32_MAX) {
            edge.faces[0] = faceIndex;
        } else {
            edge.faces[1] = faceIndex;
        }
    }
    
    m_faces.push_back(face);
    m_renderDataDirty = true;
    return faceIndex;
}

void Mesh2::setFaceUVs(uint32_t faceIndex, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3) {
    if (faceIndex >= m_faces.size()) return;
    
    Face& face = m_faces[faceIndex];
    face.uvs[0] = uv0;
    face.uvs[1] = uv1;
    face.uvs[2] = uv2;
    face.uvs[3] = uv3;
    m_renderDataDirty = true;
}

void Mesh2::buildConnectivity() {
    // Build vertex to edge/face connectivity
    m_vertexToEdges.clear();
    m_vertexToEdges.resize(m_vertices.size());
    m_vertexToFaces.clear();
    m_vertexToFaces.resize(m_vertices.size());
    
    for (uint32_t i = 0; i < m_edges.size(); i++) {
        const Edge& edge = m_edges[i];
        m_vertexToEdges[edge.vertices[0]].push_back(i);
        m_vertexToEdges[edge.vertices[1]].push_back(i);
    }
    
    for (uint32_t i = 0; i < m_faces.size(); i++) {
        const Face& face = m_faces[i];
        for (int j = 0; j < face.vertexCount; j++) {
            m_vertexToFaces[face.vertices[j]].push_back(i);
        }
    }
}

void Mesh2::computeNormals() {
    // Reset normals
    for (auto& v : m_vertices) {
        v.normal = glm::vec3(0.0f);
    }
    
    // Accumulate face normals to vertices
    for (const Face& face : m_faces) {
        glm::vec3 v0 = m_vertices[face.vertices[0]].position;
        glm::vec3 v1 = m_vertices[face.vertices[1]].position;
        glm::vec3 v2 = m_vertices[face.vertices[2]].position;
        
        glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        
        for (int i = 0; i < face.vertexCount; i++) {
            m_vertices[face.vertices[i]].normal += faceNormal;
        }
    }
    
    // Normalize
    for (auto& v : m_vertices) {
        if (glm::length(v.normal) > 0.0001f) {
            v.normal = glm::normalize(v.normal);
        } else {
            v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
    
    m_renderDataDirty = true;
}

void Mesh2::transformAllVertices(const glm::mat4& transform) {
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
    
    for (auto& v : m_vertices) {
        glm::vec4 worldPos = transform * glm::vec4(v.position, 1.0f);
        v.position = glm::vec3(worldPos);
        v.normal = glm::normalize(normalMatrix * v.normal);
    }
    
    m_renderDataDirty = true;
}

size_t Mesh2::getTriangleCount() const {
    size_t count = 0;
    for (const Face& face : m_faces) {
        count += face.isQuad() ? 2 : 1;
    }
    return count;
}

// Selection mode
void Mesh2::setSelectionMode(SelectionMode mode) {
    if (m_selectionMode != mode) {
        deselectAll();
        m_selectionMode = mode;
    }
}

void Mesh2::selectFace(size_t index, bool addToSelection) {
    if (!addToSelection) deselectAll();
    if (index < m_faces.size()) {
        m_faces[index].selected = true;
        m_renderDataDirty = true;
    }
}

void Mesh2::selectEdge(size_t index, bool addToSelection) {
    if (!addToSelection) deselectAll();
    if (index < m_edges.size()) {
        m_edges[index].selected = true;
        m_renderDataDirty = true;
    }
}

void Mesh2::selectVertex(size_t index, bool addToSelection) {
    if (!addToSelection) deselectAll();
    if (index < m_vertices.size()) {
        m_vertices[index].selected = true;
        m_renderDataDirty = true;
    }
}

void Mesh2::selectAll() {
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            for (auto& face : m_faces) face.selected = true;
            break;
        case SelectionMode::Edges:
            for (auto& edge : m_edges) edge.selected = true;
            break;
        case SelectionMode::Vertices:
            for (auto& v : m_vertices) v.selected = true;
            break;
    }
    m_renderDataDirty = true;
}

void Mesh2::deselectAll() {
    for (auto& face : m_faces) face.selected = false;
    for (auto& edge : m_edges) edge.selected = false;
    for (auto& v : m_vertices) v.selected = false;
    m_renderDataDirty = true;
}

void Mesh2::deselectFace(size_t index) {
    if (index < m_faces.size()) {
        m_faces[index].selected = false;
        m_renderDataDirty = true;
    }
}

void Mesh2::deselectEdge(size_t index) {
    if (index < m_edges.size()) {
        m_edges[index].selected = false;
        m_renderDataDirty = true;
    }
}

void Mesh2::deselectVertex(size_t index) {
    if (index < m_vertices.size()) {
        m_vertices[index].selected = false;
        m_renderDataDirty = true;
    }
}

void Mesh2::invertSelection() {
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            for (auto& face : m_faces) face.selected = !face.selected;
            break;
        case SelectionMode::Edges:
            for (auto& edge : m_edges) edge.selected = !edge.selected;
            break;
        case SelectionMode::Vertices:
            for (auto& v : m_vertices) v.selected = !v.selected;
            break;
    }
    m_renderDataDirty = true;
}

size_t Mesh2::getSelectedCount() const {
    size_t count = 0;
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            for (const auto& face : m_faces) if (face.selected) count++;
            break;
        case SelectionMode::Edges:
            for (const auto& edge : m_edges) if (edge.selected) count++;
            break;
        case SelectionMode::Vertices:
            for (const auto& v : m_vertices) if (v.selected) count++;
            break;
    }
    return count;
}

// Edge loop selection - follows edges through vertices
// At each vertex, finds the edge that doesn't share a face with the current edge
void Mesh2::selectEdgeLoop(uint32_t edgeIndex) {
    if (edgeIndex >= m_edges.size()) return;
    
    m_edges[edgeIndex].selected = true;
    
    // Traverse in both directions (from each endpoint of the starting edge)
    for (int dir = 0; dir < 2; dir++) {
        uint32_t currentEdge = edgeIndex;
        uint32_t currentVertex = m_edges[edgeIndex].vertices[dir];
        
        while (true) {
            const Edge& edge = m_edges[currentEdge];
            
            // Count edges meeting at this vertex
            std::vector<uint32_t> vertexEdges;
            for (uint32_t ei = 0; ei < m_edges.size(); ei++) {
                if (m_edges[ei].vertices[0] == currentVertex || 
                    m_edges[ei].vertices[1] == currentVertex) {
                    vertexEdges.push_back(ei);
                }
            }
            
            // Edge loop only works when exactly 4 edges meet at the vertex
            if (vertexEdges.size() != 4) break;
            
            // Find the edge that doesn't share any face with the current edge
            uint32_t nextEdge = UINT32_MAX;
            for (uint32_t ei : vertexEdges) {
                if (ei == currentEdge) continue;
                
                const Edge& candidate = m_edges[ei];
                // Check if candidate shares a face with current edge
                bool sharesFace = false;
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 2; j++) {
                        if (edge.faces[i] != UINT32_MAX && 
                            edge.faces[i] == candidate.faces[j]) {
                            sharesFace = true;
                            break;
                        }
                    }
                    if (sharesFace) break;
                }
                
                if (!sharesFace) {
                    nextEdge = ei;
                    break;
                }
            }
            
            if (nextEdge == UINT32_MAX) break;
            
            // Stop if we've looped back or already selected
            if (nextEdge == edgeIndex || m_edges[nextEdge].selected) break;
            
            m_edges[nextEdge].selected = true;
            
            // Move to the other vertex of the next edge
            const Edge& ne = m_edges[nextEdge];
            currentVertex = (ne.vertices[0] == currentVertex) ? ne.vertices[1] : ne.vertices[0];
            currentEdge = nextEdge;
        }
    }
    
    m_renderDataDirty = true;
}

// Edge ring selection - follows opposite edges through quads
// At each quad face, finds the edge that shares no vertex with current edge (opposite edge)
void Mesh2::selectEdgeRing(uint32_t edgeIndex) {
    if (edgeIndex >= m_edges.size()) return;
    
    m_edges[edgeIndex].selected = true;
    
    // Traverse in both directions through connected quads
    for (int dir = 0; dir < 2; dir++) {
        uint32_t currentEdge = edgeIndex;
        uint32_t lastFace = UINT32_MAX;
        
        while (true) {
            const Edge& edge = m_edges[currentEdge];
            
            // Find next face (not the one we came from)
            uint32_t nextFace = UINT32_MAX;
            for (int i = 0; i < 2; i++) {
                if (edge.faces[i] != UINT32_MAX && edge.faces[i] != lastFace) {
                    nextFace = edge.faces[i];
                    break;
                }
            }
            
            if (nextFace == UINT32_MAX) break;
            
            const Face& face = m_faces[nextFace];
            if (!face.isQuad()) break;
            
            // Find the opposite edge - the one that shares NO vertex with current edge
            uint32_t nextEdge = UINT32_MAX;
            uint32_t v0 = edge.vertices[0];
            uint32_t v1 = edge.vertices[1];
            
            for (int i = 0; i < 4; i++) {
                uint32_t candidateEdge = face.edges[i];
                if (candidateEdge == currentEdge) continue;
                
                const Edge& candidate = m_edges[candidateEdge];
                // Check if candidate shares any vertex with current edge
                bool sharesVertex = (candidate.vertices[0] == v0 || candidate.vertices[0] == v1 ||
                                    candidate.vertices[1] == v0 || candidate.vertices[1] == v1);
                
                if (!sharesVertex) {
                    nextEdge = candidateEdge;
                    break;
                }
            }
            
            if (nextEdge == UINT32_MAX) break;
            
            // Stop if we've looped back or already selected
            if (nextEdge == edgeIndex || m_edges[nextEdge].selected) break;
            
            m_edges[nextEdge].selected = true;
            lastFace = nextFace;
            currentEdge = nextEdge;
        }
    }
    
    m_renderDataDirty = true;
}

// Grow edge selection - adds all edges that share a vertex with any selected edge
void Mesh2::growEdgeSelection() {
    // Collect all vertices that are part of selected edges
    std::unordered_set<uint32_t> selectedVertices;
    for (size_t i = 0; i < m_edges.size(); i++) {
        if (m_edges[i].selected) {
            selectedVertices.insert(m_edges[i].vertices[0]);
            selectedVertices.insert(m_edges[i].vertices[1]);
        }
    }
    
    // Select all edges that touch any of these vertices
    for (size_t i = 0; i < m_edges.size(); i++) {
        if (m_edges[i].selected) continue;
        
        if (selectedVertices.count(m_edges[i].vertices[0]) > 0 ||
            selectedVertices.count(m_edges[i].vertices[1]) > 0) {
            m_edges[i].selected = true;
        }
    }
    
    m_renderDataDirty = true;
}

// Raycast
bool Mesh2::raycast(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                    float& outDistance, size_t& outFaceIndex) const {
    outDistance = std::numeric_limits<float>::max();
    outFaceIndex = SIZE_MAX;
    bool hit = false;
    
    for (size_t i = 0; i < m_faces.size(); i++) {
        const Face& face = m_faces[i];
        
        // Test triangles (for quads, test both triangles)
        int numTris = face.isQuad() ? 2 : 1;
        
        for (int t = 0; t < numTris; t++) {
            glm::vec3 v0 = m_vertices[face.vertices[0]].position;
            glm::vec3 v1 = m_vertices[face.vertices[t + 1]].position;
            glm::vec3 v2 = m_vertices[face.vertices[t + 2]].position;
            
            // Möller-Trumbore intersection
            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 h = glm::cross(rayDir, edge2);
            float a = glm::dot(edge1, h);
            
            if (std::abs(a) < 1e-6f) continue;
            
            float f = 1.0f / a;
            glm::vec3 s = rayOrigin - v0;
            float u = f * glm::dot(s, h);
            
            if (u < 0.0f || u > 1.0f) continue;
            
            glm::vec3 q = glm::cross(s, edge1);
            float v = f * glm::dot(rayDir, q);
            
            if (v < 0.0f || u + v > 1.0f) continue;
            
            float dist = f * glm::dot(edge2, q);
            
            if (dist > 0.0f && dist < outDistance) {
                outDistance = dist;
                outFaceIndex = i;
                hit = true;
            }
        }
    }
    
    return hit;
}

// Transform operations
glm::vec3 Mesh2::getSelectionCenter() const {
    glm::vec3 center(0.0f);
    size_t count = 0;
    
    std::unordered_set<uint32_t> selectedVertices;
    
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            for (const auto& face : m_faces) {
                if (face.selected) {
                    for (int i = 0; i < face.vertexCount; i++) {
                        selectedVertices.insert(face.vertices[i]);
                    }
                }
            }
            break;
        case SelectionMode::Edges:
            for (const auto& edge : m_edges) {
                if (edge.selected) {
                    selectedVertices.insert(edge.vertices[0]);
                    selectedVertices.insert(edge.vertices[1]);
                }
            }
            break;
        case SelectionMode::Vertices:
            for (size_t i = 0; i < m_vertices.size(); i++) {
                if (m_vertices[i].selected) {
                    selectedVertices.insert(static_cast<uint32_t>(i));
                }
            }
            break;
    }
    
    for (uint32_t vi : selectedVertices) {
        center += m_vertices[vi].position;
        count++;
    }
    
    return count > 0 ? center / static_cast<float>(count) : glm::vec3(0.0f);
}

void Mesh2::translateSelected(const glm::vec3& delta) {
    std::unordered_set<uint32_t> affectedVertices;
    
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            for (const auto& face : m_faces) {
                if (face.selected) {
                    for (int i = 0; i < face.vertexCount; i++) {
                        affectedVertices.insert(face.vertices[i]);
                    }
                }
            }
            break;
        case SelectionMode::Edges:
            for (const auto& edge : m_edges) {
                if (edge.selected) {
                    affectedVertices.insert(edge.vertices[0]);
                    affectedVertices.insert(edge.vertices[1]);
                }
            }
            break;
        case SelectionMode::Vertices:
            for (size_t i = 0; i < m_vertices.size(); i++) {
                if (m_vertices[i].selected) {
                    affectedVertices.insert(static_cast<uint32_t>(i));
                }
            }
            break;
    }
    
    for (uint32_t vi : affectedVertices) {
        m_vertices[vi].position += delta;
    }
    
    m_renderDataDirty = true;
}

void Mesh2::rotateSelected(const glm::vec3& axis, float angleRadians) {
    glm::vec3 center = getSelectionCenter();
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleRadians, axis);
    
    std::unordered_set<uint32_t> affectedVertices;
    
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            for (const auto& face : m_faces) {
                if (face.selected) {
                    for (int i = 0; i < face.vertexCount; i++) {
                        affectedVertices.insert(face.vertices[i]);
                    }
                }
            }
            break;
        case SelectionMode::Edges:
            for (const auto& edge : m_edges) {
                if (edge.selected) {
                    affectedVertices.insert(edge.vertices[0]);
                    affectedVertices.insert(edge.vertices[1]);
                }
            }
            break;
        case SelectionMode::Vertices:
            for (size_t i = 0; i < m_vertices.size(); i++) {
                if (m_vertices[i].selected) {
                    affectedVertices.insert(static_cast<uint32_t>(i));
                }
            }
            break;
    }
    
    for (uint32_t vi : affectedVertices) {
        glm::vec3 relative = m_vertices[vi].position - center;
        glm::vec4 rotated = rotation * glm::vec4(relative, 1.0f);
        m_vertices[vi].position = center + glm::vec3(rotated);
    }
    
    computeNormals();
    m_renderDataDirty = true;
}

void Mesh2::scaleSelected(const glm::vec3& center, float factor) {
    std::unordered_set<uint32_t> affectedVertices;
    
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            for (const auto& face : m_faces) {
                if (face.selected) {
                    for (int i = 0; i < face.vertexCount; i++) {
                        affectedVertices.insert(face.vertices[i]);
                    }
                }
            }
            break;
        case SelectionMode::Edges:
            for (const auto& edge : m_edges) {
                if (edge.selected) {
                    affectedVertices.insert(edge.vertices[0]);
                    affectedVertices.insert(edge.vertices[1]);
                }
            }
            break;
        case SelectionMode::Vertices:
            for (size_t i = 0; i < m_vertices.size(); i++) {
                if (m_vertices[i].selected) {
                    affectedVertices.insert(static_cast<uint32_t>(i));
                }
            }
            break;
    }
    
    for (uint32_t vi : affectedVertices) {
        m_vertices[vi].position = center + (m_vertices[vi].position - center) * factor;
    }
    
    m_renderDataDirty = true;
}

void Mesh2::scaleSelectedByOffset(const glm::vec3& center, const glm::vec3& offset) {
    glm::vec3 scaleFactor = glm::vec3(1.0f) + offset;
    
    std::unordered_set<uint32_t> affectedVertices;
    
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            for (const auto& face : m_faces) {
                if (face.selected) {
                    for (int i = 0; i < face.vertexCount; i++) {
                        affectedVertices.insert(face.vertices[i]);
                    }
                }
            }
            break;
        case SelectionMode::Edges:
            for (const auto& edge : m_edges) {
                if (edge.selected) {
                    affectedVertices.insert(edge.vertices[0]);
                    affectedVertices.insert(edge.vertices[1]);
                }
            }
            break;
        case SelectionMode::Vertices:
            for (size_t i = 0; i < m_vertices.size(); i++) {
                if (m_vertices[i].selected) {
                    affectedVertices.insert(static_cast<uint32_t>(i));
                }
            }
            break;
    }
    
    for (uint32_t vi : affectedVertices) {
        glm::vec3 relative = m_vertices[vi].position - center;
        relative.x *= scaleFactor.x;
        relative.y *= scaleFactor.y;
        relative.z *= scaleFactor.z;
        m_vertices[vi].position = center + relative;
    }
    
    m_renderDataDirty = true;
}

void Mesh2::transformSelectedByMatrix(const glm::mat4& matrix) {
    std::unordered_set<uint32_t> affectedVertices;
    
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            for (const auto& face : m_faces) {
                if (face.selected) {
                    for (int i = 0; i < face.vertexCount; i++) {
                        affectedVertices.insert(face.vertices[i]);
                    }
                }
            }
            break;
        case SelectionMode::Edges:
            for (const auto& edge : m_edges) {
                if (edge.selected) {
                    affectedVertices.insert(edge.vertices[0]);
                    affectedVertices.insert(edge.vertices[1]);
                }
            }
            break;
        case SelectionMode::Vertices:
            for (size_t i = 0; i < m_vertices.size(); i++) {
                if (m_vertices[i].selected) {
                    affectedVertices.insert(static_cast<uint32_t>(i));
                }
            }
            break;
    }
    
    for (uint32_t vi : affectedVertices) {
        glm::vec4 pos = glm::vec4(m_vertices[vi].position, 1.0f);
        m_vertices[vi].position = glm::vec3(matrix * pos);
    }
    
    computeNormals();
    m_renderDataDirty = true;
}

// Mesh operations
void Mesh2::flipSelected() {
    for (Face& face : m_faces) {
        if (!face.selected) continue;
        
        // Reverse vertex order
        if (face.isQuad()) {
            std::swap(face.vertices[1], face.vertices[3]);
            std::swap(face.edges[0], face.edges[3]);
            std::swap(face.edges[1], face.edges[2]);
        } else {
            std::swap(face.vertices[1], face.vertices[2]);
            std::swap(face.edges[0], face.edges[2]);
        }
    }
    
    computeNormals();
    m_renderDataDirty = true;
}

void Mesh2::duplicateSelected() {
    std::vector<Face> newFaces;
    std::unordered_map<uint32_t, uint32_t> vertexMap;
    
    for (size_t i = 0; i < m_faces.size(); i++) {
        Face& face = m_faces[i];
        if (!face.selected) continue;
        
        Face newFace = face;
        
        // Create new vertices for the duplicate
        for (int j = 0; j < face.vertexCount; j++) {
            uint32_t oldVertex = face.vertices[j];
            if (vertexMap.find(oldVertex) == vertexMap.end()) {
                uint32_t newVertex = addVertex(m_vertices[oldVertex].position);
                m_vertices[newVertex].normal = m_vertices[oldVertex].normal;
                vertexMap[oldVertex] = newVertex;
            }
            newFace.vertices[j] = vertexMap[oldVertex];
        }
        
        newFace.selected = true;
        face.selected = false;
        
        newFaces.push_back(newFace);
    }
    
    // Add new faces (need to rebuild edges)
    for (const Face& face : newFaces) {
        if (face.isQuad()) {
            addQuad(face.vertices[0], face.vertices[1], face.vertices[2], face.vertices[3]);
            m_faces.back().selected = true;
        } else {
            addTriangle(face.vertices[0], face.vertices[1], face.vertices[2]);
            m_faces.back().selected = true;
        }
    }
    
    buildConnectivity();
    m_renderDataDirty = true;
}

void Mesh2::deleteSelected() {
    // Remove selected faces
    std::vector<Face> remainingFaces;
    for (const Face& face : m_faces) {
        if (!face.selected) {
            remainingFaces.push_back(face);
        }
    }
    
    // Rebuild mesh with remaining faces
    std::vector<MeshVertex> oldVertices = m_vertices;
    std::unordered_map<uint32_t, uint32_t> vertexRemap;
    
    clear();
    
    // Re-add only used vertices and faces
    for (const Face& face : remainingFaces) {
        uint32_t newVertices[4];
        for (int i = 0; i < face.vertexCount; i++) {
            uint32_t oldIdx = face.vertices[i];
            if (vertexRemap.find(oldIdx) == vertexRemap.end()) {
                uint32_t newIdx = addVertex(oldVertices[oldIdx].position);
                m_vertices[newIdx].normal = oldVertices[oldIdx].normal;
                vertexRemap[oldIdx] = newIdx;
            }
            newVertices[i] = vertexRemap[oldIdx];
        }
        
        if (face.isQuad()) {
            addQuad(newVertices[0], newVertices[1], newVertices[2], newVertices[3]);
        } else {
            addTriangle(newVertices[0], newVertices[1], newVertices[2]);
        }
    }
    
    buildConnectivity();
    m_renderDataDirty = true;
}

void Mesh2::splitSelectedEdges() {
    std::unordered_map<uint32_t, uint32_t> edgeMidpoints;
    
    // First pass: create midpoint vertices for selected edges
    for (uint32_t i = 0; i < static_cast<uint32_t>(m_edges.size()); i++) {
        Edge& edge = m_edges[i];
        if (!edge.selected) continue;
        
        glm::vec3 mid = (m_vertices[edge.vertices[0]].position + 
                         m_vertices[edge.vertices[1]].position) * 0.5f;
        edgeMidpoints[i] = addVertex(mid);
    }
    
    if (edgeMidpoints.empty()) return;
    
    // Second pass: split faces containing selected edges
    std::vector<Face> newFaces;
    std::vector<size_t> facesToRemove;
    
    for (size_t fi = 0; fi < m_faces.size(); fi++) {
        Face& face = m_faces[fi];
        
        // Count how many edges of this face are being split
        std::vector<int> splitEdgePositions;
        for (int i = 0; i < face.vertexCount; i++) {
            if (edgeMidpoints.find(face.edges[i]) != edgeMidpoints.end()) {
                splitEdgePositions.push_back(i);
            }
        }
        
        if (splitEdgePositions.empty()) continue;
        
        facesToRemove.push_back(fi);
        
        if (splitEdgePositions.size() == 1) {
            // One edge split - create 2 triangles (or 1 tri + 1 quad for quads)
            int splitPos = splitEdgePositions[0];
            uint32_t midV = edgeMidpoints[face.edges[splitPos]];
            
            if (face.isQuad()) {
                // Split quad into triangle and quad
                uint32_t v0 = face.vertices[splitPos];
                uint32_t v1 = face.vertices[(splitPos + 1) % 4];
                uint32_t v2 = face.vertices[(splitPos + 2) % 4];
                uint32_t v3 = face.vertices[(splitPos + 3) % 4];
                
                Face tri;
                tri.vertexCount = 3;
                tri.vertices[0] = v0;
                tri.vertices[1] = midV;
                tri.vertices[2] = v3;
                newFaces.push_back(tri);
                
                Face quad;
                quad.vertexCount = 4;
                quad.vertices[0] = midV;
                quad.vertices[1] = v1;
                quad.vertices[2] = v2;
                quad.vertices[3] = v3;
                newFaces.push_back(quad);
            } else {
                // Split triangle into 2 triangles
                uint32_t v0 = face.vertices[splitPos];
                uint32_t v1 = face.vertices[(splitPos + 1) % 3];
                uint32_t v2 = face.vertices[(splitPos + 2) % 3];
                
                Face tri1;
                tri1.vertexCount = 3;
                tri1.vertices[0] = v0;
                tri1.vertices[1] = midV;
                tri1.vertices[2] = v2;
                newFaces.push_back(tri1);
                
                Face tri2;
                tri2.vertexCount = 3;
                tri2.vertices[0] = midV;
                tri2.vertices[1] = v1;
                tri2.vertices[2] = v2;
                newFaces.push_back(tri2);
            }
        } else if (splitEdgePositions.size() == 2 && !face.isQuad()) {
            // Triangle with 2 edges split - creates 1 triangle + 1 quad
            int pos0 = splitEdgePositions[0];
            int pos1 = splitEdgePositions[1];
            uint32_t mid0 = edgeMidpoints[face.edges[pos0]];
            uint32_t mid1 = edgeMidpoints[face.edges[pos1]];
            
            // Find the shared vertex between the two split edges
            // Each edge in a triangle is at position i and connects vertices[i] to vertices[(i+1)%3]
            // Edge at pos0 connects vertices[pos0] to vertices[(pos0+1)%3]
            // Edge at pos1 connects vertices[pos1] to vertices[(pos1+1)%3]
            
            // Find which vertex is shared between both edges
            uint32_t sharedVertex = UINT32_MAX;
            uint32_t opposite0 = UINT32_MAX; // vertex on edge0 not shared
            uint32_t opposite1 = UINT32_MAX; // vertex on edge1 not shared
            
            uint32_t e0v0 = face.vertices[pos0];
            uint32_t e0v1 = face.vertices[(pos0 + 1) % 3];
            uint32_t e1v0 = face.vertices[pos1];
            uint32_t e1v1 = face.vertices[(pos1 + 1) % 3];
            
            if (e0v0 == e1v0) {
                sharedVertex = e0v0;
                opposite0 = e0v1;
                opposite1 = e1v1;
            } else if (e0v0 == e1v1) {
                sharedVertex = e0v0;
                opposite0 = e0v1;
                opposite1 = e1v0;
            } else if (e0v1 == e1v0) {
                sharedVertex = e0v1;
                opposite0 = e0v0;
                opposite1 = e1v1;
            } else if (e0v1 == e1v1) {
                sharedVertex = e0v1;
                opposite0 = e0v0;
                opposite1 = e1v0;
            }
            
            if (sharedVertex != UINT32_MAX) {
                // Create small triangle at the shared vertex corner
                Face tri;
                tri.vertexCount = 3;
                tri.vertices[0] = sharedVertex;
                tri.vertices[1] = mid0;
                tri.vertices[2] = mid1;
                newFaces.push_back(tri);
                
                // Create quad from the two midpoints and two opposite vertices
                Face quad;
                quad.vertexCount = 4;
                quad.vertices[0] = mid0;
                quad.vertices[1] = opposite0;
                quad.vertices[2] = opposite1;
                quad.vertices[3] = mid1;
                newFaces.push_back(quad);
            }
        } else if (splitEdgePositions.size() == 2 && face.isQuad()) {
            // Two edges split on quad
            int pos0 = splitEdgePositions[0];
            int pos1 = splitEdgePositions[1];
            
            uint32_t mid0 = edgeMidpoints[face.edges[pos0]];
            uint32_t mid1 = edgeMidpoints[face.edges[pos1]];
            
            if (std::abs(pos0 - pos1) == 2) {
                // Opposite edges - split into 2 quads
                if (pos0 > pos1) {
                    std::swap(pos0, pos1);
                    std::swap(mid0, mid1);
                }
                
                Face quad1, quad2;
                quad1.vertexCount = quad2.vertexCount = 4;
                
                quad1.vertices[0] = face.vertices[pos0];
                quad1.vertices[1] = mid0;
                quad1.vertices[2] = mid1;
                quad1.vertices[3] = face.vertices[(pos1 + 1) % 4];
                
                quad2.vertices[0] = mid0;
                quad2.vertices[1] = face.vertices[(pos0 + 1) % 4];
                quad2.vertices[2] = face.vertices[pos1];
                quad2.vertices[3] = mid1;
                
                newFaces.push_back(quad1);
                newFaces.push_back(quad2);
            } else {
                // Adjacent edges - split into 1 tri + 1 quad
                // The shared vertex is between the two edges
                // Ensure pos0 < pos1
                if (pos0 > pos1) {
                    std::swap(pos0, pos1);
                    std::swap(mid0, mid1);
                }

                // Check if edges are adjacent (differ by 1 or wrap around 0-3)
                bool wraps = (pos0 == 0 && pos1 == 3);
                int sharedIdx = wraps ? 0 : pos1;
                uint32_t sharedV = face.vertices[sharedIdx];
                
                // Create triangle at the shared corner
                Face tri;
                tri.vertexCount = 3;
                if (wraps) {
                    tri.vertices[0] = mid1;  // mid of edge 3
                    tri.vertices[1] = sharedV;
                    tri.vertices[2] = mid0;  // mid of edge 0
                } else {
                    tri.vertices[0] = mid0;
                    tri.vertices[1] = sharedV;
                    tri.vertices[2] = mid1;
                }
                newFaces.push_back(tri);
                
                // Create pentagon as a quad + tri, or just handle as a quad
                // The remaining 3 original verts + 2 midpoints form a pentagon
                // Split it into a triangle and a quad
                // Actually, easier: make a quad from the 4 non-shared vertices/midpoints
                // and handle the remaining bit
                // Pentagon vertices in order: mid0, v_next..., mid1
                // We'll create 1 quad from the remaining vertices
                Face quad;
                quad.vertexCount = 4;
                if (wraps) {
                    // edges 3 and 0 share vertex 0
                    // remaining: mid0 (edge 0), v1, v2, v3 start, mid1 (edge 3)
                    // Actually: mid0, v1, v2, mid1 is wrong - need to think about winding
                    quad.vertices[0] = mid0;
                    quad.vertices[1] = face.vertices[1];
                    quad.vertices[2] = face.vertices[2];
                    quad.vertices[3] = face.vertices[3];
                    newFaces.push_back(quad);
                    
                    Face tri2;
                    tri2.vertexCount = 3;
                    tri2.vertices[0] = face.vertices[3];
                    tri2.vertices[1] = mid1;
                    tri2.vertices[2] = mid0;
                    newFaces.push_back(tri2);
                } else {
                    // pos0 and pos1 are adjacent, shared vertex is at pos1
                    // Remaining vertices form a pentagon: mid0, v(pos0+1)=shared-1, ..., mid1
                    // Split into quad + tri
                    int v_after_mid1 = (pos1 + 1) % 4;
                    int v_before_mid0 = pos0;
                    
                    quad.vertices[0] = mid0;
                    quad.vertices[1] = mid1;
                    quad.vertices[2] = face.vertices[v_after_mid1];
                    quad.vertices[3] = face.vertices[v_before_mid0];
                    newFaces.push_back(quad);
                    
                    // Remaining triangle between the two non-adjacent original verts and a midpoint
                    int remaining = (v_after_mid1 + 1) % 4;
                    if (remaining != v_before_mid0) {
                        Face tri2;
                        tri2.vertexCount = 3;
                        tri2.vertices[0] = face.vertices[v_after_mid1];
                        tri2.vertices[1] = face.vertices[remaining];
                        tri2.vertices[2] = face.vertices[v_before_mid0];
                        newFaces.push_back(tri2);
                    }
                }
            }
        } else if (static_cast<int>(splitEdgePositions.size()) == face.vertexCount) {
            // ALL edges of the face are split
            if (face.isQuad()) {
                // Quad with all 4 edges split → center vertex + 4 sub-quads
                uint32_t mid0 = edgeMidpoints[face.edges[0]];
                uint32_t mid1 = edgeMidpoints[face.edges[1]];
                uint32_t mid2 = edgeMidpoints[face.edges[2]];
                uint32_t mid3 = edgeMidpoints[face.edges[3]];
                
                glm::vec3 center = (m_vertices[mid0].position + m_vertices[mid1].position +
                                    m_vertices[mid2].position + m_vertices[mid3].position) * 0.25f;
                uint32_t centerV = addVertex(center);
                
                for (int i = 0; i < 4; i++) {
                    uint32_t mids[4] = { mid0, mid1, mid2, mid3 };
                    Face quad;
                    quad.vertexCount = 4;
                    quad.vertices[0] = face.vertices[i];
                    quad.vertices[1] = mids[i];
                    quad.vertices[2] = centerV;
                    quad.vertices[3] = mids[(i + 3) % 4];
                    newFaces.push_back(quad);
                }
            } else {
                // Triangle with all 3 edges split → 4 sub-triangles
                uint32_t mid0 = edgeMidpoints[face.edges[0]];
                uint32_t mid1 = edgeMidpoints[face.edges[1]];
                uint32_t mid2 = edgeMidpoints[face.edges[2]];
                
                // 3 corner triangles
                Face t1, t2, t3, t4;
                t1.vertexCount = t2.vertexCount = t3.vertexCount = t4.vertexCount = 3;
                
                t1.vertices[0] = face.vertices[0];
                t1.vertices[1] = mid0;
                t1.vertices[2] = mid2;
                
                t2.vertices[0] = mid0;
                t2.vertices[1] = face.vertices[1];
                t2.vertices[2] = mid1;
                
                t3.vertices[0] = mid2;
                t3.vertices[1] = mid1;
                t3.vertices[2] = face.vertices[2];
                
                // Center triangle
                t4.vertices[0] = mid0;
                t4.vertices[1] = mid1;
                t4.vertices[2] = mid2;
                
                newFaces.push_back(t1);
                newFaces.push_back(t2);
                newFaces.push_back(t3);
                newFaces.push_back(t4);
            }
        } else if (splitEdgePositions.size() == 3 && face.isQuad()) {
            // Quad with 3 edges split → 1 quad + 3 triangles
            // Find the unsplit edge
            int unsplitPos = -1;
            for (int i = 0; i < 4; i++) {
                bool found = false;
                for (int pos : splitEdgePositions) { if (pos == i) { found = true; break; } }
                if (!found) { unsplitPos = i; break; }
            }
            
            uint32_t mids[4] = {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX};
            for (int pos : splitEdgePositions) {
                mids[pos] = edgeMidpoints[face.edges[pos]];
            }
            
            // p0 = unsplit edge position, p1..p3 = next vertices in winding order
            int p0 = unsplitPos;
            int p1 = (unsplitPos + 1) % 4;
            int p2 = (unsplitPos + 2) % 4;
            int p3 = (unsplitPos + 3) % 4;
            
            // Boundary: v[p0], v[p1], mid[p1], v[p2], mid[p2], v[p3], mid[p3]
            // Decompose into 1 quad spanning unsplit edge + 3 tris
            
            // Quad: v[p0], v[p1], mid[p1], mid[p3]
            Face q1;
            q1.vertexCount = 4;
            q1.vertices[0] = face.vertices[p0];
            q1.vertices[1] = face.vertices[p1];
            q1.vertices[2] = mids[p1];
            q1.vertices[3] = mids[p3];
            newFaces.push_back(q1);
            
            // Tri: mid[p1], v[p2], mid[p2]
            Face t1;
            t1.vertexCount = 3;
            t1.vertices[0] = mids[p1];
            t1.vertices[1] = face.vertices[p2];
            t1.vertices[2] = mids[p2];
            newFaces.push_back(t1);
            
            // Tri: mid[p2], v[p3], mid[p3]
            Face t2;
            t2.vertexCount = 3;
            t2.vertices[0] = mids[p2];
            t2.vertices[1] = face.vertices[p3];
            t2.vertices[2] = mids[p3];
            newFaces.push_back(t2);
            
            // Center tri: mid[p1], mid[p2], mid[p3]
            Face t3;
            t3.vertexCount = 3;
            t3.vertices[0] = mids[p1];
            t3.vertices[1] = mids[p2];
            t3.vertices[2] = mids[p3];
            newFaces.push_back(t3);
        }
    }
    
    // Remove old faces and add new ones
    // Sort in reverse to safely remove
    std::sort(facesToRemove.rbegin(), facesToRemove.rend());
    for (size_t idx : facesToRemove) {
        m_faces.erase(m_faces.begin() + idx);
    }
    
    // Clear edge data since we need to rebuild
    m_edges.clear();
    m_edgeLookup.clear();
    
    // Re-add existing faces with new edge connectivity
    std::vector<Face> allFaces = m_faces;
    m_faces.clear();
    
    for (const Face& face : allFaces) {
        if (face.isQuad()) {
            addQuad(face.vertices[0], face.vertices[1], face.vertices[2], face.vertices[3]);
        } else {
            addTriangle(face.vertices[0], face.vertices[1], face.vertices[2]);
        }
    }
    
    for (const Face& face : newFaces) {
        if (face.isQuad()) {
            addQuad(face.vertices[0], face.vertices[1], face.vertices[2], face.vertices[3]);
        } else {
            addTriangle(face.vertices[0], face.vertices[1], face.vertices[2]);
        }
    }
    
    buildConnectivity();
    computeNormals();
    m_renderDataDirty = true;
}

void Mesh2::mergeSelectedVertices(float /*threshold*/) {
    // Find selected vertices (matches original fastMergeSelectedVertices)
    std::vector<uint32_t> selectedVerts;
    glm::vec3 center(0.0f);
    
    for (size_t i = 0; i < m_vertices.size(); i++) {
        if (m_vertices[i].selected) {
            selectedVerts.push_back(static_cast<uint32_t>(i));
            center += m_vertices[i].position;
        }
    }
    
    if (selectedVerts.size() < 2) return;
    
    center /= static_cast<float>(selectedVerts.size());
    
    // Create a NEW center vertex (like original: _vertices.add(center))
    uint32_t centerVertex = addVertex(center);
    
    // Create remap: all selected vertices -> new center vertex
    // (like original: node->data()->replaceVertex(centerNode))
    std::unordered_set<uint32_t> selectedSet(selectedVerts.begin(), selectedVerts.end());
    
    // Update all faces to use the center vertex instead of selected vertices
    for (Face& face : m_faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            if (selectedSet.count(face.vertices[i])) {
                face.vertices[i] = centerVertex;
            }
        }
    }
    
    // Process faces: collapse quads with duplicates to triangles, remove fully degenerate
    // This is like the original behavior where triangles shrink as vertices merge
    std::vector<Face> validFaces;
    for (const Face& face : m_faces) {
        // Collect unique vertices in order (preserving winding)
        std::vector<uint32_t> uniqueVerts;
        for (int i = 0; i < face.vertexCount; i++) {
            // Only add if not already the last added vertex (consecutive duplicates)
            if (uniqueVerts.empty() || uniqueVerts.back() != face.vertices[i]) {
                // Also check if it's the same as the first vertex (wrapping)
                if (i == face.vertexCount - 1 && !uniqueVerts.empty() && uniqueVerts[0] == face.vertices[i]) {
                    continue;  // Skip, would create duplicate with first
                }
                uniqueVerts.push_back(face.vertices[i]);
            }
        }
        
        // Also need to check for non-consecutive duplicates
        std::unordered_set<uint32_t> seen;
        std::vector<uint32_t> finalVerts;
        for (uint32_t v : uniqueVerts) {
            if (seen.find(v) == seen.end()) {
                seen.insert(v);
                finalVerts.push_back(v);
            }
        }
        
        if (finalVerts.size() >= 3) {
            // Valid face (either triangle or quad with 3+ unique verts)
            Face newFace;
            newFace.vertexCount = static_cast<int>(finalVerts.size());
            for (size_t i = 0; i < finalVerts.size() && i < 4; i++) {
                newFace.vertices[i] = finalVerts[i];
            }
            newFace.selected = face.selected;
            validFaces.push_back(newFace);
        }
        // Faces with < 3 unique vertices are fully degenerate, skip them
    }
    
    // Rebuild mesh with valid faces and used vertices
    std::vector<MeshVertex> oldVertices = m_vertices;
    
    clear();
    
    // Remap vertices - only add vertices that are actually used
    std::unordered_map<uint32_t, uint32_t> newVertexMap;
    for (const Face& face : validFaces) {
        for (int i = 0; i < face.vertexCount; i++) {
            uint32_t oldIdx = face.vertices[i];
            if (newVertexMap.find(oldIdx) == newVertexMap.end()) {
                uint32_t newIdx = addVertex(oldVertices[oldIdx].position);
                newVertexMap[oldIdx] = newIdx;
            }
        }
    }
    
    // Add faces with remapped vertices (this also rebuilds edges via findOrCreateEdge)
    for (const Face& face : validFaces) {
        if (face.vertexCount == 4) {
            addQuad(newVertexMap[face.vertices[0]], 
                    newVertexMap[face.vertices[1]], 
                    newVertexMap[face.vertices[2]], 
                    newVertexMap[face.vertices[3]]);
        } else {
            addTriangle(newVertexMap[face.vertices[0]], 
                        newVertexMap[face.vertices[1]], 
                        newVertexMap[face.vertices[2]]);
        }
    }
    
    buildConnectivity();
    computeNormals();
    m_renderDataDirty = true;
}

void Mesh2::triangulateSelected() {
    std::vector<Face> newFaces;
    
    for (size_t i = 0; i < m_faces.size(); i++) {
        Face& face = m_faces[i];
        
        if (!face.selected || !face.isQuad()) {
            newFaces.push_back(face);
            continue;
        }
        
        // Convert quad to two triangles
        Face tri1, tri2;
        tri1.vertexCount = tri2.vertexCount = 3;
        tri1.selected = tri2.selected = true;
        
        tri1.vertices[0] = face.vertices[0];
        tri1.vertices[1] = face.vertices[1];
        tri1.vertices[2] = face.vertices[2];
        tri1.uvs[0] = face.uvs[0];
        tri1.uvs[1] = face.uvs[1];
        tri1.uvs[2] = face.uvs[2];
        
        tri2.vertices[0] = face.vertices[0];
        tri2.vertices[1] = face.vertices[2];
        tri2.vertices[2] = face.vertices[3];
        tri2.uvs[0] = face.uvs[0];
        tri2.uvs[1] = face.uvs[2];
        tri2.uvs[2] = face.uvs[3];
        
        newFaces.push_back(tri1);
        newFaces.push_back(tri2);
    }
    
    // Rebuild with new faces
    m_faces.clear();
    m_edges.clear();
    m_edgeLookup.clear();
    
    for (const Face& face : newFaces) {
        if (face.isQuad()) {
            uint32_t fi = addQuad(face.vertices[0], face.vertices[1], face.vertices[2], face.vertices[3]);
            m_faces[fi].selected = face.selected;
            for (int u = 0; u < 4; u++) m_faces[fi].uvs[u] = face.uvs[u];
        } else {
            uint32_t fi = addTriangle(face.vertices[0], face.vertices[1], face.vertices[2]);
            m_faces[fi].selected = face.selected;
            for (int u = 0; u < 3; u++) m_faces[fi].uvs[u] = face.uvs[u];
        }
    }
    
    buildConnectivity();
    m_renderDataDirty = true;
}

void Mesh2::splitSelected() {
    switch (m_selectionMode) {
        case SelectionMode::Triangles:
            splitSelectedFaces();
            break;
        case SelectionMode::Edges:
            splitSelectedEdges();
            break;
        default:
            break;
    }
}

void Mesh2::splitSelectedFaces() {
    // Simple split: split each selected face into 4
    std::vector<Face> newFaces;
    std::unordered_map<uint64_t, uint32_t> edgeMidpoints;
    
    for (size_t fi = 0; fi < m_faces.size(); fi++) {
        Face& face = m_faces[fi];
        
        if (!face.selected) {
            newFaces.push_back(face);
            continue;
        }
        
        // Create midpoint for each edge
        uint32_t midVerts[4];
        for (int i = 0; i < face.vertexCount; i++) {
            uint32_t v0 = face.vertices[i];
            uint32_t v1 = face.vertices[(i + 1) % face.vertexCount];
            uint64_t key = makeEdgeKey(v0, v1);
            
            if (edgeMidpoints.find(key) == edgeMidpoints.end()) {
                glm::vec3 mid = (m_vertices[v0].position + m_vertices[v1].position) * 0.5f;
                edgeMidpoints[key] = addVertex(mid);
            }
            midVerts[i] = edgeMidpoints[key];
        }
        
        if (face.isQuad()) {
            // Create center vertex
            glm::vec3 center = (m_vertices[face.vertices[0]].position +
                               m_vertices[face.vertices[1]].position +
                               m_vertices[face.vertices[2]].position +
                               m_vertices[face.vertices[3]].position) * 0.25f;
            uint32_t centerV = addVertex(center);
            
            // Create 4 quads
            for (int i = 0; i < 4; i++) {
                Face quad;
                quad.vertexCount = 4;
                quad.selected = true;
                quad.vertices[0] = face.vertices[i];
                quad.vertices[1] = midVerts[i];
                quad.vertices[2] = centerV;
                quad.vertices[3] = midVerts[(i + 3) % 4];
                newFaces.push_back(quad);
            }
        } else {
            // Create 4 triangles
            Face tri1, tri2, tri3, tri4;
            tri1.vertexCount = tri2.vertexCount = tri3.vertexCount = tri4.vertexCount = 3;
            tri1.selected = tri2.selected = tri3.selected = tri4.selected = true;
            
            // Corner triangles
            tri1.vertices[0] = face.vertices[0];
            tri1.vertices[1] = midVerts[0];
            tri1.vertices[2] = midVerts[2];
            
            tri2.vertices[0] = midVerts[0];
            tri2.vertices[1] = face.vertices[1];
            tri2.vertices[2] = midVerts[1];
            
            tri3.vertices[0] = midVerts[2];
            tri3.vertices[1] = midVerts[1];
            tri3.vertices[2] = face.vertices[2];
            
            // Center triangle
            tri4.vertices[0] = midVerts[0];
            tri4.vertices[1] = midVerts[1];
            tri4.vertices[2] = midVerts[2];
            
            newFaces.push_back(tri1);
            newFaces.push_back(tri2);
            newFaces.push_back(tri3);
            newFaces.push_back(tri4);
        }
    }
    
    // Rebuild mesh
    m_faces.clear();
    m_edges.clear();
    m_edgeLookup.clear();
    
    for (const Face& face : newFaces) {
        if (face.isQuad()) {
            uint32_t fi = addQuad(face.vertices[0], face.vertices[1], face.vertices[2], face.vertices[3]);
            m_faces[fi].selected = face.selected;
        } else {
            uint32_t fi = addTriangle(face.vertices[0], face.vertices[1], face.vertices[2]);
            m_faces[fi].selected = face.selected;
        }
    }
    
    buildConnectivity();
    computeNormals();
    m_renderDataDirty = true;
}

void Mesh2::catmullClarkSubdivide(int level) {
    printf("[catmullClark] Called with level=%d, vertices=%zu, faces=%zu\n", 
           level, m_vertices.size(), m_faces.size());
    if (m_vertices.empty() || m_faces.empty() || level < 1) {
        printf("[catmullClark] Early exit: empty=%d, level=%d\n", 
               m_vertices.empty() || m_faces.empty(), level);
        return;
    }

    SubdivisionResult subdivResult = subdivideWithOpenSubdiv(m_vertices, m_faces, level);

    // Replace mesh with subdivided result
    m_vertices = std::move(subdivResult.vertices);
    m_faces = std::move(subdivResult.faces);
    m_edges.clear();
    m_edgeLookup.clear();

    // Rebuild edges from the new face topology
    for (size_t fi = 0; fi < m_faces.size(); fi++) {
        Face& face = m_faces[fi];
        for (int vi = 0; vi < face.vertexCount; vi++) {
            uint32_t v0 = face.vertices[vi];
            uint32_t v1 = face.vertices[(vi + 1) % face.vertexCount];
            uint32_t edgeIdx = findOrCreateEdge(v0, v1);
            face.edges[vi] = edgeIdx;
            // Register this face with the edge
            Edge& edge = m_edges[edgeIdx];
            if (edge.faces[0] == UINT32_MAX) {
                edge.faces[0] = static_cast<uint32_t>(fi);
            } else if (edge.faces[1] == UINT32_MAX) {
                edge.faces[1] = static_cast<uint32_t>(fi);
            }
        }
    }

    buildConnectivity();
    computeNormals();
    m_renderDataDirty = true;
}

void Mesh2::extrudeSelected() {
    std::unordered_map<uint32_t, uint32_t> vertexDuplicates;
    // Boundary edges stored with vertices in the winding order of the selected face
    std::vector<std::pair<uint32_t, uint32_t>> boundaryEdges;
    
    // Find boundary edges (edges where one face is selected and other is not, or only one face)
    // Store edge vertices in the order they appear in the selected face's winding
    for (size_t ei = 0; ei < m_edges.size(); ei++) {
        const Edge& edge = m_edges[ei];
        
        int selectedCount = 0;
        uint32_t selectedFaceIdx = UINT32_MAX;
        for (int i = 0; i < 2; i++) {
            if (edge.faces[i] != UINT32_MAX && m_faces[edge.faces[i]].selected) {
                selectedCount++;
                selectedFaceIdx = edge.faces[i];
            }
        }
        
        // Boundary if exactly one adjacent face is selected
        if (selectedCount == 1) {
            // Find the winding order of this edge in the selected face
            const Face& face = m_faces[selectedFaceIdx];
            for (int i = 0; i < face.vertexCount; i++) {
                uint32_t v0 = face.vertices[i];
                uint32_t v1 = face.vertices[(i + 1) % face.vertexCount];
                if (edge.containsVertex(v0) && edge.containsVertex(v1)) {
                    // v0→v1 is the direction in the selected face's winding
                    boundaryEdges.push_back({v0, v1});
                    break;
                }
            }
        }
    }
    
    // Duplicate vertices of selected faces
    for (Face& face : m_faces) {
        if (!face.selected) continue;
        
        for (int i = 0; i < face.vertexCount; i++) {
            uint32_t vi = face.vertices[i];
            if (vertexDuplicates.find(vi) == vertexDuplicates.end()) {
                uint32_t newVi = addVertex(m_vertices[vi].position);
                m_vertices[newVi].normal = m_vertices[vi].normal;
                vertexDuplicates[vi] = newVi;
            }
            face.vertices[i] = vertexDuplicates[vi];
        }
    }
    
    // Create side faces along boundary edges
    // The edge goes faceV0→faceV1 in the selected face's winding.
    // The side quad should maintain consistent winding with the mesh:
    // quad order: faceV0, faceV1, dup1, dup0
    for (const auto& be : boundaryEdges) {
        uint32_t faceV0 = be.first;
        uint32_t faceV1 = be.second;
        uint32_t dup0 = vertexDuplicates[faceV0];
        uint32_t dup1 = vertexDuplicates[faceV1];
        
        // Create quad with consistent winding
        addQuad(faceV0, faceV1, dup1, dup0);
    }
    
    // Rebuild edges
    m_edges.clear();
    m_edgeLookup.clear();
    
    std::vector<Face> faces = m_faces;
    m_faces.clear();
    
    for (const Face& face : faces) {
        if (face.isQuad()) {
            uint32_t fi = addQuad(face.vertices[0], face.vertices[1], face.vertices[2], face.vertices[3]);
            m_faces[fi].selected = face.selected;
        } else {
            uint32_t fi = addTriangle(face.vertices[0], face.vertices[1], face.vertices[2]);
            m_faces[fi].selected = face.selected;
        }
    }
    
    buildConnectivity();
    computeNormals();
    m_renderDataDirty = true;
}

// State capture for undo
void Mesh2::getState(std::vector<MeshVertex>& outVertices, 
                     std::vector<Face>& outFaces, 
                     std::vector<Edge>& outEdges) const {
    outVertices = m_vertices;
    outFaces = m_faces;
    outEdges = m_edges;
}

void Mesh2::setState(const std::vector<MeshVertex>& vertices,
                     const std::vector<Face>& faces,
                     const std::vector<Edge>& edges) {
    m_vertices = vertices;
    m_faces = faces;
    m_edges = edges;
    
    // Rebuild edge lookup
    m_edgeLookup.clear();
    for (size_t i = 0; i < m_edges.size(); i++) {
        uint64_t key = makeEdgeKey(m_edges[i].vertices[0], m_edges[i].vertices[1]);
        m_edgeLookup[key] = static_cast<uint32_t>(i);
    }
    
    // Rebuild connectivity and update GPU buffers
    buildConnectivity();
    computeNormals();
    m_renderDataDirty = true;
    createGPUBuffers();
}

void Mesh2::merge(const Mesh2* mesh) {
    if (!mesh) return;
    
    // Remember the offset for indices
    uint32_t vertexOffset = static_cast<uint32_t>(m_vertices.size());
    
    // Add all vertices from the other mesh
    for (const auto& vertex : mesh->m_vertices) {
        m_vertices.push_back(vertex);
    }
    
    // Add all faces with adjusted vertex indices
    for (const auto& face : mesh->m_faces) {
        Face newFace = face;
        for (size_t i = 0; i < face.vertexCount; i++) {
            newFace.vertices[i] += vertexOffset;
        }
        m_faces.push_back(newFace);
    }
    
    // Rebuild edges and connectivity
    m_edges.clear();
    m_edgeLookup.clear();
    
    for (size_t faceIdx = 0; faceIdx < m_faces.size(); faceIdx++) {
        Face& face = m_faces[faceIdx];
        for (uint32_t i = 0; i < face.vertexCount; i++) {
            uint32_t v0 = face.vertices[i];
            uint32_t v1 = face.vertices[(i + 1) % face.vertexCount];
            face.edges[i] = findOrCreateEdge(v0, v1);
        }
    }
    
    buildConnectivity();
    computeNormals();
    m_renderDataDirty = true;
    createGPUBuffers();
}

// Build render data
void Mesh2::buildRenderData() {
    m_renderVertices.clear();
    m_edgeRenderVertices.clear();
    
    for (const Face& face : m_faces) {
        glm::vec3 faceColor = face.selected ? m_selectionColor : m_color;
        
        // Calculate face normal
        glm::vec3 v0 = m_vertices[face.vertices[0]].position;
        glm::vec3 v1 = m_vertices[face.vertices[1]].position;
        glm::vec3 v2 = m_vertices[face.vertices[2]].position;
        glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        
        if (face.isQuad()) {
            glm::vec3 v3 = m_vertices[face.vertices[3]].position;
            
            // First triangle (vertices 0, 1, 2)
            m_renderVertices.push_back({v0, faceNormal, faceColor, face.uvs[0]});
            m_renderVertices.push_back({v1, faceNormal, faceColor, face.uvs[1]});
            m_renderVertices.push_back({v2, faceNormal, faceColor, face.uvs[2]});
            
            // Second triangle (vertices 0, 2, 3)
            m_renderVertices.push_back({v0, faceNormal, faceColor, face.uvs[0]});
            m_renderVertices.push_back({v2, faceNormal, faceColor, face.uvs[2]});
            m_renderVertices.push_back({v3, faceNormal, faceColor, face.uvs[3]});
        } else {
            m_renderVertices.push_back({v0, faceNormal, faceColor, face.uvs[0]});
            m_renderVertices.push_back({v1, faceNormal, faceColor, face.uvs[1]});
            m_renderVertices.push_back({v2, faceNormal, faceColor, face.uvs[2]});
        }
    }
    
    // Build edge render data
    const glm::vec3 seamColor(0.0f, 0.8f, 0.0f);  // Green for seam edges
    for (const Edge& edge : m_edges) {
        glm::vec3 edgeColor;
        if (edge.selected) {
            edgeColor = m_selectionColor;
        } else if (edge.isSeam) {
            edgeColor = seamColor;
        } else {
            edgeColor = m_wireframeColor;
        }
        glm::vec3 p0 = m_vertices[edge.vertices[0]].position;
        glm::vec3 p1 = m_vertices[edge.vertices[1]].position;
        
        m_edgeRenderVertices.push_back({p0, edgeColor});
        m_edgeRenderVertices.push_back({p1, edgeColor});
    }
    
    m_renderDataDirty = false;
}

// GPU
void Mesh2::createGPUBuffers() {
    // Skip GPU operations when disabled (for testing)
    if (s_disableGPU) return;
    
    if (m_renderDataDirty) {
        buildRenderData();
    }
    
    deleteGPUBuffers();
    
    // Solid mesh VAO/VBO
    if (!m_renderVertices.empty()) {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_renderVertices.size() * sizeof(Vertex),
                     m_renderVertices.data(), GL_STATIC_DRAW);
        
        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Color
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        // UV
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9 * sizeof(float)));
        glEnableVertexAttribArray(3);
        
        glBindVertexArray(0);
    }
    
    // Edge VAO/VBO
    if (!m_edgeRenderVertices.empty()) {
        glGenVertexArrays(1, &m_edgeVao);
        glGenBuffers(1, &m_edgeVbo);
        
        glBindVertexArray(m_edgeVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_edgeVbo);
        glBufferData(GL_ARRAY_BUFFER, m_edgeRenderVertices.size() * sizeof(LineVertex),
                     m_edgeRenderVertices.data(), GL_STATIC_DRAW);
        
        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);
        glEnableVertexAttribArray(0);
        // Color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
    }
    
    m_gpuBuffersCreated = true;
}

void Mesh2::updateGPUBuffers() {
    // Skip GPU operations when disabled (for testing)
    if (s_disableGPU) return;
    
    // Only update if buffers exist and data is dirty
    if (!m_gpuBuffersCreated) {
        createGPUBuffers();
        return;
    }
    
    if (!m_renderDataDirty) return;
    
    // Rebuild render data
    buildRenderData();
    
    // Re-upload vertex data
    if (m_vbo && !m_renderVertices.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_renderVertices.size() * sizeof(Vertex),
                     m_renderVertices.data(), GL_STATIC_DRAW);
    }
    
    // Re-upload edge data
    if (m_edgeVbo && !m_edgeRenderVertices.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, m_edgeVbo);
        glBufferData(GL_ARRAY_BUFFER, m_edgeRenderVertices.size() * sizeof(Vertex),
                     m_edgeRenderVertices.data(), GL_STATIC_DRAW);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh2::deleteGPUBuffers() {
    // Skip GPU operations when disabled (for testing)
    if (s_disableGPU) return;
    
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    if (m_edgeVao) { glDeleteVertexArrays(1, &m_edgeVao); m_edgeVao = 0; }
    if (m_edgeVbo) { glDeleteBuffers(1, &m_edgeVbo); m_edgeVbo = 0; }
    m_gpuBuffersCreated = false;
}

void Mesh2::draw(ViewMode mode) const {
    if (!m_gpuBuffersCreated) return;
    
    if (mode == ViewMode::Solid || mode == ViewMode::SolidWireframe) {
        if (m_vao && !m_renderVertices.empty()) {
            glBindVertexArray(m_vao);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_renderVertices.size()));
            glBindVertexArray(0);
        }
    }
    
    if (mode == ViewMode::Wireframe || mode == ViewMode::SolidWireframe) {
        if (m_edgeVao && !m_edgeRenderVertices.empty()) {
            glBindVertexArray(m_edgeVao);
#ifndef EMSCRIPTEN_BUILD
            glLineWidth(1.5f);
#endif
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_edgeRenderVertices.size()));
#ifndef EMSCRIPTEN_BUILD
            glLineWidth(1.0f);
#endif
            glBindVertexArray(0);
        }
    }
}

void Mesh2::drawForSelection(Shader& selectionShader) const {
    // Draw each face with its index encoded as color
    // Ensure we're using the index-based uniform (not color override)
    selectionShader.setBool("uUseColorOverride", false);
    
    for (size_t i = 0; i < m_faces.size(); i++) {
        const Face& face = m_faces[i];
        
        uint32_t colorIndex = static_cast<uint32_t>(i) + 1;
        selectionShader.setUInt("uColorIndex", colorIndex);
        
        // Draw face triangles
        std::vector<glm::vec3> positions;
        if (face.isQuad()) {
            positions.push_back(m_vertices[face.vertices[0]].position);
            positions.push_back(m_vertices[face.vertices[1]].position);
            positions.push_back(m_vertices[face.vertices[2]].position);
            positions.push_back(m_vertices[face.vertices[0]].position);
            positions.push_back(m_vertices[face.vertices[2]].position);
            positions.push_back(m_vertices[face.vertices[3]].position);
        } else {
            positions.push_back(m_vertices[face.vertices[0]].position);
            positions.push_back(m_vertices[face.vertices[1]].position);
            positions.push_back(m_vertices[face.vertices[2]].position);
        }
        
        // Simple immediate mode style (for selection only, not performance critical)
        GLuint tempVao, tempVbo;
        glGenVertexArrays(1, &tempVao);
        glGenBuffers(1, &tempVbo);
        
        glBindVertexArray(tempVao);
        glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STREAM_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(positions.size()));
        
        glDeleteBuffers(1, &tempVbo);
        glDeleteVertexArrays(1, &tempVao);
    }
}

void Mesh2::drawVerticesForSelection(Shader& selectionShader) const {
    // Draw each vertex as a point with its index encoded as color
    // Matching original MeshMaker: draws solid mesh first as background (color 0),
    // then draws vertices on top
    
    if (m_vertices.empty()) return;
    
    selectionShader.setBool("uUseColorOverride", true);
    
    // First, draw solid mesh as background with color index 0 (no selection)
    selectionShader.setVec4("uColorOverride", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    
    // Draw all faces with color 0
    for (const Face& face : m_faces) {
        std::vector<glm::vec3> positions;
        if (face.isQuad()) {
            positions.push_back(m_vertices[face.vertices[0]].position);
            positions.push_back(m_vertices[face.vertices[1]].position);
            positions.push_back(m_vertices[face.vertices[2]].position);
            positions.push_back(m_vertices[face.vertices[0]].position);
            positions.push_back(m_vertices[face.vertices[2]].position);
            positions.push_back(m_vertices[face.vertices[3]].position);
        } else {
            positions.push_back(m_vertices[face.vertices[0]].position);
            positions.push_back(m_vertices[face.vertices[1]].position);
            positions.push_back(m_vertices[face.vertices[2]].position);
        }
        
        GLuint tempVao, tempVbo;
        glGenVertexArrays(1, &tempVao);
        glGenBuffers(1, &tempVbo);
        glBindVertexArray(tempVao);
        glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(positions.size()));
        glDeleteBuffers(1, &tempVbo);
        glDeleteVertexArrays(1, &tempVao);
    }
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    // Now draw vertices as points with their indices
    selectionShader.setFloat("uPointSize", 8.0f);  // Larger for easier selection
    
    for (size_t i = 0; i < m_vertices.size(); i++) {
        uint32_t colorIndex = static_cast<uint32_t>(i) + 1;
        uint8_t r = colorIndex & 0xFF;
        uint8_t g = (colorIndex >> 8) & 0xFF;
        uint8_t b = (colorIndex >> 16) & 0xFF;
        
        selectionShader.setVec4("uColorOverride", 
            glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f));
        
        glm::vec3 pos = m_vertices[i].position;
        
        GLuint tempVao, tempVbo;
        glGenVertexArrays(1, &tempVao);
        glGenBuffers(1, &tempVbo);
        glBindVertexArray(tempVao);
        glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &pos, GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_POINTS, 0, 1);
        glDeleteBuffers(1, &tempVbo);
        glDeleteVertexArrays(1, &tempVao);
    }
    
    selectionShader.setFloat("uPointSize", 1.0f);
    selectionShader.setBool("uUseColorOverride", false);
}

void Mesh2::drawEdgesForSelection(Shader& selectionShader) const {
    // Draw each edge as a line with its index encoded as color
    
    if (m_edges.empty()) return;
    
    selectionShader.setBool("uUseColorOverride", true);
    
    // First, draw solid mesh as background with color index 0 (no selection)
    selectionShader.setVec4("uColorOverride", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    
    for (const Face& face : m_faces) {
        std::vector<glm::vec3> positions;
        if (face.isQuad()) {
            positions.push_back(m_vertices[face.vertices[0]].position);
            positions.push_back(m_vertices[face.vertices[1]].position);
            positions.push_back(m_vertices[face.vertices[2]].position);
            positions.push_back(m_vertices[face.vertices[0]].position);
            positions.push_back(m_vertices[face.vertices[2]].position);
            positions.push_back(m_vertices[face.vertices[3]].position);
        } else {
            positions.push_back(m_vertices[face.vertices[0]].position);
            positions.push_back(m_vertices[face.vertices[1]].position);
            positions.push_back(m_vertices[face.vertices[2]].position);
        }
        
        GLuint tempVao, tempVbo;
        glGenVertexArrays(1, &tempVao);
        glGenBuffers(1, &tempVbo);
        glBindVertexArray(tempVao);
        glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(positions.size()));
        glDeleteBuffers(1, &tempVbo);
        glDeleteVertexArrays(1, &tempVao);
    }
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    // Now draw edges as lines with their indices
#ifndef EMSCRIPTEN_BUILD
    glLineWidth(5.0f);  // Thicker for easier selection
#endif
    
    for (size_t i = 0; i < m_edges.size(); i++) {
        const Edge& edge = m_edges[i];
        
        uint32_t colorIndex = static_cast<uint32_t>(i) + 1;
        uint8_t r = colorIndex & 0xFF;
        uint8_t g = (colorIndex >> 8) & 0xFF;
        uint8_t b = (colorIndex >> 16) & 0xFF;
        
        selectionShader.setVec4("uColorOverride", 
            glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f));
        
        glm::vec3 positions[2] = {
            m_vertices[edge.vertices[0]].position,
            m_vertices[edge.vertices[1]].position
        };
        
        GLuint tempVao, tempVbo;
        glGenVertexArrays(1, &tempVao);
        glGenBuffers(1, &tempVbo);
        glBindVertexArray(tempVao);
        glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), positions, GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINES, 0, 2);
        glDeleteBuffers(1, &tempVbo);
        glDeleteVertexArrays(1, &tempVao);
    }
    
#ifndef EMSCRIPTEN_BUILD
    glLineWidth(1.0f);
#endif
    selectionShader.setBool("uUseColorOverride", false);
}

void Mesh2::drawVertices(Shader& pointShader) const {
    // Draw vertices as colored points: blue = deselected, red = selected
    // Matching original MeshMaker behavior from Mesh2.drawing.mm
    
    if (m_vertices.empty()) return;
    
    const glm::vec3 selectedColor(1.0f, 0.0f, 0.0f);   // Red
    const glm::vec3 normalColor(0.0f, 0.0f, 1.0f);     // Blue
    
    // Build vertex data with colors
    struct ColoredPoint {
        glm::vec3 position;
        glm::vec3 color;
    };
    
    std::vector<ColoredPoint> points;
    points.reserve(m_vertices.size());
    
    for (const auto& vertex : m_vertices) {
        ColoredPoint pt;
        pt.position = vertex.position;
        pt.color = vertex.selected ? selectedColor : normalColor;
        points.push_back(pt);
    }
    
    // Use temporary VAO/VBO for drawing
    GLuint tempVao, tempVbo;
    glGenVertexArrays(1, &tempVao);
    glGenBuffers(1, &tempVbo);
    
    glBindVertexArray(tempVao);
    glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(ColoredPoint), points.data(), GL_STREAM_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), (void*)offsetof(ColoredPoint, color));
    glEnableVertexAttribArray(1);
    
    pointShader.setFloat("uPointSize", 5.0f);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
    pointShader.setFloat("uPointSize", 1.0f);
    
    glDeleteBuffers(1, &tempVbo);
    glDeleteVertexArrays(1, &tempVao);
}

void Mesh2::drawEdges(Shader& lineShader) const {
    // Draw edges with selection colors: darker base color for deselected, red for selected
    // Green for seams (visible in 3D viewport too)
    // Matching original MeshMaker behavior
    
    if (m_edges.empty()) return;
    
    const glm::vec3 selectedColor(0.8f, 0.0f, 0.0f);   // Red
    const glm::vec3 seamColor(0.0f, 0.8f, 0.0f);       // Green for seams
    const glm::vec3 normalColor(m_wireframeColor.r - 0.2f, m_wireframeColor.g - 0.2f, m_wireframeColor.b - 0.2f);
    
    struct ColoredLine {
        glm::vec3 position;
        glm::vec3 color;
    };
    
    std::vector<ColoredLine> lines;
    lines.reserve(m_edges.size() * 2);
    
    for (const auto& edge : m_edges) {
        const glm::vec3& v0 = m_vertices[edge.vertices[0]].position;
        const glm::vec3& v1 = m_vertices[edge.vertices[1]].position;
        
        // Color priority: selected > seam > normal
        glm::vec3 color;
        if (edge.selected) {
            color = selectedColor;
        } else if (edge.isSeam) {
            color = seamColor;
        } else {
            color = normalColor;
        }
        
        lines.push_back({v0, color});
        lines.push_back({v1, color});
    }
    
    // Use temporary VAO/VBO for drawing
    GLuint tempVao, tempVbo;
    glGenVertexArrays(1, &tempVao);
    glGenBuffers(1, &tempVbo);
    
    glBindVertexArray(tempVao);
    glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(ColoredLine), lines.data(), GL_STREAM_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredLine), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredLine), (void*)offsetof(ColoredLine, color));
    glEnableVertexAttribArray(1);
    
#ifndef EMSCRIPTEN_BUILD
    glLineWidth(1.5f);
#endif
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines.size()));
#ifndef EMSCRIPTEN_BUILD
    glLineWidth(1.0f);
#endif
    
    glDeleteBuffers(1, &tempVbo);
    glDeleteVertexArrays(1, &tempVao);
}

void Mesh2::drawNormals(Shader& lineShader) const {
    if (m_faces.empty()) return;
    
    const glm::vec3 normalColor(0.0f, 0.8f, 0.8f);  // Cyan for face normals
    const float normalLength = 0.3f;
    
    struct ColoredLine {
        glm::vec3 position;
        glm::vec3 color;
    };
    
    std::vector<ColoredLine> lines;
    lines.reserve(m_faces.size() * 2);
    
    for (const Face& face : m_faces) {
        // Compute face center
        glm::vec3 center(0.0f);
        for (int i = 0; i < face.vertexCount; i++) {
            center += m_vertices[face.vertices[i]].position;
        }
        center /= static_cast<float>(face.vertexCount);
        
        // Compute face normal (same as buildRenderData)
        glm::vec3 v0 = m_vertices[face.vertices[0]].position;
        glm::vec3 v1 = m_vertices[face.vertices[1]].position;
        glm::vec3 v2 = m_vertices[face.vertices[2]].position;
        glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        
        lines.push_back({center, normalColor});
        lines.push_back({center + faceNormal * normalLength, normalColor});
    }
    
    GLuint tempVao, tempVbo;
    glGenVertexArrays(1, &tempVao);
    glGenBuffers(1, &tempVbo);
    
    glBindVertexArray(tempVao);
    glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(ColoredLine), lines.data(), GL_STREAM_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredLine), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredLine), (void*)offsetof(ColoredLine, color));
    glEnableVertexAttribArray(1);
    
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines.size()));
    
    glDeleteBuffers(1, &tempVbo);
    glDeleteVertexArrays(1, &tempVao);
}

// Primitive generation
void Mesh2::makePlane() {
    clear();
    
    uint32_t v0 = addVertex(glm::vec3(-1.0f, 0.0f, -1.0f));
    uint32_t v1 = addVertex(glm::vec3( 1.0f, 0.0f, -1.0f));
    uint32_t v2 = addVertex(glm::vec3( 1.0f, 0.0f,  1.0f));
    uint32_t v3 = addVertex(glm::vec3(-1.0f, 0.0f,  1.0f));
    
    uint32_t faceIndex = addQuad(v0, v1, v2, v3);
    setFaceUVs(faceIndex, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    
    buildConnectivity();
    computeNormals();
}

void Mesh2::makeCube() {
    clear();
    
    // 8 vertices of a cube
    uint32_t v[8];
    v[0] = addVertex(glm::vec3(-1, -1, -1));
    v[1] = addVertex(glm::vec3( 1, -1, -1));
    v[2] = addVertex(glm::vec3( 1,  1, -1));
    v[3] = addVertex(glm::vec3(-1,  1, -1));
    v[4] = addVertex(glm::vec3(-1, -1,  1));
    v[5] = addVertex(glm::vec3( 1, -1,  1));
    v[6] = addVertex(glm::vec3( 1,  1,  1));
    v[7] = addVertex(glm::vec3(-1,  1,  1));
    
    // Standard UV coordinates for each face (0,0 -> 1,0 -> 1,1 -> 0,1)
    glm::vec2 uv0(0.0f, 0.0f), uv1(1.0f, 0.0f), uv2(1.0f, 1.0f), uv3(0.0f, 1.0f);
    
    // 6 faces (quads) with UV coordinates
    uint32_t f;
    f = addQuad(v[0], v[1], v[2], v[3]); setFaceUVs(f, uv0, uv1, uv2, uv3); // Front
    f = addQuad(v[5], v[4], v[7], v[6]); setFaceUVs(f, uv0, uv1, uv2, uv3); // Back
    f = addQuad(v[4], v[0], v[3], v[7]); setFaceUVs(f, uv0, uv1, uv2, uv3); // Left
    f = addQuad(v[1], v[5], v[6], v[2]); setFaceUVs(f, uv0, uv1, uv2, uv3); // Right
    f = addQuad(v[3], v[2], v[6], v[7]); setFaceUVs(f, uv0, uv1, uv2, uv3); // Top
    f = addQuad(v[4], v[5], v[1], v[0]); setFaceUVs(f, uv0, uv1, uv2, uv3); // Bottom
    
    buildConnectivity();
    computeNormals();
}

void Mesh2::makeCylinder(uint32_t steps) {
    clear();
    
    float radius = 1.0f;
    float height = 2.0f;
    
    // Create vertices
    std::vector<uint32_t> bottomRing, topRing;
    
    for (uint32_t i = 0; i < steps; i++) {
        float angle = 2.0f * static_cast<float>(M_PI) * i / steps;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);
        
        bottomRing.push_back(addVertex(glm::vec3(x, -height/2, z)));
        topRing.push_back(addVertex(glm::vec3(x, height/2, z)));
    }
    
    uint32_t bottomCenter = addVertex(glm::vec3(0, -height/2, 0));
    uint32_t topCenter = addVertex(glm::vec3(0, height/2, 0));
    
    // Side quads with cylindrical UV mapping
    for (uint32_t i = 0; i < steps; i++) {
        uint32_t next = (i + 1) % steps;
        uint32_t faceIndex = addQuad(bottomRing[i], bottomRing[next], topRing[next], topRing[i]);
        
        float u0 = static_cast<float>(i) / steps;
        float u1 = static_cast<float>(i + 1) / steps;
        setFaceUVs(faceIndex, glm::vec2(u0, 0.0f), glm::vec2(u1, 0.0f), glm::vec2(u1, 1.0f), glm::vec2(u0, 1.0f));
    }
    
    // Top and bottom caps (triangles) with radial UV mapping
    for (uint32_t i = 0; i < steps; i++) {
        uint32_t next = (i + 1) % steps;
        
        float angle0 = 2.0f * static_cast<float>(M_PI) * i / steps;
        float angle1 = 2.0f * static_cast<float>(M_PI) * next / steps;
        
        // Bottom cap
        uint32_t bottomFace = addTriangle(bottomCenter, bottomRing[next], bottomRing[i]);
        setFaceUVs(bottomFace, 
            glm::vec2(0.5f, 0.5f),  // center
            glm::vec2(0.5f + 0.5f * cosf(angle1), 0.5f + 0.5f * sinf(angle1)),
            glm::vec2(0.5f + 0.5f * cosf(angle0), 0.5f + 0.5f * sinf(angle0)));
        
        // Top cap
        uint32_t topFace = addTriangle(topCenter, topRing[i], topRing[next]);
        setFaceUVs(topFace,
            glm::vec2(0.5f, 0.5f),  // center
            glm::vec2(0.5f + 0.5f * cosf(angle0), 0.5f + 0.5f * sinf(angle0)),
            glm::vec2(0.5f + 0.5f * cosf(angle1), 0.5f + 0.5f * sinf(angle1)));
    }
    
    buildConnectivity();
    computeNormals();
}

void Mesh2::makeSphere(uint32_t steps) {
    clear();
    
    float radius = 1.0f;
    uint32_t rings = steps;
    uint32_t sectors = steps * 2;
    
    // Create vertices
    std::vector<std::vector<uint32_t>> grid(rings + 1);
    
    for (uint32_t r = 0; r <= rings; r++) {
        float phi = static_cast<float>(M_PI) * r / rings;
        float y = radius * cosf(phi);
        float ringRadius = radius * sinf(phi);
        
        for (uint32_t s = 0; s <= sectors; s++) {
            float theta = 2.0f * static_cast<float>(M_PI) * s / sectors;
            float x = ringRadius * cosf(theta);
            float z = ringRadius * sinf(theta);
            
            grid[r].push_back(addVertex(glm::vec3(x, y, z)));
        }
    }
    
    // Create faces with spherical UV mapping
    for (uint32_t r = 0; r < rings; r++) {
        for (uint32_t s = 0; s < sectors; s++) {
            uint32_t v0 = grid[r][s];
            uint32_t v1 = grid[r][s + 1];
            uint32_t v2 = grid[r + 1][s + 1];
            uint32_t v3 = grid[r + 1][s];
            
            // Calculate UV coordinates based on ring and sector
            float u0 = static_cast<float>(s) / sectors;
            float u1 = static_cast<float>(s + 1) / sectors;
            float v_top = static_cast<float>(r) / rings;
            float v_bottom = static_cast<float>(r + 1) / rings;
            
            // Use triangles at poles, quads elsewhere
            if (r == 0) {
                // Top pole - triangle
                uint32_t faceIndex = addTriangle(v0, v3, v2);
                setFaceUVs(faceIndex, 
                    glm::vec2((u0 + u1) * 0.5f, v_top),  // pole vertex gets middle U
                    glm::vec2(u0, v_bottom),
                    glm::vec2(u1, v_bottom));
            } else if (r == rings - 1) {
                // Bottom pole - triangle
                uint32_t faceIndex = addTriangle(v0, v1, v2);
                setFaceUVs(faceIndex,
                    glm::vec2(u0, v_top),
                    glm::vec2(u1, v_top),
                    glm::vec2((u0 + u1) * 0.5f, v_bottom));  // pole vertex gets middle U
            } else {
                // Regular quad
                uint32_t faceIndex = addQuad(v0, v1, v2, v3);
                setFaceUVs(faceIndex,
                    glm::vec2(u0, v_top),
                    glm::vec2(u1, v_top),
                    glm::vec2(u1, v_bottom),
                    glm::vec2(u0, v_bottom));
            }
        }
    }
    
    buildConnectivity();
    computeNormals();
}

void Mesh2::makeIcosahedron() {
    clear();
    
    float phi = (1.0f + sqrtf(5.0f)) / 2.0f;
    float a = 1.0f;
    float b = 1.0f / phi;
    
    // 12 vertices
    uint32_t v[12];
    v[0] = addVertex(glm::normalize(glm::vec3( 0,  b, -a)));
    v[1] = addVertex(glm::normalize(glm::vec3( b,  a,  0)));
    v[2] = addVertex(glm::normalize(glm::vec3(-b,  a,  0)));
    v[3] = addVertex(glm::normalize(glm::vec3( 0,  b,  a)));
    v[4] = addVertex(glm::normalize(glm::vec3( 0, -b,  a)));
    v[5] = addVertex(glm::normalize(glm::vec3(-a,  0,  b)));
    v[6] = addVertex(glm::normalize(glm::vec3( 0, -b, -a)));
    v[7] = addVertex(glm::normalize(glm::vec3( a,  0, -b)));
    v[8] = addVertex(glm::normalize(glm::vec3( a,  0,  b)));
    v[9] = addVertex(glm::normalize(glm::vec3(-a,  0, -b)));
    v[10] = addVertex(glm::normalize(glm::vec3( b, -a,  0)));
    v[11] = addVertex(glm::normalize(glm::vec3(-b, -a,  0)));
    
    // 20 triangular faces with spherical UV mapping (using vertex positions)
    auto sphericalUV = [](const glm::vec3& pos) -> glm::vec2 {
        float u = 0.5f + atan2f(pos.z, pos.x) / (2.0f * static_cast<float>(M_PI));
        float v = 0.5f - asinf(pos.y) / static_cast<float>(M_PI);
        return glm::vec2(u, v);
    };
    
    auto addTriWithUVs = [&](uint32_t i0, uint32_t i1, uint32_t i2) {
        uint32_t faceIndex = addTriangle(v[i0], v[i1], v[i2]);
        setFaceUVs(faceIndex,
            sphericalUV(m_vertices[v[i0]].position),
            sphericalUV(m_vertices[v[i1]].position),
            sphericalUV(m_vertices[v[i2]].position));
    };
    
    addTriWithUVs(2, 1, 0);
    addTriWithUVs(1, 2, 3);
    addTriWithUVs(5, 4, 3);
    addTriWithUVs(4, 8, 3);
    addTriWithUVs(7, 6, 0);
    addTriWithUVs(6, 9, 0);
    addTriWithUVs(11, 10, 4);
    addTriWithUVs(10, 11, 6);
    addTriWithUVs(9, 5, 2);
    addTriWithUVs(5, 9, 11);
    addTriWithUVs(8, 7, 1);
    addTriWithUVs(7, 8, 10);
    addTriWithUVs(2, 5, 3);
    addTriWithUVs(8, 1, 3);
    addTriWithUVs(9, 2, 0);
    addTriWithUVs(1, 7, 0);
    addTriWithUVs(11, 9, 6);
    addTriWithUVs(7, 10, 6);
    addTriWithUVs(5, 11, 4);
    addTriWithUVs(10, 8, 4);
    
    buildConnectivity();
    computeNormals();
}

void Mesh2::make(MeshType type, uint32_t steps) {
    switch (type) {
        case MeshType::Plane: makePlane(); break;
        case MeshType::Cube: makeCube(); break;
        case MeshType::Cylinder: makeCylinder(steps); break;
        case MeshType::Sphere: makeSphere(steps); break;
        case MeshType::Icosahedron: makeIcosahedron(); break;
    }
}

// ============================================================================
// UV Projection and Manipulation
// ============================================================================

glm::vec3 computeFaceNormal(const std::vector<MeshVertex>& vertices, const Face& face) {
    if (face.vertexCount < 3) return glm::vec3(0.0f, 1.0f, 0.0f);
    
    const glm::vec3& v0 = vertices[face.vertices[0]].position;
    const glm::vec3& v1 = vertices[face.vertices[1]].position;
    const glm::vec3& v2 = vertices[face.vertices[2]].position;
    
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    
    return normal;
}

glm::vec3 computeFaceCenter(const std::vector<MeshVertex>& vertices, const Face& face) {
    glm::vec3 center(0.0f);
    for (uint8_t i = 0; i < face.vertexCount; i++) {
        center += vertices[face.vertices[i]].position;
    }
    return center / static_cast<float>(face.vertexCount);
}

void Mesh2::unwrapSelectedUVs(UVProjection projection) {
    if (projection == UVProjection::SeamBased) {
        unwrapSeamBased(true);
        return;
    }
    
    // Compute bounds for normalization
    glm::vec3 minBounds(FLT_MAX), maxBounds(-FLT_MAX);
    for (const auto& face : m_faces) {
        if (!face.selected) continue;
        for (uint8_t i = 0; i < face.vertexCount; i++) {
            const glm::vec3& pos = m_vertices[face.vertices[i]].position;
            minBounds = glm::min(minBounds, pos);
            maxBounds = glm::max(maxBounds, pos);
        }
    }
    
    glm::vec3 size = maxBounds - minBounds;
    glm::vec3 center = (minBounds + maxBounds) * 0.5f;
    float maxSize = glm::max(size.x, glm::max(size.y, size.z));
    if (maxSize < 0.0001f) maxSize = 1.0f;
    
    // For Box projection, use cube net (cross/T) with each face getting a full texture repeat
    // The cross extends into repeating texture space (beyond 0-1):
    //       [+Y]             (1,2)
    //   [-X][+Z][+X][-Z]     (0,1)(1,1)(2,1)(3,1)
    //       [-Y]             (1,0)
    // Each cell is 1x1 in UV space, so faces get full texture coverage
    
    for (size_t fi = 0; fi < m_faces.size(); fi++) {
        Face& face = m_faces[fi];
        if (!face.selected) continue;
        
        switch (projection) {
            case UVProjection::Box: {
                // Determine dominant axis from face normal
                glm::vec3 normal = computeFaceNormal(m_vertices, face);
                glm::vec3 absNormal = glm::abs(normal);
                
                // Each cell is 1x1 in UV space (uses texture repeat)
                float cellOffsetU, cellOffsetV;
                
                if (absNormal.z >= absNormal.x && absNormal.z >= absNormal.y) {
                    // Z-facing faces
                    for (uint8_t i = 0; i < face.vertexCount; i++) {
                        const glm::vec3& pos = m_vertices[face.vertices[i]].position;
                        glm::vec3 n = (pos - minBounds) / maxSize;
                        glm::vec2 uv;
                        if (normal.z >= 0) {
                            // +Z: center (col 1, row 1)
                            cellOffsetU = 1.0f;
                            cellOffsetV = 1.0f;
                            uv = glm::vec2(n.x, n.y);
                        } else {
                            // -Z: right side (col 3, row 1)
                            cellOffsetU = 3.0f;
                            cellOffsetV = 1.0f;
                            uv = glm::vec2(1.0f - n.x, n.y);
                        }
                        face.uvs[i] = glm::vec2(cellOffsetU + uv.x, cellOffsetV + uv.y);
                    }
                } else if (absNormal.x >= absNormal.y && absNormal.x >= absNormal.z) {
                    // X-facing faces
                    for (uint8_t i = 0; i < face.vertexCount; i++) {
                        const glm::vec3& pos = m_vertices[face.vertices[i]].position;
                        glm::vec3 n = (pos - minBounds) / maxSize;
                        glm::vec2 uv;
                        if (normal.x >= 0) {
                            // +X: right of +Z (col 2, row 1)
                            cellOffsetU = 2.0f;
                            cellOffsetV = 1.0f;
                            uv = glm::vec2(1.0f - n.z, n.y);
                        } else {
                            // -X: left of +Z (col 0, row 1)
                            cellOffsetU = 0.0f;
                            cellOffsetV = 1.0f;
                            uv = glm::vec2(n.z, n.y);
                        }
                        face.uvs[i] = glm::vec2(cellOffsetU + uv.x, cellOffsetV + uv.y);
                    }
                } else {
                    // Y-facing faces
                    for (uint8_t i = 0; i < face.vertexCount; i++) {
                        const glm::vec3& pos = m_vertices[face.vertices[i]].position;
                        glm::vec3 n = (pos - minBounds) / maxSize;
                        glm::vec2 uv;
                        if (normal.y >= 0) {
                            // +Y: top (col 1, row 2)
                            cellOffsetU = 1.0f;
                            cellOffsetV = 2.0f;
                            uv = glm::vec2(n.x, 1.0f - n.z);
                        } else {
                            // -Y: bottom (col 1, row 0)
                            cellOffsetU = 1.0f;
                            cellOffsetV = 0.0f;
                            uv = glm::vec2(n.x, n.z);
                        }
                        face.uvs[i] = glm::vec2(cellOffsetU + uv.x, cellOffsetV + uv.y);
                    }
                }
                break;
            }
            
            case UVProjection::Planar: {
                // Project onto XY plane (from Z axis)
                for (uint8_t i = 0; i < face.vertexCount; i++) {
                    const glm::vec3& pos = m_vertices[face.vertices[i]].position;
                    glm::vec3 normalized = (pos - minBounds) / maxSize;
                    face.uvs[i] = glm::vec2(normalized.x, normalized.y);
                }
                break;
            }
            
            case UVProjection::Cylindrical: {
                // Cylindrical projection around Y axis
                for (uint8_t i = 0; i < face.vertexCount; i++) {
                    const glm::vec3& pos = m_vertices[face.vertices[i]].position;
                    glm::vec3 fromCenter = pos - center;
                    
                    float angle = atan2f(fromCenter.z, fromCenter.x);
                    float u = (angle + static_cast<float>(M_PI)) / (2.0f * static_cast<float>(M_PI));
                    float v = (pos.y - minBounds.y) / size.y;
                    
                    face.uvs[i] = glm::vec2(u, v);
                }
                break;
            }
            
            case UVProjection::Spherical: {
                // Spherical projection (latitude/longitude)
                for (uint8_t i = 0; i < face.vertexCount; i++) {
                    const glm::vec3& pos = m_vertices[face.vertices[i]].position;
                    glm::vec3 dir = glm::normalize(pos - center);
                    
                    float u = 0.5f + atan2f(dir.z, dir.x) / (2.0f * static_cast<float>(M_PI));
                    float v = 0.5f + asinf(glm::clamp(dir.y, -1.0f, 1.0f)) / static_cast<float>(M_PI);
                    
                    face.uvs[i] = glm::vec2(u, v);
                }
                break;
            }
            
            case UVProjection::SeamBased: {
                // Handled by unwrapSeamBased, not here
                break;
            }
        }
    }
    
    m_renderDataDirty = true;
}

void Mesh2::unwrapAllUVs(UVProjection projection) {
    if (projection == UVProjection::SeamBased) {
        unwrapSeamBased(false);
        return;
    }
    
    // Select all faces temporarily
    std::vector<bool> wasSelected(m_faces.size());
    for (size_t i = 0; i < m_faces.size(); i++) {
        wasSelected[i] = m_faces[i].selected;
        m_faces[i].selected = true;
    }
    
    unwrapSelectedUVs(projection);
    
    // Restore selection
    for (size_t i = 0; i < m_faces.size(); i++) {
        m_faces[i].selected = wasSelected[i];
    }
}

// =============================================================================
// Seam-based conformal UV unwrapping (ABF/LSCM-inspired)
// =============================================================================

void Mesh2::unwrapSeamBased(bool selectedOnly) {
    if (m_faces.empty()) return;
    
    // Step 1: Find UV islands by flood-filling faces, treating seam/boundary edges as barriers
    std::vector<int> faceIsland(m_faces.size(), -1);
    int numIslands = 0;
    
    for (size_t startFace = 0; startFace < m_faces.size(); startFace++) {
        if (faceIsland[startFace] >= 0) continue;
        if (selectedOnly && !m_faces[startFace].selected) continue;
        
        // BFS flood fill
        int islandId = numIslands++;
        std::vector<uint32_t> queue;
        queue.push_back(static_cast<uint32_t>(startFace));
        faceIsland[startFace] = islandId;
        
        size_t head = 0;
        while (head < queue.size()) {
            uint32_t fi = queue[head++];
            const Face& face = m_faces[fi];
            
            // For each edge of this face, try to cross to the neighbor face
            for (int ei = 0; ei < face.vertexCount; ei++) {
                uint32_t edgeIdx = face.edges[ei];
                if (edgeIdx == UINT32_MAX) continue;
                
                const Edge& edge = m_edges[edgeIdx];
                
                // Don't cross seam edges or boundary edges
                if (edge.isSeam || edge.isBoundary()) continue;
                
                // Find the neighbor face across this edge
                uint32_t neighborFi = (edge.faces[0] == fi) ? edge.faces[1] : edge.faces[0];
                if (neighborFi == UINT32_MAX) continue;
                if (faceIsland[neighborFi] >= 0) continue;
                if (selectedOnly && !m_faces[neighborFi].selected) continue;
                
                faceIsland[neighborFi] = islandId;
                queue.push_back(neighborFi);
            }
        }
    }
    
    if (numIslands == 0) return;
    
    // Step 2: For each island, build a local vertex mapping and unwrap using LSCM
    // (Least Squares Conformal Map)
    //
    // IMPORTANT: Within an island, a 3D vertex shared by faces that are on opposite
    // sides of a seam edge needs SEPARATE UV coordinates. We use union-find to merge
    // face-corner UV vertices that are connected through non-seam edges.
    
    struct Island {
        std::vector<uint32_t> faces;       // face indices in this island
        std::vector<glm::vec2> uvResult;   // UV result per island-local UV vertex
        std::vector<uint32_t> localToGlobal;  // local UV vertex -> global 3D vertex
        // Per face-corner: which local UV vertex does face[fi].vertices[corner] map to?
        std::unordered_map<uint64_t, uint32_t> faceCornerToLocal;
        float minU, minV, maxU, maxV;      // bounding box after unwrap
    };
    
    std::vector<Island> islands(numIslands);
    
    // Collect faces per island
    for (size_t fi = 0; fi < m_faces.size(); fi++) {
        if (faceIsland[fi] >= 0) {
            islands[faceIsland[fi]].faces.push_back(static_cast<uint32_t>(fi));
        }
    }
    
    // Unwrap each island
    for (int islandIdx = 0; islandIdx < numIslands; islandIdx++) {
        Island& island = islands[islandIdx];
        if (island.faces.empty()) continue;
        
        // Step 2a: Create one UV vertex per face-corner, then merge via union-find
        // across non-seam, non-boundary edges.
        
        // Assign initial UV vertex IDs: one per face corner
        uint32_t totalCorners = 0;
        for (uint32_t fi : island.faces) {
            const Face& face = m_faces[fi];
            for (int i = 0; i < face.vertexCount; i++) {
                uint64_t key = (static_cast<uint64_t>(fi) << 32) | static_cast<uint64_t>(i);
                island.faceCornerToLocal[key] = totalCorners;
                totalCorners++;
            }
        }
        
        // Union-Find
        std::vector<uint32_t> parent(totalCorners);
        for (uint32_t i = 0; i < totalCorners; i++) parent[i] = i;
        
        // Iterative find with path compression
        auto find = [&parent](uint32_t x) -> uint32_t {
            while (parent[x] != x) {
                parent[x] = parent[parent[x]];
                x = parent[x];
            }
            return x;
        };
        auto unite = [&](uint32_t a, uint32_t b) {
            a = find(a);
            b = find(b);
            if (a != b) parent[a] = b;
        };
        
        // For each non-seam, non-boundary edge within the island, merge the shared vertices
        for (uint32_t fi : island.faces) {
            const Face& face = m_faces[fi];
            for (int ei = 0; ei < face.vertexCount; ei++) {
                uint32_t edgeIdx = face.edges[ei];
                if (edgeIdx == UINT32_MAX) continue;
                const Edge& edge = m_edges[edgeIdx];
                if (edge.isSeam || edge.isBoundary()) continue;
                
                // Find neighbor face across this edge
                uint32_t neighborFi = (edge.faces[0] == fi) ? edge.faces[1] : edge.faces[0];
                if (neighborFi == UINT32_MAX) continue;
                if (faceIsland[neighborFi] != islandIdx) continue;
                
                // Only process each edge once (from the lower face index)
                if (fi > neighborFi) continue;
                
                // Find the two shared vertices of this edge
                uint32_t ev0 = edge.vertices[0];
                uint32_t ev1 = edge.vertices[1];
                
                // Find corner indices of ev0 and ev1 in both faces
                auto findCorner = [&](uint32_t faceIdx, uint32_t vertIdx) -> int {
                    const Face& f = m_faces[faceIdx];
                    for (int c = 0; c < f.vertexCount; c++) {
                        if (f.vertices[c] == vertIdx) return c;
                    }
                    return -1;
                };
                
                int c0_this = findCorner(fi, ev0);
                int c0_neigh = findCorner(neighborFi, ev0);
                int c1_this = findCorner(fi, ev1);
                int c1_neigh = findCorner(neighborFi, ev1);
                
                if (c0_this >= 0 && c0_neigh >= 0) {
                    uint64_t key_a = (static_cast<uint64_t>(fi) << 32) | static_cast<uint64_t>(c0_this);
                    uint64_t key_b = (static_cast<uint64_t>(neighborFi) << 32) | static_cast<uint64_t>(c0_neigh);
                    unite(island.faceCornerToLocal[key_a], island.faceCornerToLocal[key_b]);
                }
                if (c1_this >= 0 && c1_neigh >= 0) {
                    uint64_t key_a = (static_cast<uint64_t>(fi) << 32) | static_cast<uint64_t>(c1_this);
                    uint64_t key_b = (static_cast<uint64_t>(neighborFi) << 32) | static_cast<uint64_t>(c1_neigh);
                    unite(island.faceCornerToLocal[key_a], island.faceCornerToLocal[key_b]);
                }
            }
        }
        
        // Compact: map union-find roots to sequential local vertex IDs
        std::unordered_map<uint32_t, uint32_t> rootToLocalVert;
        island.localToGlobal.clear();
        
        for (auto& [key, cornerIdx] : island.faceCornerToLocal) {
            uint32_t root = find(cornerIdx);
            if (rootToLocalVert.find(root) == rootToLocalVert.end()) {
                uint32_t lv = static_cast<uint32_t>(island.localToGlobal.size());
                rootToLocalVert[root] = lv;
                // Recover the global vertex index from the face-corner key
                uint32_t faceIdx = static_cast<uint32_t>(key >> 32);
                uint32_t corner = static_cast<uint32_t>(key & 0xFFFFFFFF);
                island.localToGlobal.push_back(m_faces[faceIdx].vertices[corner]);
            }
            cornerIdx = rootToLocalVert[find(cornerIdx)];
        }
        
        uint32_t numLocalVerts = static_cast<uint32_t>(island.localToGlobal.size());
        
        if (numLocalVerts < 3 || island.faces.size() < 1) {
            // Degenerate island — just set all UVs to 0
            island.uvResult.resize(numLocalVerts, glm::vec2(0.0f));
            island.minU = island.minV = 0.0f;
            island.maxU = island.maxV = 0.0f;
            continue;
        }
        
        // LSCM: Least Squares Conformal Mapping
        // We pin two vertices and solve for the rest
        // For each triangle, the conformal condition is:
        //   (u2-u0) + i*(v2-v0) = ((u1-u0) + i*(v1-v0)) * ((x2-x0) + i*(y2-y0)) / ((x1-x0) + i*(y1-y0))
        //
        // Simplified: for each triangle with local coords, we set up a linear system
        // to minimize angle distortion.
        
        // Choose two pin vertices: pick the pair with largest 3D distance
        uint32_t pin0 = 0, pin1 = 1;
        float maxDist = 0.0f;
        for (uint32_t i = 0; i < numLocalVerts; i++) {
            for (uint32_t j = i + 1; j < numLocalVerts && j < i + 50; j++) {
                float d = glm::distance(m_vertices[island.localToGlobal[i]].position,
                                        m_vertices[island.localToGlobal[j]].position);
                if (d > maxDist) {
                    maxDist = d;
                    pin0 = i;
                    pin1 = j;
                }
            }
        }
        // Also check extremes
        for (uint32_t i = 0; i < numLocalVerts; i++) {
            float d = glm::distance(m_vertices[island.localToGlobal[i]].position,
                                    m_vertices[island.localToGlobal[pin0]].position);
            if (d > maxDist) {
                maxDist = d;
                pin1 = i;
            }
        }
        
        // Triangulate quads into triangles for the LSCM solver
        struct Tri { uint32_t v[3]; }; // local vertex indices
        std::vector<Tri> tris;
        for (uint32_t fi : island.faces) {
            const Face& face = m_faces[fi];
            uint32_t lv[4];
            for (int i = 0; i < face.vertexCount; i++) {
                uint64_t key = (static_cast<uint64_t>(fi) << 32) | static_cast<uint64_t>(i);
                lv[i] = island.faceCornerToLocal[key];
            }
            tris.push_back({lv[0], lv[1], lv[2]});
            if (face.isQuad()) {
                tris.push_back({lv[0], lv[2], lv[3]});
            }
        }
        
        // For small islands (1-2 faces), use simple planar projection instead of LSCM
        if (numLocalVerts <= 4 || tris.size() <= 2) {
            // Find best projection plane from average normal
            glm::vec3 avgNormal(0.0f);
            for (uint32_t fi : island.faces) {
                avgNormal += computeFaceNormal(m_vertices, m_faces[fi]);
            }
            avgNormal = glm::normalize(avgNormal);
            
            // Choose tangent/bitangent
            glm::vec3 tangent, bitangent;
            if (fabsf(avgNormal.y) < 0.9f) {
                tangent = glm::normalize(glm::cross(glm::vec3(0, 1, 0), avgNormal));
            } else {
                tangent = glm::normalize(glm::cross(glm::vec3(1, 0, 0), avgNormal));
            }
            bitangent = glm::cross(avgNormal, tangent);
            
            island.uvResult.resize(numLocalVerts);
            for (uint32_t i = 0; i < numLocalVerts; i++) {
                const glm::vec3& pos = m_vertices[island.localToGlobal[i]].position;
                island.uvResult[i] = glm::vec2(glm::dot(pos, tangent), glm::dot(pos, bitangent));
            }
        } else {
            // Full LSCM solve
            // We solve in terms of free vertices (all except the two pinned ones)
            // Pin0 maps to (0,0), Pin1 maps to (maxDist, 0)
            
            // Create mapping: free vertex index (excludes pinned)
            std::vector<int> freeIndex(numLocalVerts, -1);
            uint32_t numFree = 0;
            for (uint32_t i = 0; i < numLocalVerts; i++) {
                if (i != pin0 && i != pin1) {
                    freeIndex[i] = static_cast<int>(numFree++);
                }
            }
            
            // Pinned positions
            glm::vec2 pinUV0(0.0f, 0.0f);
            glm::vec2 pinUV1(1.0f, 0.0f);  // normalized
            
            // Build the LSCM system: for each triangle, we get 2 equations (real & imaginary parts)
            // The system is: A * [u; v] = b
            // where A is (2*numTris) x (2*numFree) and b is (2*numTris)
            
            uint32_t numEqs = static_cast<uint32_t>(tris.size()) * 2;
            uint32_t numVars = numFree * 2;  // u and v for each free vertex
            
            // Sparse matrix representation: row -> (col, value) pairs
            std::vector<std::vector<std::pair<uint32_t, float>>> A(numEqs);
            std::vector<float> b(numEqs, 0.0f);
            
            for (size_t ti = 0; ti < tris.size(); ti++) {
                const Tri& tri = tris[ti];
                
                // Get 3D positions
                const glm::vec3& p0 = m_vertices[island.localToGlobal[tri.v[0]]].position;
                const glm::vec3& p1 = m_vertices[island.localToGlobal[tri.v[1]]].position;
                const glm::vec3& p2 = m_vertices[island.localToGlobal[tri.v[2]]].position;
                
                // Project triangle into its local 2D coordinate system
                glm::vec3 e1 = p1 - p0;
                glm::vec3 e2 = p2 - p0;
                
                float e1Len = glm::length(e1);
                if (e1Len < 1e-10f) e1Len = 1e-10f;
                
                glm::vec3 t = e1 / e1Len;
                float e2t = glm::dot(e2, t);
                glm::vec3 n = glm::cross(e1, e2);
                float nLen = glm::length(n);
                if (nLen < 1e-10f) continue; // degenerate triangle
                glm::vec3 b2 = glm::cross(n / nLen, t);
                float e2b = glm::dot(e2, b2);
                
                // Local 2D coords: q0=(0,0), q1=(e1Len,0), q2=(e2t,e2b)
                // The conformal condition per triangle:
                // W1*(U2-U0) + W2*(U0-U1) + W3*(U1-U2) = 0 (complex equation)
                // where Wi are complex weights derived from local coords
                
                float area2 = e1Len * e2b; // 2 * triangle area
                if (fabsf(area2) < 1e-10f) continue;
                
                // LSCM weights (Levy et al. 2002)
                // For the conformal energy, the gradient operator in triangle local coords gives:
                // W0 = (q1 - q2) / (2A*i), etc. rotated 90 degrees
                // Simplified: for each vertex j in the triangle with local coords (xj, yj):
                // The conformal condition is: sum_j Wj * (uj + i*vj) = 0
                // where Wj = (x_{j+1} - x_{j-1}) + i*(y_{j+1} - y_{j-1}) (using cyclic indexing)
                
                // Local coords
                float x0 = 0.0f, y0 = 0.0f;
                float x1 = e1Len, y1 = 0.0f;
                float x2 = e2t,   y2 = e2b;
                
                // Conformal weights (complex): W_j = (x_{j+1}-x_{j-1}, y_{j+1}-y_{j-1}) / sqrt(area)
                float sqrtArea = sqrtf(fabsf(area2) * 0.5f);
                if (sqrtArea < 1e-10f) sqrtArea = 1e-10f;
                float invSqrtArea = 1.0f / sqrtArea;
                
                // W0 = ((x1 - x2) + i*(y1 - y2)) / sqrtArea
                float w0r = (x1 - x2) * invSqrtArea;
                float w0i = (y1 - y2) * invSqrtArea;
                // W1 = ((x2 - x0) + i*(y2 - y0)) / sqrtArea
                float w1r = (x2 - x0) * invSqrtArea;
                float w1i = (y2 - y0) * invSqrtArea;
                // W2 = ((x0 - x1) + i*(y0 - y1)) / sqrtArea
                float w2r = (x0 - x1) * invSqrtArea;
                float w2i = (y0 - y1) * invSqrtArea;
                
                float wr[3] = { w0r, w1r, w2r };
                float wi[3] = { w0i, w1i, w2i };
                
                uint32_t eqReal = static_cast<uint32_t>(ti) * 2;
                uint32_t eqImag = eqReal + 1;
                
                for (int j = 0; j < 3; j++) {
                    uint32_t vj = tri.v[j];
                    
                    if (vj == pin0) {
                        // Pinned vertex contributes to RHS
                        b[eqReal] -= wr[j] * pinUV0.x - wi[j] * pinUV0.y;
                        b[eqImag] -= wr[j] * pinUV0.y + wi[j] * pinUV0.x;
                    } else if (vj == pin1) {
                        b[eqReal] -= wr[j] * pinUV1.x - wi[j] * pinUV1.y;
                        b[eqImag] -= wr[j] * pinUV1.y + wi[j] * pinUV1.x;
                    } else {
                        int fi_local = freeIndex[vj];
                        // Real eq: wr[j]*uj - wi[j]*vj
                        A[eqReal].push_back({static_cast<uint32_t>(fi_local), wr[j]});
                        A[eqReal].push_back({static_cast<uint32_t>(numFree + fi_local), -wi[j]});
                        // Imag eq: wr[j]*vj + wi[j]*uj
                        A[eqImag].push_back({static_cast<uint32_t>(fi_local), wi[j]});
                        A[eqImag].push_back({static_cast<uint32_t>(numFree + fi_local), wr[j]});
                    }
                }
            }
            
            // Solve AtA * x = Atb using conjugate gradient
            // First compute AtA (sparse) and Atb
            std::vector<std::unordered_map<uint32_t, float>> AtA(numVars);
            std::vector<float> Atb(numVars, 0.0f);
            
            for (uint32_t row = 0; row < numEqs; row++) {
                for (const auto& [col_j, val_j] : A[row]) {
                    Atb[col_j] += val_j * b[row];
                    for (const auto& [col_k, val_k] : A[row]) {
                        AtA[col_j][col_k] += val_j * val_k;
                    }
                }
            }
            
            // Conjugate Gradient solver for AtA * x = Atb
            std::vector<float> x(numVars, 0.0f);
            std::vector<float> r(numVars);
            std::vector<float> p(numVars);
            std::vector<float> Ap(numVars);
            
            // r = Atb - AtA*x (x starts at 0, so r = Atb)
            for (uint32_t i = 0; i < numVars; i++) {
                r[i] = Atb[i];
                p[i] = r[i];
            }
            
            float rsOld = 0.0f;
            for (uint32_t i = 0; i < numVars; i++) rsOld += r[i] * r[i];
            
            uint32_t maxIter = std::min(numVars * 3, 1000u);
            for (uint32_t iter = 0; iter < maxIter; iter++) {
                if (rsOld < 1e-12f) break;
                
                // Ap = AtA * p
                for (uint32_t i = 0; i < numVars; i++) {
                    float sum = 0.0f;
                    for (const auto& [col, val] : AtA[i]) {
                        sum += val * p[col];
                    }
                    Ap[i] = sum;
                }
                
                float pAp = 0.0f;
                for (uint32_t i = 0; i < numVars; i++) pAp += p[i] * Ap[i];
                if (fabsf(pAp) < 1e-15f) break;
                
                float alpha = rsOld / pAp;
                
                for (uint32_t i = 0; i < numVars; i++) {
                    x[i] += alpha * p[i];
                    r[i] -= alpha * Ap[i];
                }
                
                float rsNew = 0.0f;
                for (uint32_t i = 0; i < numVars; i++) rsNew += r[i] * r[i];
                
                float beta = rsNew / rsOld;
                for (uint32_t i = 0; i < numVars; i++) {
                    p[i] = r[i] + beta * p[i];
                }
                rsOld = rsNew;
            }
            
            // Extract UV results
            island.uvResult.resize(numLocalVerts);
            island.uvResult[pin0] = pinUV0;
            island.uvResult[pin1] = pinUV1;
            for (uint32_t i = 0; i < numLocalVerts; i++) {
                if (i == pin0 || i == pin1) continue;
                int fi_local = freeIndex[i];
                island.uvResult[i] = glm::vec2(x[fi_local], x[numFree + fi_local]);
            }
        }
        
        // Normalize island UVs to [0,1] range
        island.minU = FLT_MAX;
        island.minV = FLT_MAX;
        island.maxU = -FLT_MAX;
        island.maxV = -FLT_MAX;
        for (const auto& uv : island.uvResult) {
            island.minU = std::min(island.minU, uv.x);
            island.minV = std::min(island.minV, uv.y);
            island.maxU = std::max(island.maxU, uv.x);
            island.maxV = std::max(island.maxV, uv.y);
        }
        
        float islandW = island.maxU - island.minU;
        float islandH = island.maxV - island.minV;
        if (islandW < 1e-6f) islandW = 1e-6f;
        if (islandH < 1e-6f) islandH = 1e-6f;
        
        // Normalize to [0,1]
        for (auto& uv : island.uvResult) {
            uv.x = (uv.x - island.minU) / islandW;
            uv.y = (uv.y - island.minV) / islandH;
        }
        
        // Update bounding box to normalized
        island.maxU = islandW;  // store original aspect ratio width
        island.maxV = islandH;  // store original aspect ratio height
    }
    
    // Step 3: Pack islands into UV space
    // Sort islands by height (tallest first) for better packing
    std::vector<int> islandOrder(numIslands);
    for (int i = 0; i < numIslands; i++) islandOrder[i] = i;
    std::sort(islandOrder.begin(), islandOrder.end(), [&](int a, int b) {
        float areaA = islands[a].maxU * islands[a].maxV;
        float areaB = islands[b].maxU * islands[b].maxV;
        return areaA > areaB;
    });
    
    // Compute total area for sizing
    float totalArea = 0.0f;
    for (int i = 0; i < numIslands; i++) {
        if (islands[i].faces.empty()) continue;
        totalArea += islands[i].maxU * islands[i].maxV;
    }
    
    // Scale islands to maintain relative proportions
    float scaleFactor = 1.0f;
    if (totalArea > 0.0f) {
        // Target ~70% fill of UV space
        scaleFactor = sqrtf(0.7f / totalArea);
    }
    
    // Simple shelf packing
    float cursorX = 0.0f;
    float cursorY = 0.0f;
    float rowHeight = 0.0f;
    float padding = 0.02f;
    
    for (int idx : islandOrder) {
        Island& island = islands[idx];
        if (island.faces.empty()) continue;
        
        float w = island.maxU * scaleFactor;
        float h = island.maxV * scaleFactor;
        
        // Wrap to next row if needed
        if (cursorX + w > 1.0f && cursorX > padding) {
            cursorX = 0.0f;
            cursorY += rowHeight + padding;
            rowHeight = 0.0f;
        }
        
        // Place island
        float offsetX = cursorX;
        float offsetY = cursorY;
        
        // Scale and offset each UV in the island
        for (auto& uv : island.uvResult) {
            uv.x = uv.x * w + offsetX;
            uv.y = uv.y * h + offsetY;
        }
        
        cursorX += w + padding;
        rowHeight = std::max(rowHeight, h);
    }
    
    // Step 4: Write UVs back to faces
    for (int islandIdx = 0; islandIdx < numIslands; islandIdx++) {
        const Island& island = islands[islandIdx];
        for (uint32_t fi : island.faces) {
            Face& face = m_faces[fi];
            for (int i = 0; i < face.vertexCount; i++) {
                uint64_t key = (static_cast<uint64_t>(fi) << 32) | static_cast<uint64_t>(i);
                auto it = island.faceCornerToLocal.find(key);
                if (it != island.faceCornerToLocal.end()) {
                    face.uvs[i] = island.uvResult[it->second];
                }
            }
        }
    }
    
    m_renderDataDirty = true;
}

// Seam marking
void Mesh2::markEdgeAsSeam(uint32_t edgeIndex, bool isSeam) {
    if (edgeIndex >= m_edges.size()) return;
    m_edges[edgeIndex].isSeam = isSeam;
    m_renderDataDirty = true;
}

bool Mesh2::isEdgeSeam(uint32_t edgeIndex) const {
    if (edgeIndex >= m_edges.size()) return false;
    return m_edges[edgeIndex].isSeam;
}

void Mesh2::markSelectedEdgesAsSeam(bool isSeam) {
    for (auto& edge : m_edges) {
        if (edge.selected) {
            edge.isSeam = isSeam;
        }
    }
    m_renderDataDirty = true;
}

void Mesh2::clearAllSeams() {
    for (auto& edge : m_edges) {
        edge.isSeam = false;
    }
    m_renderDataDirty = true;
}

std::vector<uint32_t> Mesh2::getConnectedVertices(uint32_t vertexIndex) const {
    std::vector<uint32_t> connected;
    if (vertexIndex < m_vertexToEdges.size()) {
        for (uint32_t ei : m_vertexToEdges[vertexIndex]) {
            const Edge& edge = m_edges[ei];
            connected.push_back(edge.otherVertex(vertexIndex));
        }
    }
    return connected;
}

std::vector<uint32_t> Mesh2::getVertexEdges(uint32_t vertexIndex) const {
    if (vertexIndex < m_vertexToEdges.size()) {
        return m_vertexToEdges[vertexIndex];
    }
    return {};
}

std::vector<uint32_t> Mesh2::getVertexFaces(uint32_t vertexIndex) const {
    if (vertexIndex < m_vertexToFaces.size()) {
        return m_vertexToFaces[vertexIndex];
    }
    return {};
}

// UV Editor rendering
void Mesh2::drawUV(Shader& uvShader) const {
    if (s_disableGPU || m_faces.empty()) return;
    
    // Build UV render data - faces as triangles with UV as position
    std::vector<float> uvData; // UV(2) + Color(3) + TexCoord(2) per vertex
    
    for (size_t fi = 0; fi < m_faces.size(); fi++) {
        const Face& face = m_faces[fi];
        
        // Face color based on selection
        glm::vec3 color = face.selected ? glm::vec3(1.0f, 0.55f, 0.0f) : glm::vec3(0.4f, 0.4f, 0.6f);
        
        // Triangle(s) from face
        auto addVertex = [&](int cornerIdx) {
            uvData.push_back(0.0f); // aPosition.x (unused, but needed for layout)
            uvData.push_back(0.0f); // aPosition.y
            uvData.push_back(0.0f); // aPosition.z
            uvData.push_back(0.0f); // aNormal.x
            uvData.push_back(0.0f); // aNormal.y
            uvData.push_back(0.0f); // aNormal.z
            uvData.push_back(color.r);
            uvData.push_back(color.g);
            uvData.push_back(color.b);
            uvData.push_back(face.uvs[cornerIdx].x);
            uvData.push_back(face.uvs[cornerIdx].y);
        };
        
        // First triangle
        addVertex(0);
        addVertex(1);
        addVertex(2);
        
        // Second triangle for quads
        if (face.isQuad()) {
            addVertex(0);
            addVertex(2);
            addVertex(3);
        }
    }
    
    if (uvData.empty()) return;
    
    // Create temp VAO/VBO for UV rendering
    GLuint tempVao, tempVbo;
    glGenVertexArrays(1, &tempVao);
    glGenBuffers(1, &tempVbo);
    
    glBindVertexArray(tempVao);
    glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
    glBufferData(GL_ARRAY_BUFFER, uvData.size() * sizeof(float), uvData.data(), GL_STREAM_DRAW);
    
    // Layout: pos(3) + normal(3) + color(3) + texcoord(2) = 11 floats
    size_t stride = 11 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);                    // aPosition
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float))); // aNormal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float))); // aColor
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float))); // aTexCoord
    glEnableVertexAttribArray(3);
    
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(uvData.size() / 11));
    
    glDeleteBuffers(1, &tempVbo);
    glDeleteVertexArrays(1, &tempVao);
    glBindVertexArray(0);
}

void Mesh2::drawUVEdges(Shader& uvColoredShader) const {
    if (s_disableGPU || m_faces.empty()) return;
    
    // Draw edges in UV space
    std::vector<float> uvEdgeData; // UV(2) + Color(3) per vertex
    
    for (size_t fi = 0; fi < m_faces.size(); fi++) {
        const Face& face = m_faces[fi];
        
        for (uint8_t i = 0; i < face.count(); i++) {
            uint8_t next = (i + 1) % face.count();
            
            // Get edge selection and seam state
            uint32_t ei = face.edges[i];
            bool selected = (ei < m_edges.size()) ? m_edges[ei].selected : false;
            bool isSeam = (ei < m_edges.size()) ? m_edges[ei].isSeam : false;
            
            // Color priority: seam (green) > selected (red) > normal (blue)
            glm::vec3 color;
            if (isSeam) {
                color = glm::vec3(0.0f, 1.0f, 0.0f);  // Green for seams
            } else if (selected) {
                color = glm::vec3(1.0f, 0.0f, 0.0f);  // Red for selected
            } else {
                color = glm::vec3(0.0f, 0.0f, 1.0f);  // Blue for normal
            }
            
            // First vertex
            uvEdgeData.push_back(face.uvs[i].x);
            uvEdgeData.push_back(face.uvs[i].y);
            uvEdgeData.push_back(color.r);
            uvEdgeData.push_back(color.g);
            uvEdgeData.push_back(color.b);
            
            // Second vertex
            uvEdgeData.push_back(face.uvs[next].x);
            uvEdgeData.push_back(face.uvs[next].y);
            uvEdgeData.push_back(color.r);
            uvEdgeData.push_back(color.g);
            uvEdgeData.push_back(color.b);
        }
    }
    
    if (uvEdgeData.empty()) return;
    
    GLuint tempVao, tempVbo;
    glGenVertexArrays(1, &tempVao);
    glGenBuffers(1, &tempVbo);
    
    glBindVertexArray(tempVao);
    glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
    glBufferData(GL_ARRAY_BUFFER, uvEdgeData.size() * sizeof(float), uvEdgeData.data(), GL_STREAM_DRAW);
    
    size_t stride = 5 * sizeof(float);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(uvEdgeData.size() / 5));
    
    glDeleteBuffers(1, &tempVbo);
    glDeleteVertexArrays(1, &tempVao);
    glBindVertexArray(0);
}

void Mesh2::drawUVForSelection(Shader& selectionShader) const {
    if (s_disableGPU || m_faces.empty()) return;
    
    // Draw each face in UV space with its index encoded as color
    for (size_t fi = 0; fi < m_faces.size(); fi++) {
        const Face& face = m_faces[fi];
        
        uint32_t colorIndex = static_cast<uint32_t>(fi) + 1;
        selectionShader.setUInt("uColorIndex", colorIndex);
        
        std::vector<float> uvData; // UV(2) per vertex
        
        // First triangle
        uvData.push_back(face.uvs[0].x); uvData.push_back(face.uvs[0].y);
        uvData.push_back(face.uvs[1].x); uvData.push_back(face.uvs[1].y);
        uvData.push_back(face.uvs[2].x); uvData.push_back(face.uvs[2].y);
        
        if (face.isQuad()) {
            uvData.push_back(face.uvs[0].x); uvData.push_back(face.uvs[0].y);
            uvData.push_back(face.uvs[2].x); uvData.push_back(face.uvs[2].y);
            uvData.push_back(face.uvs[3].x); uvData.push_back(face.uvs[3].y);
        }
        
        GLuint tempVao, tempVbo;
        glGenVertexArrays(1, &tempVao);
        glGenBuffers(1, &tempVbo);
        
        glBindVertexArray(tempVao);
        glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
        glBufferData(GL_ARRAY_BUFFER, uvData.size() * sizeof(float), uvData.data(), GL_STREAM_DRAW);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(uvData.size() / 2));
        
        glDeleteBuffers(1, &tempVbo);
        glDeleteVertexArrays(1, &tempVao);
    }
    
    glBindVertexArray(0);
}
