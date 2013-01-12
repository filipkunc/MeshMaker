//
//  ItemCollection.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#include "OpenGLDrawing.h"
#include "ItemCollection.h"

ItemManipulationState::ItemManipulationState(ItemCollection &collection, uint index)
{
    _index = index;
    Item *item = collection.itemAtIndex(_index);
    _position = item->position;
    _rotation = item->rotation;
    _scale = item->scale;
}

ItemManipulationState::~ItemManipulationState()
{

}

void ItemManipulationState::apply(ItemCollection &collection)
{
    Item *item = collection.itemAtIndex(_index);
    item->position = _position;
    item->rotation = _rotation;
    item->scale = _scale;
    item->selected = true;
}

RemovedItem::RemovedItem(ItemCollection &collection, uint index)
{
    _index = index;
    _item = collection.itemAtIndex(_index)->duplicate();
}

RemovedItem::~RemovedItem()
{
    delete _item;
}

void RemovedItem::selectItemForRemove(ItemCollection &collection)
{
    collection.setSelectedAtIndex(_index, true);
}

void RemovedItem::insert(ItemCollection &collection)
{
    collection.insertItemAtIndex(_index, _item->duplicate());
}

MeshState::MeshState(ItemCollection &collection, uint index)
{
    _index = index;
    Mesh2 *mesh = collection.itemAtIndex(_index)->mesh;
    mesh->toIndexRepresentation(_vertices, _texCoords, _triangles);
    _selectionMode = mesh->selectionMode();
    mesh->getSelection(_selection);
}

MeshState::~MeshState()
{
}

void MeshState::apply(ItemCollection &collection)
{
    Item *item = collection.itemAtIndex(_index);
    item->selected = true;
    Mesh2 *mesh = item->mesh;
    mesh->fromIndexRepresentation(_vertices, _texCoords, _triangles);
    mesh->setSelectionMode(_selectionMode);
    mesh->setSelection(_selection);
}

uint MeshState::index()
{
    return _index;
}

ItemCollection::ItemCollection()
{
}

ItemCollection::~ItemCollection()
{
    for (uint i = 0; i < items.size(); i++)
        delete items[i];
    
    items.clear();
}

ItemCollection::ItemCollection(MemoryReadStream *stream)
{
    uint itemsCount;
    stream->readBytes(&itemsCount, sizeof(uint));
    for (uint i = 0; i < itemsCount; i++)
    {
        Item *item = new Item(stream);
        items.push_back(item);
    }
}

void ItemCollection::encode(MemoryWriteStream *stream)
{
    uint itemsCount = items.size();
    stream->writeBytes(&itemsCount, sizeof(uint));
	for (uint i = 0; i < itemsCount; i++)
	{
		Item *item = items.at(i);
        item->encode(stream);
	}
}

IUndoState *ItemCollection::currentManipulations()
{
    vector<ItemManipulationState *> *manipulations = new vector<ItemManipulationState *>();
	
	for (uint i = 0; i < items.size(); i++)
	{
		Item *item = items.at(i);
		if (item->selected)
            manipulations->push_back(new ItemManipulationState(*this, i));
	}
	
	return new UndoState<vector<ItemManipulationState *>>(manipulations);
}

void ItemCollection::setCurrentManipulations(IUndoState *undoState)
{
    vector<ItemManipulationState *> *manipulations = dynamic_cast<UndoState<vector<ItemManipulationState *>> *>(undoState)->state();
    
    deselectAll();
	
    for (uint i = 0; i < manipulations->size(); i++)
    {
        ItemManipulationState *state = manipulations->at(i);
        state->apply(*this);
	}
}

IUndoState *ItemCollection::currentMeshState()
{
    for (uint i = 0; i < items.size(); i++)
	{
		Item *item = items.at(i);
		if (item->selected)
		{
			MeshState *meshState = new MeshState(*this, i);
			return new UndoState<MeshState>(meshState);
		}
	}
	return NULL;
}

void ItemCollection::setCurrentMeshState(IUndoState *undoState)
{
    if (undoState == NULL)
        return;
    
    MeshState *meshState = dynamic_cast<UndoState<MeshState> *>(undoState)->state();
    
    deselectAll();
	
    meshState->apply(*this);
}

IUndoState *ItemCollection::currentSelection()
{
    vector<uint> *selection = new vector<uint>();
	
	for (uint i = 0; i < items.size(); i++)
	{
		Item *item = items.at(i);
		if (item->selected)
            selection->push_back(i);
	}
	
	return new UndoState<vector<uint>>(selection);
}

