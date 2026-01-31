#include "Item.h"
#include "Shader.h"
#include "Texture.h"

#ifdef EMSCRIPTEN_BUILD
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#endif

#include <glm/gtc/matrix_transform.hpp>

Item::Item()
    : position(0.0f)
    , rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    , scale(1.0f)
    , selected(false)
    , visible(true)
    , mesh(std::make_unique<Mesh2>())
{
}

Item::Item(std::unique_ptr<Mesh2> aMesh)
    : position(0.0f)
    , rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    , scale(1.0f)
    , selected(false)
    , visible(true)
    , mesh(std::move(aMesh))
{
}

Item::~Item() = default;

glm::mat4 Item::getTransformMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model *= glm::mat4_cast(rotation);
    model = glm::scale(model, scale);
    return model;
}

void Item::moveByOffset(const glm::vec3& offset) {
    position += offset;
}

void Item::rotateByOffset(const glm::quat& offset) {
    rotation = offset * rotation;
}

void Item::scaleByOffset(const glm::vec3& offset) {
    scale += offset;
    // Clamp to prevent zero/negative scale
    scale = glm::max(scale, glm::vec3(0.001f));
}

std::unique_ptr<Item> Item::duplicate() const {
    auto newItem = std::make_unique<Item>();
    newItem->position = position;
    newItem->rotation = rotation;
    newItem->scale = scale;
    newItem->selected = true;  // Select the duplicate
    newItem->visible = visible;
    
    // Merge source mesh into new empty mesh (original pattern)
    newItem->mesh->merge(mesh.get());
    
    return newItem;
}

void Item::draw(ViewMode mode) const {
    if (!visible) return;
    mesh->draw(mode);
}

void Item::drawForSelection(Shader& selectionShader, uint32_t baseIndex) const {
    if (!visible) return;
    mesh->drawForSelection(selectionShader);
}

void Item::setPositionToGeometricCenter() {
    // Calculate the center of all vertices
    glm::vec3 center(0.0f);
    size_t vertexCount = mesh->getVertexCount();
    
    if (vertexCount == 0) return;
    
    for (size_t i = 0; i < vertexCount; i++) {
        center += mesh->getVertex(static_cast<uint32_t>(i)).position;
    }
    center /= static_cast<float>(vertexCount);
    
    // Move all vertices by -center to center the mesh at origin
    glm::mat4 translateToOrigin = glm::translate(glm::mat4(1.0f), -center);
    mesh->transformAllVertices(translateToOrigin);
    mesh->createGPUBuffers();
    
    // Set item position to where the center was
    position = center;
}

glm::vec3 Item::getWorldCenter() const {
    glm::vec3 localCenter(0.0f);
    size_t vertexCount = mesh->getVertexCount();
    
    if (vertexCount > 0) {
        for (size_t i = 0; i < vertexCount; i++) {
            localCenter += mesh->getVertex(static_cast<uint32_t>(i)).position;
        }
        localCenter /= static_cast<float>(vertexCount);
    }
    
    // Transform to world space
    glm::vec4 worldCenter = getTransformMatrix() * glm::vec4(localCenter, 1.0f);
    return glm::vec3(worldCenter);
}

void Item::setTexture(std::shared_ptr<Texture> tex) {
    texture = tex;
}

std::shared_ptr<Texture> Item::getTexture() const {
    return texture;
}

bool Item::hasTexture() const {
    return texture != nullptr;
}
