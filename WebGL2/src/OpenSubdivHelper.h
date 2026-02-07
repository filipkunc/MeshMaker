#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <cstdint>

struct MeshVertex;
struct Face;
struct Edge;

// Result of a Catmull-Clark or Loop subdivision operation
struct SubdivisionResult {
    std::vector<MeshVertex> vertices;
    std::vector<Face> faces;
    // Edges and connectivity are rebuilt by Mesh2 after applying the result
};

// Perform Catmull-Clark subdivision on the given mesh data.
// Automatically selects Catmull-Clark for quad/mixed meshes or Loop for all-triangle meshes.
// Returns the subdivided mesh at the specified refinement level (default 1).
SubdivisionResult subdivideWithOpenSubdiv(
    const std::vector<MeshVertex>& vertices,
    const std::vector<Face>& faces,
    int level = 1
);
