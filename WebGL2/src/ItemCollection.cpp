#include "ItemCollection.h"
#include "Shader.h"

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

// Item operations
void ItemCollection::duplicateSelectedItems() {
    std::vector<std::unique_ptr<Item>> newItems;
    
    for (auto& item : m_items) {
        if (item->selected) {
            auto duplicate = std::make_unique<Item>();
            duplicate->position = item->position + glm::vec3(0.5f, 0.0f, 0.0f);
            duplicate->rotation = item->rotation;
            duplicate->scale = item->scale;
            duplicate->selected = true;
            
            // Copy mesh by recreating (TODO: proper deep copy)
            duplicate->mesh = std::make_unique<Mesh2>();
            // For now, just copy vertex/face data manually
            // This is a simplified copy - need proper mesh cloning
            
            item->selected = false;
            newItems.push_back(std::move(duplicate));
        }
    }
    
    for (auto& item : newItems) {
        item->mesh->createGPUBuffers();
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
    for (auto& item : m_items) {
        if (item->selected) {
            item->rotateByOffset(offset);
        }
    }
}

void ItemCollection::scaleSelectedItems(const glm::vec3& offset) {
    for (auto& item : m_items) {
        if (item->selected) {
            item->scaleByOffset(offset);
        }
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
void ItemCollection::translateSelectedComponents(const glm::vec3& offset) {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->translateSelected(offset);
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::rotateSelectedComponents(const glm::vec3& axis, float angleRadians) {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->rotateSelected(axis, angleRadians);
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::scaleSelectedComponents(const glm::vec3& center, float factor) {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->scaleSelected(center, factor);
            item->mesh->createGPUBuffers();
        }
    }
}

void ItemCollection::scaleSelectedComponentsByOffset(const glm::vec3& center, const glm::vec3& offset) {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->scaleSelectedByOffset(center, offset);
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

void ItemCollection::subdivideSelectedFaces() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->subdivideSelected();
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

void ItemCollection::splitSelectedEdges() {
    for (auto& item : m_items) {
        if (item->selected && item->mesh) {
            item->mesh->splitSelectedEdges();
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
void ItemCollection::draw(Shader& meshShader, Shader& wireShader, ViewMode mode,
                          const glm::mat4& view, const glm::mat4& projection) const {
    for (const auto& item : m_items) {
        if (!item->visible) continue;
        
        glm::mat4 model = item->getTransformMatrix();
        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(view * model));
        
        // Always draw solid for selected or if mode requires it
        bool drawSolid = (mode == ViewMode::Solid || mode == ViewMode::SolidWireframe);
        
        // Only draw wireframe for selected items (matching original MeshMaker behavior)
        // In Items mode, wireframe indicates selection. In component modes, show for all.
        bool drawWireframe = (mode == ViewMode::Wireframe) ||
                            (mode == ViewMode::SolidWireframe && item->selected) ||
                            (m_editMode != EditMode::Items);
        
        // Draw solid
        if (drawSolid) {
            if (drawWireframe && mode == ViewMode::SolidWireframe) {
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1.0f, 1.0f);
            }
            
            meshShader.use();
            meshShader.setMat4("uModel", model);
            meshShader.setMat4("uView", view);
            meshShader.setMat4("uProjection", projection);
            meshShader.setMat3("uNormalMatrix", normalMatrix);
            
            item->mesh->draw(ViewMode::Solid);
            
            if (drawWireframe && mode == ViewMode::SolidWireframe) {
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
        }
        
        // Draw wireframe overlay for selected items
        if (drawWireframe) {
            wireShader.use();
            wireShader.setMat4("uModel", model);
            wireShader.setMat4("uView", view);
            wireShader.setMat4("uProjection", projection);
            
            item->mesh->draw(ViewMode::Wireframe);
        }
    }
}

void ItemCollection::drawComponentOverlay(Shader& coloredShader,
                                           const glm::mat4& view, const glm::mat4& projection) const {
    // Draw vertex points or edge highlights for component modes
    // Only draws for selected items in component editing modes
    if (m_editMode == EditMode::Items) return;
    
    for (const auto& item : m_items) {
        if (!item->visible || !item->selected) continue;
        
        glm::mat4 model = item->getTransformMatrix();
        
        coloredShader.use();
        coloredShader.setMat4("uModel", model);
        coloredShader.setMat4("uView", view);
        coloredShader.setMat4("uProjection", projection);
        
        if (m_editMode == EditMode::Vertices) {
            item->mesh->drawVertices(coloredShader);
        } else if (m_editMode == EditMode::Edges) {
            item->mesh->drawEdges(coloredShader);
        }
        // Triangles mode - selection is shown via face colors in regular draw
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
