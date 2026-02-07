#include <gtest/gtest.h>
#include "Mesh2.h"
#include "ItemCollection.h"
#include "Item.h"
#include <glm/glm.hpp>
#include <cmath>

// =============================================================================
// Test Environment - Disables GPU operations for all tests
// =============================================================================

class UVTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        Mesh2::s_disableGPU = true;
    }
    
    void TearDown() override {
        Mesh2::s_disableGPU = false;
    }
};

testing::Environment* const uv_env = 
    testing::AddGlobalTestEnvironment(new UVTestEnvironment);

// =============================================================================
// Helper Utilities
// =============================================================================

bool uvEqual(const glm::vec2& a, const glm::vec2& b, float epsilon = 0.001f) {
    return std::abs(a.x - b.x) < epsilon && std::abs(a.y - b.y) < epsilon;
}

// =============================================================================
// Default UV Tests - Verify primitives have valid UVs after creation
// =============================================================================

class DefaultUVTest : public ::testing::Test {
protected:
    Mesh2 mesh;
};

TEST_F(DefaultUVTest, Cube_HasValidDefaultUVs) {
    mesh.makeCube();
    
    const auto& faces = mesh.getFaces();
    ASSERT_EQ(faces.size(), 6u);
    
    for (const auto& face : faces) {
        ASSERT_EQ(face.vertexCount, 4);
        // Each UV should be in [0,1] range for default box projection
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_GE(face.uvs[i].x, 0.0f);
            EXPECT_LE(face.uvs[i].x, 1.0f);
            EXPECT_GE(face.uvs[i].y, 0.0f);
            EXPECT_LE(face.uvs[i].y, 1.0f);
        }
    }
}

TEST_F(DefaultUVTest, Plane_HasValidDefaultUVs) {
    mesh.makePlane();
    
    const auto& faces = mesh.getFaces();
    ASSERT_EQ(faces.size(), 1u);
    ASSERT_EQ(faces[0].vertexCount, 4);
    
    // Plane should have UVs covering the unit square
    for (int i = 0; i < faces[0].vertexCount; i++) {
        EXPECT_GE(faces[0].uvs[i].x, 0.0f);
        EXPECT_LE(faces[0].uvs[i].x, 1.0f);
        EXPECT_GE(faces[0].uvs[i].y, 0.0f);
        EXPECT_LE(faces[0].uvs[i].y, 1.0f);
    }
}

TEST_F(DefaultUVTest, Cylinder_HasValidDefaultUVs) {
    mesh.makeCylinder(8);
    
    const auto& faces = mesh.getFaces();
    EXPECT_GT(faces.size(), 0u);
    
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_GE(face.uvs[i].x, -0.01f);
            EXPECT_LE(face.uvs[i].x, 1.01f);
            EXPECT_GE(face.uvs[i].y, -0.01f);
            EXPECT_LE(face.uvs[i].y, 1.01f);
        }
    }
}

TEST_F(DefaultUVTest, Sphere_HasValidDefaultUVs) {
    mesh.makeSphere(8);
    
    const auto& faces = mesh.getFaces();
    EXPECT_GT(faces.size(), 0u);
    
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_GE(face.uvs[i].x, -0.01f);
            EXPECT_LE(face.uvs[i].x, 1.01f);
            EXPECT_GE(face.uvs[i].y, -0.01f);
            EXPECT_LE(face.uvs[i].y, 1.01f);
        }
    }
}

TEST_F(DefaultUVTest, Icosahedron_HasValidDefaultUVs) {
    mesh.makeIcosahedron();
    
    const auto& faces = mesh.getFaces();
    EXPECT_GT(faces.size(), 0u);
    
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_GE(face.uvs[i].x, -0.01f);
            EXPECT_LE(face.uvs[i].x, 1.01f);
            EXPECT_GE(face.uvs[i].y, -0.01f);
            EXPECT_LE(face.uvs[i].y, 1.01f);
        }
    }
}

// =============================================================================
// setFaceUVs Tests
// =============================================================================

class SetFaceUVTest : public ::testing::Test {
protected:
    Mesh2 mesh;
    
    void SetUp() override {
        mesh.makePlane();
    }
};

TEST_F(SetFaceUVTest, SetFaceUVs_UpdatesCorrectly) {
    glm::vec2 uv0(0.1f, 0.2f);
    glm::vec2 uv1(0.3f, 0.4f);
    glm::vec2 uv2(0.5f, 0.6f);
    glm::vec2 uv3(0.7f, 0.8f);
    
    mesh.setFaceUVs(0, uv0, uv1, uv2, uv3);
    
    const auto& faces = mesh.getFaces();
    EXPECT_TRUE(uvEqual(faces[0].uvs[0], uv0));
    EXPECT_TRUE(uvEqual(faces[0].uvs[1], uv1));
    EXPECT_TRUE(uvEqual(faces[0].uvs[2], uv2));
    EXPECT_TRUE(uvEqual(faces[0].uvs[3], uv3));
}

