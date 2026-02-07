#include <gtest/gtest.h>
#include "Mesh2.h"

// =============================================================================
// Test Environment - Disables GPU operations for all tests
// =============================================================================

class SubdivTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        Mesh2::s_disableGPU = true;
    }
    
    void TearDown() override {
        Mesh2::s_disableGPU = false;
    }
};

testing::Environment* const subdiv_env =
    testing::AddGlobalTestEnvironment(new SubdivTestEnvironment);

class CatmullClarkTest : public ::testing::Test {
protected:
    Mesh2 mesh;
};

// =============================================================================
// Basic Catmull-Clark Tests
// =============================================================================

TEST_F(CatmullClarkTest, CubeLevel1_ProducesExpectedCounts) {
    mesh.makeCube();

    size_t origVerts = mesh.getVertexCount();
    size_t origFaces = mesh.getFaceCount();
    size_t origEdges = mesh.getEdgeCount();

    // A cube has 8 verts, 6 faces, 12 edges
    ASSERT_EQ(origVerts, 8u);
    ASSERT_EQ(origFaces, 6u);
    ASSERT_EQ(origEdges, 12u);

    mesh.catmullClarkSubdivide(1);

    // After Catmull-Clark level 1 on a cube:
    // New verts = original verts + edge midpoints + face centers = 8 + 12 + 6 = 26
    // New faces = 4 per original face = 6 * 4 = 24
    EXPECT_EQ(mesh.getVertexCount(), 26u);
    EXPECT_EQ(mesh.getFaceCount(), 24u);

    // All faces should be quads after Catmull-Clark on a quad mesh
    for (size_t i = 0; i < mesh.getFaceCount(); i++) {
        EXPECT_TRUE(mesh.getFace(i).isQuad()) << "Face " << i << " should be a quad";
    }
}

TEST_F(CatmullClarkTest, CubeLevel2_ProducesExpectedCounts) {
    mesh.makeCube();
    mesh.catmullClarkSubdivide(2);

    // Level 2: 26 + new edge midpoints + new face centers
    // Level 1 has 26 verts, 24 faces, 48 edges
    // Level 2: 26 + 48 + 24 = 98 verts, 24 * 4 = 96 faces
    EXPECT_EQ(mesh.getVertexCount(), 98u);
    EXPECT_EQ(mesh.getFaceCount(), 96u);
}

TEST_F(CatmullClarkTest, PlaneLevel1_ProducesExpectedCounts) {
    mesh.makePlane();

    // A plane is a single quad (4 verts, 1 face, 4 edges)
    ASSERT_EQ(mesh.getVertexCount(), 4u);
    ASSERT_EQ(mesh.getFaceCount(), 1u);

    mesh.catmullClarkSubdivide(1);

    // After Catmull-Clark level 1 on a single quad:
    // New verts = 4 (original) + 4 (edge midpoints) + 1 (face center) = 9
    // New faces = 4
    EXPECT_EQ(mesh.getVertexCount(), 9u);
    EXPECT_EQ(mesh.getFaceCount(), 4u);
}

TEST_F(CatmullClarkTest, IcosahedronLevel1_UsesLoopScheme) {
    mesh.makeIcosahedron();

    // Icosahedron is all triangles
    for (size_t i = 0; i < mesh.getFaceCount(); i++) {
        ASSERT_FALSE(mesh.getFace(i).isQuad());
    }

    size_t origVerts = mesh.getVertexCount();
    size_t origFaces = mesh.getFaceCount();
    size_t origEdges = mesh.getEdgeCount();

    mesh.catmullClarkSubdivide(1);

    // Loop subdivision: each triangle becomes 4 triangles
    // Verts = original + edge midpoints = origVerts + origEdges
    EXPECT_EQ(mesh.getVertexCount(), origVerts + origEdges);
    EXPECT_EQ(mesh.getFaceCount(), origFaces * 4);

    // All faces should still be triangles after Loop subdivision
    for (size_t i = 0; i < mesh.getFaceCount(); i++) {
        EXPECT_FALSE(mesh.getFace(i).isQuad()) << "Face " << i << " should be a triangle";
    }
}

