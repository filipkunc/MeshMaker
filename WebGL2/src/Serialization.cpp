#include "Serialization.h"
#include "ItemCollection.h"
#include "Item.h"
#include "Mesh2.h"
#include "Texture.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <map>
#include <tuple>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Serialization {

// Helper to format float for JSON (avoids scientific notation and handles special values)
std::string jsonFloat(float f) {
    if (std::isnan(f)) return "0";
    if (std::isinf(f)) return f > 0 ? "3.4028235e38" : "-3.4028235e38";
    std::ostringstream ss;
    ss << std::setprecision(7) << f;
    return ss.str();
}

// ============================================================================
// OBJ Format Export
// ============================================================================

std::string exportToOBJ(const ItemCollection& items) {
    std::ostringstream ss;
    
    ss << "# Exported from MeshMaker WebGL2\n";
    ss << "# https://github.com/filipkunc/MeshMaker\n\n";
    
    if (items.getItemCount() == 0) {
        ss << "# No objects to export\n";
        return ss.str();
    }
    
    // OBJ indices are 1-based
    uint32_t vertexOffset = 1;
    uint32_t texCoordOffset = 1;
    
    for (size_t itemIndex = 0; itemIndex < items.getItemCount(); itemIndex++) {
        const Item* item = items.getItemAtIndex(itemIndex);
        if (!item || !item->mesh) continue;
        
        const Mesh2& mesh = *item->mesh;
        const auto& vertices = mesh.getVertices();
        const auto& faces = mesh.getFaces();
        
        // Get transform matrix
        glm::mat4 transform = item->getTransformMatrix();
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
        
        ss << "# Object " << itemIndex << "\n";
        ss << "o Item_" << itemIndex << "\n";
        
        // Export vertices (transformed to world space)
        ss << "# " << vertices.size() << " vertices\n";
        for (const auto& v : vertices) {
            glm::vec4 worldPos = transform * glm::vec4(v.position, 1.0f);
            ss << "v " << worldPos.x << " " << worldPos.y << " " << worldPos.z << "\n";
        }
        
        // Export texture coordinates (one per face corner)
        uint32_t texCoordCount = 0;
        for (const auto& face : faces) {
            for (int i = 0; i < face.vertexCount; i++) {
                ss << "vt " << face.uvs[i].x << " " << face.uvs[i].y << "\n";
                texCoordCount++;
            }
        }
        ss << "# " << texCoordCount << " texture coordinates\n";
        
        // Export normals (transformed)
        ss << "# " << vertices.size() << " normals\n";
        for (const auto& v : vertices) {
            glm::vec3 worldNormal = glm::normalize(normalMatrix * v.normal);
            ss << "vn " << worldNormal.x << " " << worldNormal.y << " " << worldNormal.z << "\n";
        }
        
        // Export faces with texture coordinates
        ss << "# " << faces.size() << " faces\n";
        uint32_t texCoordIdx = texCoordOffset;
        for (const auto& face : faces) {
            ss << "f";
            for (int i = 0; i < face.vertexCount; i++) {
                uint32_t vIdx = face.vertices[i] + vertexOffset;
                uint32_t vtIdx = texCoordIdx++;
                ss << " " << vIdx << "/" << vtIdx << "/" << vIdx;
            }
            ss << "\n";
        }
        
        ss << "\n";
        vertexOffset += static_cast<uint32_t>(vertices.size());
        texCoordOffset += texCoordCount;
    }
    
    return ss.str();
}

std::string exportMeshToOBJ(const Mesh2& mesh, const std::string& objectName) {
    std::ostringstream ss;
    
    ss << "# Exported from MeshMaker WebGL2\n";
    ss << "o " << objectName << "\n";
    
    const auto& vertices = mesh.getVertices();
    const auto& faces = mesh.getFaces();
    
    // Export vertices
    for (const auto& v : vertices) {
        ss << "v " << v.position.x << " " << v.position.y << " " << v.position.z << "\n";
    }
    
    // Export texture coordinates (one per face corner)
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexCount; i++) {
            ss << "vt " << face.uvs[i].x << " " << face.uvs[i].y << "\n";
        }
    }
    
    // Export normals
    for (const auto& v : vertices) {
        ss << "vn " << v.normal.x << " " << v.normal.y << " " << v.normal.z << "\n";
    }
    
    // Export faces with texture coordinates (1-based indices)
    uint32_t texCoordIdx = 1;
    for (const auto& face : faces) {
        ss << "f";
        for (int i = 0; i < face.vertexCount; i++) {
            uint32_t vIdx = face.vertices[i] + 1;
            uint32_t vtIdx = texCoordIdx++;
            ss << " " << vIdx << "/" << vtIdx << "/" << vIdx;
        }
        ss << "\n";
    }
    
    return ss.str();
}

// ============================================================================
// OBJ Format Import
// ============================================================================

