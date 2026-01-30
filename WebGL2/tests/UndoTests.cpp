#include <gtest/gtest.h>
#include "ItemCollection.h"
#include "Item.h"
#include "Mesh2.h"
#include "UndoManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <memory>

// =============================================================================
// Test Environment - Disables GPU operations for all tests
// =============================================================================

class UndoTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        Mesh2::s_disableGPU = true;
    }
    
    void TearDown() override {
        Mesh2::s_disableGPU = false;
    }
};

testing::Environment* const undo_env = 
    testing::AddGlobalTestEnvironment(new UndoTestEnvironment);

// =============================================================================
// State Capture Structures (mirrors main.cpp but for testing)
// =============================================================================

struct ItemState {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    bool selected = false;
    bool visible = true;
    SelectionMode meshSelectionMode = SelectionMode::Triangles;
    std::vector<MeshVertex> meshVertices;
    std::vector<Face> meshFaces;
    std::vector<Edge> meshEdges;
};

struct SceneState {
    std::vector<ItemState> items;
};

// Helper function to compare vectors with tolerance
bool vec3Equal(const glm::vec3& a, const glm::vec3& b, float epsilon = 0.0001f) {
    return glm::length(a - b) < epsilon;
}

// =============================================================================
// Undo Test Fixture
// =============================================================================

class UndoSystemTest : public ::testing::Test {
protected:
    std::unique_ptr<ItemCollection> collection;
    UndoManager undoManager;
    
    void SetUp() override {
        collection = std::make_unique<ItemCollection>();
    }
    
    void TearDown() override {
        collection.reset();
        undoManager.clear();
    }
    
    // Capture full scene state
    SceneState captureSceneState() {
        SceneState state;
        for (size_t i = 0; i < collection->getItemCount(); ++i) {
            Item* item = collection->getItemAtIndex(i);
            if (item) {
                ItemState itemState;
                itemState.position = item->position;
                itemState.rotation = item->rotation;
                itemState.scale = item->scale;
                itemState.selected = item->selected;
                itemState.visible = item->visible;
                
                if (item->mesh) {
                    itemState.meshSelectionMode = item->mesh->getSelectionMode();
                    item->mesh->getState(itemState.meshVertices, itemState.meshFaces, itemState.meshEdges);
                }
                
                state.items.push_back(std::move(itemState));
            }
        }
        return state;
    }
    
    // Apply scene state
    void applySceneState(const SceneState& state) {
        // Clear current items
        while (collection->getItemCount() > 0) {
            collection->removeItemAtIndex(0);
        }
        
        // Recreate from state
        for (const auto& itemState : state.items) {
            auto item = std::make_unique<Item>();
            item->position = itemState.position;
            item->rotation = itemState.rotation;
            item->scale = itemState.scale;
            item->selected = itemState.selected;
            item->visible = itemState.visible;
            
            item->mesh->setState(itemState.meshVertices, itemState.meshFaces, itemState.meshEdges);
            item->mesh->setSelectionMode(itemState.meshSelectionMode);
            
            collection->addItem(std::move(item));
        }
    }
    
    // Perform a scene action with undo support
    // The key insight is that undo and redo are symmetric operations.
    // When we undo, we apply oldState and prepare a redo that applies newState.
    // When we redo, we apply newState and prepare an undo that applies oldState.
    void sceneActionWithUndo(const std::string& actionName, std::function<void()> action) {
        SceneState oldState = captureSceneState();
        
        action();
        
        SceneState newState = captureSceneState();
        
        // Register the initial undo action
        pushUndoAction(actionName, oldState, newState);
    }
    
    // Push an undo action that, when executed, applies fromState and prepares its inverse
    void pushUndoAction(const std::string& actionName, const SceneState& fromState, const SceneState& toState) {
        undoManager.prepareUndo(actionName, [this, actionName, fromState, toState]() {
            applySceneState(fromState);
            // Prepare the inverse action (which will be pushed to redo or undo depending on context)
            pushUndoAction(actionName, toState, fromState);
        });
    }
    
    // Get positions of all items
    std::vector<glm::vec3> getItemPositions() {
        std::vector<glm::vec3> positions;
        for (size_t i = 0; i < collection->getItemCount(); ++i) {
            Item* item = collection->getItemAtIndex(i);
            if (item) {
                positions.push_back(item->position);
            }
        }
        return positions;
    }
    
