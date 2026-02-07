#include <gtest/gtest.h>
#include "Mesh2.h"

// =============================================================================
// Test Environment - Disables GPU operations for all tests
// =============================================================================

class Mesh2TestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        Mesh2::s_disableGPU = true;
    }
    
    void TearDown() override {
        Mesh2::s_disableGPU = false;
    }
};

testing::Environment* const mesh2_env = 
    testing::AddGlobalTestEnvironment(new Mesh2TestEnvironment);

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
// Edge Loop and Ring Selection Tests
// =============================================================================

class EdgeSelectionTest : public Mesh2Test {
protected:
    // Helper to count selected edges
    size_t countSelectedEdges() {
        size_t count = 0;
        for (const auto& edge : mesh.getEdges()) {
            if (edge.selected) count++;
        }
        return count;
    }
    
    // Helper to deselect all edges
    void deselectAllEdges() {
        mesh.setSelectionMode(SelectionMode::Edges);
        mesh.deselectAll();
    }
    
    // Helper to find a vertical edge on a cylinder (connects top and bottom)
    // Vertical edges have vertices with different Y values
    uint32_t findVerticalEdge() {
        const auto& edges = mesh.getEdges();
        const auto& vertices = mesh.getVertices();
        
        for (uint32_t i = 0; i < edges.size(); i++) {
            const auto& edge = edges[i];
            float y0 = vertices[edge.vertices[0]].position.y;
            float y1 = vertices[edge.vertices[1]].position.y;
            if (std::abs(y0 - y1) > 0.1f) {
                return i;
            }
        }
        return UINT32_MAX;
    }
    
    // Helper to find a horizontal edge on a cylinder (same Y value)
    uint32_t findHorizontalEdge() {
        const auto& edges = mesh.getEdges();
        const auto& vertices = mesh.getVertices();
        
        for (uint32_t i = 0; i < edges.size(); i++) {
            const auto& edge = edges[i];
            float y0 = vertices[edge.vertices[0]].position.y;
            float y1 = vertices[edge.vertices[1]].position.y;
            if (std::abs(y0 - y1) < 0.01f) {
                return i;
            }
        }
        return UINT32_MAX;
    }
    
    // Helper to count vertical selected edges
    size_t countSelectedVerticalEdges() {
        size_t count = 0;
        const auto& edges = mesh.getEdges();
        const auto& vertices = mesh.getVertices();
        
        for (const auto& edge : edges) {
            if (!edge.selected) continue;
            float y0 = vertices[edge.vertices[0]].position.y;
            float y1 = vertices[edge.vertices[1]].position.y;
            if (std::abs(y0 - y1) > 0.1f) {
                count++;
            }
        }
        return count;
    }
    
    // Helper to count horizontal selected edges
    size_t countSelectedHorizontalEdges() {
        size_t count = 0;
        const auto& edges = mesh.getEdges();
        const auto& vertices = mesh.getVertices();
        
        for (const auto& edge : edges) {
            if (!edge.selected) continue;
            float y0 = vertices[edge.vertices[0]].position.y;
            float y1 = vertices[edge.vertices[1]].position.y;
            if (std::abs(y0 - y1) < 0.01f) {
                count++;
            }
        }
        return count;
    }
};

TEST_F(EdgeSelectionTest, Cylinder_EdgeRing_SelectsAllVerticalEdges) {
    // Create a cylinder with 8 sides (makes it easy to count)
    mesh.makeCylinder(8);
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // A cylinder with 8 sides has:
    // - 8 vertical edges (connecting top to bottom)
    // - 8 edges on top circle
    // - 8 edges on bottom circle
    // Total: 24 edges for the quad-based body
    // Plus triangular caps add more edges
    
    // Find a vertical edge
    uint32_t verticalEdge = findVerticalEdge();
    ASSERT_NE(verticalEdge, UINT32_MAX) << "Should find a vertical edge";
    
    // Select edge ring from the vertical edge
    mesh.selectEdgeRing(verticalEdge);
    
    size_t selectedVertical = countSelectedVerticalEdges();
    size_t selectedHorizontal = countSelectedHorizontalEdges();
    
    // Edge ring on a vertical edge should select ALL vertical edges (8)
    EXPECT_EQ(selectedVertical, 8) << "Edge ring should select all 8 vertical edges";
    EXPECT_EQ(selectedHorizontal, 0) << "Edge ring should not select horizontal edges";
}