TEST_F(SetFaceUVTest, SetFaceUVs_DoesNotAffectOtherFaces) {
    mesh.makeCube();
    
    glm::vec2 newUV(0.99f, 0.99f);
    mesh.setFaceUVs(0, newUV, newUV, newUV, newUV);
    
    const auto& faces = mesh.getFaces();
    // Face 0 should be updated
    EXPECT_TRUE(uvEqual(faces[0].uvs[0], newUV));
    
    // Face 1 should still have its original UVs (not all 0.99)
    bool face1Changed = true;
    for (int i = 0; i < faces[1].vertexCount; i++) {
        if (!uvEqual(faces[1].uvs[i], newUV)) {
            face1Changed = false;
            break;
        }
    }
    EXPECT_FALSE(face1Changed);
}

// =============================================================================
// Seam Marking Tests
// =============================================================================

class SeamTest : public ::testing::Test {
protected:
    Mesh2 mesh;
    
    void SetUp() override {
        mesh.makeCube();
    }
};

TEST_F(SeamTest, NoSeamsByDefault) {
    const auto& edges = mesh.getEdges();
    for (size_t i = 0; i < edges.size(); i++) {
        EXPECT_FALSE(mesh.isEdgeSeam(static_cast<uint32_t>(i)))
            << "Edge " << i << " should not be a seam by default";
    }
}

TEST_F(SeamTest, MarkEdgeAsSeam) {
    mesh.markEdgeAsSeam(0, true);
    EXPECT_TRUE(mesh.isEdgeSeam(0));
    
    // Other edges should still not be seams
    EXPECT_FALSE(mesh.isEdgeSeam(1));
}

TEST_F(SeamTest, UnmarkSeam) {
    mesh.markEdgeAsSeam(0, true);
    EXPECT_TRUE(mesh.isEdgeSeam(0));
    
    mesh.markEdgeAsSeam(0, false);
    EXPECT_FALSE(mesh.isEdgeSeam(0));
}

TEST_F(SeamTest, MarkMultipleEdgesAsSeam) {
    mesh.markEdgeAsSeam(0, true);
    mesh.markEdgeAsSeam(2, true);
    mesh.markEdgeAsSeam(5, true);
    
    EXPECT_TRUE(mesh.isEdgeSeam(0));
    EXPECT_FALSE(mesh.isEdgeSeam(1));
    EXPECT_TRUE(mesh.isEdgeSeam(2));
    EXPECT_FALSE(mesh.isEdgeSeam(3));
    EXPECT_FALSE(mesh.isEdgeSeam(4));
    EXPECT_TRUE(mesh.isEdgeSeam(5));
}

TEST_F(SeamTest, ClearAllSeams) {
    // Mark several edges as seams
    mesh.markEdgeAsSeam(0, true);
    mesh.markEdgeAsSeam(3, true);
    mesh.markEdgeAsSeam(7, true);
    
    mesh.clearAllSeams();
    
    const auto& edges = mesh.getEdges();
    for (size_t i = 0; i < edges.size(); i++) {
        EXPECT_FALSE(mesh.isEdgeSeam(static_cast<uint32_t>(i)))
            << "Edge " << i << " should not be a seam after clearAllSeams";
    }
}

TEST_F(SeamTest, MarkSelectedEdgesAsSeam) {
    // Select a few edges
    mesh.selectEdge(0, false);
    mesh.selectEdge(3, true);
    
    mesh.markSelectedEdgesAsSeam(true);
    
    EXPECT_TRUE(mesh.isEdgeSeam(0));
    EXPECT_FALSE(mesh.isEdgeSeam(1));
    EXPECT_FALSE(mesh.isEdgeSeam(2));
    EXPECT_TRUE(mesh.isEdgeSeam(3));
}

TEST_F(SeamTest, UnmarkSelectedEdgesAsSeam) {
    // First mark all edges
    const auto& edges = mesh.getEdges();
    for (size_t i = 0; i < edges.size(); i++) {
        mesh.markEdgeAsSeam(static_cast<uint32_t>(i), true);
    }
    
    // Select a few and unmark them
    mesh.selectEdge(0, false);
    mesh.selectEdge(3, true);
    
    mesh.markSelectedEdgesAsSeam(false);
    
    EXPECT_FALSE(mesh.isEdgeSeam(0));
    EXPECT_TRUE(mesh.isEdgeSeam(1));
    EXPECT_TRUE(mesh.isEdgeSeam(2));
    EXPECT_FALSE(mesh.isEdgeSeam(3));
}

TEST_F(SeamTest, IsEdgeSeam_OutOfBounds_ReturnsFalse) {
    EXPECT_FALSE(mesh.isEdgeSeam(999));
}

