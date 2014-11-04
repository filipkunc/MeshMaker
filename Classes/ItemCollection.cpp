//
//  ItemCollection.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#include "OpenGLDrawing.h"
#include "ItemCollection.h"

ItemManipulationState::ItemManipulationState(ItemCollection &collection, unsigned int index)
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

RemovedItem::RemovedItem(ItemCollection &collection, unsigned int index)
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

MeshState::MeshState(ItemCollection &collection, unsigned int index)
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

unsigned int MeshState::index()
{
    return _index;
}

ItemCollection::ItemCollection()
{
}

ItemCollection::~ItemCollection()
{
    for (unsigned int i = 0; i < items.size(); i++)
        delete items[i];
    
    items.clear();
}

ItemCollection::ItemCollection(MemoryReadStream *stream, TextureCollection &textures)
{
    unsigned int itemsCount = stream->read<unsigned int>();
    for (unsigned int i = 0; i < itemsCount; i++)
    {
        Item *item = new Item(stream, textures);
        items.push_back(item);
    }
}

void ItemCollection::encode(MemoryWriteStream *stream, TextureCollection &textures)
{
    unsigned int itemsCount = static_cast<unsigned int>(items.size());
    stream->write<unsigned int>(itemsCount);
	for (unsigned int i = 0; i < itemsCount; i++)
	{
		Item *item = items.at(i);
        item->encode(stream, textures);
	}
}

IUndoState *ItemCollection::currentManipulations()
{
    vector<ItemManipulationState *> *manipulations = new vector<ItemManipulationState *>();
	
	for (unsigned int i = 0; i < items.size(); i++)
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
	
    for (unsigned int i = 0; i < manipulations->size(); i++)
    {
        ItemManipulationState *state = manipulations->at(i);
        state->apply(*this);
	}
}

IUndoState *ItemCollection::currentMeshState()
{
    for (unsigned int i = 0; i < items.size(); i++)
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
    vector<unsigned int> *selection = new vector<unsigned int>();
	
	for (unsigned int i = 0; i < items.size(); i++)
	{
		Item *item = items.at(i);
		if (item->selected)
            selection->push_back(i);
	}
	
	return new UndoState<vector<unsigned int>>(selection);
}

void ItemCollection::setCurrentSelection(IUndoState *undoState)
{
    vector<unsigned int> *selection = dynamic_cast<UndoState<vector<unsigned int>> *>(undoState)->state();
    
    deselectAll();
	
    for (unsigned int i = 0; i < selection->size(); i++)
    {
        setSelectedAtIndex(selection->at(i), true);
    }
}

IUndoState *ItemCollection::currentItems()
{
    vector<RemovedItem *> *removedItems = new vector<RemovedItem *>();
	
	for (unsigned int i = 0; i < items.size(); i++)
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
    
    for (unsigned int i = 0; i < removedItems->size(); i++)
    {
        RemovedItem *removedItem = removedItems->at(i);
        removedItem->insert(*this);
    }
}

IUndoState *ItemCollection::allItems()
{
    vector<Item *> *duplicates = new vector<Item *>();
	
	for (unsigned int i = 0; i < items.size(); i++)
	{
		Item *duplicate = items.at(i)->duplicate();
        duplicates->push_back(duplicate);
	}
	
	return new UndoState<vector<Item *>>(duplicates);
}

void ItemCollection::setAllItems(IUndoState *undoState)
{
    vector<Item *> *duplicates = dynamic_cast<UndoState<vector<Item *>> *>(undoState)->state();
    
    for (unsigned int i = 0; i < items.size(); i++)
        delete items[i];
    
    items.clear();
    
    for (unsigned int i = 0; i < duplicates->size(); i++)
    {
        Item *duplicate = duplicates->at(i)->duplicate();
        items.push_back(duplicate);
    }
}

Item *ItemCollection::itemAtIndex(unsigned int index)
{
    return items.at(index);
}

void ItemCollection::addItem(Item *item)
{
    items.push_back(item);
}

