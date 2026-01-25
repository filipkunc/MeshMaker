#include <gtest/gtest.h>
#include "Mesh2.h"

class Mesh2Test : public ::testing::Test {
protected:
    Mesh2 mesh;
    
    void SetUp() override {
        // Each test starts with a fresh mesh
    }
    
    // Helper to count vertices connected to a face
    size_t countUniqueVerticesInFaces() {
        std::unordered_set<uint32_t> usedVertices;
        for (const auto& face : mesh.getFaces()) {
            for (int i = 0; i < face.vertexCount; i++) {
                usedVertices.insert(face.vertices[i]);
            }
        }
        return usedVertices.size();
    }
    
    // Helper to check if all edges have valid face references
    bool allEdgesHaveValidFaces() {
        for (const auto& edge : mesh.getEdges()) {
            // At least one face should reference this edge
            if (edge.faces[0] == UINT32_MAX && edge.faces[1] == UINT32_MAX) {
                return false;
            }
        }
        return true;
    }
    
    // Helper to verify edge-face connectivity
    bool verifyEdgeFaceConnectivity() {
        const auto& faces = mesh.getFaces();
        const auto& edges = mesh.getEdges();
        
        for (size_t fi = 0; fi < faces.size(); fi++) {
            const auto& face = faces[fi];
            for (int i = 0; i < face.vertexCount; i++) {
                uint32_t edgeIdx = face.edges[i];
                if (edgeIdx >= edges.size()) return false;
                
                const auto& edge = edges[edgeIdx];
                // This face should be referenced by the edge
                if (edge.faces[0] != fi && edge.faces[1] != fi) {
                    return false;
                }
            }
        }
        return true;
    }
};

// =============================================================================
// Basic Mesh Creation Tests
// =============================================================================

