//
//  ItemCollection.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "Item.h"
#include "OpenGLSelecting.h"
#include "OpenGLManipulating.h"
#include "OpenGLManipulatingController.h"

class ItemCollection;

class ItemManipulationState
{
private:
    uint _index;
	Vector3D _position;
	Quaternion _rotation;
	Vector3D _scale;
public:
    ItemManipulationState(ItemCollection &collection, uint index);
    ~ItemManipulationState();
    
    void apply(ItemCollection &collection);
};

class RemovedItem
{
private:
    uint _index;
    Item *_item;
public:
    RemovedItem(ItemCollection &collection, uint index);
    ~RemovedItem();
    
    void selectItemForRemove(ItemCollection &collection);
    void insert(ItemCollection &collection);
};

class MeshState
{
private:
    uint _index;
	vector<Vector3D> _vertices;
    vector<Vector3D> _texCoords;
	vector<TriQuad> _triangles;
	vector<bool> _selection;
	MeshSelectionMode _selectionMode;
public:
    MeshState(ItemCollection &collection, uint index);
    ~MeshState();
    
    uint index();
    void apply(ItemCollection &collection);
};

class IUndoState
{
public:
    virtual ~IUndoState() { }
};

template <class T>
class UndoState : public IUndoState
{
    T *_state;
public:
    UndoState(T *state) : _state(state) { }
    T *state() { return _state; }
    virtual ~UndoState() { delete _state; }
};

class ItemCollection : public IOpenGLManipulatingModelItem
{
private:
    vector<Item *> items;
public:
    ItemCollection();
    virtual ~ItemCollection();

    ItemCollection(MemoryReadStream *stream, TextureCollection &textures);
    void encode(MemoryWriteStream *stream, TextureCollection &textures);
    
    IUndoState *currentManipulations();
    void setCurrentManipulations(IUndoState *undoState);
    IUndoState *currentMeshState();
    void setCurrentMeshState(IUndoState *undoState);
    IUndoState *currentSelection();
    void setCurrentSelection(IUndoState *undoState);
    IUndoState *currentItems();
    void setCurrentItems(IUndoState *undoState);
    IUndoState *allItems();
    void setAllItems(IUndoState *undoState);
    
    Item *itemAtIndex(uint index);
    void addItem(Item *item);
    void removeItem(Item *item);
    void removeLastItem();
    void removeItemAtIndex(uint index);
    void removeItemsInRange(uint location, uint length);
    void insertItemAtIndex(uint index, Item *item);
    void mergeSelectedItems();
    void setSelectionFromRemovedItems(IUndoState *undoState);
    void deselectAll();
    void getVertexAndTriangleCount(uint &vertexCount, uint &triangleCount);
    Mesh2 *currentMesh();
    Item *firstSelectedItem();
    
    // IOpenGLManipulatingModel
    
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
    
    virtual Vector4D selectionColor();
    virtual void setSelectionColor(Vector4D color);
    virtual void willSelectThrough(bool selectThrough);
    virtual void didSelect();
    virtual bool needsCullFace();
    
    // IOpenGLManipulatingModelItem
    
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