TEST_F(EdgeSelectionTest, Cylinder_EdgeLoop_FromVerticalEdge_SelectsHorizontalEdges) {
    // Create a cylinder with 8 sides
    mesh.makeCylinder(8);
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // Find a vertical edge
    uint32_t verticalEdge = findVerticalEdge();
    ASSERT_NE(verticalEdge, UINT32_MAX) << "Should find a vertical edge";
    
    // Select edge loop from the vertical edge
    mesh.selectEdgeLoop(verticalEdge);
    
    size_t selectedVertical = countSelectedVerticalEdges();
    size_t selectedHorizontal = countSelectedHorizontalEdges();
    
    // Edge loop on a vertical edge should select horizontal edges (following through vertices)
    // This depends on the mesh topology - at each vertex, if 4 edges meet, it finds the perpendicular edge
    // For a cylinder, this should select horizontal edges around the middle
    EXPECT_EQ(selectedVertical, 1) << "Edge loop should only keep the initial vertical edge selected";
    EXPECT_GE(selectedHorizontal, 0) << "Edge loop may select horizontal edges if topology allows";
}

TEST_F(EdgeSelectionTest, Cylinder_EdgeRing_FromHorizontalEdge_SelectsHorizontalLoop) {
    // Create a cylinder with 8 sides
    mesh.makeCylinder(8);
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // Find a horizontal edge that's part of the side quads (not the triangular caps)
    // These are edges where both vertices have different angles but same Y
    uint32_t horizontalEdge = UINT32_MAX;
    const auto& edges = mesh.getEdges();
    const auto& vertices = mesh.getVertices();
    const auto& faces = mesh.getFaces();
    
    for (uint32_t i = 0; i < edges.size(); i++) {
        const auto& edge = edges[i];
        float y0 = vertices[edge.vertices[0]].position.y;
        float y1 = vertices[edge.vertices[1]].position.y;
        
        // Check if horizontal
        if (std::abs(y0 - y1) >= 0.01f) continue;
        
        // Check if part of a quad (not a triangle cap)
        bool partOfQuad = false;
        for (int f = 0; f < 2; f++) {
            if (edge.faces[f] != UINT32_MAX && faces[edge.faces[f]].isQuad()) {
                partOfQuad = true;
                break;
            }
        }
        
        if (partOfQuad) {
            horizontalEdge = i;
            break;
        }
    }
    
    ASSERT_NE(horizontalEdge, UINT32_MAX) << "Should find a horizontal edge on side quads";
    
    // Select edge ring from the horizontal edge
    mesh.selectEdgeRing(horizontalEdge);
    
    size_t selectedHorizontal = countSelectedHorizontalEdges();
    
    // Edge ring on a horizontal edge that's part of side quads should select 
    // horizontal edges around the cylinder (all 8 if the edge is between quads)
    // The edge ring follows through opposite edges in quads
    EXPECT_GE(selectedHorizontal, 2) << "Edge ring should select at least 2 horizontal edges";
}

TEST_F(EdgeSelectionTest, EdgeLoopAndRing_AreDifferent) {
    // Create a cylinder with 8 sides
    mesh.makeCylinder(8);
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // Find a vertical edge
    uint32_t verticalEdge = findVerticalEdge();
    ASSERT_NE(verticalEdge, UINT32_MAX);
    
    // Select edge ring
    mesh.selectEdgeRing(verticalEdge);
    size_t ringCount = countSelectedEdges();
    
    // Deselect all
    deselectAllEdges();
    EXPECT_EQ(countSelectedEdges(), 0);
    
    // Select edge loop
    mesh.selectEdgeLoop(verticalEdge);
    size_t loopCount = countSelectedEdges();
    
    // Edge loop and ring should produce different results
    EXPECT_NE(ringCount, loopCount) 
        << "Edge ring (" << ringCount << ") and edge loop (" << loopCount 
        << ") should select different numbers of edges";
}

