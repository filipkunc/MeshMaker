#include "ItemCollection.h"
#include "Shader.h"
#include "Texture.h"
#include "DebugLog.h"

#include <algorithm>

#ifdef EMSCRIPTEN_BUILD
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#endif

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

ItemCollection::ItemCollection()
    : m_editMode(EditMode::Items)
{
}

ItemCollection::~ItemCollection() = default;

// Item management
void ItemCollection::addItem(std::unique_ptr<Item> item) {
    m_items.push_back(std::move(item));
}

void ItemCollection::removeItemAtIndex(size_t index) {
    if (index < m_items.size()) {
        m_items.erase(m_items.begin() + index);
    }
}

void ItemCollection::removeSelectedItems() {
    m_items.erase(
        std::remove_if(m_items.begin(), m_items.end(),
            [](const std::unique_ptr<Item>& item) { return item->selected; }),
        m_items.end());
}

Item* ItemCollection::getItemAtIndex(size_t index) {
    return index < m_items.size() ? m_items[index].get() : nullptr;
}

const Item* ItemCollection::getItemAtIndex(size_t index) const {
    return index < m_items.size() ? m_items[index].get() : nullptr;
}

// Add primitives
void ItemCollection::addCube() {
    auto item = std::make_unique<Item>();
    item->mesh->makeCube();
    item->mesh->createGPUBuffers();
    item->selected = true;
    
    // Deselect others if not adding
    for (auto& other : m_items) {
        other->selected = false;
    }
    
    m_items.push_back(std::move(item));
}

void ItemCollection::addPlane() {
    auto item = std::make_unique<Item>();
    item->mesh->makePlane();
    item->mesh->createGPUBuffers();
    item->selected = true;
    
    for (auto& other : m_items) {
        other->selected = false;
    }
    
    m_items.push_back(std::move(item));
}

void ItemCollection::addCylinder(uint32_t steps) {
    auto item = std::make_unique<Item>();
    item->mesh->makeCylinder(steps);
    item->mesh->createGPUBuffers();
    item->selected = true;
    
    for (auto& other : m_items) {
        other->selected = false;
    }
    
    m_items.push_back(std::move(item));
}

void ItemCollection::addSphere(uint32_t steps) {
    auto item = std::make_unique<Item>();
    item->mesh->makeSphere(steps);
    item->mesh->createGPUBuffers();
    item->selected = true;
    
    for (auto& other : m_items) {
        other->selected = false;
    }
    
    m_items.push_back(std::move(item));
}

void ItemCollection::addIcosahedron() {
    auto item = std::make_unique<Item>();
    item->mesh->makeIcosahedron();
    item->mesh->createGPUBuffers();
    item->selected = true;
    
    for (auto& other : m_items) {
        other->selected = false;
    }
    
    m_items.push_back(std::move(item));
}

// Edit mode
void ItemCollection::setEditMode(EditMode mode) {
    if (m_editMode == mode) return;
    
    // When switching to Items mode, deselect all components
    if (mode == EditMode::Items) {
        for (auto& item : m_items) {
            if (item->selected && item->mesh) {
                item->mesh->deselectAll();
                item->mesh->createGPUBuffers();
            }
        }
    }
    // When switching to component mode, set mesh selection mode
    else {
        SelectionMode meshMode;
        switch (mode) {
            case EditMode::Vertices: meshMode = SelectionMode::Vertices; break;
            case EditMode::Edges: meshMode = SelectionMode::Edges; break;
            default: meshMode = SelectionMode::Triangles; break;
        }
        for (auto& item : m_items) {
            if (item->selected && item->mesh) {
                item->mesh->setSelectionMode(meshMode);
            }
        }
    }
    
    m_editMode = mode;
}

// Item selection
void ItemCollection::selectItemAtIndex(size_t index, bool addToSelection) {
    if (!addToSelection) {
        deselectAllItems();
    }
    if (index < m_items.size()) {
        m_items[index]->selected = true;
    }
}

void ItemCollection::deselectItemAtIndex(size_t index) {
    if (index < m_items.size()) {
        m_items[index]->selected = false;
    }
}

void ItemCollection::selectAllItems() {
    for (auto& item : m_items) {
        item->selected = true;
    }
}

