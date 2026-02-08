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
// Phased GLB Import (for progress reporting)
// ============================================================================

struct GLBImportInfo {
    int stepCount;      // Number of meshes/nodes to import
    bool success;
};

struct GLBStepInfo {
    std::string name;           // Node or mesh name
    size_t estimatedVertices;   // Estimated vertex count for this step
};

// Phase 1: Parse GLB, load textures, compute transforms (stores context internally)
bool beginGLBImport(std::vector<uint8_t> glbData, size_t importStartIndex, GLBImportInfo& outInfo);

// Get info about a specific import step (name, vertex count)
GLBStepInfo getActiveGLBStepInfo(int stepIndex);

// Phase 2: Import a single mesh by step index [0, info.stepCount)
bool executeGLBStep(ItemCollection& items, int stepIndex);

// Phase 3: Finalize (auto-scale, center, deselect old items)
bool finalizeActiveGLBImport(ItemCollection& items);

// ============================================================================
// Utilities
// ============================================================================

// Get file extension (lowercase, without dot)
std::string getFileExtension(const std::string& filename);

// Validate GLB magic header
bool isValidGLB(const std::vector<uint8_t>& data);

} // namespace Serialization
