#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include "Mesh2.h"

class Shader;

// A single Item in the scene - contains a mesh with its own transform
class Item {
public:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    bool selected;
    bool visible;
    
    std::unique_ptr<Mesh2> mesh;
    
    Item();
    Item(std::unique_ptr<Mesh2> aMesh);
    ~Item();
    
    // Get the model matrix for this item
    glm::mat4 getTransformMatrix() const;
    
    // Transform operations on the Item itself
    void moveByOffset(const glm::vec3& offset);
    void rotateByOffset(const glm::quat& offset);
    void scaleByOffset(const glm::vec3& offset);
    
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