void ItemCollection::deselectAllItems() {
    for (auto& item : m_items) {
        item->selected = false;
    }
}

size_t ItemCollection::getSelectedItemCount() const {
    size_t count = 0;
    for (const auto& item : m_items) {
        if (item->selected) count++;
    }
    return count;
}

Item* ItemCollection::getFirstSelectedItem() {
    for (auto& item : m_items) {
        if (item->selected) return item.get();
    }
    return nullptr;
}

Mesh2* ItemCollection::getCurrentMesh() {
    Item* item = getFirstSelectedItem();
    return item ? item->mesh.get() : nullptr;
}

const Mesh2* ItemCollection::getCurrentMesh() const {
    for (const auto& item : m_items) {
        if (item->selected) return item->mesh.get();
    }
    return nullptr;
}

// Item operations
void ItemCollection::duplicateSelectedItems() {
    std::vector<std::unique_ptr<Item>> newItems;
    
    for (auto& item : m_items) {
        if (item->selected) {
            auto duplicate = item->duplicate();
            duplicate->position = item->position + glm::vec3(0.5f, 0.0f, 0.0f);  // Offset slightly
            
            item->selected = false;
            newItems.push_back(std::move(duplicate));
        }
    }
    
    for (auto& item : newItems) {
        m_items.push_back(std::move(item));
    }
}

void ItemCollection::deleteSelectedItems() {
    removeSelectedItems();
}

// Transform items
void ItemCollection::translateSelectedItems(const glm::vec3& offset) {
    for (auto& item : m_items) {
        if (item->selected) {
            item->moveByOffset(offset);
        }
    }
}

void ItemCollection::rotateSelectedItems(const glm::quat& offset) {
    size_t selectedCount = 0;
    Item* lastSelected = nullptr;
    
    for (auto& item : m_items) {
        if (item->selected) {
            selectedCount++;
            lastSelected = item.get();
        }
    }
    
    if (selectedCount > 1) {
        // Multiple items selected - rotate positions around center AND rotate each item
        glm::vec3 rotationCenter = getSelectedItemsCenter();
        glm::mat4 offsetMatrix = glm::mat4_cast(offset);
        
        for (auto& item : m_items) {
            if (item->selected) {
                // Rotate position around center
                glm::vec3 itemPosition = item->position;
                itemPosition -= rotationCenter;
                itemPosition = glm::vec3(offsetMatrix * glm::vec4(itemPosition, 1.0f));
                itemPosition += rotationCenter;
                item->position = itemPosition;
                
                // Also rotate the item itself
                item->rotateByOffset(offset);
            }
        }
    } else if (lastSelected != nullptr) {
        // Single item - just rotate the item (no position change)
        lastSelected->rotateByOffset(offset);
    }
}

void ItemCollection::scaleSelectedItems(const glm::vec3& offset) {
    size_t selectedCount = 0;
    Item* lastSelected = nullptr;
    
    for (auto& item : m_items) {
        if (item->selected) {
            selectedCount++;
            lastSelected = item.get();
        }
    }
    
    if (selectedCount > 1) {
        // Multiple items selected - scale positions relative to center AND scale each item
        glm::vec3 center = getSelectedItemsCenter();
        
        for (auto& item : m_items) {
            if (item->selected) {
                glm::vec3 itemPosition = item->position;
                itemPosition -= center;
                itemPosition.x *= 1.0f + offset.x;
                itemPosition.y *= 1.0f + offset.y;
                itemPosition.z *= 1.0f + offset.z;
                itemPosition += center;
                item->position = itemPosition;
                
                // Also scale the item itself
                item->scaleByOffset(offset);
            }
        }
    } else if (lastSelected != nullptr) {
        // Single item - just scale it
        lastSelected->scaleByOffset(offset);
    }
}