void ItemCollection::setCurrentSelection(IUndoState *undoState)
{
    vector<uint> *selection = dynamic_cast<UndoState<vector<uint>> *>(undoState)->state();
    
    deselectAll();
	
    for (uint i = 0; i < selection->size(); i++)
    {
        setSelectedAtIndex(selection->at(i), true);
    }
}

IUndoState *ItemCollection::currentItems()
{
    vector<RemovedItem *> *removedItems = new vector<RemovedItem *>();
	
	for (uint i = 0; i < items.size(); i++)
	{
		Item *item = items.at(i);
		if (item->selected)
		{
            RemovedItem *removedItem = new RemovedItem(*this, i);
            removedItems->push_back(removedItem);
		}
	}
	
	return new UndoState<vector<RemovedItem *>>(removedItems);
}

void ItemCollection::setCurrentItems(IUndoState *undoState)
{
    vector<RemovedItem *> *removedItems = dynamic_cast<UndoState<vector<RemovedItem *>> *>(undoState)->state();
    
    deselectAll();
    
    for (uint i = 0; i < removedItems->size(); i++)
    {
        RemovedItem *removedItem = removedItems->at(i);
        removedItem->insert(*this);
    }
}

IUndoState *ItemCollection::allItems()
{
    vector<Item *> *duplicates = new vector<Item *>();
	
	for (uint i = 0; i < items.size(); i++)
	{
		Item *duplicate = items.at(i)->duplicate();
        duplicates->push_back(duplicate);
	}
	
	return new UndoState<vector<Item *>>(duplicates);
}

void ItemCollection::setAllItems(IUndoState *undoState)
{
    vector<Item *> *duplicates = dynamic_cast<UndoState<vector<Item *>> *>(undoState)->state();
    
    for (uint i = 0; i < items.size(); i++)
        delete items[i];
    
    items.clear();
    
    for (uint i = 0; i < duplicates->size(); i++)
    {
        Item *duplicate = duplicates->at(i)->duplicate();
        items.push_back(duplicate);
    }
}

Item *ItemCollection::itemAtIndex(uint index)
{
    return items.at(index);
}

void ItemCollection::addItem(Item *item)
{
    items.push_back(item);
}

void ItemCollection::removeItem(Item *item)
{
    for (uint i = 0; i < items.size(); i++)
    {
        if (items[i] == item)
        {
            removeItemAtIndex(i);
            break;
        }
    }
}

void ItemCollection::removeLastItem()
{
    if (items.size() > 0)
    {
        Item *lastItem = items[items.size() - 1];
        delete lastItem;
        items.pop_back();
    }
}

void ItemCollection::removeItemAtIndex(uint index)
{
    Item *item = items.at(index);
    delete item;
    items.erase(items.begin() + index);

}

void ItemCollection::removeItemsInRange(uint location, uint length)
{
    for (uint i = location; i < location + length; i++)
        delete items[i];
    
    items.erase(items.begin() + location, items.begin() + location + length);
}

void ItemCollection::insertItemAtIndex(uint index, Item *item)
{
    items.insert(items.begin() + index, item);
}

void ItemCollection::mergeSelectedItems()
{
    Vector3D center = Vector3D();
	uint selectedCount = 0;
	
	for (uint i = 0; i < items.size(); i++)
	{
        Item *item = items[i];
		if (item->selected)
		{
			selectedCount++;
			center += item->position;
		}
	}
	
	if (selectedCount < 2)
		return;
	
	center /= selectedCount;
	
    Mesh2 *mesh = new Mesh2();
	Item *newItem = new Item(mesh);
    newItem->position = center;
	
	Matrix4x4 firstMatrix, itemMatrix;
	
	firstMatrix.TranslateRotateScale(newItem->position, newItem->rotation, newItem->scale);
	firstMatrix = firstMatrix.Inverse();
	
	for (int i = 0; i < (int)items.size(); i++)
	{
        Item *item = items[i];
		if (item->selected)
		{
			Vector3D scale = item->scale;
			
			itemMatrix.TranslateRotateScale(item->position, item->rotation, scale);
			
			Matrix4x4 finalMatrix = firstMatrix * itemMatrix;
			Mesh2 *itemMesh = item->mesh;
			
			itemMesh->transformAll(finalMatrix);
			
			// mirror detection, some component of scale is negative
			if (scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f)
                itemMesh->flipAllTriangles();
            
            mesh->merge(itemMesh);
            removeItemAtIndex(i);
			i--;
		}
	}
	
    newItem->selected = true;
    addItem(newItem);
}

void ItemCollection::setSelectionFromRemovedItems(IUndoState *undoState)
{
    vector<RemovedItem *> *removedItems = dynamic_cast<UndoState<vector<RemovedItem *>> *>(undoState)->state();
    
    deselectAll();
    
    for (uint i = 0; i < removedItems->size(); i++)
    {
        RemovedItem *removedItem = removedItems->at(i);
        removedItem->selectItemForRemove(*this);
    }
}

