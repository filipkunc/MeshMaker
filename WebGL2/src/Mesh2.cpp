#include "Mesh2.h"
#include "Shader.h"

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

Mesh2::Mesh2()
    : m_selectionMode(SelectionMode::Triangles)
    , m_color(0.7f, 0.7f, 0.7f)
    , m_wireframeColor(0.0f, 0.0f, 0.0f)
    , m_selectionColor(1.0f, 0.5f, 0.0f)
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

// Edge loop selection - follows connected edges through quads
void Mesh2::selectEdgeLoop(uint32_t edgeIndex) {
    if (edgeIndex >= m_edges.size()) return;
    
    m_edges[edgeIndex].selected = true;
    
    // Traverse in both directions
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
            
            // Only continue through quads for edge loops
            if (!face.isQuad()) break;
            
            // Find the opposite edge in the quad
            int edgePos = -1;
            for (int i = 0; i < 4; i++) {
                if (face.edges[i] == currentEdge) {
                    edgePos = i;
                    break;
                }
            }
            
            if (edgePos < 0) break;
            
            // Opposite edge is 2 positions away
            uint32_t nextEdge = face.edges[(edgePos + 2) % 4];
            
            // Stop if we've looped back or already selected
            if (nextEdge == edgeIndex || m_edges[nextEdge].selected) break;
            
            m_edges[nextEdge].selected = true;
            lastFace = nextFace;
            currentEdge = nextEdge;
        }
    }
    
    m_renderDataDirty = true;
}

// Edge ring selection - follows perpendicular edges through quads
void Mesh2::selectEdgeRing(uint32_t edgeIndex) {
    if (edgeIndex >= m_edges.size()) return;
    
    m_edges[edgeIndex].selected = true;
    
    // Traverse through connected quads perpendicular to the edge
    for (int dir = 0; dir < 2; dir++) {
        uint32_t currentEdge = edgeIndex;
        
        while (true) {
            const Edge& edge = m_edges[currentEdge];
            uint32_t faceIndex = edge.faces[dir];
            
            if (faceIndex == UINT32_MAX) break;
            
            const Face& face = m_faces[faceIndex];
            if (!face.isQuad()) break;
            
            // Find adjacent edges (perpendicular)
            int edgePos = -1;
            for (int i = 0; i < 4; i++) {
                if (face.edges[i] == currentEdge) {
                    edgePos = i;
                    break;
                }
            }
            
            if (edgePos < 0) break;
            
            // Get adjacent edge (not opposite, but adjacent)
            uint32_t nextEdge = face.edges[(edgePos + 1) % 4];
            
            if (nextEdge == edgeIndex || m_edges[nextEdge].selected) break;
            
            m_edges[nextEdge].selected = true;
            currentEdge = nextEdge;
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
        } else if (splitEdgePositions.size() == 2 && face.isQuad()) {
            // Two opposite edges split - create 3 quads (subdivision)
            int pos0 = splitEdgePositions[0];
            int pos1 = splitEdgePositions[1];
            
            if (std::abs(pos0 - pos1) == 2) {
                // Opposite edges - this is edge loop subdivision
                uint32_t mid0 = edgeMidpoints[face.edges[pos0]];
                uint32_t mid1 = edgeMidpoints[face.edges[pos1]];
                
                // Create two quads
                Face quad1, quad2;
                quad1.vertexCount = quad2.vertexCount = 4;
                
                if (pos0 < pos1) {
                    quad1.vertices[0] = face.vertices[pos0];
                    quad1.vertices[1] = mid0;
                    quad1.vertices[2] = mid1;
                    quad1.vertices[3] = face.vertices[(pos1 + 1) % 4];
                    
                    quad2.vertices[0] = mid0;
                    quad2.vertices[1] = face.vertices[(pos0 + 1) % 4];
                    quad2.vertices[2] = face.vertices[pos1];
                    quad2.vertices[3] = mid1;
                } else {
                    std::swap(pos0, pos1);
                    std::swap(mid0, mid1);
                    
                    quad1.vertices[0] = face.vertices[pos0];
                    quad1.vertices[1] = mid0;
                    quad1.vertices[2] = mid1;
                    quad1.vertices[3] = face.vertices[(pos1 + 1) % 4];
                    
                    quad2.vertices[0] = mid0;
                    quad2.vertices[1] = face.vertices[(pos0 + 1) % 4];
                    quad2.vertices[2] = face.vertices[pos1];
                    quad2.vertices[3] = mid1;
                }
                
                newFaces.push_back(quad1);
                newFaces.push_back(quad2);
            }
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
        
        tri2.vertices[0] = face.vertices[0];
        tri2.vertices[1] = face.vertices[2];
        tri2.vertices[2] = face.vertices[3];
        
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
        } else {
            uint32_t fi = addTriangle(face.vertices[0], face.vertices[1], face.vertices[2]);
            m_faces[fi].selected = face.selected;
        }
    }
    
    buildConnectivity();
    m_renderDataDirty = true;
}

