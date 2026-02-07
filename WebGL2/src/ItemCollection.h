#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Item.h"
#include "Mesh2.h"

class Shader;

// Edit mode determines what we're selecting/manipulating
enum class EditMode {
    Items = 0,      // Select whole items (meshes)
    Vertices = 1,   // Select vertices within selected items
    Triangles = 2,  // Select faces within selected items
    Edges = 3       // Select edges within selected items
};

// Collection of Items in the scene
class ItemCollection {
public:
    ItemCollection();
    ~ItemCollection();
    
    // Item management
    void addItem(std::unique_ptr<Item> item);
    void removeItemAtIndex(size_t index);
    void removeSelectedItems();
    Item* getItemAtIndex(size_t index);
    const Item* getItemAtIndex(size_t index) const;
    size_t getItemCount() const { return m_items.size(); }
    
    // Add primitives as new items
    void addCube();
    void addPlane();
    void addCylinder(uint32_t steps = 20);
    void addSphere(uint32_t steps = 20);
    void addIcosahedron();
    
    // Edit mode
    EditMode getEditMode() const { return m_editMode; }
    void setEditMode(EditMode mode);
    
    // Selection in Items mode
    void selectItemAtIndex(size_t index, bool addToSelection = false);
    void deselectItemAtIndex(size_t index);
    void selectAllItems();
    void deselectAllItems();
    size_t getSelectedItemCount() const;
    Item* getFirstSelectedItem();
    
    // Get the mesh being edited (first selected item's mesh)
    Mesh2* getCurrentMesh();
    const Mesh2* getCurrentMesh() const;
    
    // Operations on selected items
    void duplicateSelectedItems();
    void deleteSelectedItems();
    
    // Transform selected items (in Items mode)
    void translateSelectedItems(const glm::vec3& offset);
    void rotateSelectedItems(const glm::quat& offset);
    void scaleSelectedItems(const glm::vec3& offset);
    void scaleSelectedItemsByOffset(const glm::vec3& center, const glm::vec3& offset);
    glm::vec3 getSelectedItemsCenter() const;
    
    // For component editing (forwards to current mesh when in Vertices/Triangles/Edges mode)
    // Note: Offsets/centers are in world space and will be transformed to local space
    void translateSelectedComponents(const glm::vec3& offset);
    void rotateSelectedComponents(const glm::vec3& center, const glm::quat& offset);
    void scaleSelectedComponents(const glm::vec3& center, float factor);
    void scaleSelectedComponentsByOffset(const glm::vec3& center, const glm::vec3& offset);
    glm::vec3 getSelectedComponentsCenter() const;
    
    // Component selection (forwards to meshes of selected items)
    void selectFaceAtIndex(size_t faceIndex, bool addToSelection = false);
    void deselectFaceAtIndex(size_t faceIndex);
    void selectAllComponents();
    void deselectAllComponents();
    size_t getSelectedComponentCount() const;
    bool isFaceSelected(size_t index) const;
    
    // Edge loop/ring selection
    void selectEdgeLoopFromSelected();
    void selectEdgeRingFromSelected();
    void growEdgeSelection();
    
    // Component operations
    void flipSelectedFaces();
    void duplicateSelectedFaces();
    void deleteSelectedFaces();
    void splitSelected();
    void catmullClarkSubdivide(int level = 1);
    void triangulateSelectedFaces();
    void mergeSelectedVertices(float threshold = 0.001f);
    void extrudeSelectedFaces();
    
    // Drawing
    void draw(Shader& meshShader, Shader& wireShader, ViewMode mode, 
              const glm::mat4& view, const glm::mat4& projection) const;
    void drawComponentOverlay(Shader& coloredShader,
                              const glm::mat4& view, const glm::mat4& projection) const;
    void drawNormals(Shader& coloredShader,
                     const glm::mat4& view, const glm::mat4& projection) const;
    void drawForSelection(Shader& selectionShader, 
                          const glm::mat4& view, const glm::mat4& projection) const;
    
    // UV Editor drawing
    void drawUV(Shader& uvShader, Shader& uvColoredShader, 
                const glm::vec2& offset, float zoom, const glm::vec2& aspectAdjust) const;
    void drawUVForSelection(Shader& selectionShader,
                            const glm::vec2& offset, float zoom, const glm::vec2& aspectAdjust) const;
    std::shared_ptr<Texture> getFirstSelectedTexture() const;
    
    // GPU buffers
    void createAllGPUBuffers();
    
    // Stats
    void getVertexAndFaceCount(size_t& outVertices, size_t& outFaces) const;

private:
    std::vector<std::unique_ptr<Item>> m_items;
    EditMode m_editMode;
};