    // Verify item count
    void expectItemCount(size_t expected) {
        EXPECT_EQ(collection->getItemCount(), expected);
    }
    
    // Verify item position at index
    void expectItemPosition(size_t index, const glm::vec3& expected) {
        ASSERT_LT(index, collection->getItemCount());
        Item* item = collection->getItemAtIndex(index);
        ASSERT_NE(item, nullptr);
        EXPECT_TRUE(vec3Equal(item->position, expected)) 
            << "Expected position (" << expected.x << ", " << expected.y << ", " << expected.z << ")"
            << " but got (" << item->position.x << ", " << item->position.y << ", " << item->position.z << ")";
    }
};

// =============================================================================
// Basic Undo Tests
// =============================================================================

TEST_F(UndoSystemTest, EmptyUndoStack) {
    EXPECT_FALSE(undoManager.canUndo());
    EXPECT_FALSE(undoManager.canRedo());
    EXPECT_EQ(undoManager.getUndoCount(), 0);
    EXPECT_EQ(undoManager.getRedoCount(), 0);
}

TEST_F(UndoSystemTest, AddCubeAndUndo) {
    expectItemCount(0);
    
    // Debug: check initial state
    ASSERT_EQ(undoManager.getUndoCount(), 0) << "Undo stack should be empty initially";
    ASSERT_FALSE(undoManager.canUndo()) << "Should not be able to undo initially";
    
    sceneActionWithUndo("Add Cube", [this]() {
        collection->addCube();
    });
    
    expectItemCount(1);
    
    // Debug: check undo stack after action - get all values first
    size_t undoCountAfterAdd = undoManager.getUndoCount();
    size_t redoCountAfterAdd = undoManager.getRedoCount();
    bool canUndoAfterAdd = undoManager.canUndo();
    bool canRedoAfterAdd = undoManager.canRedo();
    
    // Now assert
    ASSERT_EQ(undoCountAfterAdd, 1) << "Undo count should be 1";
    ASSERT_EQ(redoCountAfterAdd, 0) << "Redo count should be 0";
    ASSERT_TRUE(canUndoAfterAdd) << "canUndo should be true when undoCount=" << undoCountAfterAdd;
    ASSERT_FALSE(canRedoAfterAdd) << "canRedo should be false when redoCount=" << redoCountAfterAdd;
    
    undoManager.undo();
    
    expectItemCount(0);
    EXPECT_FALSE(undoManager.canUndo());
    EXPECT_TRUE(undoManager.canRedo());
    
    undoManager.redo();
    
    expectItemCount(1);
    EXPECT_TRUE(undoManager.canUndo());
    EXPECT_FALSE(undoManager.canRedo());
}

TEST_F(UndoSystemTest, MultipleActionsAndUndoRedo) {
    // Add first cube
    sceneActionWithUndo("Add Cube 1", [this]() {
        collection->addCube();
    });
    expectItemCount(1);
    
    // Add second cube
    sceneActionWithUndo("Add Cube 2", [this]() {
        collection->addCube();
    });
    expectItemCount(2);
    
    // Add third cube
    sceneActionWithUndo("Add Cube 3", [this]() {
        collection->addCube();
    });
    expectItemCount(3);
    
    EXPECT_EQ(undoManager.getUndoCount(), 3);
    
    // Undo all
    undoManager.undo();
    expectItemCount(2);
    undoManager.undo();
    expectItemCount(1);
    undoManager.undo();
    expectItemCount(0);
    
    EXPECT_EQ(undoManager.getUndoCount(), 0);
    EXPECT_EQ(undoManager.getRedoCount(), 3);
    
    // Redo all
    undoManager.redo();
    expectItemCount(1);
    undoManager.redo();
    expectItemCount(2);
    undoManager.redo();
    expectItemCount(3);
    
    EXPECT_EQ(undoManager.getUndoCount(), 3);
    EXPECT_EQ(undoManager.getRedoCount(), 0);
}

// =============================================================================
// Complex Scenario Test - User's Bug Report
// =============================================================================