void ItemCollection::deselectAll()
{
    for (uint i = 0; i < items.size(); i++)
        items[i]->selected = false;
}

void ItemCollection::getVertexAndTriangleCount(uint &vertexCount, uint &triangleCount)
{
    vertexCount = 0;
	triangleCount = 0;
	for (uint i = 0; i < items.size(); i++)
    {
        Mesh2 *mesh = items[i]->mesh;
		vertexCount += mesh->vertexCount();
		triangleCount += mesh->triangleCount();
	}
}

Mesh2 *ItemCollection::currentMesh()
{
    for (uint i = 0; i < items.size(); i++)
    {
        Item *item = items[i];
		if (item->selected)
			return item->mesh;
	}
    return NULL;

}

Item *ItemCollection::firstSelectedItem()
{
    for (uint i = 0; i < items.size(); i++)
    {
        Item *item = items[i];
		if (item->selected)
			return item;
	}
    return NULL;
}

ViewMode ItemCollection::viewMode()
{
    for (uint i = 0; i < items.size(); i++)
    {
        Item *item = items[i];
		if (item->selected)
			return item->viewMode();
	}
    return ViewMode::SolidFlat;
}

void ItemCollection::setViewMode(ViewMode viewMode)
{
    for (uint i = 0; i < items.size(); i++)
    {
        Item *item = items[i];
		if (item->selected)
			item->setViewMode(viewMode);
	}
}

uint ItemCollection::count()
{
    return items.size();
}

bool ItemCollection::isSelectedAtIndex(uint index)
{
    return items.at(index)->selected;
}

void ItemCollection::setSelectedAtIndex(uint index, bool selected)
{
    items.at(index)->selected = selected;
}

void ItemCollection::expandSelectionFromIndex(uint index, bool invert)
{
    for (uint i = 0; i < items.size(); i++)
        setSelectedAtIndex(i, true);
}

void ItemCollection::duplicateSelected()
{
    uint count = items.size();
	for (uint i = 0; i < count; i++)
	{
        Item *oldItem = items[i];
		if (oldItem->selected)
		{
			Item *newItem = oldItem->duplicate();
            oldItem->selected = false;
            addItem(newItem);
		}
	}
}

void ItemCollection::removeSelected()
{
    for (int i = 0; i < (int)items.size(); i++)
	{
		if (items[i]->selected)
		{
            removeItemAtIndex(i);
			i--;
		}
	}
}

void ItemCollection::hideSelected()
{
    for (uint i = 0; i < items.size(); i++)
	{
        Item *item = items[i];
		if (item->selected)
		{
            item->selected = false;
            item->visible = false;
		}
	}
}

void ItemCollection::unhideAll()
{
    for (uint i = 0; i < items.size(); i++)
		items[i]->visible = true;
}

Vector4D ItemCollection::selectionColor()
{
    Item *first = firstSelectedItem();
    if (first)
        return first->selectionColor();
    return Vector4D();
}

void ItemCollection::setSelectionColor(Vector4D color)
{
    for (uint i = 0; i < items.size(); i++)
	{
        Item *item = items[i];
		if (item->selected)
            item->setSelectionColor(color);
	}
}

void ItemCollection::willSelectThrough(bool selectThrough)
{
    
}

void ItemCollection::didSelect()
{
    
}

bool ItemCollection::needsCullFace()
{
    return false;
}

Vector3D ItemCollection::positionAtIndex(uint index)
{
    return items.at(index)->position;
}

Quaternion ItemCollection::rotationAtIndex(uint index)
{
    return items.at(index)->rotation;
}

Vector3D ItemCollection::scaleAtIndex(uint index)
{
    return items.at(index)->scale;
}

void ItemCollection::setPositionAtIndex(uint index, Vector3D position)
{
    items.at(index)->position = position;
}

void ItemCollection::setRotationAtIndex(uint index, Quaternion rotation)
{
    items.at(index)->rotation = rotation;
}

void ItemCollection::setScaleAtIndex(uint index, Vector3D scale)
{
    items.at(index)->scale = scale;
}

void ItemCollection::moveByOffset(uint index, Vector3D offset)
{
    items.at(index)->moveByOffset(offset);
}

void ItemCollection::rotateByOffset(uint index, Quaternion offset)
{
    items.at(index)->rotateByOffset(offset);
}

void ItemCollection::scaleByOffset(uint index, Vector3D offset)
{
    items.at(index)->scaleByOffset(offset);
}

void ItemCollection::drawAtIndex(uint index, bool forSelection)
{
    items.at(index)->drawForSelection(forSelection);
}
