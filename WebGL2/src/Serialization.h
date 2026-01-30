#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

class ItemCollection;
class Item;
class Mesh2;

namespace Serialization {

// ============================================================================
// OBJ Format (Wavefront)
// ============================================================================

// Export the entire scene to OBJ format
std::string exportToOBJ(const ItemCollection& items);

// Export a single mesh to OBJ format
std::string exportMeshToOBJ(const Mesh2& mesh, const std::string& objectName = "Object");

// Import OBJ data and add items to the collection
// Returns true on success
bool importFromOBJ(ItemCollection& items, const std::string& objData);

// ============================================================================
// GLB Format (Binary glTF 2.0)
// ============================================================================

// Export the entire scene to GLB format
// Returns binary data as vector of bytes
std::vector<uint8_t> exportToGLB(const ItemCollection& items);

// Export a single mesh to GLB format
std::vector<uint8_t> exportMeshToGLB(const Mesh2& mesh, const std::string& meshName = "Mesh");

// Import GLB data and add items to the collection
// Returns true on success
bool importFromGLB(ItemCollection& items, const std::vector<uint8_t>& glbData);

// ============================================================================
// Utilities
// ============================================================================

// Get file extension (lowercase, without dot)
std::string getFileExtension(const std::string& filename);

// Validate GLB magic header
bool isValidGLB(const std::vector<uint8_t>& data);

} // namespace Serialization