TEST_F(UndoSystemTest, ComplexScenario_AddTranslateDuplicateMultipleTimes) {
    // This tests the exact scenario from the bug report:
    // 1. Add a Cube to empty scene
    // 2. Move the Cube via Translate
    // 3. Duplicate
    // 4. Move the Duplicate
    // 5. Duplicate again
    // 6. Move again
    // Then: Undo all, Redo all, Undo all, Redo all
    
    // Note: duplicateSelectedItems() adds a 0.5 X offset to duplicates
    const glm::vec3 duplicateOffset(0.5f, 0.0f, 0.0f);
    
    const glm::vec3 pos0(0.0f, 0.0f, 0.0f);
    const glm::vec3 move1(1.0f, 0.0f, 0.0f);
    const glm::vec3 move2(0.0f, 1.0f, 0.0f);
    const glm::vec3 move3(0.0f, 0.0f, 1.0f);
    
    // =========================================================================
    // Step 1: Add a Cube to empty scene
    // =========================================================================
    sceneActionWithUndo("Add Cube", [this]() {
        collection->addCube();
    });
    
    expectItemCount(1);
    expectItemPosition(0, pos0);
    
    // =========================================================================
    // Step 2: Move the Cube via Translate
    // =========================================================================
    sceneActionWithUndo("Translate", [this, &move1]() {
        collection->translateSelectedItems(move1);
    });
    
    expectItemCount(1);
    glm::vec3 cube1Pos = pos0 + move1;  // Cube at (1, 0, 0)
    expectItemPosition(0, cube1Pos);
    
    // =========================================================================
    // Step 3: Duplicate (creates second cube with 0.5 X offset)
    // =========================================================================
    sceneActionWithUndo("Duplicate", [this]() {
        collection->duplicateSelectedItems();
    });
    
    expectItemCount(2);
    glm::vec3 cube2Pos = cube1Pos + duplicateOffset;  // Duplicate at (1.5, 0, 0)
    expectItemPosition(0, cube1Pos);   // Original at (1, 0, 0)
    expectItemPosition(1, cube2Pos);   // Duplicate at (1.5, 0, 0)
    
    // =========================================================================
    // Step 4: Move the Duplicate
    // =========================================================================
    sceneActionWithUndo("Translate Duplicate", [this, &move2]() {
        collection->translateSelectedItems(move2);
    });
    
    expectItemCount(2);
    cube2Pos = cube2Pos + move2;  // Duplicate at (1.5, 1, 0)
    expectItemPosition(0, cube1Pos);   // Original still at (1, 0, 0)
    expectItemPosition(1, cube2Pos);   // Duplicate at (1.5, 1, 0)
    
    // =========================================================================
    // Step 5: Duplicate again
    // =========================================================================
    sceneActionWithUndo("Duplicate 2", [this]() {
        collection->duplicateSelectedItems();
    });
    
    expectItemCount(3);
    glm::vec3 cube3Pos = cube2Pos + duplicateOffset;  // Third cube at (2, 1, 0)
    expectItemPosition(0, cube1Pos);   // Original at (1, 0, 0)
    expectItemPosition(1, cube2Pos);   // Second at (1.5, 1, 0)
    expectItemPosition(2, cube3Pos);   // Third at (2, 1, 0)
    
    // =========================================================================
    // Step 6: Move again
    // =========================================================================
    sceneActionWithUndo("Translate 3", [this, &move3]() {
        collection->translateSelectedItems(move3);
    });
    
    expectItemCount(3);
    cube3Pos = cube3Pos + move3;  // Third cube at (2, 1, 1)
    expectItemPosition(0, cube1Pos);   // Original at (1, 0, 0)
    expectItemPosition(1, cube2Pos);   // Second at (1.5, 1, 0)
    expectItemPosition(2, cube3Pos);   // Third at (2, 1, 1)
    
    // Record final state for verification
    const glm::vec3 finalPos0 = cube1Pos;
    const glm::vec3 finalPos1 = cube2Pos;
    const glm::vec3 finalPos2 = cube3Pos;
    
    EXPECT_EQ(undoManager.getUndoCount(), 6);
    EXPECT_EQ(undoManager.getRedoCount(), 0);
    
    // =========================================================================
    // UNDO ALL STEPS (first time)
    // =========================================================================
    
    // Undo Step 6: Move again
    undoManager.undo();
    expectItemCount(3);
    expectItemPosition(0, cube1Pos);
    expectItemPosition(1, cube2Pos);
    expectItemPosition(2, cube2Pos + duplicateOffset);  // Before move3: at (2, 1, 0)
    
    // Undo Step 5: Duplicate again
    undoManager.undo();
    expectItemCount(2);
    expectItemPosition(0, cube1Pos);
    expectItemPosition(1, cube2Pos);
    
    // Undo Step 4: Move the Duplicate
    undoManager.undo();
    expectItemCount(2);
    expectItemPosition(0, cube1Pos);
    expectItemPosition(1, cube1Pos + duplicateOffset);  // Before move2: at (1.5, 0, 0)
    
    // Undo Step 3: Duplicate
    undoManager.undo();
    expectItemCount(1);
    expectItemPosition(0, cube1Pos);
    
    // Undo Step 2: Move the Cube
    undoManager.undo();
    expectItemCount(1);
    expectItemPosition(0, pos0);  // Back to origin
    
    // Undo Step 1: Add Cube
    undoManager.undo();
    expectItemCount(0);
    
    EXPECT_EQ(undoManager.getUndoCount(), 0);
    EXPECT_EQ(undoManager.getRedoCount(), 6);
    
    // =========================================================================
    // REDO ALL STEPS (first time)
    // =========================================================================
    
    // Redo Step 1: Add Cube
    undoManager.redo();
    expectItemCount(1);
    expectItemPosition(0, pos0);
    
    // Redo Step 2: Move the Cube
    undoManager.redo();
    expectItemCount(1);
    expectItemPosition(0, cube1Pos);
    
    // Redo Step 3: Duplicate
    undoManager.redo();
    expectItemCount(2);
    expectItemPosition(0, cube1Pos);
    expectItemPosition(1, cube1Pos + duplicateOffset);
    
    // Redo Step 4: Move the Duplicate
    undoManager.redo();
    expectItemCount(2);
    expectItemPosition(0, cube1Pos);
    expectItemPosition(1, cube2Pos);
    
    // Redo Step 5: Duplicate again
    undoManager.redo();
    expectItemCount(3);
    expectItemPosition(0, cube1Pos);
    expectItemPosition(1, cube2Pos);
    expectItemPosition(2, cube2Pos + duplicateOffset);
    
    // Redo Step 6: Move again
    undoManager.redo();
    expectItemCount(3);
    expectItemPosition(0, finalPos0);
    expectItemPosition(1, finalPos1);
    expectItemPosition(2, finalPos2);
    
    EXPECT_EQ(undoManager.getUndoCount(), 6);
    EXPECT_EQ(undoManager.getRedoCount(), 0);
    
    // =========================================================================
    // UNDO ALL STEPS (second time)
    // =========================================================================
    
    for (int i = 0; i < 6; ++i) {
        undoManager.undo();
    }
    
    expectItemCount(0);
    EXPECT_EQ(undoManager.getUndoCount(), 0);
    EXPECT_EQ(undoManager.getRedoCount(), 6);
    
    // =========================================================================
    // REDO ALL STEPS (second time)
    // =========================================================================
    
    for (int i = 0; i < 6; ++i) {
        undoManager.redo();
    }
    
    expectItemCount(3);
    expectItemPosition(0, finalPos0);
    expectItemPosition(1, finalPos1);
    expectItemPosition(2, finalPos2);
    
    EXPECT_EQ(undoManager.getUndoCount(), 6);
    EXPECT_EQ(undoManager.getRedoCount(), 0);
}