void ItemCollection::scaleSelectedItemsByOffset(const glm::vec3& center, const glm::vec3& offset) {
    size_t selectedCount = 0;
    Item* lastSelected = nullptr;
    
    for (auto& item : m_items) {
        if (item->selected) {
            selectedCount++;
            lastSelected = item.get();
        }
    }
    
    if (selectedCount > 1) {
        // Multiple items selected - scale positions relative to center
        for (auto& item : m_items) {
            if (item->selected) {
                glm::vec3 itemPosition = item->position;
                itemPosition -= center;
                itemPosition.x *= 1.0f + offset.x;
                itemPosition.y *= 1.0f + offset.y;
                itemPosition.z *= 1.0f + offset.z;
                itemPosition += center;
                item->position = itemPosition;
                item->scaleByOffset(offset);
            }
        }
    } else if (lastSelected != nullptr) {
        // Single item - just scale it
        lastSelected->scaleByOffset(offset);
    }
}

glm::vec3 ItemCollection::getSelectedItemsCenter() const {
    glm::vec3 center(0.0f);
    size_t count = 0;
    
    for (const auto& item : m_items) {
        if (item->selected) {
            center += item->position;
            count++;
        }
    }
    
    return count > 0 ? center / static_cast<float>(count) : glm::vec3(0.0f);
}

// Component operations (forwards to meshes)
// Note: Offsets from manipulator are in world space, but mesh vertices are in local space.
// We need to transform offsets by the inverse of the item's rotation and scale.
void ItemCollection::translateSelectedComponents(const glm::vec3& offset) {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            // Transform the offset from world space to local space
            // by applying inverse rotation and inverse scale (like original OpenGLManipulatingController)
            glm::quat inverseRotation = glm::conjugate(item->rotation);
            glm::vec3 inverseScale = 1.0f / item->scale;
            
            // Apply inverse rotation then inverse scale: m = s * r
            glm::mat4 invRotMatrix = glm::mat4_cast(inverseRotation);
            glm::mat4 invScaleMatrix = glm::scale(glm::mat4(1.0f), inverseScale);
            glm::mat4 transformMatrix = invScaleMatrix * invRotMatrix;
            
            glm::vec3 transformedOffset = glm::vec3(transformMatrix * glm::vec4(offset, 0.0f));
            
            item->mesh->translateSelected(transformedOffset);
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::rotateSelectedComponents(const glm::vec3& center, const glm::quat& offset) {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            // Transform the rotation center from world space to local space
            glm::mat4 invTransform = glm::inverse(item->getTransformMatrix());
            glm::vec3 localCenter = glm::vec3(invTransform * glm::vec4(center, 1.0f));
            
            // Transform the rotation from world space to local space
            // This is quaternion conjugation: R_local = Q_inv * R_world * Q
            // This ensures that rotating around "world X" actually rotates around
            // the axis that appears as X in world space, not the local X axis
            glm::quat invRotation = glm::conjugate(item->rotation);
            glm::quat localRotation = invRotation * offset * item->rotation;
            
            // Build the rotation matrix: translate to origin, rotate, translate back
            glm::mat4 t1 = glm::translate(glm::mat4(1.0f), -localCenter);
            glm::mat4 r = glm::mat4_cast(localRotation);
            glm::mat4 t2 = glm::translate(glm::mat4(1.0f), localCenter);
            glm::mat4 m = t2 * r * t1;
            
            item->mesh->transformSelectedByMatrix(m);
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::scaleSelectedComponents(const glm::vec3& center, float factor) {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            // Transform the scale center from world space to local space
            glm::mat4 invTransform = glm::inverse(item->getTransformMatrix());
            glm::vec3 localCenter = glm::vec3(invTransform * glm::vec4(center, 1.0f));
            
            item->mesh->scaleSelected(localCenter, factor);
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::scaleSelectedComponentsByOffset(const glm::vec3& center, const glm::vec3& offset) {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            // Transform the scale center from world space to local space
            glm::mat4 invTransform = glm::inverse(item->getTransformMatrix());
            glm::vec3 localCenter = glm::vec3(invTransform * glm::vec4(center, 1.0f));
            
            // Transform the scale from world space to local space
            // Similar to rotation: S_local = R_inv * S_world * R
            // This ensures that scaling along "world X" actually scales along
            // the axis that appears as X in world space, not the local X axis
            glm::mat4 invRotMatrix = glm::mat4_cast(glm::conjugate(item->rotation));
            glm::mat4 rotMatrix = glm::mat4_cast(item->rotation);
            glm::mat4 worldScale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) + offset);
            glm::mat4 localScale = invRotMatrix * worldScale * rotMatrix;
            
            // Build the transform matrix: translate to origin, scale, translate back
            glm::mat4 t1 = glm::translate(glm::mat4(1.0f), -localCenter);
            glm::mat4 t2 = glm::translate(glm::mat4(1.0f), localCenter);
            glm::mat4 m = t2 * localScale * t1;
            
            item->mesh->transformSelectedByMatrix(m);
            item->mesh->createGPUBuffers();
        }
    }
}