// =============================================================================
// UV Unwrap Tests
// =============================================================================

class UnwrapTest : public ::testing::Test {
protected:
    Mesh2 mesh;
    
    void SetUp() override {
        mesh.makeCube();
    }
};

TEST_F(UnwrapTest, UnwrapAllUVs_Box_ProducesValidUVs) {
    mesh.unwrapAllUVs(Mesh2::UVProjection::Box);
    
    const auto& faces = mesh.getFaces();
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            // Box projection tiles faces across multiple UV tiles
            EXPECT_FALSE(std::isnan(face.uvs[i].x));
            EXPECT_FALSE(std::isnan(face.uvs[i].y));
            EXPECT_FALSE(std::isinf(face.uvs[i].x));
            EXPECT_FALSE(std::isinf(face.uvs[i].y));
            EXPECT_GE(face.uvs[i].x, -0.01f);
            EXPECT_GE(face.uvs[i].y, -0.01f);
        }
    }
}

TEST_F(UnwrapTest, UnwrapAllUVs_Planar_ProducesValidUVs) {
    mesh.unwrapAllUVs(Mesh2::UVProjection::Planar);
    
    const auto& faces = mesh.getFaces();
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            // Planar projection may go slightly outside [0,1] but should be reasonable
            EXPECT_GE(face.uvs[i].x, -2.0f);
            EXPECT_LE(face.uvs[i].x, 2.0f);
            EXPECT_GE(face.uvs[i].y, -2.0f);
            EXPECT_LE(face.uvs[i].y, 2.0f);
        }
    }
}

TEST_F(UnwrapTest, UnwrapAllUVs_Cylindrical_ProducesValidUVs) {
    mesh.unwrapAllUVs(Mesh2::UVProjection::Cylindrical);
    
    const auto& faces = mesh.getFaces();
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_GE(face.uvs[i].x, -0.01f);
            EXPECT_LE(face.uvs[i].x, 1.01f);
            EXPECT_GE(face.uvs[i].y, -0.01f);
            EXPECT_LE(face.uvs[i].y, 1.01f);
        }
    }
}

TEST_F(UnwrapTest, UnwrapAllUVs_Spherical_ProducesValidUVs) {
    mesh.unwrapAllUVs(Mesh2::UVProjection::Spherical);
    
    const auto& faces = mesh.getFaces();
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_GE(face.uvs[i].x, -0.01f);
            EXPECT_LE(face.uvs[i].x, 1.01f);
            EXPECT_GE(face.uvs[i].y, -0.01f);
            EXPECT_LE(face.uvs[i].y, 1.01f);
        }
    }
}

TEST_F(UnwrapTest, UnwrapSelectedUVs_OnlyAffectsSelectedFaces) {
    // Select only face 0
    mesh.selectFace(0, false);
    
    // Store original UVs for face 1
    const auto& faces = mesh.getFaces();
    glm::vec2 originalUV1[4];
    for (int i = 0; i < faces[1].vertexCount; i++) {
        originalUV1[i] = faces[1].uvs[i];
    }
    
    // Unwrap selected
    mesh.unwrapSelectedUVs(Mesh2::UVProjection::Box);
    
    // Face 1 should be unchanged
    const auto& updatedFaces = mesh.getFaces();
    for (int i = 0; i < updatedFaces[1].vertexCount; i++) {
        EXPECT_TRUE(uvEqual(updatedFaces[1].uvs[i], originalUV1[i]))
            << "Face 1 UV corner " << i << " was modified but shouldn't have been";
    }
}

TEST_F(UnwrapTest, UnwrapSelectedUVs_NoSelection_DoesNothing) {
    // Store original UVs
    auto& faces = mesh.getFaces();
    std::vector<glm::vec2> originalUVs;
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            originalUVs.push_back(face.uvs[i]);
        }
    }
    
    // Unwrap with no selection
    mesh.unwrapSelectedUVs(Mesh2::UVProjection::Box);
    
    // All UVs should be unchanged
    size_t idx = 0;
    const auto& updatedFaces = mesh.getFaces();
    for (const auto& face : updatedFaces) {
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_TRUE(uvEqual(face.uvs[i], originalUVs[idx]))
                << "UV at index " << idx << " changed without selection";
            idx++;
        }
    }
}

// =============================================================================
// UV Preservation Through Operations
// =============================================================================

class UVPreservationTest : public ::testing::Test {
protected:
    Mesh2 mesh;
    
    void SetUp() override {
        mesh.makeCube();
        // Set custom UVs for face 0 so we can track them
        mesh.setFaceUVs(0, 
            glm::vec2(0.1f, 0.2f),
            glm::vec2(0.3f, 0.4f),
            glm::vec2(0.5f, 0.6f),
            glm::vec2(0.7f, 0.8f));
    }
};