// =============================================================================
// Test redo stack clearing on new action
// =============================================================================

TEST_F(UndoSystemTest, RedoStackClearedOnNewAction) {
    // Add cube
    sceneActionWithUndo("Add Cube", [this]() {
        collection->addCube();
    });
    
    // Move it
    sceneActionWithUndo("Translate", [this]() {
        collection->translateSelectedItems(glm::vec3(1.0f, 0.0f, 0.0f));
    });
    
    // Undo the move
    undoManager.undo();
    
    EXPECT_EQ(undoManager.getUndoCount(), 1);
    EXPECT_EQ(undoManager.getRedoCount(), 1);
    
    // Now do a different action - this should clear the redo stack
    sceneActionWithUndo("Move Different", [this]() {
        collection->translateSelectedItems(glm::vec3(0.0f, 1.0f, 0.0f));
    });
    
    EXPECT_EQ(undoManager.getUndoCount(), 2);
    EXPECT_EQ(undoManager.getRedoCount(), 0);  // Redo stack should be cleared
}

// =============================================================================
// Test partial undo/redo cycles
// =============================================================================

TEST_F(UndoSystemTest, PartialUndoRedoCycles) {
    // Create 5 actions
    for (int i = 0; i < 5; ++i) {
        sceneActionWithUndo("Add Cube", [this]() {
            collection->addCube();
        });
    }
    
    EXPECT_EQ(collection->getItemCount(), 5);
    EXPECT_EQ(undoManager.getUndoCount(), 5);
    
    // Undo 3 times
    undoManager.undo();
    undoManager.undo();
    undoManager.undo();
    
    EXPECT_EQ(collection->getItemCount(), 2);
    EXPECT_EQ(undoManager.getUndoCount(), 2);
    EXPECT_EQ(undoManager.getRedoCount(), 3);
    
    // Redo 2 times
    undoManager.redo();
    undoManager.redo();
    
    EXPECT_EQ(collection->getItemCount(), 4);
    EXPECT_EQ(undoManager.getUndoCount(), 4);
    EXPECT_EQ(undoManager.getRedoCount(), 1);
    
    // Undo 1 time
    undoManager.undo();
    
    EXPECT_EQ(collection->getItemCount(), 3);
    EXPECT_EQ(undoManager.getUndoCount(), 3);
    EXPECT_EQ(undoManager.getRedoCount(), 2);
    
    // Redo all remaining
    undoManager.redo();
    undoManager.redo();
    
    EXPECT_EQ(collection->getItemCount(), 5);
    EXPECT_EQ(undoManager.getUndoCount(), 5);
    EXPECT_EQ(undoManager.getRedoCount(), 0);
}