bool importFromOBJ(ItemCollection& items, const std::string& objData) {
    std::istringstream ss(objData);
    std::string line;
    
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    
    // Temporary storage for current object
    struct FaceVertex {
        uint32_t positionIdx;
        uint32_t texCoordIdx;
        uint32_t normalIdx;
    };
    struct ObjObject {
        std::string name;
        std::vector<std::vector<FaceVertex>> faces;
    };
    std::vector<ObjObject> objects;
    ObjObject currentObject;
    currentObject.name = "Object";
    
    while (std::getline(ss, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream lineStream(line);
        std::string prefix;
        lineStream >> prefix;
        
        if (prefix == "o" || prefix == "g") {
            // New object/group - save current if it has faces
            if (!currentObject.faces.empty()) {
                objects.push_back(currentObject);
            }
            currentObject.faces.clear();
            lineStream >> currentObject.name;
            if (currentObject.name.empty()) {
                currentObject.name = "Object_" + std::to_string(objects.size());
            }
        }
        else if (prefix == "v") {
            // Vertex position
            glm::vec3 pos;
            lineStream >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vn") {
            // Vertex normal
            glm::vec3 n;
            lineStream >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (prefix == "vt") {
            // Texture coordinate
            glm::vec2 uv;
            lineStream >> uv.x >> uv.y;
            texCoords.push_back(uv);
        }
        else if (prefix == "f") {
            // Face definition
            std::vector<FaceVertex> faceVerts;
            std::string vertexDef;
            
            while (lineStream >> vertexDef) {
                // Parse v, v/vt, v/vt/vn, or v//vn format
                FaceVertex fv = {UINT32_MAX, UINT32_MAX, UINT32_MAX};
                
                // Count slashes to determine format
                size_t slash1 = vertexDef.find('/');
                if (slash1 == std::string::npos) {
                    // Just vertex index
                    fv.positionIdx = std::stoul(vertexDef) - 1;
                } else {
                    fv.positionIdx = std::stoul(vertexDef.substr(0, slash1)) - 1;
                    size_t slash2 = vertexDef.find('/', slash1 + 1);
                    if (slash2 == std::string::npos) {
                        // v/vt format
                        if (slash1 + 1 < vertexDef.size()) {
                            fv.texCoordIdx = std::stoul(vertexDef.substr(slash1 + 1)) - 1;
                        }
                    } else {
                        // v/vt/vn or v//vn format
                        if (slash2 > slash1 + 1) {
                            fv.texCoordIdx = std::stoul(vertexDef.substr(slash1 + 1, slash2 - slash1 - 1)) - 1;
                        }
                        if (slash2 + 1 < vertexDef.size()) {
                            fv.normalIdx = std::stoul(vertexDef.substr(slash2 + 1)) - 1;
                        }
                    }
                }
                
                faceVerts.push_back(fv);
            }
            
            if (faceVerts.size() >= 3) {
                currentObject.faces.push_back(faceVerts);
            }
        }
    }
    
    // Don't forget the last object
    if (!currentObject.faces.empty()) {
        objects.push_back(currentObject);
    }
    
    // If no objects were explicitly defined, create one from all faces
    if (objects.empty() && !positions.empty()) {
        return false;
    }
    
    // Create items from objects
    for (const auto& obj : objects) {
        auto mesh = std::make_unique<Mesh2>();
        
        // Find which vertices are used by this object
        std::unordered_map<uint32_t, uint32_t> globalToLocal;
        std::vector<uint32_t> localToGlobal;
        
        for (const auto& face : obj.faces) {
            for (const auto& fv : face) {
                if (globalToLocal.find(fv.positionIdx) == globalToLocal.end()) {
                    globalToLocal[fv.positionIdx] = static_cast<uint32_t>(localToGlobal.size());
                    localToGlobal.push_back(fv.positionIdx);
                }
            }
        }
        
        // Add vertices to mesh
        for (uint32_t globalIdx : localToGlobal) {
            if (globalIdx < positions.size()) {
                mesh->addVertex(positions[globalIdx]);
            }
        }
        
        // Add faces to mesh with UV coordinates
        for (const auto& face : obj.faces) {
            // Extract UV coordinates for this face
            glm::vec2 uvs[4] = {glm::vec2(0), glm::vec2(0), glm::vec2(0), glm::vec2(0)};
            for (size_t i = 0; i < face.size() && i < 4; i++) {
                if (face[i].texCoordIdx != UINT32_MAX && face[i].texCoordIdx < texCoords.size()) {
                    uvs[i] = texCoords[face[i].texCoordIdx];
                }
            }
            
            if (face.size() == 3) {
                uint32_t faceIdx = mesh->addTriangle(
                    globalToLocal[face[0].positionIdx],
                    globalToLocal[face[1].positionIdx],
                    globalToLocal[face[2].positionIdx]
                );
                mesh->setFaceUVs(faceIdx, uvs[0], uvs[1], uvs[2]);
            } else if (face.size() == 4) {
                uint32_t faceIdx = mesh->addQuad(
                    globalToLocal[face[0].positionIdx],
                    globalToLocal[face[1].positionIdx],
                    globalToLocal[face[2].positionIdx],
                    globalToLocal[face[3].positionIdx]
                );
                mesh->setFaceUVs(faceIdx, uvs[0], uvs[1], uvs[2], uvs[3]);
            } else if (face.size() > 4) {
                // Triangulate polygon using fan method
                for (size_t i = 2; i < face.size(); i++) {
                    glm::vec2 triUVs[3] = {
                        (face[0].texCoordIdx != UINT32_MAX && face[0].texCoordIdx < texCoords.size()) ? texCoords[face[0].texCoordIdx] : glm::vec2(0),
                        (face[i-1].texCoordIdx != UINT32_MAX && face[i-1].texCoordIdx < texCoords.size()) ? texCoords[face[i-1].texCoordIdx] : glm::vec2(0),
                        (face[i].texCoordIdx != UINT32_MAX && face[i].texCoordIdx < texCoords.size()) ? texCoords[face[i].texCoordIdx] : glm::vec2(0)
                    };
                    uint32_t faceIdx = mesh->addTriangle(
                        globalToLocal[face[0].positionIdx],
                        globalToLocal[face[i - 1].positionIdx],
                        globalToLocal[face[i].positionIdx]
                    );
                    mesh->setFaceUVs(faceIdx, triUVs[0], triUVs[1], triUVs[2]);
                }
            }
        }
        
        mesh->buildConnectivity();
        mesh->computeNormals();
        mesh->createGPUBuffers();
        
        auto item = std::make_unique<Item>(std::move(mesh));
        item->setPositionToGeometricCenter();
        item->selected = true;
        
        // Deselect other items
        for (size_t i = 0; i < items.getItemCount(); i++) {
            Item* existing = items.getItemAtIndex(i);
            if (existing) existing->selected = false;
        }
        
        items.addItem(std::move(item));
    }
    
    return !objects.empty();
}

// ============================================================================
// GLB Format Export
// ============================================================================

// Helper to write little-endian values
template<typename T>
void writeLittleEndian(std::vector<uint8_t>& buffer, T value) {
    for (size_t i = 0; i < sizeof(T); i++) {
        buffer.push_back(static_cast<uint8_t>(value >> (i * 8)));
    }
}

// Helper to pad buffer to 4-byte alignment
void padTo4Bytes(std::vector<uint8_t>& buffer) {
    while (buffer.size() % 4 != 0) {
        buffer.push_back(0);
    }
}

void padStringTo4Bytes(std::string& str) {
    while (str.size() % 4 != 0) {
        str.push_back(' ');
    }
}

std::vector<uint8_t> exportToGLB(const ItemCollection& items) {
    // Collect all meshes
    std::vector<const Mesh2*> meshes;
    std::vector<glm::mat4> transforms;
    
    for (size_t i = 0; i < items.getItemCount(); i++) {
        const Item* item = items.getItemAtIndex(i);
        if (item && item->mesh) {
            meshes.push_back(item->mesh.get());
            transforms.push_back(item->getTransformMatrix());
        }
    }
    
    if (meshes.empty()) {
        return {};
    }
    
    // Build binary buffer with all mesh data
    // GLB requires per-vertex attributes, but our mesh has per-face-corner UVs
    // So we need to create vertices per face corner (may duplicate positions/normals)
    std::vector<uint8_t> binBuffer;
    
    struct MeshAccessorInfo {
        size_t positionOffset;
        size_t positionCount;
        size_t normalOffset;
        size_t normalCount;
        size_t texCoordOffset;
        size_t texCoordCount;
        size_t indexOffset;
        size_t indexCount;
        glm::vec3 posMin;
        glm::vec3 posMax;
    };
    std::vector<MeshAccessorInfo> meshInfos;
    
    for (size_t meshIdx = 0; meshIdx < meshes.size(); meshIdx++) {
        const Mesh2& mesh = *meshes[meshIdx];
        const glm::mat4& transform = transforms[meshIdx];
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
        
        const auto& vertices = mesh.getVertices();
        const auto& faces = mesh.getFaces();
        
        MeshAccessorInfo info;
        
        // Build vertex data per face corner (to support per-corner UVs)
        // This creates a vertex for each corner of each face, duplicating positions/normals as needed
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<uint16_t> indices;
        
        info.posMin = glm::vec3(std::numeric_limits<float>::max());
        info.posMax = glm::vec3(std::numeric_limits<float>::lowest());
        
        uint16_t vertexIndex = 0;
        for (const auto& face : faces) {
            // For each face, create vertices for each corner
            std::vector<uint16_t> faceIndices;
            
            for (int i = 0; i < face.vertexCount; i++) {
                const auto& v = vertices[face.vertices[i]];
                glm::vec4 worldPos = transform * glm::vec4(v.position, 1.0f);
                glm::vec3 pos(worldPos);
                glm::vec3 worldNormal = glm::normalize(normalMatrix * v.normal);
                
                positions.push_back(pos);
                normals.push_back(worldNormal);
                // Flip V for glTF (V=0 at top, we use V=0 at bottom)
                texCoords.push_back(glm::vec2(face.uvs[i].x, 1.0f - face.uvs[i].y));
                
                info.posMin = glm::min(info.posMin, pos);
                info.posMax = glm::max(info.posMax, pos);
                
                faceIndices.push_back(vertexIndex++);
            }
            
            // Triangulate: for tri it's 0,1,2; for quad it's 0,1,2 and 0,2,3
            if (face.vertexCount == 3) {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[1]);
                indices.push_back(faceIndices[2]);
            } else if (face.vertexCount == 4) {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[1]);
                indices.push_back(faceIndices[2]);
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[2]);
                indices.push_back(faceIndices[3]);
            }
        }
        
        // Write positions
        info.positionOffset = binBuffer.size();
        info.positionCount = positions.size();
        
        for (const auto& pos : positions) {
            const float* fp = &pos.x;
            for (int i = 0; i < 3; i++) {
                uint32_t bits;
                std::memcpy(&bits, &fp[i], sizeof(float));
                writeLittleEndian(binBuffer, bits);
            }
        }
        padTo4Bytes(binBuffer);
        
        // Write normals
        info.normalOffset = binBuffer.size();
        info.normalCount = normals.size();
        
        for (const auto& n : normals) {
            const float* fp = &n.x;
            for (int i = 0; i < 3; i++) {
                uint32_t bits;
                std::memcpy(&bits, &fp[i], sizeof(float));
                writeLittleEndian(binBuffer, bits);
            }
        }
        padTo4Bytes(binBuffer);
        
        // Write texture coordinates
        info.texCoordOffset = binBuffer.size();
        info.texCoordCount = texCoords.size();
        
        for (const auto& uv : texCoords) {
            uint32_t bits;
            std::memcpy(&bits, &uv.x, sizeof(float));
            writeLittleEndian(binBuffer, bits);
            std::memcpy(&bits, &uv.y, sizeof(float));
            writeLittleEndian(binBuffer, bits);
        }
        padTo4Bytes(binBuffer);
        
        // Write indices
        info.indexOffset = binBuffer.size();
        info.indexCount = indices.size();
        
        for (uint16_t idx : indices) {
            writeLittleEndian(binBuffer, idx);
        }
        padTo4Bytes(binBuffer);
        
        meshInfos.push_back(info);
    }
    
    // Build JSON
    std::ostringstream json;
    json << std::setprecision(7);  // Sufficient precision for floats
    json << "{";
    json << "\"asset\":{\"version\":\"2.0\",\"generator\":\"MeshMaker WebGL2\"},";
    
    // Buffers
    json << "\"buffers\":[{\"byteLength\":" << binBuffer.size() << "}],";
    
    // Buffer views (4 per mesh: position, normal, texcoord, index)
    json << "\"bufferViews\":[";
    for (size_t i = 0; i < meshInfos.size(); i++) {
        const auto& info = meshInfos[i];
        
        if (i > 0) json << ",";
        
        // Position buffer view
        json << "{\"buffer\":0,\"byteOffset\":" << info.positionOffset 
             << ",\"byteLength\":" << (info.positionCount * 12) << ",\"target\":34962}";
        
        // Normal buffer view
        json << ",{\"buffer\":0,\"byteOffset\":" << info.normalOffset 
             << ",\"byteLength\":" << (info.normalCount * 12) << ",\"target\":34962}";
        
        // TexCoord buffer view
        json << ",{\"buffer\":0,\"byteOffset\":" << info.texCoordOffset 
             << ",\"byteLength\":" << (info.texCoordCount * 8) << ",\"target\":34962}";
        
        // Index buffer view
        json << ",{\"buffer\":0,\"byteOffset\":" << info.indexOffset 
             << ",\"byteLength\":" << (info.indexCount * 2) << ",\"target\":34963}";
    }
    json << "],";
    
    // Accessors (4 per mesh: position, normal, texcoord, index)
    json << "\"accessors\":[";
    for (size_t i = 0; i < meshInfos.size(); i++) {
        const auto& info = meshInfos[i];
        size_t baseBufferView = i * 4;
        
        if (i > 0) json << ",";
        
        // Position accessor (use jsonFloat to avoid scientific notation issues)
        json << "{\"bufferView\":" << baseBufferView 
             << ",\"componentType\":5126,\"count\":" << info.positionCount 
             << ",\"type\":\"VEC3\""
             << ",\"min\":[" << jsonFloat(info.posMin.x) << "," << jsonFloat(info.posMin.y) << "," << jsonFloat(info.posMin.z) << "]"
             << ",\"max\":[" << jsonFloat(info.posMax.x) << "," << jsonFloat(info.posMax.y) << "," << jsonFloat(info.posMax.z) << "]}";
        
        // Normal accessor
        json << ",{\"bufferView\":" << (baseBufferView + 1) 
             << ",\"componentType\":5126,\"count\":" << info.normalCount 
             << ",\"type\":\"VEC3\"}";
        
        // TexCoord accessor
        json << ",{\"bufferView\":" << (baseBufferView + 2) 
             << ",\"componentType\":5126,\"count\":" << info.texCoordCount 
             << ",\"type\":\"VEC2\"}";
        
        // Index accessor
        json << ",{\"bufferView\":" << (baseBufferView + 3) 
             << ",\"componentType\":5123,\"count\":" << info.indexCount 
             << ",\"type\":\"SCALAR\"}";
    }
    json << "],";
    
    // Meshes
    json << "\"meshes\":[";
    for (size_t i = 0; i < meshInfos.size(); i++) {
        size_t baseAccessor = i * 4;
        if (i > 0) json << ",";
        json << "{\"name\":\"Mesh_" << i << "\",\"primitives\":[{"
             << "\"attributes\":{\"POSITION\":" << baseAccessor 
             << ",\"NORMAL\":" << (baseAccessor + 1) 
             << ",\"TEXCOORD_0\":" << (baseAccessor + 2) << "},"
             << "\"indices\":" << (baseAccessor + 3)
             << "}]}";
    }
    json << "],";
    
    // Nodes
    json << "\"nodes\":[";
    for (size_t i = 0; i < meshInfos.size(); i++) {
        if (i > 0) json << ",";
        json << "{\"mesh\":" << i << ",\"name\":\"Node_" << i << "\"}";
    }
    json << "],";
    
    // Scene
    json << "\"scenes\":[{\"nodes\":[";
    for (size_t i = 0; i < meshInfos.size(); i++) {
        if (i > 0) json << ",";
        json << i;
    }
    json << "]}],\"scene\":0";
    
    json << "}";
    
    std::string jsonStr = json.str();
    padStringTo4Bytes(jsonStr);
    
    // Build GLB
    std::vector<uint8_t> glb;
    
    // Header
    glb.push_back('g');
    glb.push_back('l');
    glb.push_back('T');
    glb.push_back('F');
    writeLittleEndian(glb, static_cast<uint32_t>(2)); // Version
    
    uint32_t totalLength = 12 + 8 + static_cast<uint32_t>(jsonStr.size()) + 8 + static_cast<uint32_t>(binBuffer.size());
    writeLittleEndian(glb, totalLength);
    
    // JSON chunk
    writeLittleEndian(glb, static_cast<uint32_t>(jsonStr.size()));
    writeLittleEndian(glb, static_cast<uint32_t>(0x4E4F534A)); // "JSON"
    for (char c : jsonStr) {
        glb.push_back(static_cast<uint8_t>(c));
    }
    
    // Binary chunk
    writeLittleEndian(glb, static_cast<uint32_t>(binBuffer.size()));
    writeLittleEndian(glb, static_cast<uint32_t>(0x004E4942)); // "BIN\0"
    glb.insert(glb.end(), binBuffer.begin(), binBuffer.end());
    
    return glb;
}