TEST_F(UVPreservationTest, Subdivide_PreservesUVRange) {
    // Select all faces
    mesh.selectAll();
    
    mesh.splitSelected();
    
    // After subdivision, all UVs should still be in valid range
    const auto& faces = mesh.getFaces();
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_GE(face.uvs[i].x, -0.01f);
            EXPECT_LE(face.uvs[i].x, 1.01f);
            EXPECT_GE(face.uvs[i].y, -0.01f);
            EXPECT_LE(face.uvs[i].y, 1.01f);
        }
    }
}

TEST_F(UVPreservationTest, Triangulate_PreservesUVs) {
    // Get the custom UVs we set on face 0
    const auto& origFaces = mesh.getFaces();
    glm::vec2 origUV0 = origFaces[0].uvs[0];
    glm::vec2 origUV1 = origFaces[0].uvs[1];
    glm::vec2 origUV2 = origFaces[0].uvs[2];
    glm::vec2 origUV3 = origFaces[0].uvs[3];
    
    // Select face 0 and triangulate
    mesh.selectFace(0, false);
    mesh.triangulateSelected();
    
    // After triangulation, the two resulting triangles should use
    // corner UVs from the original quad
    const auto& faces = mesh.getFaces();
    bool foundUV0 = false, foundUV1 = false, foundUV2 = false, foundUV3 = false;
    
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            if (uvEqual(face.uvs[i], origUV0)) foundUV0 = true;
            if (uvEqual(face.uvs[i], origUV1)) foundUV1 = true;
            if (uvEqual(face.uvs[i], origUV2)) foundUV2 = true;
            if (uvEqual(face.uvs[i], origUV3)) foundUV3 = true;
        }
    }
    
    EXPECT_TRUE(foundUV0) << "Original UV0 not found after triangulation";
    EXPECT_TRUE(foundUV1) << "Original UV1 not found after triangulation";
    EXPECT_TRUE(foundUV2) << "Original UV2 not found after triangulation";
    EXPECT_TRUE(foundUV3) << "Original UV3 not found after triangulation";
}

// =============================================================================
// Seam + Undo Integration Test (via MeshState)
// =============================================================================

TEST(SeamUndoTest, SeamsPreservedInMeshState) {
    Mesh2 mesh;
    mesh.makeCube();
    
    // Mark some seams
    mesh.markEdgeAsSeam(0, true);
    mesh.markEdgeAsSeam(5, true);
    
    // Capture state
    std::vector<MeshVertex> savedVerts;
    std::vector<Face> savedFaces;
    std::vector<Edge> savedEdges;
    mesh.getState(savedVerts, savedFaces, savedEdges);
    
    // Clear seams
    mesh.clearAllSeams();
    EXPECT_FALSE(mesh.isEdgeSeam(0));
    EXPECT_FALSE(mesh.isEdgeSeam(5));
    
    // Restore state
    mesh.setState(savedVerts, savedFaces, savedEdges);
    EXPECT_TRUE(mesh.isEdgeSeam(0));
    EXPECT_TRUE(mesh.isEdgeSeam(5));
}

TEST(SeamUndoTest, UVsPreservedInMeshState) {
    Mesh2 mesh;
    mesh.makePlane();
    
    glm::vec2 customUV(0.42f, 0.73f);
    mesh.setFaceUVs(0, customUV, customUV, customUV, customUV);
    
    // Capture state
    std::vector<MeshVertex> savedVerts;
    std::vector<Face> savedFaces;
    std::vector<Edge> savedEdges;
    mesh.getState(savedVerts, savedFaces, savedEdges);
    
    // Overwrite UVs
    mesh.unwrapAllUVs(Mesh2::UVProjection::Box);
    
    // Restore state
    mesh.setState(savedVerts, savedFaces, savedEdges);
    
    const auto& faces = mesh.getFaces();
    for (int i = 0; i < faces[0].vertexCount; i++) {
        EXPECT_TRUE(uvEqual(faces[0].uvs[i], customUV))
            << "UV corner " << i << " not restored correctly";
    }
}

// =============================================================================
// ItemCollection UV Tests
// =============================================================================

class ItemCollectionUVTest : public ::testing::Test {
protected:
    ItemCollection items;
    
    void SetUp() override {
        items.addCube();
        items.selectAllItems();
    }
};

TEST_F(ItemCollectionUVTest, UnwrapAllUVs_ViaItemCollection) {
    Mesh2* mesh = items.getCurrentMesh();
    ASSERT_NE(mesh, nullptr);
    mesh->unwrapAllUVs(Mesh2::UVProjection::Box);
    ASSERT_NE(mesh, nullptr);
    
    const auto& faces = mesh->getFaces();
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            // Box projection tiles faces so UVs can exceed 1.0
            EXPECT_FALSE(std::isnan(face.uvs[i].x));
            EXPECT_FALSE(std::isinf(face.uvs[i].x));
            EXPECT_GE(face.uvs[i].x, -0.01f);
        }
    }
}