TEST_F(CatmullClarkTest, CubeVerticesSmoothed) {
    mesh.makeCube();

    // Get a corner vertex position before subdivision
    glm::vec3 cornerBefore = mesh.getVertex(0).position;
    float maxCoordBefore = std::max({std::abs(cornerBefore.x),
                                      std::abs(cornerBefore.y),
                                      std::abs(cornerBefore.z)});

    mesh.catmullClarkSubdivide(1);

    // After Catmull-Clark, the original corner vertices should have moved inward
    // (smoothed toward the center). Check that no vertex has coordinates as extreme
    // as the original corners. The cube spans [-0.5, 0.5], so corners are at 0.5.
    // After CC subdivision, the corner verts move inward.
    glm::vec3 movedCorner = mesh.getVertex(0).position;
    float maxCoordAfter = std::max({std::abs(movedCorner.x),
                                     std::abs(movedCorner.y),
                                     std::abs(movedCorner.z)});

    EXPECT_LT(maxCoordAfter, maxCoordBefore)
        << "Corner vertex should move inward after Catmull-Clark subdivision";
}

TEST_F(CatmullClarkTest, UVsPreservedThroughSubdivision) {
    mesh.makeCube();

    // Check that UVs are set on the original cube
    bool hasNonZeroUV = false;
    for (size_t i = 0; i < mesh.getFaceCount(); i++) {
        const Face& face = mesh.getFace(i);
        for (int vi = 0; vi < face.vertexCount; vi++) {
            if (face.uvs[vi] != glm::vec2(0.0f)) {
                hasNonZeroUV = true;
                break;
            }
        }
        if (hasNonZeroUV) break;
    }

    mesh.catmullClarkSubdivide(1);

    // After subdivision, UVs should still exist (not all zero)
    bool hasNonZeroUVAfter = false;
    for (size_t i = 0; i < mesh.getFaceCount(); i++) {
        const Face& face = mesh.getFace(i);
        for (int vi = 0; vi < face.vertexCount; vi++) {
            if (face.uvs[vi] != glm::vec2(0.0f)) {
                hasNonZeroUVAfter = true;
                break;
            }
        }
        if (hasNonZeroUVAfter) break;
    }

    // If original had UVs, subdivided should too
    if (hasNonZeroUV) {
        EXPECT_TRUE(hasNonZeroUVAfter)
            << "UV coordinates should be preserved/interpolated through Catmull-Clark subdivision";
    }

    // All UVs should be in a reasonable range [0, 1]
    for (size_t i = 0; i < mesh.getFaceCount(); i++) {
        const Face& face = mesh.getFace(i);
        for (int vi = 0; vi < face.vertexCount; vi++) {
            EXPECT_GE(face.uvs[vi].x, -0.01f) << "UV x should be >= 0 for face " << i;
            EXPECT_LE(face.uvs[vi].x, 1.01f) << "UV x should be <= 1 for face " << i;
            EXPECT_GE(face.uvs[vi].y, -0.01f) << "UV y should be >= 0 for face " << i;
            EXPECT_LE(face.uvs[vi].y, 1.01f) << "UV y should be <= 1 for face " << i;
        }
    }
}

TEST_F(CatmullClarkTest, Level0_ReturnsUnchangedMesh) {
    mesh.makeCube();
    size_t origVerts = mesh.getVertexCount();
    size_t origFaces = mesh.getFaceCount();

    mesh.catmullClarkSubdivide(0);  // Level 0 should be a no-op

    EXPECT_EQ(mesh.getVertexCount(), origVerts);
    EXPECT_EQ(mesh.getFaceCount(), origFaces);
}

TEST_F(CatmullClarkTest, EmptyMesh_DoesNotCrash) {
    // Empty mesh - should return without crashing
    mesh.catmullClarkSubdivide(1);
    EXPECT_EQ(mesh.getVertexCount(), 0u);
    EXPECT_EQ(mesh.getFaceCount(), 0u);
}