std::vector<uint8_t> exportMeshToGLB(const Mesh2& mesh, const std::string& meshName) {
    // Create a temporary item collection with just this mesh
    // For simplicity, we'll implement a direct export here
    
    const auto& vertices = mesh.getVertices();
    const auto& faces = mesh.getFaces();
    
    if (vertices.empty() || faces.empty()) {
        return {};
    }
    
    // Build binary buffer
    std::vector<uint8_t> binBuffer;
    
    // Positions
    glm::vec3 posMin(std::numeric_limits<float>::max());
    glm::vec3 posMax(std::numeric_limits<float>::lowest());
    
    for (const auto& v : vertices) {
        posMin = glm::min(posMin, v.position);
        posMax = glm::max(posMax, v.position);
        
        const float* fp = &v.position.x;
        for (int i = 0; i < 3; i++) {
            uint32_t bits;
            std::memcpy(&bits, &fp[i], sizeof(float));
            writeLittleEndian(binBuffer, bits);
        }
    }
    padTo4Bytes(binBuffer);
    
    // Normals
    size_t normalOffset = binBuffer.size();
    for (const auto& v : vertices) {
        const float* fp = &v.normal.x;
        for (int i = 0; i < 3; i++) {
            uint32_t bits;
            std::memcpy(&bits, &fp[i], sizeof(float));
            writeLittleEndian(binBuffer, bits);
        }
    }
    padTo4Bytes(binBuffer);
    
    // Indices
    size_t indexOffset = binBuffer.size();
    size_t indexCount = 0;
    
    for (const auto& face : faces) {
        if (face.vertexCount == 3) {
            for (int i = 0; i < 3; i++) {
                writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[i]));
            }
            indexCount += 3;
        } else if (face.vertexCount == 4) {
            writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[0]));
            writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[1]));
            writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[2]));
            writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[0]));
            writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[2]));
            writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[3]));
            indexCount += 6;
        }
    }
    padTo4Bytes(binBuffer);
    
    // Build JSON
    std::ostringstream json;
    json << "{";
    json << "\"asset\":{\"version\":\"2.0\",\"generator\":\"MeshMaker WebGL2\"},";
    json << "\"buffers\":[{\"byteLength\":" << binBuffer.size() << "}],";
    json << "\"bufferViews\":[";
    json << "{\"buffer\":0,\"byteOffset\":0,\"byteLength\":" << (vertices.size() * 12) << ",\"target\":34962},";
    json << "{\"buffer\":0,\"byteOffset\":" << normalOffset << ",\"byteLength\":" << (vertices.size() * 12) << ",\"target\":34962},";
    json << "{\"buffer\":0,\"byteOffset\":" << indexOffset << ",\"byteLength\":" << (indexCount * 2) << ",\"target\":34963}";
    json << "],";
    json << "\"accessors\":[";
    json << "{\"bufferView\":0,\"componentType\":5126,\"count\":" << vertices.size() 
         << ",\"type\":\"VEC3\""
         << ",\"min\":[" << posMin.x << "," << posMin.y << "," << posMin.z << "]"
         << ",\"max\":[" << posMax.x << "," << posMax.y << "," << posMax.z << "]},";
    json << "{\"bufferView\":1,\"componentType\":5126,\"count\":" << vertices.size() << ",\"type\":\"VEC3\"},";
    json << "{\"bufferView\":2,\"componentType\":5123,\"count\":" << indexCount << ",\"type\":\"SCALAR\"}";
    json << "],";
    json << "\"meshes\":[{\"name\":\"" << meshName << "\",\"primitives\":[{";
    json << "\"attributes\":{\"POSITION\":0,\"NORMAL\":1},\"indices\":2}]}],";
    json << "\"nodes\":[{\"mesh\":0,\"name\":\"" << meshName << "\"}],";
    json << "\"scenes\":[{\"nodes\":[0]}],\"scene\":0";
    json << "}";
    
    std::string jsonStr = json.str();
    padStringTo4Bytes(jsonStr);
    
    // Build GLB
    std::vector<uint8_t> glb;
    glb.push_back('g');
    glb.push_back('l');
    glb.push_back('T');
    glb.push_back('F');
    writeLittleEndian(glb, static_cast<uint32_t>(2));
    
    uint32_t totalLength = 12 + 8 + static_cast<uint32_t>(jsonStr.size()) + 8 + static_cast<uint32_t>(binBuffer.size());
    writeLittleEndian(glb, totalLength);
    
    writeLittleEndian(glb, static_cast<uint32_t>(jsonStr.size()));
    writeLittleEndian(glb, static_cast<uint32_t>(0x4E4F534A));
    for (char c : jsonStr) {
        glb.push_back(static_cast<uint8_t>(c));
    }
    
    writeLittleEndian(glb, static_cast<uint32_t>(binBuffer.size()));
    writeLittleEndian(glb, static_cast<uint32_t>(0x004E4942));
    glb.insert(glb.end(), binBuffer.begin(), binBuffer.end());
    
    return glb;
}

