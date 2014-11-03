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
    unsigned int _index;
	Vector3D _position;
	Quaternion _rotation;
	Vector3D _scale;
public:
    ItemManipulationState(ItemCollection &collection, unsigned int index);
    ~ItemManipulationState();
    
    void apply(ItemCollection &collection);
};

class RemovedItem
{
private:
    unsigned int _index;
    Item *_item;
public:
    RemovedItem(ItemCollection &collection, unsigned int index);
    ~RemovedItem();
    
    void selectItemForRemove(ItemCollection &collection);
    void insert(ItemCollection &collection);
};

class MeshState
{
private:
    unsigned int _index;
	vector<Vector3D> _vertices;
    vector<Vector3D> _texCoords;
	vector<TriQuad> _triangles;
	vector<bool> _selection;
	MeshSelectionMode _selectionMode;
public:
    MeshState(ItemCollection &collection, unsigned int index);
    ~MeshState();
    
    unsigned int index();
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
    
    Item *itemAtIndex(unsigned int index);
    void addItem(Item *item);
    void removeItem(Item *item);
    void removeLastItem();
    void removeItemAtIndex(unsigned int index);
    void removeItemsInRange(unsigned int location, unsigned int length);
    void insertItemAtIndex(unsigned int index, Item *item);
    void mergeSelectedItems();
    void setSelectionFromRemovedItems(IUndoState *undoState);
    void deselectAll();
    void getVertexAndTriangleCount(unsigned int &vertexCount, unsigned int &triangleCount);
    Mesh2 *currentMesh();
    Item *firstSelectedItem();
    
    // IOpenGLManipulatingModel
    
    virtual ViewMode viewMode();
    virtual void setViewMode(ViewMode viewMode);
    virtual unsigned int count();
    virtual bool isSelectedAtIndex(unsigned int index);
    virtual void setSelectedAtIndex(unsigned int index, bool selected);
    virtual void expandSelectionFromIndex(unsigned int index, bool invert);
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
    
    virtual Vector3D positionAtIndex(unsigned int index);
    virtual Quaternion rotationAtIndex(unsigned int index);
    virtual Vector3D scaleAtIndex(unsigned int index);
    virtual void setPositionAtIndex(unsigned int index, Vector3D position);
    virtual void setRotationAtIndex(unsigned int index, Quaternion rotation);
    virtual void setScaleAtIndex(unsigned int index, Vector3D scale);
    virtual void moveByOffset(unsigned int index, Vector3D offset);
    virtual void rotateByOffset(unsigned int index, Quaternion offset);
    virtual void scaleByOffset(unsigned int index, Vector3D offset);
    virtual void drawAtIndex(unsigned int index, bool forSelection);
};
