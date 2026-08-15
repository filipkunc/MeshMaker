#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include "Mesh2.h"

class Shader;
class Texture;

// A single Item in the scene - contains a mesh with its own transform
class Item {
public:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    bool selected;
    bool visible;
    glm::vec4 baseColor{1.0f};
    float metallic = 0.0f;
    float roughness = 0.4f;
    glm::vec3 emissiveColor{0.0f};
    float clearcoat = 0.0f;
    float clearcoatRoughness = 0.01f;
    float ior = 1.5f;
    
    std::unique_ptr<Mesh2> mesh;
    std::shared_ptr<Texture> texture;  // Optional texture for this item
    std::shared_ptr<Texture> normalTexture;
    
    Item();
    Item(std::unique_ptr<Mesh2> aMesh);
    ~Item();
    
    // Get the model matrix for this item
    glm::mat4 getTransformMatrix() const;
    
    // Transform operations on the Item itself
    void moveByOffset(const glm::vec3& offset);
    void rotateByOffset(const glm::quat& offset);
    void scaleByOffset(const glm::vec3& offset);
    
    // Texture management
    void setTexture(std::shared_ptr<Texture> tex);
    std::shared_ptr<Texture> getTexture() const;
    bool hasTexture() const;
    void setNormalTexture(std::shared_ptr<Texture> tex) { normalTexture = tex; }
    std::shared_ptr<Texture> getNormalTexture() const { return normalTexture; }
    bool hasNormalTexture() const { return normalTexture != nullptr; }
    
    // Create a duplicate of this item
    std::unique_ptr<Item> duplicate() const;
    
    // Drawing
    void draw(ViewMode mode) const;
    void drawForSelection(Shader& selectionShader, uint32_t baseIndex) const;
    
    // Center the item position to the geometric center of its mesh
    void setPositionToGeometricCenter();
    
    // Get bounding box center in world space
    glm::vec3 getWorldCenter() const;
};