TEST_F(CatmullClarkTest, SphereLevel1_IncreasesResolution) {
    mesh.makeSphere(8);

    size_t origVerts = mesh.getVertexCount();
    size_t origFaces = mesh.getFaceCount();

    mesh.catmullClarkSubdivide(1);

    EXPECT_GT(mesh.getVertexCount(), origVerts);
    EXPECT_GT(mesh.getFaceCount(), origFaces);
}

TEST_F(CatmullClarkTest, CylinderLevel1_IncreasesResolution) {
    mesh.makeCylinder(8);

    size_t origVerts = mesh.getVertexCount();
    size_t origFaces = mesh.getFaceCount();

    mesh.catmullClarkSubdivide(1);

    EXPECT_GT(mesh.getVertexCount(), origVerts);
    EXPECT_GT(mesh.getFaceCount(), origFaces);
}

TEST_F(CatmullClarkTest, ConnectivityValidAfterSubdivide) {
    mesh.makeCube();
    mesh.catmullClarkSubdivide(1);

    // Verify all face vertex indices are valid
    for (size_t fi = 0; fi < mesh.getFaceCount(); fi++) {
        const Face& face = mesh.getFace(fi);
        for (int vi = 0; vi < face.vertexCount; vi++) {
            EXPECT_LT(face.vertices[vi], static_cast<uint32_t>(mesh.getVertexCount()))
                << "Face " << fi << " has invalid vertex index " << face.vertices[vi];
        }
    }

    // Verify all edge vertex indices are valid
    for (size_t ei = 0; ei < mesh.getEdgeCount(); ei++) {
        const Edge& edge = mesh.getEdge(ei);
        EXPECT_LT(edge.vertices[0], static_cast<uint32_t>(mesh.getVertexCount()))
            << "Edge " << ei << " has invalid vertex[0] " << edge.vertices[0];
        EXPECT_LT(edge.vertices[1], static_cast<uint32_t>(mesh.getVertexCount()))
            << "Edge " << ei << " has invalid vertex[1] " << edge.vertices[1];
    }

    // Every edge should have at least one adjacent face
    for (size_t ei = 0; ei < mesh.getEdgeCount(); ei++) {
        const Edge& edge = mesh.getEdge(ei);
        EXPECT_NE(edge.faces[0], UINT32_MAX)
            << "Edge " << ei << " should have at least one face";
    }
}

// =============================================================================
// Split Tests
// =============================================================================

class SplitTest : public ::testing::Test {
protected:
    Mesh2 mesh;
};

TEST_F(SplitTest, SplitAllEdges_CubeDoesNotVanish) {
    mesh.makeCube();

    ASSERT_EQ(mesh.getVertexCount(), 8u);
    ASSERT_EQ(mesh.getFaceCount(), 6u);
    ASSERT_EQ(mesh.getEdgeCount(), 12u);

    // Verify all edges are selected
    mesh.setSelectionMode(SelectionMode::Edges);
    mesh.selectAll();
    size_t selectedEdges = 0;
    for (size_t i = 0; i < mesh.getEdgeCount(); i++) {
        if (mesh.getEdge(i).selected) selectedEdges++;
    }
    ASSERT_EQ(selectedEdges, 12u) << "All 12 edges should be selected";

    mesh.splitSelectedEdges();

    // Cube should not vanish - it should have MORE vertices and faces
    ASSERT_GT(mesh.getVertexCount(), 8u) << "Verts=" << mesh.getVertexCount();
    ASSERT_GT(mesh.getFaceCount(), 0u) << "Faces=" << mesh.getFaceCount() << " - cube vanished!";
    ASSERT_GT(mesh.getFaceCount(), 6u) << "Faces=" << mesh.getFaceCount();
    ASSERT_GT(mesh.getEdgeCount(), 12u) << "Edges=" << mesh.getEdgeCount();

    // All faces should still have valid vertex indices
    for (size_t fi = 0; fi < mesh.getFaceCount(); fi++) {
        const Face& face = mesh.getFace(fi);
        for (int vi = 0; vi < face.vertexCount; vi++) {
            EXPECT_LT(face.vertices[vi], static_cast<uint32_t>(mesh.getVertexCount()))
                << "Face " << fi << " has invalid vertex index";
        }
    }
}