void ItemCollection::removeItem(Item *item)
{
    for (unsigned int i = 0; i < items.size(); i++)
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

void ItemCollection::removeItemAtIndex(unsigned int index)
{
    Item *item = items.at(index);
    delete item;
    items.erase(items.begin() + index);

}

void ItemCollection::removeItemsInRange(unsigned int location, unsigned int length)
{
    for (unsigned int i = location; i < location + length; i++)
        delete items[i];
    
    items.erase(items.begin() + location, items.begin() + location + length);
}

void ItemCollection::insertItemAtIndex(unsigned int index, Item *item)
{
    items.insert(items.begin() + index, item);
}

void ItemCollection::mergeSelectedItems()
{
    Vector3D center = Vector3D();
	unsigned int selectedCount = 0;
	
	for (unsigned int i = 0; i < items.size(); i++)
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
    
    for (unsigned int i = 0; i < removedItems->size(); i++)
    {
        RemovedItem *removedItem = removedItems->at(i);
        removedItem->selectItemForRemove(*this);
    }
}

void ItemCollection::deselectAll()
{
    for (unsigned int i = 0; i < items.size(); i++)
        items[i]->selected = false;
}

void ItemCollection::getVertexAndTriangleCount(unsigned int &vertexCount, unsigned int &triangleCount)
{
    vertexCount = 0;
	triangleCount = 0;
	for (unsigned int i = 0; i < items.size(); i++)
    {
        Mesh2 *mesh = items[i]->mesh;
		vertexCount += mesh->vertexCount();
		triangleCount += mesh->triangleCount();
	}
}

Mesh2 *ItemCollection::currentMesh()
{
    for (unsigned int i = 0; i < items.size(); i++)
    {
        Item *item = items[i];
		if (item->selected)
			return item->mesh;
	}
    return NULL;

}

Item *ItemCollection::firstSelectedItem()
{
    for (unsigned int i = 0; i < items.size(); i++)
    {
        Item *item = items[i];
		if (item->selected)
			return item;
	}
    return NULL;
}

ViewMode ItemCollection::viewMode()
{
    for (unsigned int i = 0; i < items.size(); i++)
    {
        Item *item = items[i];
		if (item->selected)
			return item->viewMode();
	}
    return ViewMode::SolidFlat;
}

void ItemCollection::setViewMode(ViewMode viewMode)
{
    for (unsigned int i = 0; i < items.size(); i++)
    {
        Item *item = items[i];
		if (item->selected)
			item->setViewMode(viewMode);
	}
}

unsigned int ItemCollection::count()
{
    return static_cast<unsigned int>(items.size());
}

bool ItemCollection::isSelectedAtIndex(unsigned int index)
{
    return items.at(index)->selected;
}

void ItemCollection::setSelectedAtIndex(unsigned int index, bool selected)
{
    items.at(index)->selected = selected;
}

void ItemCollection::expandSelectionFromIndex(unsigned int index, bool invert)
{
    for (unsigned int i = 0; i < items.size(); i++)
        setSelectedAtIndex(i, true);
}

void ItemCollection::duplicateSelected()
{
    unsigned int count = static_cast<unsigned int>(items.size());
	for (unsigned int i = 0; i < count; i++)
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
    for (unsigned int i = 0; i < items.size(); i++)
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
    for (unsigned int i = 0; i < items.size(); i++)
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
    for (unsigned int i = 0; i < items.size(); i++)
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

Vector3D ItemCollection::positionAtIndex(unsigned int index)
{
    return items.at(index)->position;
}

Quaternion ItemCollection::rotationAtIndex(unsigned int index)
{
    return items.at(index)->rotation;
}

Vector3D ItemCollection::scaleAtIndex(unsigned int index)
{
    return items.at(index)->scale;
}

void ItemCollection::setPositionAtIndex(unsigned int index, Vector3D position)
{
    items.at(index)->position = position;
}

void ItemCollection::setRotationAtIndex(unsigned int index, Quaternion rotation)
{
    items.at(index)->rotation = rotation;
}

void ItemCollection::setScaleAtIndex(unsigned int index, Vector3D scale)
{
    items.at(index)->scale = scale;
}

void ItemCollection::moveByOffset(unsigned int index, Vector3D offset)
{
    items.at(index)->moveByOffset(offset);
}

void ItemCollection::rotateByOffset(unsigned int index, Quaternion offset)
{
    items.at(index)->rotateByOffset(offset);
}

void ItemCollection::scaleByOffset(unsigned int index, Vector3D offset)
{
    items.at(index)->scaleByOffset(offset);
}

void ItemCollection::drawAtIndex(unsigned int index, bool forSelection)
{
    items.at(index)->drawForSelection(forSelection);
}