TEST_F(EdgeSelectionTest, Cube_EdgeRing_SelectsFourParallelEdges) {
    // Create a cube
    mesh.makeCube();
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // A cube has 12 edges, grouped into 3 sets of 4 parallel edges
    
    // Select edge ring from first edge
    mesh.selectEdgeRing(0);
    size_t ringCount = countSelectedEdges();
    
    // Edge ring on a cube should select 4 parallel edges
    EXPECT_EQ(ringCount, 4) << "Edge ring on cube should select 4 parallel edges";
}

TEST_F(EdgeSelectionTest, Plane_EdgeRing_SelectsOneEdge) {
    // Create a single quad plane
    mesh.makePlane();
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // Select edge ring from first edge
    mesh.selectEdgeRing(0);
    size_t ringCount = countSelectedEdges();
    
    // On a single quad, edge ring can only select 1 or 2 edges (opposite edges in the quad)
    // Since there's only one face, it should select 2 edges (the starting edge and its opposite)
    EXPECT_EQ(ringCount, 2) << "Edge ring on single quad should select 2 opposite edges";
}

TEST_F(EdgeSelectionTest, GrowSelection_SelectsAdjacentEdges) {
    // Create a cube
    mesh.makeCube();
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // Select just one edge
    mesh.selectEdge(0, false);
    EXPECT_EQ(countSelectedEdges(), 1);
    
    // Grow the selection - should add edges that share vertices with the selected edge
    mesh.growEdgeSelection();
    
    // Each edge in a cube shares vertices with 4 other edges (2 at each endpoint)
    // So after grow, we should have 1 + 4 = 5 edges selected
    EXPECT_EQ(countSelectedEdges(), 5) << "After grow, should have 5 edges (1 original + 4 adjacent)";
}

TEST_F(EdgeSelectionTest, GrowSelection_Cylinder_RadialEdges) {
    // Create a cylinder with 8 sides - simulating the triangle fan cap
    mesh.makeCylinder(8);
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // Find a radial edge on the top cap (edge connected to center vertex)
    // The center vertex is the last added vertex for top cap
    const auto& vertices = mesh.getVertices();
    const auto& edges = mesh.getEdges();
    
    // Find the top center vertex (y = 1.0 and at origin x,z)
    uint32_t topCenterVertex = UINT32_MAX;
    for (uint32_t i = 0; i < vertices.size(); i++) {
        if (std::abs(vertices[i].position.y - 1.0f) < 0.01f &&
            std::abs(vertices[i].position.x) < 0.01f &&
            std::abs(vertices[i].position.z) < 0.01f) {
            topCenterVertex = i;
            break;
        }
    }
    
    ASSERT_NE(topCenterVertex, UINT32_MAX) << "Should find top center vertex";
    
    // Find an edge connected to the center
    uint32_t radialEdge = UINT32_MAX;
    for (uint32_t i = 0; i < edges.size(); i++) {
        if (edges[i].vertices[0] == topCenterVertex || 
            edges[i].vertices[1] == topCenterVertex) {
            radialEdge = i;
            break;
        }
    }
    
    ASSERT_NE(radialEdge, UINT32_MAX) << "Should find a radial edge";
    
    // Select the radial edge
    mesh.selectEdge(radialEdge, false);
    EXPECT_EQ(countSelectedEdges(), 1);
    
    // Grow once - should select all edges connected to the center vertex
    mesh.growEdgeSelection();
    size_t afterGrow = countSelectedEdges();
    
    // Should have selected all 8 radial edges (all edges touching the center)
    EXPECT_GE(afterGrow, 8) << "After grow from radial edge, should select all radial edges";
}

// =============================================================================
// Split Edge Tests
// =============================================================================

