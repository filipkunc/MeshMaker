//
//  ItemCollection.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "Item.h"
#import "OpenGLSelecting.h"
#import "OpenGLManipulating.h"
#import "OpenGLManipulatingController.h"
#import "IndexedItem.h"
#import "MeshState.h"

class ItemCollection : public IOpenGLManipulatingModelItem
{
private:
    vector<Item *> items;
public:
    ItemCollection();
    virtual ~ItemCollection();

    ItemCollection(MemoryReadStream *stream);
    void encode(MemoryWriteStream *stream);
    
    NSMutableArray *currentManipulations();
    void setCurrentManipulations(NSMutableArray *manipulations);
    MeshState *currentMeshState();
    void setCurrentMeshState(MeshState *meshState);
    NSMutableArray *currentSelection();
    void setCurrentSelection(NSMutableArray *selection);
    NSMutableArray *currentItems();
    void setCurrentItems(NSMutableArray *anItems);
    NSMutableArray *allItems();
    void setAllItems(NSMutableArray *anItems);
    
    Item *itemAtIndex(uint index);
    void addItem(Item *item);
    void removeItem(Item *item);
    void removeLastItem();
    void removeItemAtIndex(uint index);
    void removeItemsInRange(NSRange range);
    void insertItemAtIndex(uint index, Item *item);
    void mergeSelectedItems();
    void setSelectionFromIndexedItems(NSMutableArray *indexedItems);
    void deselectAll();
    void getVertexAndTriangleCount(uint &vertexCount, uint &triangleCount);
    Mesh2 *currentMesh();
    Item *firstSelectedItem();
    
    virtual ViewMode viewMode();
    virtual void setViewMode(ViewMode viewMode);
    virtual uint count();
    virtual bool isSelectedAtIndex(uint index);
    virtual void setSelectedAtIndex(uint index, bool selected);
    virtual void expandSelectionFromIndex(uint index, bool invert);
    virtual void duplicateSelected();
    virtual void removeSelected();
    virtual void hideSelected();
    virtual void unhideAll();
    
    virtual NSColor *selectionColor(); // can be Vector4D instead of NSColor *
    virtual void setSelectionColor(NSColor *color);
    virtual void willSelectThrough(bool selectThrough);
    virtual void didSelect();
    virtual bool needsCullFace();
    
    virtual Vector3D positionAtIndex(uint index);
    virtual Quaternion rotationAtIndex(uint index);
    virtual Vector3D scaleAtIndex(uint index);
    virtual void setPositionAtIndex(uint index, Vector3D position);
    virtual void setRotationAtIndex(uint index, Quaternion rotation);
    virtual void setScaleAtIndex(uint index, Vector3D scale);
    virtual void moveByOffset(uint index, Vector3D offset);
    virtual void rotateByOffset(uint index, Quaternion offset);
    virtual void scaleByOffset(uint index, Vector3D offset);
    virtual void drawAtIndex(uint index, bool forSelection);
};