// =============================================================================
// Test deep undo/redo with position verification
// =============================================================================

TEST_F(UndoSystemTest, DeepUndoRedoWithPositionVerification) {
    // Build up a complex state with recorded positions
    std::vector<std::pair<size_t, std::vector<glm::vec3>>> stateHistory;
    
    // Record initial empty state
    stateHistory.push_back({0, {}});
    
    // Add cube at origin
    sceneActionWithUndo("Add Cube", [this]() {
        collection->addCube();
    });
    stateHistory.push_back({1, {glm::vec3(0, 0, 0)}});
    
    // Move to (1, 0, 0)
    sceneActionWithUndo("Translate", [this]() {
        collection->translateSelectedItems(glm::vec3(1, 0, 0));
    });
    stateHistory.push_back({1, {glm::vec3(1, 0, 0)}});
    
    // Move to (1, 2, 0)
    sceneActionWithUndo("Translate", [this]() {
        collection->translateSelectedItems(glm::vec3(0, 2, 0));
    });
    stateHistory.push_back({1, {glm::vec3(1, 2, 0)}});
    
    // Move to (1, 2, 3)
    sceneActionWithUndo("Translate", [this]() {
        collection->translateSelectedItems(glm::vec3(0, 0, 3));
    });
    stateHistory.push_back({1, {glm::vec3(1, 2, 3)}});
    
    EXPECT_EQ(undoManager.getUndoCount(), 4);
    
    // Verify we can walk through history correctly
    // Current state should match last entry
    expectItemCount(stateHistory.back().first);
    for (size_t i = 0; i < stateHistory.back().second.size(); ++i) {
        expectItemPosition(i, stateHistory.back().second[i]);
    }
    
    // Undo through all states
    for (int step = static_cast<int>(stateHistory.size()) - 2; step >= 0; --step) {
        undoManager.undo();
        expectItemCount(stateHistory[step].first);
        for (size_t i = 0; i < stateHistory[step].second.size(); ++i) {
            expectItemPosition(i, stateHistory[step].second[i]);
        }
    }
    
    // Redo through all states
    for (size_t step = 1; step < stateHistory.size(); ++step) {
        undoManager.redo();
        expectItemCount(stateHistory[step].first);
        for (size_t i = 0; i < stateHistory[step].second.size(); ++i) {
            expectItemPosition(i, stateHistory[step].second[i]);
        }
    }
}