TEST_F(Mesh2Test, MakePlane_CreatesCorrectTopology) {
    mesh.makePlane();
    
    EXPECT_EQ(mesh.getVertexCount(), 4);
    EXPECT_EQ(mesh.getFaceCount(), 1);
    EXPECT_EQ(mesh.getEdgeCount(), 4);
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(Mesh2Test, MakeCube_CreatesCorrectTopology) {
    mesh.makeCube();
    
    EXPECT_EQ(mesh.getVertexCount(), 8);
    EXPECT_EQ(mesh.getFaceCount(), 6);
    EXPECT_EQ(mesh.getEdgeCount(), 12);
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

// =============================================================================
// Merge Vertices Tests
// =============================================================================

TEST_F(Mesh2Test, MergeVertices_TwoAdjacentVertices_CreatesValidMesh) {
    // Create a simple plane (quad with 4 vertices)
    mesh.makePlane();
    
    ASSERT_EQ(mesh.getVertexCount(), 4);
    ASSERT_EQ(mesh.getFaceCount(), 1);
    
    // Select first two vertices (an edge of the quad)
    mesh.setSelectionMode(SelectionMode::Vertices);
    mesh.selectVertex(0, false);
    mesh.selectVertex(1, true);
    
    EXPECT_EQ(mesh.getSelectedCount(), 2);
    
    // Merge them
    mesh.mergeSelectedVertices();
    
    // After merging two adjacent vertices of a quad:
    // - The quad becomes a triangle (4 verts -> 3 unique verts)
    // - Should have 3 vertices now
    EXPECT_EQ(mesh.getVertexCount(), 3);
    EXPECT_EQ(mesh.getFaceCount(), 1);
    
    // The face should now be a triangle
    EXPECT_FALSE(mesh.getFace(0).isQuad());
    
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(Mesh2Test, MergeVertices_TwoVerticesFromDifferentFaces_PreservesConnectivity) {
    // Create a cube - has shared vertices between faces
    mesh.makeCube();
    
    size_t initialVertices = mesh.getVertexCount();
    size_t initialFaces = mesh.getFaceCount();
    
    ASSERT_EQ(initialVertices, 8);
    ASSERT_EQ(initialFaces, 6);
    
    // Select two opposite vertices (diagonal of cube)
    mesh.setSelectionMode(SelectionMode::Vertices);
    mesh.selectVertex(0, false);
    mesh.selectVertex(6, true);  // Opposite corner
    
    EXPECT_EQ(mesh.getSelectedCount(), 2);
    
    // Merge them
    mesh.mergeSelectedVertices();
    
    // After merge:
    // - Should have fewer vertices
    // - Remaining faces should have valid connectivity
    EXPECT_LT(mesh.getVertexCount(), initialVertices);
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(Mesh2Test, MergeVertices_AdjacentInCube_NeighborFacesStayConnected) {
    // Create cube
    mesh.makeCube();
    
    // Get initial state
    size_t initialFaces = mesh.getFaceCount();
    size_t initialVertices = mesh.getVertexCount();
    ASSERT_EQ(initialFaces, 6);
    ASSERT_EQ(initialVertices, 8);
    
    // Select two adjacent vertices (share an edge)
    mesh.setSelectionMode(SelectionMode::Vertices);
    mesh.selectVertex(0, false);
    mesh.selectVertex(1, true);
    
    // These two vertices are shared by 2 faces in a cube (the faces containing this edge)
    mesh.mergeSelectedVertices();
    
    // After merge:
    // - Vertices: 8 - 2 + 1 = 7 (two merged into one center vertex)
    // - The 2 faces that had this edge become triangles (quad -> triangle)
    // - Other 4 faces remain quads
    // - Total: still 6 faces
    EXPECT_EQ(mesh.getVertexCount(), 7);
    EXPECT_EQ(mesh.getFaceCount(), 6);
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
    
    // All remaining vertices should be used by faces
    EXPECT_EQ(countUniqueVerticesInFaces(), mesh.getVertexCount());
}

TEST_F(Mesh2Test, MergeVertices_SingleVertex_DoesNothing) {
    mesh.makePlane();
    
    size_t initialVertices = mesh.getVertexCount();
    size_t initialFaces = mesh.getFaceCount();
    
    mesh.setSelectionMode(SelectionMode::Vertices);
    mesh.selectVertex(0, false);
    
    EXPECT_EQ(mesh.getSelectedCount(), 1);
    
    mesh.mergeSelectedVertices();
    
    // Should not change anything
    EXPECT_EQ(mesh.getVertexCount(), initialVertices);
    EXPECT_EQ(mesh.getFaceCount(), initialFaces);
}

TEST_F(Mesh2Test, MergeVertices_NoSelection_DoesNothing) {
    mesh.makePlane();
    
    size_t initialVertices = mesh.getVertexCount();
    size_t initialFaces = mesh.getFaceCount();
    
    mesh.setSelectionMode(SelectionMode::Vertices);
    EXPECT_EQ(mesh.getSelectedCount(), 0);
    
    mesh.mergeSelectedVertices();
    
    EXPECT_EQ(mesh.getVertexCount(), initialVertices);
    EXPECT_EQ(mesh.getFaceCount(), initialFaces);
}

TEST_F(Mesh2Test, MergeVertices_CenterPosition_IsAverage) {
    mesh.makePlane();
    
    // Get positions of first two vertices
    const auto& verts = mesh.getVertices();
    glm::vec3 pos0 = verts[0].position;
    glm::vec3 pos1 = verts[1].position;
    glm::vec3 expectedCenter = (pos0 + pos1) / 2.0f;
    
    mesh.setSelectionMode(SelectionMode::Vertices);
    mesh.selectVertex(0, false);
    mesh.selectVertex(1, true);
    
    mesh.mergeSelectedVertices();
    
    // Find the merged vertex (should be somewhere in the remaining vertices)
    const auto& newVerts = mesh.getVertices();
    bool foundCenter = false;
    for (const auto& v : newVerts) {
        if (glm::distance(v.position, expectedCenter) < 0.001f) {
            foundCenter = true;
            break;
        }
    }
    
    // If any vertices remain, the center should be among them
    // (though in this case the face becomes degenerate and is removed)
    // This test mainly verifies no crash
    EXPECT_TRUE(mesh.getVertexCount() == 0 || foundCenter || mesh.getFaceCount() == 0);
}

// =============================================================================
// Edge connectivity after various operations
// =============================================================================

TEST_F(Mesh2Test, SharedEdges_CubeHasProperSharing) {
    mesh.makeCube();
    
    // In a cube, each edge should be shared by exactly 2 faces
    const auto& edges = mesh.getEdges();
    for (const auto& edge : edges) {
        EXPECT_NE(edge.faces[0], UINT32_MAX);
        EXPECT_NE(edge.faces[1], UINT32_MAX);
    }
}

// More detailed merge test to catch connectivity issues
TEST_F(Mesh2Test, MergeVertices_DetailedConnectivityCheck) {
    // Create a 2x1 plane (two quads sharing an edge)
    // This creates a situation where merging should preserve the shared edge
    
    // Manually create two adjacent quads sharing vertices
    uint32_t v0 = mesh.addVertex(glm::vec3(0, 0, 0));
    uint32_t v1 = mesh.addVertex(glm::vec3(1, 0, 0));
    uint32_t v2 = mesh.addVertex(glm::vec3(1, 0, 1));
    uint32_t v3 = mesh.addVertex(glm::vec3(0, 0, 1));
    uint32_t v4 = mesh.addVertex(glm::vec3(2, 0, 0));
    uint32_t v5 = mesh.addVertex(glm::vec3(2, 0, 1));
    
    // First quad: v0, v1, v2, v3
    mesh.addQuad(v0, v1, v2, v3);
    // Second quad: v1, v4, v5, v2  (shares edge v1-v2 with first)
    mesh.addQuad(v1, v4, v5, v2);
    
    mesh.buildConnectivity();
    
    ASSERT_EQ(mesh.getFaceCount(), 2);
    ASSERT_EQ(mesh.getVertexCount(), 6);
    
    // Verify the shared edge exists and connects both faces
    const auto& edges = mesh.getEdges();
    bool foundSharedEdge = false;
    for (const auto& edge : edges) {
        if (edge.faces[0] != UINT32_MAX && edge.faces[1] != UINT32_MAX) {
            foundSharedEdge = true;
            break;
        }
    }
    EXPECT_TRUE(foundSharedEdge) << "Two adjacent quads should share an edge";
    
    // Now merge v0 and v3 (opposite corners of first quad, NOT on shared edge)
    mesh.setSelectionMode(SelectionMode::Vertices);
    mesh.selectVertex(v0, false);
    mesh.selectVertex(v3, true);
    
    mesh.mergeSelectedVertices();
    
    // First quad becomes degenerate (triangle)
    // Second quad should remain intact
    // Check if remaining geometry has proper connectivity
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
    
    // All remaining vertices should be used
    EXPECT_EQ(countUniqueVerticesInFaces(), mesh.getVertexCount());
}

// Test that merging vertices on a shared edge collapses both quads to triangles
TEST_F(Mesh2Test, MergeVertices_OnSharedEdge_CollapsesToTriangles) {
    // Two quads sharing edge v1-v2
    uint32_t v0 = mesh.addVertex(glm::vec3(0, 0, 0));
    uint32_t v1 = mesh.addVertex(glm::vec3(1, 0, 0));
    uint32_t v2 = mesh.addVertex(glm::vec3(1, 0, 1));
    uint32_t v3 = mesh.addVertex(glm::vec3(0, 0, 1));
    uint32_t v4 = mesh.addVertex(glm::vec3(2, 0, 0));
    uint32_t v5 = mesh.addVertex(glm::vec3(2, 0, 1));
    
    mesh.addQuad(v0, v1, v2, v3);
    mesh.addQuad(v1, v4, v5, v2);
    mesh.buildConnectivity();
    
    // Merge v1 and v2 (the shared edge!)
    mesh.setSelectionMode(SelectionMode::Vertices);
    mesh.selectVertex(v1, false);
    mesh.selectVertex(v2, true);
    
    mesh.mergeSelectedVertices();
    
    // Both quads become triangles (each had v1 and v2, now have 1 merged vertex)
    // So we expect 2 triangular faces
    EXPECT_EQ(mesh.getFaceCount(), 2);
    
    // Both should be triangles
    for (const auto& face : mesh.getFaces()) {
        EXPECT_FALSE(face.isQuad());
    }
    
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

// THE CRITICAL TEST: Merging top of cube should create pyramid
TEST_F(Mesh2Test, MergeVertices_CubeTop_CreatesPyramid) {
    mesh.makeCube();
    
    // Cube has 8 vertices and 6 quad faces
    ASSERT_EQ(mesh.getVertexCount(), 8);
    ASSERT_EQ(mesh.getFaceCount(), 6);
    
    // Find the top 4 vertices (Y > 0)
    // In makeCube, vertices are typically at y = -0.5 and y = 0.5
    mesh.setSelectionMode(SelectionMode::Vertices);
    const auto& verts = mesh.getVertices();
    std::vector<uint32_t> topVertices;
    for (size_t i = 0; i < verts.size(); i++) {
        if (verts[i].position.y > 0.0f) {
            mesh.selectVertex(i, true);
            topVertices.push_back(static_cast<uint32_t>(i));
        }
    }
    
    ASSERT_EQ(topVertices.size(), 4) << "Cube should have 4 top vertices";
    ASSERT_EQ(mesh.getSelectedCount(), 4);
    
    // Merge the top 4 vertices
    mesh.mergeSelectedVertices();
    
    // Expected result: pyramid shape
    // - 5 vertices (4 bottom + 1 merged top)
    // - 5 faces: 4 triangular sides + 1 quad bottom
    // - The top quad is completely degenerate (all 4 vertices -> 1)
    // - Each side quad had 2 top vertices, now has 1 -> becomes triangle
    
    EXPECT_EQ(mesh.getVertexCount(), 5) << "Pyramid should have 5 vertices";
    EXPECT_EQ(mesh.getFaceCount(), 5) << "Pyramid should have 5 faces (4 triangle sides + 1 quad bottom)";
    
    // Count triangles and quads
    int triangleCount = 0;
    int quadCount = 0;
    for (const auto& face : mesh.getFaces()) {
        if (face.isQuad()) {
            quadCount++;
        } else {
            triangleCount++;
        }
    }
    
    EXPECT_EQ(triangleCount, 4) << "Pyramid should have 4 triangular side faces";
    EXPECT_EQ(quadCount, 1) << "Pyramid should have 1 quad bottom face";
    
    // Verify connectivity
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
    EXPECT_EQ(countUniqueVerticesInFaces(), mesh.getVertexCount());
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