TEST_F(ItemCollectionUVTest, MarkSelectedEdgesAsSeam_ViaItemCollection) {
    items.setEditMode(EditMode::Edges);
    
    Mesh2* mesh = items.getCurrentMesh();
    ASSERT_NE(mesh, nullptr);
    
    // Select edges
    mesh->selectEdge(0, false);
    mesh->selectEdge(1, true);
    
    mesh->markSelectedEdgesAsSeam(true);
    
    EXPECT_TRUE(mesh->isEdgeSeam(0));
    EXPECT_TRUE(mesh->isEdgeSeam(1));
    EXPECT_FALSE(mesh->isEdgeSeam(2));
}

TEST_F(ItemCollectionUVTest, ClearAllSeams_ViaItemCollection) {
    Mesh2* mesh = items.getCurrentMesh();
    ASSERT_NE(mesh, nullptr);
    
    mesh->markEdgeAsSeam(0, true);
    mesh->markEdgeAsSeam(3, true);
    
    mesh->clearAllSeams();
    
    const auto& edges = mesh->getEdges();
    for (size_t i = 0; i < edges.size(); i++) {
        EXPECT_FALSE(mesh->isEdgeSeam(static_cast<uint32_t>(i)));
    }
}

// =============================================================================
// Seam-Based Unwrap Tests
// =============================================================================

class SeamUnwrapTest : public ::testing::Test {
protected:
    Mesh2 mesh;
    
    void SetUp() override {
        mesh.makeCube();
    }
    
    // Helper to check all UVs are finite and non-NaN
    void expectAllUVsValid() {
        const auto& faces = mesh.getFaces();
        for (size_t fi = 0; fi < faces.size(); fi++) {
            const Face& face = faces[fi];
            for (int i = 0; i < face.vertexCount; i++) {
                EXPECT_FALSE(std::isnan(face.uvs[i].x)) << "NaN UV.x at face " << fi << " corner " << i;
                EXPECT_FALSE(std::isnan(face.uvs[i].y)) << "NaN UV.y at face " << fi << " corner " << i;
                EXPECT_FALSE(std::isinf(face.uvs[i].x)) << "Inf UV.x at face " << fi << " corner " << i;
                EXPECT_FALSE(std::isinf(face.uvs[i].y)) << "Inf UV.y at face " << fi << " corner " << i;
            }
        }
    }
};

TEST_F(SeamUnwrapTest, NoSeams_ProducesSingleIsland) {
    // With no seams, the whole cube is one UV island
    mesh.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    
    expectAllUVsValid();
    
    // All UVs should be non-negative (packed from origin)
    const auto& faces = mesh.getFaces();
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_GE(face.uvs[i].x, -0.01f);
            EXPECT_GE(face.uvs[i].y, -0.01f);
        }
    }
}

TEST_F(SeamUnwrapTest, WithSeams_CreatesMultipleIslands) {
    // Mark a ring of edges as seams to split the cube
    // First, find edges and mark some as seams
    const auto& edges = mesh.getEdges();
    int seamCount = 0;
    for (size_t i = 0; i < edges.size(); i++) {
        // Mark every 3rd edge as a seam
        if (i % 3 == 0) {
            mesh.markEdgeAsSeam(static_cast<uint32_t>(i), true);
            seamCount++;
        }
    }
    ASSERT_GT(seamCount, 0);
    
    mesh.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    
    expectAllUVsValid();
}

TEST_F(SeamUnwrapTest, AllEdgesAsSeams_EachFaceIsAnIsland) {
    // Mark ALL edges as seams — each face becomes its own island
    const auto& edges = mesh.getEdges();
    for (size_t i = 0; i < edges.size(); i++) {
        mesh.markEdgeAsSeam(static_cast<uint32_t>(i), true);
    }
    
    mesh.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    
    expectAllUVsValid();
    
    // Each face should have UVs that span some area (not all zeros)
    const auto& faces = mesh.getFaces();
    for (size_t fi = 0; fi < faces.size(); fi++) {
        const Face& face = faces[fi];
        // At least some UV variation within the face
        float minU = FLT_MAX, maxU = -FLT_MAX;
        float minV = FLT_MAX, maxV = -FLT_MAX;
        for (int i = 0; i < face.vertexCount; i++) {
            minU = std::min(minU, face.uvs[i].x);
            maxU = std::max(maxU, face.uvs[i].x);
            minV = std::min(minV, face.uvs[i].y);
            maxV = std::max(maxV, face.uvs[i].y);
        }
        float area = (maxU - minU) * (maxV - minV);
        EXPECT_GT(area, 1e-6f) << "Face " << fi << " has zero UV area";
    }
}