// ============================================================================
// GLB Format Import
// ============================================================================

// Helper to read little-endian values
template<typename T>
T readLittleEndian(const uint8_t* data) {
    T value = 0;
    for (size_t i = 0; i < sizeof(T); i++) {
        value |= static_cast<T>(data[i]) << (i * 8);
    }
    return value;
}

float readFloat(const uint8_t* data) {
    uint32_t bits = readLittleEndian<uint32_t>(data);
    float value;
    std::memcpy(&value, &bits, sizeof(float));
    return value;
}

// Simple JSON value parser (minimal implementation)
struct JsonValue {
    enum Type { Null, Number, String, Array, Object };
    Type type = Null;
    double number = 0;
    std::string str;
    std::vector<JsonValue> array;
    std::unordered_map<std::string, JsonValue> object;
    
    int asInt() const { return static_cast<int>(number); }
    size_t asSize() const { return static_cast<size_t>(number); }
    double asDouble() const { return number; }
    float asFloat() const { return static_cast<float>(number); }
};

class JsonParser {
public:
    JsonParser(const std::string& json) : data(json), pos(0) {}
    
    JsonValue parse() {
        skipWhitespace();
        return parseValue();
    }
    
private:
    const std::string& data;
    size_t pos;
    
    void skipWhitespace() {
        while (pos < data.size() && std::isspace(data[pos])) pos++;
    }
    
