#include "OpenSubdivHelper.h"
#include "Mesh2.h"

#include <opensubdiv/far/topologyDescriptor.h>
#include <opensubdiv/far/topologyRefinerFactory.h>
#include <opensubdiv/far/primvarRefiner.h>

#include <cassert>

using namespace OpenSubdiv;

// Wrapper struct for vertex position interpolation with OpenSubdiv
struct OsdVertex {
    float position[3];

    OsdVertex() { Clear(); }

    void Clear() {
        position[0] = position[1] = position[2] = 0.0f;
    }

    void AddWithWeight(OsdVertex const& src, float weight) {
        position[0] += weight * src.position[0];
        position[1] += weight * src.position[1];
        position[2] += weight * src.position[2];
    }
};

// Wrapper struct for face-varying UV interpolation with OpenSubdiv
struct OsdFVarUV {
    float uv[2];

    OsdFVarUV() { Clear(); }

    void Clear() {
        uv[0] = uv[1] = 0.0f;
    }

    void AddWithWeight(OsdFVarUV const& src, float weight) {
        uv[0] += weight * src.uv[0];
        uv[1] += weight * src.uv[1];
    }
};

SubdivisionResult subdivideWithOpenSubdiv(
    const std::vector<MeshVertex>& vertices,
    const std::vector<Face>& faces,
    int level)
{
    SubdivisionResult result;
    if (vertices.empty() || faces.empty() || level < 1) {
        result.vertices.assign(vertices.begin(), vertices.end());
        result.faces.assign(faces.begin(), faces.end());
        return result;
    }

    // Determine if all faces are triangles (use Loop) or not (use Catmull-Clark)
    bool allTriangles = true;
    for (const auto& face : faces) {
        if (face.isQuad()) {
            allTriangles = false;
            break;
        }
    }

    Sdc::SchemeType scheme = allTriangles
        ? Sdc::SCHEME_LOOP
        : Sdc::SCHEME_CATMARK;

    // Build the topology descriptor arrays
    int numVertices = static_cast<int>(vertices.size());
    int numFaces = static_cast<int>(faces.size());

    std::vector<int> vertsPerFace(numFaces);
    std::vector<int> vertIndices;
    std::vector<float> fvarUVs;

    vertIndices.reserve(numFaces * 4);
    fvarUVs.reserve(numFaces * 4 * 2);

    for (int fi = 0; fi < numFaces; fi++) {
        const Face& face = faces[fi];
        vertsPerFace[fi] = face.vertexCount;
        for (int vi = 0; vi < face.vertexCount; vi++) {
            vertIndices.push_back(static_cast<int>(face.vertices[vi]));
        }
        for (int vi = 0; vi < face.vertexCount; vi++) {
            fvarUVs.push_back(face.uvs[vi].x);
            fvarUVs.push_back(face.uvs[vi].y);
        }
    }

    int numFVarValues = static_cast<int>(fvarUVs.size() / 2);

    // Build face-varying index array (sequential - one UV per face-corner)
    std::vector<int> fvarIndices(vertIndices.size());
    for (int i = 0; i < static_cast<int>(fvarIndices.size()); i++) {
        fvarIndices[i] = i;
    }

    // Set up the face-varying channel
    Far::TopologyDescriptor::FVarChannel fvarChannel;
    fvarChannel.numValues = numFVarValues;
    fvarChannel.valueIndices = fvarIndices.data();

    // Create the topology descriptor
    Far::TopologyDescriptor desc;
    desc.numVertices = numVertices;
    desc.numFaces = numFaces;
    desc.numVertsPerFace = vertsPerFace.data();
    desc.vertIndicesPerFace = vertIndices.data();
    desc.numFVarChannels = 1;
    desc.fvarChannels = &fvarChannel;

    // Create the topology refiner
    Sdc::Options sdc_options;
    sdc_options.SetVtxBoundaryInterpolation(Sdc::Options::VTX_BOUNDARY_EDGE_ONLY);
    sdc_options.SetFVarLinearInterpolation(Sdc::Options::FVAR_LINEAR_CORNERS_ONLY);

    Far::TopologyRefiner* refiner = Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Create(
        desc,
        Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Options(scheme, sdc_options)
    );

    if (!refiner) {
        // Failed to create refiner, return original mesh
        result.vertices.assign(vertices.begin(), vertices.end());
        result.faces.assign(faces.begin(), faces.end());
        return result;
    }

    // Perform uniform refinement
    Far::TopologyRefiner::UniformOptions uniformOptions(level);
    uniformOptions.fullTopologyInLastLevel = true;
    refiner->RefineUniform(uniformOptions);

    // Allocate vertex buffers for all refinement levels
    int nTotalVerts = refiner->GetNumVerticesTotal();
    std::vector<OsdVertex> osdVerts(nTotalVerts);

    // Copy coarse vertex positions
    for (int i = 0; i < numVertices; i++) {
        osdVerts[i].position[0] = vertices[i].position.x;
        osdVerts[i].position[1] = vertices[i].position.y;
        osdVerts[i].position[2] = vertices[i].position.z;
    }

    // Allocate face-varying UV buffers for all refinement levels
    int nTotalFVarValues = refiner->GetNumFVarValuesTotal(0);
    std::vector<OsdFVarUV> osdUVs(nTotalFVarValues);

    // Copy coarse face-varying UV values
    for (int i = 0; i < numFVarValues; i++) {
        osdUVs[i].uv[0] = fvarUVs[i * 2 + 0];
        osdUVs[i].uv[1] = fvarUVs[i * 2 + 1];
    }

    // Interpolate vertex and face-varying data through each refinement level
    Far::PrimvarRefiner primvarRefiner(*refiner);

    OsdVertex* srcVert = osdVerts.data();
    OsdFVarUV* srcUV = osdUVs.data();

    for (int lvl = 1; lvl <= level; lvl++) {
        Far::TopologyLevel const& parentLevel = refiner->GetLevel(lvl - 1);
        Far::TopologyLevel const& childLevel = refiner->GetLevel(lvl);

        OsdVertex* dstVert = srcVert + parentLevel.GetNumVertices();
        OsdFVarUV* dstUV = srcUV + parentLevel.GetNumFVarValues(0);

        primvarRefiner.Interpolate(lvl, srcVert, dstVert);
        primvarRefiner.InterpolateFaceVarying(lvl, srcUV, dstUV, 0);

        srcVert = dstVert;
        srcUV = dstUV;
    }

    // Extract the finest level topology
    Far::TopologyLevel const& fineLevel = refiner->GetLevel(level);

    int nFineVerts = fineLevel.GetNumVertices();
    int nFineFaces = fineLevel.GetNumFaces();

    // Compute the offset to the finest level vertices and UVs
    int vertOffset = 0;
    int fvarOffset = 0;
    for (int lvl = 0; lvl < level; lvl++) {
        vertOffset += refiner->GetLevel(lvl).GetNumVertices();
        fvarOffset += refiner->GetLevel(lvl).GetNumFVarValues(0);
    }

    // Build result vertices
    result.vertices.resize(nFineVerts);
    for (int i = 0; i < nFineVerts; i++) {
        result.vertices[i].position = glm::vec3(
            osdVerts[vertOffset + i].position[0],
            osdVerts[vertOffset + i].position[1],
            osdVerts[vertOffset + i].position[2]
        );
        result.vertices[i].selected = false;
    }

    // Build result faces with face-varying UVs
    result.faces.resize(nFineFaces);
    for (int fi = 0; fi < nFineFaces; fi++) {
        auto faceVerts = fineLevel.GetFaceVertices(fi);
        auto faceFVarValues = fineLevel.GetFaceFVarValues(fi, 0);

        Face& outFace = result.faces[fi];
        outFace.vertexCount = static_cast<uint8_t>(faceVerts.size());
        outFace.selected = false;

        for (int vi = 0; vi < outFace.vertexCount; vi++) {
            outFace.vertices[vi] = static_cast<uint32_t>(faceVerts[vi]);

            int fvarIdx = faceFVarValues[vi];
            outFace.uvs[vi] = glm::vec2(
                osdUVs[fvarOffset + fvarIdx].uv[0],
                osdUVs[fvarOffset + fvarIdx].uv[1]
            );
        }
        // Zero out unused slots
        for (int vi = outFace.vertexCount; vi < 4; vi++) {
            outFace.vertices[vi] = UINT32_MAX;
            outFace.uvs[vi] = glm::vec2(0.0f);
        }
    }

    delete refiner;
    return result;
}