TEST_F(SeamUnwrapTest, SelectedOnly_OnlyAffectsSelectedFaces) {
    // Set custom UVs on face 0
    glm::vec2 customUV(0.42f, 0.73f);
    mesh.setFaceUVs(0, customUV, customUV, customUV, customUV);
    
    // Select only face 1
    mesh.selectFace(1, false);
    
    // Unwrap selected only
    mesh.unwrapSelectedUVs(Mesh2::UVProjection::SeamBased);
    
    // Face 0 should still have its custom UVs
    const auto& faces = mesh.getFaces();
    for (int i = 0; i < faces[0].vertexCount; i++) {
        EXPECT_NEAR(faces[0].uvs[i].x, customUV.x, 0.001f)
            << "Face 0 corner " << i << " was modified by selected-only unwrap";
        EXPECT_NEAR(faces[0].uvs[i].y, customUV.y, 0.001f);
    }
    
    expectAllUVsValid();
}

TEST_F(SeamUnwrapTest, Plane_ProducesValidUVs) {
    Mesh2 plane;
    plane.makePlane();
    
    plane.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    
    const auto& faces = plane.getFaces();
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            EXPECT_FALSE(std::isnan(face.uvs[i].x));
            EXPECT_FALSE(std::isnan(face.uvs[i].y));
        }
    }
}

TEST_F(SeamUnwrapTest, Cylinder_WithSeams_ProducesValidUVs) {
    Mesh2 cyl;
    cyl.makeCylinder(8);
    
    // Mark some vertical edges as seams 
    const auto& edges = cyl.getEdges();
    for (size_t i = 0; i < edges.size(); i += 5) {
        cyl.markEdgeAsSeam(static_cast<uint32_t>(i), true);
    }
    
    cyl.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    
    const auto& faces = cyl.getFaces();
    for (size_t fi = 0; fi < faces.size(); fi++) {
        for (int i = 0; i < faces[fi].vertexCount; i++) {
            EXPECT_FALSE(std::isnan(faces[fi].uvs[i].x)) << "NaN at face " << fi;
            EXPECT_FALSE(std::isnan(faces[fi].uvs[i].y)) << "NaN at face " << fi;
        }
    }
}

TEST_F(SeamUnwrapTest, Sphere_WithSeams_ProducesValidUVs) {
    Mesh2 sphere;
    sphere.makeSphere(8);
    
    // Mark boundary-like edges as seams
    const auto& edges = sphere.getEdges();
    for (size_t i = 0; i < edges.size(); i += 4) {
        sphere.markEdgeAsSeam(static_cast<uint32_t>(i), true);
    }
    
    sphere.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    
    const auto& faces = sphere.getFaces();
    for (size_t fi = 0; fi < faces.size(); fi++) {
        for (int i = 0; i < faces[fi].vertexCount; i++) {
            EXPECT_FALSE(std::isnan(faces[fi].uvs[i].x)) << "NaN at face " << fi;
            EXPECT_FALSE(std::isnan(faces[fi].uvs[i].y)) << "NaN at face " << fi;
        }
    }
}

TEST_F(SeamUnwrapTest, UndoRestoresUVsAfterSeamUnwrap) {
    // Save original UVs
    std::vector<MeshVertex> savedVerts;
    std::vector<Face> savedFaces;
    std::vector<Edge> savedEdges;
    mesh.getState(savedVerts, savedFaces, savedEdges);
    
    // Mark seams and unwrap
    mesh.markEdgeAsSeam(0, true);
    mesh.markEdgeAsSeam(3, true);
    mesh.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    
    // Verify UVs changed
    const auto& facesAfter = mesh.getFaces();
    bool anyChanged = false;
    for (size_t fi = 0; fi < facesAfter.size(); fi++) {
        for (int i = 0; i < facesAfter[fi].vertexCount; i++) {
            if (glm::distance(facesAfter[fi].uvs[i], savedFaces[fi].uvs[i]) > 0.001f) {
                anyChanged = true;
                break;
            }
        }
        if (anyChanged) break;
    }
    EXPECT_TRUE(anyChanged) << "Seam unwrap should change at least some UVs";
    
    // Restore state (simulates undo)
    mesh.setState(savedVerts, savedFaces, savedEdges);
    
    // Verify UVs restored
    const auto& facesRestored = mesh.getFaces();
    for (size_t fi = 0; fi < facesRestored.size(); fi++) {
        for (int i = 0; i < facesRestored[fi].vertexCount; i++) {
            EXPECT_NEAR(facesRestored[fi].uvs[i].x, savedFaces[fi].uvs[i].x, 0.001f);
            EXPECT_NEAR(facesRestored[fi].uvs[i].y, savedFaces[fi].uvs[i].y, 0.001f);
        }
    }
}