glm::vec3 ItemCollection::getSelectedComponentsCenter() const {
    for (const auto& item : m_items) {
        if (item->selected && item->mesh) {
            // Transform mesh selection center to world space
            glm::vec3 localCenter = item->mesh->getSelectionCenter();
            glm::vec4 worldCenter = item->getTransformMatrix() * glm::vec4(localCenter, 1.0f);
            return glm::vec3(worldCenter);
        }
    }
    return glm::vec3(0.0f);
}

// Component selection
void ItemCollection::selectFaceAtIndex(size_t faceIndex, bool addToSelection) {
    Mesh2* mesh = getCurrentMesh();
    if (mesh) {
        mesh->selectFace(faceIndex, addToSelection);
        mesh->createGPUBuffers();
    }
}

void ItemCollection::deselectFaceAtIndex(size_t faceIndex) {
    Mesh2* mesh = getCurrentMesh();
    if (mesh) {
        mesh->deselectFace(faceIndex);
        mesh->createGPUBuffers();
    }
}

void ItemCollection::selectAllComponents() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->selectAll();
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::deselectAllComponents() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->deselectAll();
            item->mesh->createGPUBuffers();
        }
    }
}

size_t ItemCollection::getSelectedComponentCount() const {
    size_t count = 0;
    for (const auto& item : m_items) {
        if (item->selected && item->mesh) {
            count += item->mesh->getSelectedCount();
        }
    }
    return count;
}

bool ItemCollection::isFaceSelected(size_t index) const {
    const Mesh2* mesh = nullptr;
    for (const auto& item : m_items) {
        if (item->selected && item->mesh) {
            mesh = item->mesh.get();
            break;
        }
    }
    return mesh ? mesh->isFaceSelected(index) : false;
}

// Edge loop/ring selection
void ItemCollection::selectEdgeLoopFromSelected() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            // Get currently selected edges and expand to loops
            auto& edges = item->mesh->getEdges();
            std::vector<uint32_t> selectedIndices;
            for (uint32_t i = 0; i < edges.size(); i++) {
                if (edges[i].selected) {
                    selectedIndices.push_back(i);
                }
            }
            // Select edge loops for each selected edge
            for (uint32_t idx : selectedIndices) {
                item->mesh->selectEdgeLoop(idx);
            }
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::selectEdgeRingFromSelected() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            // Get currently selected edges and expand to rings
            auto& edges = item->mesh->getEdges();
            std::vector<uint32_t> selectedIndices;
            for (uint32_t i = 0; i < edges.size(); i++) {
                if (edges[i].selected) {
                    selectedIndices.push_back(i);
                }
            }
            // Select edge rings for each selected edge
            for (uint32_t idx : selectedIndices) {
                item->mesh->selectEdgeRing(idx);
            }
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::growEdgeSelection() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->growEdgeSelection();
            item->mesh->createGPUBuffers();
        }
    }
}

// Component operations
void ItemCollection::flipSelectedFaces() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->flipSelected();
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::duplicateSelectedFaces() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->duplicateSelected();
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::deleteSelectedFaces() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->deleteSelected();
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::splitSelected() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->splitSelected();
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::catmullClarkSubdivide(int level) {
    DEBUG_LOG("[ItemCollection::catmullClark] items=%zu, level=%d\n", m_items.size(), level);
    for (auto& item : m_items) {
        DEBUG_LOG("[ItemCollection::catmullClark] item selected=%d, hasMesh=%d\n", 
                 item->selected, item->mesh != nullptr);
        if (item->selected && item->mesh) {
            item->mesh->catmullClarkSubdivide(level);
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::triangulateSelectedFaces() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->triangulateSelected();
            item->mesh->createGPUBuffers();
        }
    }
}