    JsonValue parseValue() {
        skipWhitespace();
        if (pos >= data.size()) return JsonValue();
        
        char c = data[pos];
        if (c == '{') return parseObject();
        if (c == '[') return parseArray();
        if (c == '"') return parseString();
        if (c == '-' || std::isdigit(c)) return parseNumber();
        if (data.substr(pos, 4) == "null") { pos += 4; return JsonValue(); }
        if (data.substr(pos, 4) == "true") { pos += 4; JsonValue v; v.type = JsonValue::Number; v.number = 1; return v; }
        if (data.substr(pos, 5) == "false") { pos += 5; JsonValue v; v.type = JsonValue::Number; v.number = 0; return v; }
        return JsonValue();
    }
    
    JsonValue parseObject() {
        JsonValue v;
        v.type = JsonValue::Object;
        pos++; // skip '{'
        skipWhitespace();
        
        while (pos < data.size() && data[pos] != '}') {
            skipWhitespace();
            if (data[pos] != '"') break;
            
            JsonValue keyVal = parseString();
            std::string key = keyVal.str;
            
            skipWhitespace();
            if (pos < data.size() && data[pos] == ':') pos++;
            skipWhitespace();
            
            v.object[key] = parseValue();
            
            skipWhitespace();
            if (pos < data.size() && data[pos] == ',') pos++;
        }
        
        if (pos < data.size() && data[pos] == '}') pos++;
        return v;
    }
    
    JsonValue parseArray() {
        JsonValue v;
        v.type = JsonValue::Array;
        pos++; // skip '['
        skipWhitespace();
        
        while (pos < data.size() && data[pos] != ']') {
            v.array.push_back(parseValue());
            skipWhitespace();
            if (pos < data.size() && data[pos] == ',') pos++;
            skipWhitespace();
        }
        
        if (pos < data.size() && data[pos] == ']') pos++;
        return v;
    }
    
    JsonValue parseString() {
        JsonValue v;
        v.type = JsonValue::String;
        pos++; // skip opening '"'
        
        while (pos < data.size() && data[pos] != '"') {
            if (data[pos] == '\\' && pos + 1 < data.size()) {
                pos++;
                switch (data[pos]) {
                    case 'n': v.str += '\n'; break;
                    case 't': v.str += '\t'; break;
                    case 'r': v.str += '\r'; break;
                    case '\\': v.str += '\\'; break;
                    case '"': v.str += '"'; break;
                    default: v.str += data[pos]; break;
                }
            } else {
                v.str += data[pos];
            }
            pos++;
        }
        
        if (pos < data.size() && data[pos] == '"') pos++;
        return v;
    }
    
    JsonValue parseNumber() {
        JsonValue v;
        v.type = JsonValue::Number;
        
        size_t start = pos;
        if (data[pos] == '-') pos++;
        while (pos < data.size() && std::isdigit(data[pos])) pos++;
        if (pos < data.size() && data[pos] == '.') {
            pos++;
            while (pos < data.size() && std::isdigit(data[pos])) pos++;
        }
        if (pos < data.size() && (data[pos] == 'e' || data[pos] == 'E')) {
            pos++;
            if (pos < data.size() && (data[pos] == '+' || data[pos] == '-')) pos++;
            while (pos < data.size() && std::isdigit(data[pos])) pos++;
        }
        
        v.number = std::stod(data.substr(start, pos - start));
        return v;
    }
};