TEST_F(SeamUnwrapTest, IslandsSeparatedBySeams_HaveDistinctUVs) {
    // Mark all edges as seams to make each face its own island
    const auto& edges = mesh.getEdges();
    for (size_t i = 0; i < edges.size(); i++) {
        mesh.markEdgeAsSeam(static_cast<uint32_t>(i), true);
    }
    
    mesh.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    
    // Compute the center UV of each face
    const auto& faces = mesh.getFaces();
    std::vector<glm::vec2> faceCenters;
    for (const auto& face : faces) {
        glm::vec2 center(0.0f);
        for (int i = 0; i < face.vertexCount; i++) {
            center += face.uvs[i];
        }
        center /= static_cast<float>(face.vertexCount);
        faceCenters.push_back(center);
    }
    
    // Each face-island should have a distinct center (no overlap)
    // Check that no two face centers are exactly the same
    bool allDistinct = true;
    for (size_t i = 0; i < faceCenters.size() && allDistinct; i++) {
        for (size_t j = i + 1; j < faceCenters.size(); j++) {
            if (glm::distance(faceCenters[i], faceCenters[j]) < 0.001f) {
                allDistinct = false;
                break;
            }
        }
    }
    EXPECT_TRUE(allDistinct) << "Each isolated island should have distinct UV placement";
}

TEST_F(SeamUnwrapTest, EmptyMesh_DoesNotCrash) {
    Mesh2 empty;
    // Should not crash on empty mesh
    empty.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    empty.unwrapSelectedUVs(Mesh2::UVProjection::SeamBased);
}

// =============================================================================
// Cross Unwrap Test - Verify a cube can be unwrapped into a cross pattern
// by marking the correct 7 edges as seams (leaving 5 fold edges unmarked).
//
// Cube faces created by makeCube:
//   Face 0: Front  (v0,v1,v2,v3)  z=-1
//   Face 1: Back   (v5,v4,v7,v6)  z=+1
//   Face 2: Left   (v4,v0,v3,v7)  x=-1
//   Face 3: Right  (v1,v5,v6,v2)  x=+1
//   Face 4: Top    (v3,v2,v6,v7)  y=+1
//   Face 5: Bottom (v4,v5,v1,v0)  y=-1
//
// Target cross layout (Front as center):
//           [Top]
//     [Left][Front][Right]
//           [Bottom]
//           [Back]
//
// Fold edges (NO seam, 5 edges):
//   Front↔Top, Front↔Bottom, Front↔Left, Front↔Right, Bottom↔Back
// Cut edges (seam, 7 edges):
//   Top↔Left, Top↔Right, Top↔Back, Bottom↔Left, Bottom↔Right,
//   Left↔Back, Right↔Back
// =============================================================================