void ItemCollection::mergeSelectedVertices(float threshold) {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->mergeSelectedVertices(threshold);
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::extrudeSelectedFaces() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->extrudeSelected();
            item->mesh->createGPUBuffers();
        }
    }
}

// Drawing
void ItemCollection::draw(Shader& meshShader, Shader& meshWireShader, Shader& thickLineColoredShader, ViewMode mode,
                          const glm::mat4& view, const glm::mat4& projection,
                          float viewportWidth, float viewportHeight) const {
    for (const auto& item : m_items) {
        if (!item->visible) continue;
        
        glm::mat4 model = item->getTransformMatrix();
        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(view * model));
        
        // Only draw wireframe for selected items (matching original MeshMaker behavior)
        // In Items mode, wireframe indicates selection. In component modes, show for all.
        bool drawWireframe = (mode == ViewMode::Wireframe) ||
                            (mode == ViewMode::SolidWireframe && item->selected) ||
                            (m_editMode != EditMode::Items);
        
        if (mode == ViewMode::SolidWireframe && drawWireframe) {
            // Single-pass solid+wireframe using barycentric coordinate shader
            // No polygon offset needed - wireframe is computed in the same triangles
            meshWireShader.use();
            meshWireShader.setMat4("uModel", model);
            meshWireShader.setMat4("uView", view);
            meshWireShader.setMat4("uProjection", projection);
            meshWireShader.setMat3("uNormalMatrix", normalMatrix);
            meshWireShader.setVec3("uWireColor", item->mesh->getWireframeColor());
            meshWireShader.setVec3("uSelectionColor", glm::vec3(1.0f, 0.5f, 0.0f));
            meshWireShader.setVec3("uSeamColor", glm::vec3(0.0f, 0.8f, 0.0f));
            meshWireShader.setFloat("uLineWidth", 1.0f);
            
            if (item->hasTexture()) {
                item->getTexture()->bind(0);
                meshWireShader.setInt("uTexture", 0);
                meshWireShader.setBool("uUseTexture", true);
            } else {
                meshWireShader.setBool("uUseTexture", false);
            }
            
            item->mesh->drawSolidWireframe();
            
            if (item->hasTexture()) {
                Texture::unbind(0);
            }
        } else if (mode == ViewMode::Solid || (mode == ViewMode::SolidWireframe && !drawWireframe)) {
            // Solid only
            meshShader.use();
            meshShader.setMat4("uModel", model);
            meshShader.setMat4("uView", view);
            meshShader.setMat4("uProjection", projection);
            meshShader.setMat3("uNormalMatrix", normalMatrix);
            
            if (item->hasTexture()) {
                item->getTexture()->bind(0);
                meshShader.setInt("uTexture", 0);
                meshShader.setBool("uUseTexture", true);
            } else {
                meshShader.setBool("uUseTexture", false);
            }
            
            item->mesh->draw(ViewMode::Solid);
            
            if (item->hasTexture()) {
                Texture::unbind(0);
            }
        } else if (mode == ViewMode::Wireframe) {
            // Wireframe only - use thick line shader
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            
            thickLineColoredShader.use();
            thickLineColoredShader.setMat4("uModel", model);
            thickLineColoredShader.setMat4("uView", view);
            thickLineColoredShader.setMat4("uProjection", projection);
            thickLineColoredShader.setVec2("uViewportSize", glm::vec2(viewportWidth, viewportHeight));
            thickLineColoredShader.setFloat("uLineWidth", 1.5f);
            thickLineColoredShader.setBool("uAntialias", true);
            
            item->mesh->draw(ViewMode::Wireframe);
            
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
    }
}

void ItemCollection::drawComponentOverlay(Shader& coloredShader, Shader& thickLineColoredShader,
                                           const glm::mat4& view, const glm::mat4& projection,
                                           float viewportWidth, float viewportHeight) const {
    // Draw vertex points or edge highlights for component modes
    // Only draws for selected items in component editing modes
    if (m_editMode == EditMode::Items) return;
    
    for (const auto& item : m_items) {
        if (!item->visible || !item->selected) continue;
        
        glm::mat4 model = item->getTransformMatrix();
        
        if (m_editMode == EditMode::Vertices) {
            coloredShader.use();
            coloredShader.setMat4("uModel", model);
            coloredShader.setMat4("uView", view);
            coloredShader.setMat4("uProjection", projection);
            item->mesh->drawVertices(coloredShader);
        } else if (m_editMode == EditMode::Edges) {
            // Edge selection/seam colors are handled by the barycentric wireframe shader
            // in the main draw pass. No separate overlay needed.
        }
        // Triangles mode - selection is shown via face colors in regular draw
    }
}