bool importFromGLB(ItemCollection& items, const std::vector<uint8_t>& glbData) {
    if (glbData.size() < 12) return false;
    
    // Check magic
    if (glbData[0] != 'g' || glbData[1] != 'l' || glbData[2] != 'T' || glbData[3] != 'F') {
        return false;
    }
    
    uint32_t version = readLittleEndian<uint32_t>(&glbData[4]);
    if (version != 2) return false;
    
    uint32_t length = readLittleEndian<uint32_t>(&glbData[8]);
    if (length > glbData.size()) return false;
    
    // Parse chunks
    std::string jsonStr;
    const uint8_t* binData = nullptr;
    size_t binLength = 0;
    
    size_t offset = 12;
    while (offset + 8 <= glbData.size()) {
        uint32_t chunkLength = readLittleEndian<uint32_t>(&glbData[offset]);
        uint32_t chunkType = readLittleEndian<uint32_t>(&glbData[offset + 4]);
        offset += 8;
        
        if (offset + chunkLength > glbData.size()) break;
        
        if (chunkType == 0x4E4F534A) { // "JSON"
            jsonStr = std::string(reinterpret_cast<const char*>(&glbData[offset]), chunkLength);
        } else if (chunkType == 0x004E4942) { // "BIN\0"
            binData = &glbData[offset];
            binLength = chunkLength;
        }
        
        offset += chunkLength;
    }
    
    if (jsonStr.empty() || !binData) return false;
    
    // Parse JSON
    JsonParser parser(jsonStr);
    JsonValue root = parser.parse();
    
    if (root.type != JsonValue::Object) return false;
    
    // Get required arrays
    auto& accessors = root.object["accessors"];
    auto& bufferViews = root.object["bufferViews"];
    auto& meshesJson = root.object["meshes"];
    
    if (accessors.type != JsonValue::Array || 
        bufferViews.type != JsonValue::Array || 
        meshesJson.type != JsonValue::Array) {
        return false;
    }
    
    // Get optional arrays
    bool hasNodes = root.object.count("nodes") && root.object["nodes"].type == JsonValue::Array;
    bool hasImages = root.object.count("images") && root.object["images"].type == JsonValue::Array;
    bool hasTextures = root.object.count("textures") && root.object["textures"].type == JsonValue::Array;
    bool hasMaterials = root.object.count("materials") && root.object["materials"].type == JsonValue::Array;
    
    // Load embedded textures
    std::vector<std::shared_ptr<Texture>> loadedTextures;
    if (hasImages && hasTextures) {
        auto& imagesJson = root.object["images"];
        auto& texturesJson = root.object["textures"];
        
        // Load images
        std::vector<std::shared_ptr<Texture>> loadedImages;
        for (const auto& imageJson : imagesJson.array) {
            auto tex = std::make_shared<Texture>();
            
            if (imageJson.type == JsonValue::Object && imageJson.object.count("bufferView")) {
                // Image is embedded in buffer
                int bvIdx = imageJson.object.at("bufferView").asInt();
                if (bvIdx >= 0 && static_cast<size_t>(bvIdx) < bufferViews.array.size()) {
                    auto& bv = bufferViews.array[bvIdx];
                    size_t byteOffset = bv.object.count("byteOffset") ? bv.object.at("byteOffset").asSize() : 0;
                    size_t byteLength = bv.object.at("byteLength").asSize();
                    
                    if (byteOffset + byteLength <= binLength) {
                        tex->loadFromFileData(binData + byteOffset, byteLength);
                    }
                }
            }
            loadedImages.push_back(tex);
        }
        
        // Map textures to images
        for (const auto& texJson : texturesJson.array) {
            if (texJson.type == JsonValue::Object && texJson.object.count("source")) {
                int sourceIdx = texJson.object.at("source").asInt();
                if (sourceIdx >= 0 && static_cast<size_t>(sourceIdx) < loadedImages.size()) {
                    loadedTextures.push_back(loadedImages[sourceIdx]);
                } else {
                    loadedTextures.push_back(nullptr);
                }
            } else {
                loadedTextures.push_back(nullptr);
            }
        }
    }
    
    // Map material index to texture
    std::vector<std::shared_ptr<Texture>> materialTextures;
    if (hasMaterials) {
        auto& materialsJson = root.object["materials"];
        for (const auto& matJson : materialsJson.array) {
            std::shared_ptr<Texture> tex = nullptr;
            
            if (matJson.type == JsonValue::Object && matJson.object.count("pbrMetallicRoughness")) {
                auto& pbr = matJson.object.at("pbrMetallicRoughness");
                if (pbr.type == JsonValue::Object && pbr.object.count("baseColorTexture")) {
                    auto& baseColorTex = pbr.object.at("baseColorTexture");
                    if (baseColorTex.type == JsonValue::Object && baseColorTex.object.count("index")) {
                        int texIdx = baseColorTex.object.at("index").asInt();
                        if (texIdx >= 0 && static_cast<size_t>(texIdx) < loadedTextures.size()) {
                            tex = loadedTextures[texIdx];
                        }
                    }
                }
            }
            materialTextures.push_back(tex);
        }
    }
    
    // Helper to get material for a primitive
    auto getMaterialTexture = [&](const JsonValue& primitive) -> std::shared_ptr<Texture> {
        if (primitive.type == JsonValue::Object && primitive.object.count("material")) {
            int matIdx = primitive.object.at("material").asInt();
            if (matIdx >= 0 && static_cast<size_t>(matIdx) < materialTextures.size()) {
                return materialTextures[matIdx];
            }
        }
        return nullptr;
    };
    
    // Helper to import a single primitive from a mesh
    auto importPrimitive = [&](const JsonValue& primitive) -> std::pair<std::unique_ptr<Mesh2>, std::shared_ptr<Texture>> {
        if (primitive.type != JsonValue::Object) return {nullptr, nullptr};
        
        auto attrIt = primitive.object.find("attributes");
        if (attrIt == primitive.object.end()) return {nullptr, nullptr};
        auto& attributes = attrIt->second;
        if (attributes.type != JsonValue::Object) return {nullptr, nullptr};
        
        // Get accessor indices
        int posAccessorIdx = -1;
        int texCoordAccessorIdx = -1;
        int indexAccessorIdx = -1;
        
        if (attributes.object.count("POSITION")) {
            posAccessorIdx = attributes.object.at("POSITION").asInt();
        }
        if (attributes.object.count("TEXCOORD_0")) {
            texCoordAccessorIdx = attributes.object.at("TEXCOORD_0").asInt();
        }
        if (primitive.object.count("indices")) {
            indexAccessorIdx = primitive.object.at("indices").asInt();
        }
        
        if (posAccessorIdx < 0) return {nullptr, nullptr};
        
        // Get position data
        auto& posAccessor = accessors.array[posAccessorIdx];
        if (!posAccessor.object.count("bufferView") || !posAccessor.object.count("count")) {
            return {nullptr, nullptr};
        }
        int posBvIdx = posAccessor.object.at("bufferView").asInt();
        size_t posCount = posAccessor.object.at("count").asSize();
        size_t posAccessorByteOffset = posAccessor.object.count("byteOffset") ? 
            posAccessor.object.at("byteOffset").asSize() : 0;
        
        if (posBvIdx < 0 || static_cast<size_t>(posBvIdx) >= bufferViews.array.size()) {
            return {nullptr, nullptr};
        }
        auto& posBv = bufferViews.array[posBvIdx];
        size_t posByteOffset = posBv.object.count("byteOffset") ? posBv.object.at("byteOffset").asSize() : 0;
        posByteOffset += posAccessorByteOffset;
        size_t posByteStride = posBv.object.count("byteStride") ? posBv.object.at("byteStride").asSize() : 12;
        if (posByteStride == 0) posByteStride = 12; // Default for VEC3 float
        
        // Read all positions
        std::vector<glm::vec3> positions;
        for (size_t i = 0; i < posCount; i++) {
            size_t off = posByteOffset + i * posByteStride;
            if (off + 12 > binLength) break;
            
            float x = readFloat(binData + off);
            float y = readFloat(binData + off + 4);
            float z = readFloat(binData + off + 8);
            positions.push_back(glm::vec3(x, y, z));
        }
        
        // Read texture coordinates if present
        std::vector<glm::vec2> texCoords;
        if (texCoordAccessorIdx >= 0 && static_cast<size_t>(texCoordAccessorIdx) < accessors.array.size()) {
            auto& texAccessor = accessors.array[texCoordAccessorIdx];
            if (texAccessor.object.count("bufferView") && texAccessor.object.count("count")) {
                int texBvIdx = texAccessor.object.at("bufferView").asInt();
                size_t texCount = texAccessor.object.at("count").asSize();
                size_t texAccessorByteOffset = texAccessor.object.count("byteOffset") ?
                    texAccessor.object.at("byteOffset").asSize() : 0;
                
                if (texBvIdx >= 0 && static_cast<size_t>(texBvIdx) < bufferViews.array.size()) {
                    auto& texBv = bufferViews.array[texBvIdx];
                    size_t texByteOffset = texBv.object.count("byteOffset") ? texBv.object.at("byteOffset").asSize() : 0;
                    texByteOffset += texAccessorByteOffset;
                    size_t texByteStride = texBv.object.count("byteStride") ? texBv.object.at("byteStride").asSize() : 8;
                    if (texByteStride == 0) texByteStride = 8; // Default for VEC2 float
                    
                    for (size_t i = 0; i < texCount; i++) {
                        size_t off = texByteOffset + i * texByteStride;
                        if (off + 8 > binLength) break;
                        
                        float u = readFloat(binData + off);
                        float v = readFloat(binData + off + 4);
                        // glTF uses V=0 at top, OpenGL uses V=0 at bottom
                        // Since we flip textures on load, we also need to flip V
                        texCoords.push_back(glm::vec2(u, 1.0f - v));
                    }
                }
            }
        }
        
        auto mesh = std::make_unique<Mesh2>();
        
        // Deduplicate vertices
        std::map<std::tuple<float, float, float>, int> positionToVertex;
        std::vector<int> originalToMesh(positions.size(), -1);
        
        for (size_t i = 0; i < positions.size(); i++) {
            const auto& pos = positions[i];
            auto key = std::make_tuple(pos.x, pos.y, pos.z);
            
            if (positionToVertex.count(key)) {
                originalToMesh[i] = positionToVertex[key];
            } else {
                int newIdx = mesh->addVertex(pos);
                positionToVertex[key] = newIdx;
                originalToMesh[i] = newIdx;
            }
        }
        
        // Get indices
        if (indexAccessorIdx >= 0 && static_cast<size_t>(indexAccessorIdx) < accessors.array.size()) {
            auto& idxAccessor = accessors.array[indexAccessorIdx];
            if (!idxAccessor.object.count("bufferView") || 
                !idxAccessor.object.count("count") ||
                !idxAccessor.object.count("componentType")) {
                return {nullptr, nullptr};
            }
            int idxBvIdx = idxAccessor.object.at("bufferView").asInt();
            size_t idxCount = idxAccessor.object.at("count").asSize();
            int componentType = idxAccessor.object.at("componentType").asInt();
            size_t idxAccessorByteOffset = idxAccessor.object.count("byteOffset") ?
                idxAccessor.object.at("byteOffset").asSize() : 0;
            
            if (idxBvIdx < 0 || static_cast<size_t>(idxBvIdx) >= bufferViews.array.size()) {
                return {nullptr, nullptr};
            }
            auto& idxBv = bufferViews.array[idxBvIdx];
            size_t idxByteOffset = idxBv.object.count("byteOffset") ? idxBv.object.at("byteOffset").asSize() : 0;
            idxByteOffset += idxAccessorByteOffset;
            
            std::vector<uint32_t> indices;
            
            for (size_t i = 0; i < idxCount; i++) {
                uint32_t idx = 0;
                if (componentType == 5123) { // UNSIGNED_SHORT
                    size_t off = idxByteOffset + i * 2;
                    if (off + 2 <= binLength) {
                        idx = readLittleEndian<uint16_t>(binData + off);
                    }
                } else if (componentType == 5125) { // UNSIGNED_INT
                    size_t off = idxByteOffset + i * 4;
                    if (off + 4 <= binLength) {
                        idx = readLittleEndian<uint32_t>(binData + off);
                    }
                } else if (componentType == 5121) { // UNSIGNED_BYTE
                    size_t off = idxByteOffset + i;
                    if (off + 1 <= binLength) {
                        idx = binData[off];
                    }
                }
                indices.push_back(idx);
            }
            
            // Add triangles with UVs
            for (size_t i = 0; i + 2 < indices.size(); i += 3) {
                uint32_t origIdx0 = indices[i];
                uint32_t origIdx1 = indices[i + 1];
                uint32_t origIdx2 = indices[i + 2];
                
                // Bounds check
                if (origIdx0 >= originalToMesh.size() || 
                    origIdx1 >= originalToMesh.size() || 
                    origIdx2 >= originalToMesh.size()) {
                    continue;
                }
                
                int v0 = originalToMesh[origIdx0];
                int v1 = originalToMesh[origIdx1];
                int v2 = originalToMesh[origIdx2];
                
                if (v0 < 0 || v1 < 0 || v2 < 0) continue;
                
                int faceIdx = mesh->addTriangle(v0, v1, v2);
                
                if (!texCoords.empty() && faceIdx >= 0) {
                    glm::vec2 uv0 = (origIdx0 < texCoords.size()) ? texCoords[origIdx0] : glm::vec2(0);
                    glm::vec2 uv1 = (origIdx1 < texCoords.size()) ? texCoords[origIdx1] : glm::vec2(0);
                    glm::vec2 uv2 = (origIdx2 < texCoords.size()) ? texCoords[origIdx2] : glm::vec2(0);
                    mesh->setFaceUVs(faceIdx, uv0, uv1, uv2);
                }
            }
        }
        
        mesh->buildConnectivity();
        mesh->computeNormals();
        mesh->createGPUBuffers();
        
        return {std::move(mesh), getMaterialTexture(primitive)};
    };
    
    // Helper to import all primitives from a mesh
    auto importMesh = [&](size_t meshIdx) -> std::vector<std::pair<std::unique_ptr<Mesh2>, std::shared_ptr<Texture>>> {
        std::vector<std::pair<std::unique_ptr<Mesh2>, std::shared_ptr<Texture>>> result;
        
        if (meshIdx >= meshesJson.array.size()) return result;
        
        const auto& meshJson = meshesJson.array[meshIdx];
        if (meshJson.type != JsonValue::Object) return result;
        
        auto primIt = meshJson.object.find("primitives");
        if (primIt == meshJson.object.end()) return result;
        auto& primitives = primIt->second;
        if (primitives.type != JsonValue::Array || primitives.array.empty()) return result;
        
        // Import ALL primitives from this mesh
        for (const auto& primitive : primitives.array) {
            auto [mesh, texture] = importPrimitive(primitive);
            if (mesh) {
                result.push_back({std::move(mesh), texture});
            }
        }
        
        return result;
    };
    
    // Helper to get local transform matrix for a node
    auto getNodeLocalTransform = [](const JsonValue& nodeJson) -> glm::mat4 {
        // Check for matrix property first (takes precedence)
        if (nodeJson.object.count("matrix")) {
            auto& m = nodeJson.object.at("matrix");
            if (m.type == JsonValue::Array && m.array.size() >= 16) {
                // glTF matrices are column-major
                glm::mat4 mat;
                for (int i = 0; i < 16; i++) {
                    mat[i / 4][i % 4] = m.array[i].asFloat();
                }
                return mat;
            }
        }
        
        // Otherwise use T/R/S
        glm::vec3 translation(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale(1.0f);
        
        if (nodeJson.object.count("translation")) {
            auto& t = nodeJson.object.at("translation");
            if (t.type == JsonValue::Array && t.array.size() >= 3) {
                translation.x = t.array[0].asFloat();
                translation.y = t.array[1].asFloat();
                translation.z = t.array[2].asFloat();
            }
        }
        
        if (nodeJson.object.count("rotation")) {
            auto& r = nodeJson.object.at("rotation");
            if (r.type == JsonValue::Array && r.array.size() >= 4) {
                rotation.x = r.array[0].asFloat();
                rotation.y = r.array[1].asFloat();
                rotation.z = r.array[2].asFloat();
                rotation.w = r.array[3].asFloat();
            }
        }
        
        if (nodeJson.object.count("scale")) {
            auto& s = nodeJson.object.at("scale");
            if (s.type == JsonValue::Array && s.array.size() >= 3) {
                scale.x = s.array[0].asFloat();
                scale.y = s.array[1].asFloat();
                scale.z = s.array[2].asFloat();
            }
        }
        
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, translation);
        mat = mat * glm::mat4_cast(rotation);
        mat = glm::scale(mat, scale);
        return mat;
    };
    
    // Build world transforms for all nodes by traversing the hierarchy
    std::vector<glm::mat4> nodeWorldTransforms;
    
    // Import via nodes if available (for transforms), otherwise directly from meshes
    bool imported = false;
    
    if (hasNodes) {
        auto& nodesJson = root.object["nodes"];
        size_t nodeCount = nodesJson.array.size();
        
        // Initialize all transforms to identity
        nodeWorldTransforms.resize(nodeCount, glm::mat4(1.0f));
        
        // First pass: compute local transforms
        std::vector<glm::mat4> localTransforms(nodeCount);
        for (size_t i = 0; i < nodeCount; i++) {
            localTransforms[i] = getNodeLocalTransform(nodesJson.array[i]);
        }
        
        // Build parent index map
        std::vector<int> parentIndex(nodeCount, -1);
        for (size_t i = 0; i < nodeCount; i++) {
            const auto& nodeJson = nodesJson.array[i];
            if (nodeJson.type == JsonValue::Object && nodeJson.object.count("children")) {
                auto& children = nodeJson.object.at("children");
                if (children.type == JsonValue::Array) {
                    for (const auto& child : children.array) {
                        int childIdx = child.asInt();
                        if (childIdx >= 0 && static_cast<size_t>(childIdx) < nodeCount) {
                            parentIndex[childIdx] = static_cast<int>(i);
                        }
                    }
                }
            }
        }
        
        // Compute world transforms (multiply from root to node)
        for (size_t i = 0; i < nodeCount; i++) {
            int parent = parentIndex[i];
            
            // Walk up the hierarchy to collect ancestors
            std::vector<int> ancestors;
            while (parent >= 0) {
                ancestors.push_back(parent);
                parent = parentIndex[parent];
            }
            
            // Apply transforms from root down (ancestors are in child-to-root order)
            // So we need to start from the end (root) and multiply: root * ... * parent * local
            glm::mat4 worldMat(1.0f);
            for (auto it = ancestors.rbegin(); it != ancestors.rend(); ++it) {
                worldMat = worldMat * localTransforms[*it];
            }
            // Finally multiply by the node's own local transform
            worldMat = worldMat * localTransforms[i];
            
            nodeWorldTransforms[i] = worldMat;
        }
        
        // Track starting index for newly imported items
        size_t importStartIndex = items.getItemCount();
        
        // Now import meshes with their world transforms
        for (size_t nodeIdx = 0; nodeIdx < nodeCount; nodeIdx++) {
            const auto& nodeJson = nodesJson.array[nodeIdx];
            if (nodeJson.type != JsonValue::Object) continue;
            if (!nodeJson.object.count("mesh")) continue;
            
            int meshIdx = nodeJson.object.at("mesh").asInt();
            auto meshPrimitives = importMesh(meshIdx);
            
            // Create an item for each primitive in the mesh
            for (auto& [mesh, texture] : meshPrimitives) {
                if (!mesh) continue;
                
                auto item = std::make_unique<Item>(std::move(mesh));
                
                // Apply world transform to mesh vertices
                item->mesh->transformAllVertices(nodeWorldTransforms[nodeIdx]);
                item->mesh->createGPUBuffers();
                
                // Don't center individual items - keep them in world space
                // This preserves the spatial relationships between parts
                // The item position stays at origin, vertices are in world space
                item->position = glm::vec3(0.0f);
                
                // Set texture if available
                if (texture && texture->isValid()) {
                    item->setTexture(texture);
                }
                
                item->selected = true;
                
                items.addItem(std::move(item));
                imported = true;
            }
        }
        
        // Deselect pre-existing items, keep newly imported selected
        for (size_t i = 0; i < importStartIndex; i++) {
            Item* existing = items.getItemAtIndex(i);
            if (existing) existing->selected = false;
        }
    } else {
        // Fallback: import meshes directly without transforms
        size_t importStartIndex = items.getItemCount();
        
        for (size_t i = 0; i < meshesJson.array.size(); i++) {
            auto meshPrimitives = importMesh(i);
            
            for (auto& [mesh, texture] : meshPrimitives) {
                if (!mesh) continue;
                
                auto item = std::make_unique<Item>(std::move(mesh));
                // Don't center here - let auto-scale handle it
                item->position = glm::vec3(0.0f);
                
                if (texture && texture->isValid()) {
                    item->setTexture(texture);
                }
                
                item->selected = true;
                items.addItem(std::move(item));
                imported = true;
            }
        }
        
        // Deselect pre-existing items
        for (size_t i = 0; i < importStartIndex; i++) {
            Item* existing = items.getItemAtIndex(i);
            if (existing) existing->selected = false;
        }
    }
    
    // Auto-scale and center imported items
    if (imported) {
        // Calculate combined bounding box of all selected (newly imported) items
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        
        for (size_t i = 0; i < items.getItemCount(); ++i) {
            Item* item = items.getItemAtIndex(i);
            if (!item || !item->selected || !item->mesh) continue;
            
            for (const auto& v : item->mesh->getVertices()) {
                glm::vec3 worldPos = item->position + item->rotation * (item->scale * v.position);
                minX = std::min(minX, worldPos.x);
                maxX = std::max(maxX, worldPos.x);
                minY = std::min(minY, worldPos.y);
                maxY = std::max(maxY, worldPos.y);
                minZ = std::min(minZ, worldPos.z);
                maxZ = std::max(maxZ, worldPos.z);
            }
        }
        
        // Calculate the size of the bounding box
        float sizeX = maxX - minX;
        float sizeY = maxY - minY;
        float sizeZ = maxZ - minZ;
        float maxSize = std::max({sizeX, sizeY, sizeZ});
        
        // Target size for imported models (units) - 10 units fits well with default grid
        const float targetSize = 10.0f;
        const float minSizeThreshold = 0.001f;
        
        // Determine scale factor - always scale to target size
        float scaleFactor = 1.0f;
        if (maxSize > minSizeThreshold) {
            scaleFactor = targetSize / maxSize;
        }

        // Apply scaling to all imported items
        for (size_t i = 0; i < items.getItemCount(); ++i) {
            Item* item = items.getItemAtIndex(i);
            if (!item || !item->mesh) continue;
            
            item->scale *= scaleFactor;
            item->mesh->createGPUBuffers();
        }
    }
    
    return imported;
}

// ============================================================================
// Utilities
// ============================================================================

std::string getFileExtension(const std::string& filename) {
    size_t dotPos = filename.rfind('.');
    if (dotPos == std::string::npos) return "";
    
    std::string ext = filename.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

bool isValidGLB(const std::vector<uint8_t>& data) {
    if (data.size() < 12) return false;
    return data[0] == 'g' && data[1] == 'l' && data[2] == 'T' && data[3] == 'F';
}

} // namespace Serialization