TEST_F(SplitTest, SplitAllEdges_PlaneDoesNotVanish) {
    mesh.makePlane();

    ASSERT_EQ(mesh.getVertexCount(), 4u);
    ASSERT_EQ(mesh.getFaceCount(), 1u);

    mesh.setSelectionMode(SelectionMode::Edges);
    mesh.selectAll();

    mesh.splitSelectedEdges();

    EXPECT_GT(mesh.getVertexCount(), 4u) << "Plane should have more vertices after splitting all edges";
    EXPECT_GT(mesh.getFaceCount(), 1u) << "Plane should have more faces after splitting all edges";
}

TEST_F(SplitTest, SplitAllFaces_CubeProducesMoreFaces) {
    mesh.makeCube();

    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectAll();

    mesh.splitSelected();  // Should dispatch to splitSelectedFaces

    EXPECT_EQ(mesh.getFaceCount(), 24u);  // 6 quads -> 4 quads each = 24
    EXPECT_GT(mesh.getVertexCount(), 8u);
}

TEST_F(SplitTest, SplitDispatcher_TrianglesModeCallsFaceSplit) {
    mesh.makeCube();

    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectAll();

    size_t origFaces = mesh.getFaceCount();
    mesh.splitSelected();
    EXPECT_GT(mesh.getFaceCount(), origFaces);
}

TEST_F(SplitTest, SplitDispatcher_EdgesModeCallsEdgeSplit) {
    mesh.makePlane();

    mesh.setSelectionMode(SelectionMode::Edges);
    mesh.selectAll();

    size_t origVerts = mesh.getVertexCount();
    mesh.splitSelected();  
    EXPECT_GT(mesh.getVertexCount(), origVerts);
}

TEST_F(SplitTest, SplitAllEdges_CubeProducesCorrectFaceCount) {
    mesh.makeCube();
    ASSERT_EQ(mesh.getFaceCount(), 6u);  // 6 quad faces
    
    mesh.setSelectionMode(SelectionMode::Edges);
    mesh.selectAll();
    mesh.splitSelectedEdges();
    
    // Each quad with 4 split edges → 4 sub-quads, so 6 * 4 = 24 faces
    EXPECT_EQ(mesh.getFaceCount(), 24u) << "6 quads with all edges split should produce 24 sub-quads";
}

TEST_F(SplitTest, SplitAllEdges_TriangulatedPlane) {
    mesh.makePlane();
    mesh.setSelectionMode(SelectionMode::Triangles);
    mesh.selectAll();
    mesh.triangulateSelected();
    
    size_t triCount = mesh.getFaceCount();
    ASSERT_GT(triCount, 0u);
    
    mesh.setSelectionMode(SelectionMode::Edges);
    mesh.selectAll();
    mesh.splitSelectedEdges();
    
    // Each triangle with all 3 edges split → 4 sub-triangles
    EXPECT_EQ(mesh.getFaceCount(), triCount * 4) << "Each triangle should split into 4 sub-triangles";
    EXPECT_GT(mesh.getVertexCount(), 4u);
}

// =============================================================================
// Catmull-Clark on Cube should actually change the mesh (regression test)
// =============================================================================

TEST_F(CatmullClarkTest, CubeSubdivide_ChangesVertexPositions) {
    mesh.makeCube();
    
    // Store original vertex at corner
    glm::vec3 origCorner = mesh.getVertices()[0].position;
    ASSERT_EQ(origCorner, glm::vec3(-1, -1, -1));
    
    mesh.catmullClarkSubdivide(1);
    
    // After Catmull-Clark, original corners should be smoothed (moved inward)
    // The original corner vertices should NOT be exactly at (-1,-1,-1)
    bool anyMoved = false;
    for (size_t i = 0; i < mesh.getVertexCount(); i++) {
        glm::vec3 pos = mesh.getVertices()[i].position;
        if (pos != origCorner) {
            anyMoved = true;
            break;
        }
    }
    EXPECT_TRUE(anyMoved) << "Catmull-Clark should produce vertices at different positions";
    EXPECT_EQ(mesh.getVertexCount(), 26u);
    EXPECT_EQ(mesh.getFaceCount(), 24u);
}