void ItemCollection::drawNormals(Shader& coloredShader,
                                  const glm::mat4& view, const glm::mat4& projection) const {
    for (const auto& item : m_items) {
        if (!item->visible) continue;
        
        glm::mat4 model = item->getTransformMatrix();
        
        coloredShader.use();
        coloredShader.setMat4("uModel", model);
        coloredShader.setMat4("uView", view);
        coloredShader.setMat4("uProjection", projection);
        
        item->mesh->drawNormals(coloredShader);
    }
}

void ItemCollection::drawForSelection(Shader& selectionShader, 
                                       const glm::mat4& view, const glm::mat4& projection) const {
    uint32_t baseIndex = 0;
    
    for (const auto& item : m_items) {
        if (!item->visible) continue;
        
        glm::mat4 model = item->getTransformMatrix();
        
        selectionShader.use();
        selectionShader.setMat4("uModel", model);
        selectionShader.setMat4("uView", view);
        selectionShader.setMat4("uProjection", projection);
        
        item->mesh->drawForSelection(selectionShader);
        
        baseIndex += static_cast<uint32_t>(item->mesh->getFaceCount());
    }
}

void ItemCollection::createAllGPUBuffers() {
    for (auto& item : m_items) {
        if (item->mesh) {
            item->mesh->createGPUBuffers();
        }
    }
}

// UV Editor drawing - renders faces in UV space
void ItemCollection::drawUV(Shader& uvShader, Shader& uvColoredShader,
                            const glm::vec2& offset, float zoom, const glm::vec2& aspectAdjust) const {
    for (const auto& item : m_items) {
        if (!item->visible || !item->selected) continue;
        if (!item->mesh) continue;
        
        // Draw UV faces
        uvShader.use();
        uvShader.setVec2("uOffset", offset);
        uvShader.setFloat("uZoom", zoom);
        uvShader.setVec2("uAspectAdjust", aspectAdjust);
        uvShader.setBool("uHasTexture", item->hasTexture());
        uvShader.setFloat("uAlpha", 0.6f);
        
        if (item->hasTexture()) {
            item->getTexture()->bind(0);
            uvShader.setInt("uTexture", 0);
        }
        
        item->mesh->drawUV(uvShader);
        
        if (item->hasTexture()) {
            Texture::unbind(0);
        }
        
        // Always draw UV edges for visibility
        uvColoredShader.use();
        uvColoredShader.setVec2("uOffset", offset);
        uvColoredShader.setFloat("uZoom", zoom);
        uvColoredShader.setVec2("uAspectAdjust", aspectAdjust);
        uvColoredShader.setFloat("uPointSize", 6.0f);
        
        item->mesh->drawUVEdges(uvColoredShader);
    }
}

void ItemCollection::drawUVForSelection(Shader& selectionShader,
                                         const glm::vec2& offset, float zoom, const glm::vec2& aspectAdjust) const {
    for (const auto& item : m_items) {
        if (!item->visible || !item->selected) continue;
        if (!item->mesh) continue;
        
        selectionShader.use();
        selectionShader.setVec2("uOffset", offset);
        selectionShader.setFloat("uZoom", zoom);
        selectionShader.setVec2("uAspectAdjust", aspectAdjust);
        
        item->mesh->drawUVForSelection(selectionShader);
    }
}

void ItemCollection::getVertexAndFaceCount(size_t& outVertices, size_t& outFaces) const {
    outVertices = 0;
    outFaces = 0;
    
    for (const auto& item : m_items) {
        if (item->mesh) {
            outVertices += item->mesh->getVertexCount();
            outFaces += item->mesh->getFaceCount();
        }
    }
}
std::shared_ptr<Texture> ItemCollection::getFirstSelectedTexture() const {
    for (const auto& item : m_items) {
        if (item->visible && item->selected && item->hasTexture()) {
            return item->getTexture();
        }
    }
    return nullptr;
}