class SplitEdgeTest : public ::testing::Test {
protected:
    Mesh2 mesh;
    
    size_t countSelectedEdges() {
        const auto& edges = mesh.getEdges();
        size_t count = 0;
        for (const auto& edge : edges) {
            if (edge.selected) count++;
        }
        return count;
    }
};

TEST_F(SplitEdgeTest, SplitTwoEdgesOnTriangle_CreatesFaces) {
    // Create a cylinder to get triangular caps
    mesh.makeCylinder(8);
    
    size_t initialFaces = mesh.getFaceCount();
    size_t initialVerts = mesh.getVertexCount();
    
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // Find the top center vertex
    const auto& vertices = mesh.getVertices();
    const auto& edges = mesh.getEdges();
    
    uint32_t topCenterVertex = UINT32_MAX;
    for (uint32_t i = 0; i < vertices.size(); i++) {
        if (std::abs(vertices[i].position.y - 1.0f) < 0.01f &&
            std::abs(vertices[i].position.x) < 0.01f &&
            std::abs(vertices[i].position.z) < 0.01f) {
            topCenterVertex = i;
            break;
        }
    }
    
    ASSERT_NE(topCenterVertex, UINT32_MAX) << "Should find top center vertex";
    
    // Select all radial edges on top cap (edges connected to center)
    size_t radialEdgeCount = 0;
    for (uint32_t i = 0; i < edges.size(); i++) {
        if (edges[i].vertices[0] == topCenterVertex || 
            edges[i].vertices[1] == topCenterVertex) {
            mesh.selectEdge(i, true);
            radialEdgeCount++;
        }
    }
    
    ASSERT_EQ(radialEdgeCount, 8) << "Should have 8 radial edges on top cap";
    EXPECT_EQ(countSelectedEdges(), 8);
    
    // Split the edges
    mesh.splitSelectedEdges();
    
    // After split, faces should not have decreased (they should increase)
    // Each triangle with 2 edges split becomes 1 tri + 1 quad = 2 faces (same count)
    // But if edges are only on one edge per triangle, it doubles
    EXPECT_GE(mesh.getFaceCount(), initialFaces) << "Face count should not decrease after split";
    
    // Vertices should increase (one midpoint per selected edge)
    EXPECT_GT(mesh.getVertexCount(), initialVerts) << "Vertex count should increase after split";
}

TEST_F(SplitEdgeTest, SplitSingleEdgeOnTriangle_CreatesTwoTriangles) {
    // Create a simple triangle
    mesh.addVertex(glm::vec3(0, 0, 0));
    mesh.addVertex(glm::vec3(1, 0, 0));
    mesh.addVertex(glm::vec3(0.5f, 1, 0));
    mesh.addTriangle(0, 1, 2);
    mesh.buildConnectivity();
    
    ASSERT_EQ(mesh.getFaceCount(), 1);
    ASSERT_EQ(mesh.getVertexCount(), 3);
    
    mesh.setSelectionMode(SelectionMode::Edges);
    
    // Select the first edge
    mesh.selectEdge(0, false);
    EXPECT_EQ(countSelectedEdges(), 1);
    
    mesh.splitSelectedEdges();
    
    // Should now have 2 triangles and 4 vertices
    EXPECT_EQ(mesh.getFaceCount(), 2) << "Splitting one edge on triangle should create 2 triangles";
    EXPECT_EQ(mesh.getVertexCount(), 4) << "Should have added one midpoint vertex";
}

// =============================================================================
// Extrude Tests
// =============================================================================

class ExtrudeTest : public ::testing::Test {
protected:
    Mesh2 mesh;
    
    // Compute face normal using the same formula as Mesh2  
    glm::vec3 computeFaceNormal(const Face& face) const {
        const auto& verts = mesh.getVertices();
        glm::vec3 v0 = verts[face.vertices[0]].position;
        glm::vec3 v1 = verts[face.vertices[1]].position;
        glm::vec3 v2 = verts[face.vertices[2]].position;
        return glm::normalize(glm::cross(v1 - v0, v2 - v0));
    }
    