TEST_F(SeamUnwrapTest, CrossUnwrap_CubeSevenSeams_ProducesSixIslandsOrConnectedCross) {
    // A cube from makeCube() has 12 edges.
    // We need to identify which edge connects which two faces, then mark 7 as seams.
    
    const auto& edges = mesh.getEdges();
    const auto& faces = mesh.getFaces();
    ASSERT_EQ(faces.size(), 6u);
    ASSERT_EQ(edges.size(), 12u);
    
    // Build a lookup: for each edge, which two faces does it border?
    // Then decide if it should be a seam based on our cross pattern.
    
    // Helper: determine which face index a face belongs to by checking its normal direction.
    // Face 0=Front(z-), 1=Back(z+), 2=Left(x-), 3=Right(x+), 4=Top(y+), 5=Bottom(y-)
    // We can use the face indices directly since makeCube creates them in order.
    
    // For the cross pattern with Front as center:
    //   Fold edges (keep connected): Front↔Top, Front↔Bottom, Front↔Left, Front↔Right, Bottom↔Back
    //   Cut edges (mark as seam): everything else
    
    auto isFoldEdge = [](uint32_t f0, uint32_t f1) -> bool {
        // Pairs that should stay connected (sorted)
        uint32_t lo = std::min(f0, f1);
        uint32_t hi = std::max(f0, f1);
        
        if (lo == 0 && hi == 4) return true;  // Front↔Top
        if (lo == 0 && hi == 5) return true;  // Front↔Bottom
        if (lo == 0 && hi == 2) return true;  // Front↔Left
        if (lo == 0 && hi == 3) return true;  // Front↔Right
        if (lo == 1 && hi == 5) return true;  // Bottom↔Back
        return false;
    };
    
    int seamCount = 0;
    int foldCount = 0;
    for (size_t ei = 0; ei < edges.size(); ei++) {
        const Edge& edge = edges[ei];
        uint32_t f0 = edge.faces[0];
        uint32_t f1 = edge.faces[1];
        
        // All cube edges should have exactly 2 faces
        ASSERT_NE(f0, UINT32_MAX) << "Edge " << ei << " missing face 0";
        ASSERT_NE(f1, UINT32_MAX) << "Edge " << ei << " missing face 1";
        
        if (isFoldEdge(f0, f1)) {
            foldCount++;
            // Don't mark as seam — this is a fold edge
        } else {
            mesh.markEdgeAsSeam(static_cast<uint32_t>(ei), true);
            seamCount++;
        }
    }
    
    EXPECT_EQ(foldCount, 5) << "Should have exactly 5 fold edges";
    EXPECT_EQ(seamCount, 7) << "Should have exactly 7 seam edges";
    
    // Verify seam count
    int actualSeams = 0;
    for (size_t ei = 0; ei < edges.size(); ei++) {
        if (edges[ei].isSeam) actualSeams++;
    }
    EXPECT_EQ(actualSeams, 7);
    
    // Run the seam-based unwrap
    mesh.unwrapAllUVs(Mesh2::UVProjection::SeamBased);
    
    // All UVs should be valid
    expectAllUVsValid();
    
    // The cross pattern with 5 fold edges should produce exactly 1 connected island
    // (all 6 faces connected via the 5 fold edges form a spanning tree of the dual graph).
    // After unwrap, the 6 faces should be laid out as a connected cross shape.
    
    // Compute UV center of each face
    std::vector<glm::vec2> faceCenters(6);
    for (int fi = 0; fi < 6; fi++) {
        glm::vec2 center(0.0f);
        for (int i = 0; i < faces[fi].vertexCount; i++) {
            center += faces[fi].uvs[i];
        }
        center /= static_cast<float>(faces[fi].vertexCount);
        faceCenters[fi] = center;
    }
    
    // The front face (0) is the center of the cross.
    // In a cross layout, the 4 adjacent faces should be roughly one face-width away
    // from the front face center, in 4 different directions.
    glm::vec2 frontCenter = faceCenters[0];
    
    // Check that Top, Bottom, Left, Right faces are each roughly equidistant from Front
    float distTop    = glm::length(faceCenters[4] - frontCenter);
    float distBottom = glm::length(faceCenters[5] - frontCenter);
    float distLeft   = glm::length(faceCenters[2] - frontCenter);
    float distRight  = glm::length(faceCenters[3] - frontCenter);
    
    // All 4 direct neighbors should be roughly the same distance from Front center
    float avgDist = (distTop + distBottom + distLeft + distRight) / 4.0f;
    EXPECT_GT(avgDist, 0.01f) << "Neighbor faces should not overlap with front face";
    
    for (float d : {distTop, distBottom, distLeft, distRight}) {
        EXPECT_NEAR(d, avgDist, avgDist * 0.5f) 
            << "All 4 direct neighbor faces should be roughly equidistant from front center";
    }
    
    // Back face should be further from front (it's connected via Bottom)
    float distBack = glm::length(faceCenters[1] - frontCenter);
    EXPECT_GT(distBack, avgDist * 0.9f) 
        << "Back face should be at least as far as direct neighbors";
    
    // Check that faces sharing a fold edge have adjacent (touching) UV regions.
    // Two faces that share a fold edge should have at least one pair of UV coordinates
    // that are very close (shared edge in UV space).
    auto facesShareUVEdge = [&](uint32_t f0, uint32_t f1) -> bool {
        // Check if any UV vertex of face f0 is very close to any UV vertex of face f1
        int closeCount = 0;
        for (int i = 0; i < faces[f0].vertexCount; i++) {
            for (int j = 0; j < faces[f1].vertexCount; j++) {
                float dist = glm::length(faces[f0].uvs[i] - faces[f1].uvs[j]);
                if (dist < 0.02f) {
                    closeCount++;
                }
            }
        }
        // A shared edge means at least 2 UV vertices should match
        return closeCount >= 2;
    };
    
    // Fold edges should produce shared UV edges (faces touching in UV space)
    EXPECT_TRUE(facesShareUVEdge(0, 4)) << "Front and Top should share a UV edge";
    EXPECT_TRUE(facesShareUVEdge(0, 5)) << "Front and Bottom should share a UV edge";
    EXPECT_TRUE(facesShareUVEdge(0, 2)) << "Front and Left should share a UV edge";
    EXPECT_TRUE(facesShareUVEdge(0, 3)) << "Front and Right should share a UV edge";
    EXPECT_TRUE(facesShareUVEdge(5, 1)) << "Bottom and Back should share a UV edge";
    
    // Each face should have non-degenerate UV area (not collapsed to a line or point)
    for (int fi = 0; fi < 6; fi++) {
        // For a quad, compute area via two triangles
        glm::vec2 a = faces[fi].uvs[0];
        glm::vec2 b = faces[fi].uvs[1];
        glm::vec2 c = faces[fi].uvs[2];
        glm::vec2 d = faces[fi].uvs[3];
        float area = 0.5f * std::abs((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y))
                   + 0.5f * std::abs((c.x - a.x) * (d.y - a.y) - (d.x - a.x) * (c.y - a.y));
        EXPECT_GT(area, 1e-6f) << "Face " << fi << " should have non-degenerate UV area";
    }
}