void Mesh2::subdivideSelected() {
    // Simple subdivision: split each selected face into 4
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

void Mesh2::extrudeSelected() {
    std::unordered_map<uint32_t, uint32_t> vertexDuplicates;
    std::vector<std::pair<uint32_t, uint32_t>> boundaryEdges;  // Original edge vertices
    
    // Find boundary edges (edges where one face is selected and other is not, or only one face)
    for (size_t ei = 0; ei < m_edges.size(); ei++) {
        const Edge& edge = m_edges[ei];
        
        int selectedCount = 0;
        for (int i = 0; i < 2; i++) {
            if (edge.faces[i] != UINT32_MAX && m_faces[edge.faces[i]].selected) {
                selectedCount++;
            }
        }
        
        // Boundary if exactly one adjacent face is selected
        if (selectedCount == 1) {
            boundaryEdges.push_back({edge.vertices[0], edge.vertices[1]});
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
    for (const auto& be : boundaryEdges) {
        uint32_t orig0 = be.first;
        uint32_t orig1 = be.second;
        uint32_t dup0 = vertexDuplicates[orig0];
        uint32_t dup1 = vertexDuplicates[orig1];
        
        // Create quad connecting original and duplicate
        addQuad(orig1, orig0, dup0, dup1);
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
            
            // First triangle
            m_renderVertices.push_back({v0, faceNormal, faceColor});
            m_renderVertices.push_back({v1, faceNormal, faceColor});
            m_renderVertices.push_back({v2, faceNormal, faceColor});
            
            // Second triangle
            m_renderVertices.push_back({v0, faceNormal, faceColor});
            m_renderVertices.push_back({v2, faceNormal, faceColor});
            m_renderVertices.push_back({v3, faceNormal, faceColor});
        } else {
            m_renderVertices.push_back({v0, faceNormal, faceColor});
            m_renderVertices.push_back({v1, faceNormal, faceColor});
            m_renderVertices.push_back({v2, faceNormal, faceColor});
        }
    }
    
    // Build edge render data
    for (const Edge& edge : m_edges) {
        glm::vec3 edgeColor = edge.selected ? m_selectionColor : m_wireframeColor;
        glm::vec3 p0 = m_vertices[edge.vertices[0]].position;
        glm::vec3 p1 = m_vertices[edge.vertices[1]].position;
        
        m_edgeRenderVertices.push_back({p0, edgeColor});
        m_edgeRenderVertices.push_back({p1, edgeColor});
    }
    
    m_renderDataDirty = false;
}

// GPU
void Mesh2::createGPUBuffers() {
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

void Mesh2::deleteGPUBuffers() {
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
    // Matching original MeshMaker behavior
    
    if (m_edges.empty()) return;
    
    const glm::vec3 selectedColor(0.8f, 0.0f, 0.0f);   // Red
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
        const glm::vec3& color = edge.selected ? selectedColor : normalColor;
        
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

// Primitive generation
void Mesh2::makePlane() {
    clear();
    
    uint32_t v0 = addVertex(glm::vec3(-1.0f, 0.0f, -1.0f));
    uint32_t v1 = addVertex(glm::vec3( 1.0f, 0.0f, -1.0f));
    uint32_t v2 = addVertex(glm::vec3( 1.0f, 0.0f,  1.0f));
    uint32_t v3 = addVertex(glm::vec3(-1.0f, 0.0f,  1.0f));
    
    addQuad(v0, v1, v2, v3);
    
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
    
    // 6 faces (quads)
    addQuad(v[0], v[1], v[2], v[3]); // Front
    addQuad(v[5], v[4], v[7], v[6]); // Back
    addQuad(v[4], v[0], v[3], v[7]); // Left
    addQuad(v[1], v[5], v[6], v[2]); // Right
    addQuad(v[3], v[2], v[6], v[7]); // Top
    addQuad(v[4], v[5], v[1], v[0]); // Bottom
    
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
    
    // Side quads
    for (uint32_t i = 0; i < steps; i++) {
        uint32_t next = (i + 1) % steps;
        addQuad(bottomRing[i], bottomRing[next], topRing[next], topRing[i]);
    }
    
    // Top and bottom caps (triangles)
    for (uint32_t i = 0; i < steps; i++) {
        uint32_t next = (i + 1) % steps;
        addTriangle(bottomCenter, bottomRing[next], bottomRing[i]);
        addTriangle(topCenter, topRing[i], topRing[next]);
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
    
    // Create faces
    for (uint32_t r = 0; r < rings; r++) {
        for (uint32_t s = 0; s < sectors; s++) {
            uint32_t v0 = grid[r][s];
            uint32_t v1 = grid[r][s + 1];
            uint32_t v2 = grid[r + 1][s + 1];
            uint32_t v3 = grid[r + 1][s];
            
            // Use triangles at poles, quads elsewhere
            if (r == 0) {
                addTriangle(v0, v3, v2);
            } else if (r == rings - 1) {
                addTriangle(v0, v1, v2);
            } else {
                addQuad(v0, v1, v2, v3);
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
    
    // 20 triangular faces
    addTriangle(v[2], v[1], v[0]);
    addTriangle(v[1], v[2], v[3]);
    addTriangle(v[5], v[4], v[3]);
    addTriangle(v[4], v[8], v[3]);
    addTriangle(v[7], v[6], v[0]);
    addTriangle(v[6], v[9], v[0]);
    addTriangle(v[11], v[10], v[4]);
    addTriangle(v[10], v[11], v[6]);
    addTriangle(v[9], v[5], v[2]);
    addTriangle(v[5], v[9], v[11]);
    addTriangle(v[8], v[7], v[1]);
    addTriangle(v[7], v[8], v[10]);
    addTriangle(v[2], v[5], v[3]);
    addTriangle(v[8], v[1], v[3]);
    addTriangle(v[9], v[2], v[0]);
    addTriangle(v[1], v[7], v[0]);
    addTriangle(v[11], v[9], v[6]);
    addTriangle(v[7], v[10], v[6]);
    addTriangle(v[5], v[11], v[4]);
    addTriangle(v[10], v[8], v[4]);
    
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