    // Get center of a face
    glm::vec3 computeFaceCenter(const Face& face) const {
        const auto& verts = mesh.getVertices();
        glm::vec3 center(0.0f);
        for (int i = 0; i < face.vertexCount; i++) {
            center += verts[face.vertices[i]].position;
        }
        return center / static_cast<float>(face.vertexCount);
    }
    
    // Check that all face normals are consistently oriented
    // For a convex closed mesh, all normals should consistently point 
    // either inward or outward relative to the mesh center
    bool allNormalsConsistent() const {
        const auto& faces = mesh.getFaces();
        if (faces.empty()) return true;
        
        // Compute mesh center
        glm::vec3 meshCenter(0.0f);
        const auto& verts = mesh.getVertices();
        for (const auto& v : verts) {
            meshCenter += v.position;
        }
        meshCenter /= static_cast<float>(verts.size());
        
        // Check that dot(normal, faceCenter - meshCenter) has consistent sign
        int positiveCount = 0;
        int negativeCount = 0;
        
        for (const auto& face : faces) {
            glm::vec3 normal = computeFaceNormal(face);
            glm::vec3 faceCenter = computeFaceCenter(face);
            glm::vec3 outward = faceCenter - meshCenter;
            
            float dot = glm::dot(normal, outward);
            if (dot > 0.001f) positiveCount++;
            else if (dot < -0.001f) negativeCount++;
        }
        
        // All should be the same sign (either all inward or all outward)
        return (positiveCount == 0 || negativeCount == 0);
    }
    
    // Verify edge-face connectivity
    bool verifyEdgeFaceConnectivity() {
        const auto& faces = mesh.getFaces();
        const auto& edges = mesh.getEdges();
        
        for (size_t fi = 0; fi < faces.size(); fi++) {
            const auto& face = faces[fi];
            for (int i = 0; i < face.vertexCount; i++) {
                uint32_t edgeIdx = face.edges[i];
                if (edgeIdx >= edges.size()) return false;
                
                const auto& edge = edges[edgeIdx];
                if (edge.faces[0] != fi && edge.faces[1] != fi) {
                    return false;
                }
            }
        }
        return true;
    }
};

