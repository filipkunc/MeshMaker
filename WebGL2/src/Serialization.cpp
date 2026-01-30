#include "Serialization.h"
#include "ItemCollection.h"
#include "Item.h"
#include "Mesh2.h"

#include <sstream>
#include <algorithm>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

namespace Serialization {

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
        
        // Export normals (transformed)
        ss << "# " << vertices.size() << " normals\n";
        for (const auto& v : vertices) {
            glm::vec3 worldNormal = glm::normalize(normalMatrix * v.normal);
            ss << "vn " << worldNormal.x << " " << worldNormal.y << " " << worldNormal.z << "\n";
        }
        
        // Export faces
        ss << "# " << faces.size() << " faces\n";
        for (const auto& face : faces) {
            ss << "f";
            for (int i = 0; i < face.vertexCount; i++) {
                uint32_t idx = face.vertices[i] + vertexOffset;
                ss << " " << idx << "//" << idx;
            }
            ss << "\n";
        }
        
        ss << "\n";
        vertexOffset += static_cast<uint32_t>(vertices.size());
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
    
    // Export normals
    for (const auto& v : vertices) {
        ss << "vn " << v.normal.x << " " << v.normal.y << " " << v.normal.z << "\n";
    }
    
    // Export faces (1-based indices)
    for (const auto& face : faces) {
        ss << "f";
        for (int i = 0; i < face.vertexCount; i++) {
            uint32_t idx = face.vertices[i] + 1;
            ss << " " << idx << "//" << idx;
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
    
    // Temporary storage for current object
    struct ObjObject {
        std::string name;
        std::vector<std::vector<uint32_t>> faceVertexIndices;
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
            if (!currentObject.faceVertexIndices.empty()) {
                objects.push_back(currentObject);
            }
            currentObject.faceVertexIndices.clear();
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
            // Texture coordinate (skip for now)
        }
        else if (prefix == "f") {
            // Face definition
            std::vector<uint32_t> faceVerts;
            std::string vertexDef;
            
            while (lineStream >> vertexDef) {
                // Parse v, v/vt, v/vt/vn, or v//vn format
                uint32_t vi = 0, ti = 0, ni = 0;
                
                // Count slashes to determine format
                size_t slash1 = vertexDef.find('/');
                if (slash1 == std::string::npos) {
                    // Just vertex index
                    vi = std::stoul(vertexDef);
                } else {
                    vi = std::stoul(vertexDef.substr(0, slash1));
                    size_t slash2 = vertexDef.find('/', slash1 + 1);
                    if (slash2 == std::string::npos) {
                        // v/vt format
                        if (slash1 + 1 < vertexDef.size()) {
                            ti = std::stoul(vertexDef.substr(slash1 + 1));
                        }
                    } else {
                        // v/vt/vn or v//vn format
                        if (slash2 > slash1 + 1) {
                            ti = std::stoul(vertexDef.substr(slash1 + 1, slash2 - slash1 - 1));
                        }
                        if (slash2 + 1 < vertexDef.size()) {
                            ni = std::stoul(vertexDef.substr(slash2 + 1));
                        }
                    }
                }
                
                // Convert to 0-based index
                if (vi > 0) vi--;
                faceVerts.push_back(vi);
            }
            
            if (faceVerts.size() >= 3) {
                currentObject.faceVertexIndices.push_back(faceVerts);
            }
        }
    }
    
    // Don't forget the last object
    if (!currentObject.faceVertexIndices.empty()) {
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
        
        for (const auto& faceVerts : obj.faceVertexIndices) {
            for (uint32_t globalIdx : faceVerts) {
                if (globalToLocal.find(globalIdx) == globalToLocal.end()) {
                    globalToLocal[globalIdx] = static_cast<uint32_t>(localToGlobal.size());
                    localToGlobal.push_back(globalIdx);
                }
            }
        }
        
        // Add vertices to mesh
        for (uint32_t globalIdx : localToGlobal) {
            if (globalIdx < positions.size()) {
                mesh->addVertex(positions[globalIdx]);
            }
        }
        
        // Add faces to mesh
        for (const auto& faceVerts : obj.faceVertexIndices) {
            if (faceVerts.size() == 3) {
                mesh->addTriangle(
                    globalToLocal[faceVerts[0]],
                    globalToLocal[faceVerts[1]],
                    globalToLocal[faceVerts[2]]
                );
            } else if (faceVerts.size() == 4) {
                mesh->addQuad(
                    globalToLocal[faceVerts[0]],
                    globalToLocal[faceVerts[1]],
                    globalToLocal[faceVerts[2]],
                    globalToLocal[faceVerts[3]]
                );
            } else if (faceVerts.size() > 4) {
                // Triangulate polygon using fan method
                for (size_t i = 2; i < faceVerts.size(); i++) {
                    mesh->addTriangle(
                        globalToLocal[faceVerts[0]],
                        globalToLocal[faceVerts[i - 1]],
                        globalToLocal[faceVerts[i]]
                    );
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
    std::vector<uint8_t> binBuffer;
    
    struct MeshAccessorInfo {
        size_t positionOffset;
        size_t positionCount;
        size_t normalOffset;
        size_t normalCount;
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
        
        // Positions
        info.positionOffset = binBuffer.size();
        info.positionCount = vertices.size();
        info.posMin = glm::vec3(std::numeric_limits<float>::max());
        info.posMax = glm::vec3(std::numeric_limits<float>::lowest());
        
        for (const auto& v : vertices) {
            glm::vec4 worldPos = transform * glm::vec4(v.position, 1.0f);
            glm::vec3 pos(worldPos);
            
            info.posMin = glm::min(info.posMin, pos);
            info.posMax = glm::max(info.posMax, pos);
            
            // Write as little-endian floats
            const float* fp = &pos.x;
            for (int i = 0; i < 3; i++) {
                uint32_t bits;
                std::memcpy(&bits, &fp[i], sizeof(float));
                writeLittleEndian(binBuffer, bits);
            }
        }
        padTo4Bytes(binBuffer);
        
        // Normals
        info.normalOffset = binBuffer.size();
        info.normalCount = vertices.size();
        
        for (const auto& v : vertices) {
            glm::vec3 worldNormal = glm::normalize(normalMatrix * v.normal);
            
            const float* fp = &worldNormal.x;
            for (int i = 0; i < 3; i++) {
                uint32_t bits;
                std::memcpy(&bits, &fp[i], sizeof(float));
                writeLittleEndian(binBuffer, bits);
            }
        }
        padTo4Bytes(binBuffer);
        
        // Indices - triangulate quads
        info.indexOffset = binBuffer.size();
        info.indexCount = 0;
        
        for (const auto& face : faces) {
            if (face.vertexCount == 3) {
                for (int i = 0; i < 3; i++) {
                    writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[i]));
                }
                info.indexCount += 3;
            } else if (face.vertexCount == 4) {
                // Triangulate quad: 0-1-2, 0-2-3
                writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[0]));
                writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[1]));
                writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[2]));
                writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[0]));
                writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[2]));
                writeLittleEndian(binBuffer, static_cast<uint16_t>(face.vertices[3]));
                info.indexCount += 6;
            }
        }
        padTo4Bytes(binBuffer);
        
        meshInfos.push_back(info);
    }
    
    // Build JSON
    std::ostringstream json;
    json << "{";
    json << "\"asset\":{\"version\":\"2.0\",\"generator\":\"MeshMaker WebGL2\"},";
    
    // Buffers
    json << "\"buffers\":[{\"byteLength\":" << binBuffer.size() << "}],";
    
    // Buffer views
    json << "\"bufferViews\":[";
    size_t bufferViewIdx = 0;
    for (size_t i = 0; i < meshInfos.size(); i++) {
        const auto& info = meshInfos[i];
        
        if (i > 0) json << ",";
        
        // Position buffer view
        json << "{\"buffer\":0,\"byteOffset\":" << info.positionOffset 
             << ",\"byteLength\":" << (info.positionCount * 12) << ",\"target\":34962}";
        
        // Normal buffer view
        json << ",{\"buffer\":0,\"byteOffset\":" << info.normalOffset 
             << ",\"byteLength\":" << (info.normalCount * 12) << ",\"target\":34962}";
        
        // Index buffer view
        json << ",{\"buffer\":0,\"byteOffset\":" << info.indexOffset 
             << ",\"byteLength\":" << (info.indexCount * 2) << ",\"target\":34963}";
    }
    json << "],";
    
    // Accessors
    json << "\"accessors\":[";
    size_t accessorIdx = 0;
    for (size_t i = 0; i < meshInfos.size(); i++) {
        const auto& info = meshInfos[i];
        size_t baseBufferView = i * 3;
        
        if (i > 0) json << ",";
        
        // Position accessor
        json << "{\"bufferView\":" << baseBufferView 
             << ",\"componentType\":5126,\"count\":" << info.positionCount 
             << ",\"type\":\"VEC3\""
             << ",\"min\":[" << info.posMin.x << "," << info.posMin.y << "," << info.posMin.z << "]"
             << ",\"max\":[" << info.posMax.x << "," << info.posMax.y << "," << info.posMax.z << "]}";
        
        // Normal accessor
        json << ",{\"bufferView\":" << (baseBufferView + 1) 
             << ",\"componentType\":5126,\"count\":" << info.normalCount 
             << ",\"type\":\"VEC3\"}";
        
        // Index accessor
        json << ",{\"bufferView\":" << (baseBufferView + 2) 
             << ",\"componentType\":5123,\"count\":" << info.indexCount 
             << ",\"type\":\"SCALAR\"}";
    }
    json << "],";
    
    // Meshes
    json << "\"meshes\":[";
    for (size_t i = 0; i < meshInfos.size(); i++) {
        size_t baseAccessor = i * 3;
        if (i > 0) json << ",";
        json << "{\"name\":\"Mesh_" << i << "\",\"primitives\":[{"
             << "\"attributes\":{\"POSITION\":" << baseAccessor << ",\"NORMAL\":" << (baseAccessor + 1) << "},"
             << "\"indices\":" << (baseAccessor + 2)
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
    size_t positionOffset = 0;
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
    
    // Get accessors and buffer views
    auto& accessors = root.object["accessors"];
    auto& bufferViews = root.object["bufferViews"];
    auto& meshesJson = root.object["meshes"];
    
    if (accessors.type != JsonValue::Array || 
        bufferViews.type != JsonValue::Array || 
        meshesJson.type != JsonValue::Array) {
        return false;
    }
    
    // Import each mesh
    for (const auto& meshJson : meshesJson.array) {
        if (meshJson.type != JsonValue::Object) continue;
        
        auto& primitives = meshJson.object.at("primitives");
        if (primitives.type != JsonValue::Array || primitives.array.empty()) continue;
        
        auto& primitive = primitives.array[0];
        if (primitive.type != JsonValue::Object) continue;
        
        auto& attributes = primitive.object.at("attributes");
        if (attributes.type != JsonValue::Object) continue;
        
        // Get accessor indices
        int posAccessorIdx = -1;
        int normalAccessorIdx = -1;
        int indexAccessorIdx = -1;
        
        if (attributes.object.count("POSITION")) {
            posAccessorIdx = attributes.object.at("POSITION").asInt();
        }
        if (attributes.object.count("NORMAL")) {
            normalAccessorIdx = attributes.object.at("NORMAL").asInt();
        }
        if (primitive.object.count("indices")) {
            indexAccessorIdx = primitive.object.at("indices").asInt();
        }
        
        if (posAccessorIdx < 0) continue;
        
        // Get position data
        auto& posAccessor = accessors.array[posAccessorIdx];
        int posBvIdx = posAccessor.object.at("bufferView").asInt();
        size_t posCount = posAccessor.object.at("count").asSize();
        
        auto& posBv = bufferViews.array[posBvIdx];
        size_t posByteOffset = posBv.object.count("byteOffset") ? posBv.object.at("byteOffset").asSize() : 0;
        
        auto mesh = std::make_unique<Mesh2>();
        
        // Add vertices
        for (size_t i = 0; i < posCount; i++) {
            size_t offset = posByteOffset + i * 12;
            if (offset + 12 > binLength) break;
            
            float x = readFloat(binData + offset);
            float y = readFloat(binData + offset + 4);
            float z = readFloat(binData + offset + 8);
            mesh->addVertex(glm::vec3(x, y, z));
        }
        
        // Get indices if present
        if (indexAccessorIdx >= 0 && static_cast<size_t>(indexAccessorIdx) < accessors.array.size()) {
            auto& idxAccessor = accessors.array[indexAccessorIdx];
            int idxBvIdx = idxAccessor.object.at("bufferView").asInt();
            size_t idxCount = idxAccessor.object.at("count").asSize();
            int componentType = idxAccessor.object.at("componentType").asInt();
            
            auto& idxBv = bufferViews.array[idxBvIdx];
            size_t idxByteOffset = idxBv.object.count("byteOffset") ? idxBv.object.at("byteOffset").asSize() : 0;
            
            std::vector<uint32_t> indices;
            
            for (size_t i = 0; i < idxCount; i++) {
                uint32_t idx = 0;
                if (componentType == 5123) { // UNSIGNED_SHORT
                    size_t offset = idxByteOffset + i * 2;
                    if (offset + 2 <= binLength) {
                        idx = readLittleEndian<uint16_t>(binData + offset);
                    }
                } else if (componentType == 5125) { // UNSIGNED_INT
                    size_t offset = idxByteOffset + i * 4;
                    if (offset + 4 <= binLength) {
                        idx = readLittleEndian<uint32_t>(binData + offset);
                    }
                }
                indices.push_back(idx);
            }
            
            // Add triangles
            for (size_t i = 0; i + 2 < indices.size(); i += 3) {
                mesh->addTriangle(indices[i], indices[i + 1], indices[i + 2]);
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
    
    return true;
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