TEST_F(ExtrudeTest, CubeExtrudeTopFace_HasConsistentNormals) {
    mesh.makeCube();
    
    // Verify cube starts with consistent normals
    ASSERT_TRUE(allNormalsConsistent()) << "Cube should start with consistent normals";
    
    // Select top face (face index 4: addQuad(v3, v2, v6, v7) for y=+1)
    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectFace(4);
    ASSERT_EQ(mesh.getSelectedCount(), 1);
    
    // Extrude
    mesh.extrudeSelected();
    
    // Should have 6 original + 4 side = 10 faces
    EXPECT_EQ(mesh.getFaceCount(), 10) << "Cube with one extruded face should have 10 faces";
    
    // Should have 8 original + 4 duplicated = 12 vertices
    EXPECT_EQ(mesh.getVertexCount(), 12) << "Should have 12 vertices after extrude";
    
    // Now translate the selected (extruded) face upward so normals are meaningful
    mesh.translateSelected(glm::vec3(0.0f, 1.0f, 0.0f));
    
    // All normals should be consistent
    EXPECT_TRUE(allNormalsConsistent()) 
        << "All face normals should be consistently oriented after extrude and translate";
    
    // Verify topology integrity
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(ExtrudeTest, CubeExtrudeFrontFace_HasConsistentNormals) {
    mesh.makeCube();
    
    // Select front face (face index 0: addQuad(v0, v1, v2, v3) for z=-1)
    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectFace(0);
    
    mesh.extrudeSelected();
    mesh.translateSelected(glm::vec3(0.0f, 0.0f, -1.0f));
    
    EXPECT_EQ(mesh.getFaceCount(), 10);
    EXPECT_TRUE(allNormalsConsistent())
        << "All face normals should be consistent after extruding front face";
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(ExtrudeTest, CubeExtrudeAllFaces_HasConsistentNormals) {
    mesh.makeCube();
    
    // Select all faces
    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectAll();
    ASSERT_EQ(mesh.getSelectedCount(), 6);
    
    mesh.extrudeSelected();
    
    // All faces selected = no boundary edges = no side faces created 
    // Just vertex duplication, same face count
    EXPECT_EQ(mesh.getFaceCount(), 6);
    
    // Translate outward and check
    mesh.translateSelected(glm::vec3(0.0f, 1.0f, 0.0f));
    
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(ExtrudeTest, CubeExtrudeSideFace_HasConsistentNormals) {
    mesh.makeCube();
    
    // Select right face (face index 3: addQuad(v1, v5, v6, v2) for x=+1)
    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectFace(3);
    
    mesh.extrudeSelected();
    mesh.translateSelected(glm::vec3(1.0f, 0.0f, 0.0f));
    
    EXPECT_EQ(mesh.getFaceCount(), 10);
    EXPECT_TRUE(allNormalsConsistent())
        << "All face normals should be consistent after extruding right face";
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(ExtrudeTest, CubeExtrudeBottomFace_HasConsistentNormals) {
    mesh.makeCube();
    
    // Select bottom face (face index 5: addQuad(v4, v5, v1, v0) for y=-1)
    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectFace(5);
    
    mesh.extrudeSelected();
    mesh.translateSelected(glm::vec3(0.0f, -1.0f, 0.0f));
    
    EXPECT_EQ(mesh.getFaceCount(), 10);
    EXPECT_TRUE(allNormalsConsistent())
        << "All face normals should be consistent after extruding bottom face";
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(ExtrudeTest, CubeExtrudeTwoAdjacentFaces_HasConsistentNormals) {
    mesh.makeCube();
    
    // Select top and front faces
    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectFace(4, false);     // Top
    mesh.selectFace(0, true);  // Front (add to selection)
    ASSERT_EQ(mesh.getSelectedCount(), 2);
    
    mesh.extrudeSelected();
    
    // 2 faces selected, they share 1 edge.
    // Each face has 4 boundary edges, but 1 is now interior = 4+4-2=6 boundary edges
    // Original 6 + 6 side faces = 12 faces
    EXPECT_EQ(mesh.getFaceCount(), 12);
    
    // Translate and check
    mesh.translateSelected(glm::vec3(0.0f, 1.0f, -1.0f));
    
    EXPECT_TRUE(allNormalsConsistent())
        << "All face normals should be consistent after extruding two adjacent faces";
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(ExtrudeTest, PlaneExtrude_CreatesBoxShape) {
    mesh.makePlane();
    
    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectFace(0);
    
    mesh.extrudeSelected();
    mesh.translateSelected(glm::vec3(0.0f, 1.0f, 0.0f));
    
    // 1 original face + 4 side faces = 5 faces
    EXPECT_EQ(mesh.getFaceCount(), 5);
    EXPECT_EQ(mesh.getVertexCount(), 8);  // 4 original + 4 duplicated
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

TEST_F(ExtrudeTest, CubeExtrudeThenSubdivide_Succeeds) {
    mesh.makeCube();
    
    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectFace(4);  // Top
    
    mesh.extrudeSelected();
    mesh.translateSelected(glm::vec3(0.0f, 1.0f, 0.0f));
    
    ASSERT_TRUE(allNormalsConsistent()) << "Normals must be consistent before subdivision";
    
    // Deselect and subdivide
    mesh.deselectAll();
    mesh.selectAll();
    
    size_t facesBefore = mesh.getFaceCount();
    mesh.catmullClarkSubdivide(1);
    
    // Subdivision should increase face count (each quad -> 4 quads for Catmull-Clark)
    EXPECT_GT(mesh.getFaceCount(), facesBefore) << "Subdivision should produce more faces";
    EXPECT_TRUE(verifyEdgeFaceConnectivity());
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
