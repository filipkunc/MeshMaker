//
//  ItemCollection.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#import "OpenGLDrawing.h"
#import "ItemCollection.h"
#import "ItemManipulationState.h"

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
    [stream readBytes:&itemsCount length:sizeof(uint)];
    for (uint i = 0; i < itemsCount; i++)
    {
        Item *item = new Item(stream);
        items.push_back(item);
    }

}

void ItemCollection::encode(MemoryWriteStream *stream)
{
    uint itemsCount = items.size();
    [stream writeBytes:&itemsCount length:sizeof(uint)];
	for (uint i = 0; i < itemsCount; i++)
	{
		Item *item = items.at(i);
        item->encode(stream);
	}

}

NSMutableArray *ItemCollection::currentManipulations()
{
    NSMutableArray *manipulations = [[NSMutableArray alloc] init];
	
	for (uint i = 0; i < items.size(); i++)
	{
		Item *item = items.at(i);
		if (item->selected)
		{
			ItemManipulationState *itemState = [[ItemManipulationState alloc] initWithItem:item index:i];
			[manipulations addObject:itemState];
		}
	}
	
	return manipulations;
}

void ItemCollection::setCurrentManipulations(NSMutableArray *manipulations)
{
    deselectAll();
	
	for (ItemManipulationState *manipulation in manipulations)
	{
		Item *item = items.at(manipulation.itemIndex);
		[manipulation applyManipulationToItem:item];
	}
}

MeshState *ItemCollection::currentMeshState()
{
    for (uint i = 0; i < items.size(); i++)
	{
		Item *item = items.at(i);
		if (item->selected)
		{
			MeshState *meshState = [[MeshState alloc] initWithMesh:item->mesh
                                                         itemIndex:i];
			return meshState;
		}
	}
	return nil;
}

void ItemCollection::setCurrentMeshState(MeshState *meshState)
{
    deselectAll();
	
	Item *item = items.at(meshState.itemIndex);
    item->selected = true;
	[meshState applyToMesh:item->mesh];
}

NSMutableArray *ItemCollection::currentSelection()
{
    NSMutableArray *selection = [[NSMutableArray alloc] init];
	
	for (uint i = 0; i < items.size(); i++)
	{
		Item *item = items.at(i);
		if (item->selected)
		{
			NSNumber *number = [[NSNumber alloc] initWithUnsignedInt:i];
			[selection addObject:number];
		}
	}
	
	return selection;
}

void ItemCollection::setCurrentSelection(NSMutableArray *selection)
{
    deselectAll();
	
	for (NSNumber *number in selection)
	{
        setSelectedAtIndex(number.unsignedIntValue, true);
	}
}

NSMutableArray *ItemCollection::currentItems()
{
    NSMutableArray *anItems = [[NSMutableArray alloc] init];
	
	for (uint i = 0; i < items.size(); i++)
	{
		Item *item = items.at(i);
		if (item->selected)
		{
			IndexedItem *indexedItem = [[IndexedItem alloc] initWithIndex:i item:item];
			[anItems addObject:indexedItem];
		}
	}
	
	return anItems;
}

void ItemCollection::setCurrentItems(NSMutableArray *anItems)
{
    deselectAll();
	
	for (IndexedItem *indexedItem in anItems)
	{
        insertItemAtIndex(indexedItem.index, indexedItem.item);
	}
}

NSMutableArray *ItemCollection::allItems()
{
    NSMutableArray *anItems = [[NSMutableArray alloc] init];
	
	for (uint i = 0; i < items.size(); i++)
	{
		Item *duplicate = items.at(i)->duplicate();
		[anItems addObject:[NSValue valueWithPointer:duplicate]];
	}
	
	return anItems;
}

void ItemCollection::setAllItems(NSMutableArray *anItems)
{
    for (uint i = 0; i < items.size(); i++)
        delete items[i];
    
    items.clear();
    
    for (NSValue *value in anItems)
    {
        Item *item = (Item *)value.pointerValue;
        items.push_back(item);
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

void ItemCollection::removeItemsInRange(NSRange range)
{
    for (uint i = range.location; i < range.location + range.length; i++)
        delete items[i];
    
    items.erase(items.begin() + range.location, items.begin() + range.location + range.length);
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

void ItemCollection::setSelectionFromIndexedItems(NSMutableArray *indexedItems)
{
    deselectAll();
	
	for (IndexedItem *indexedItem in indexedItems)
	{
        setSelectedAtIndex(indexedItem.index, true);
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
    return ViewModeSolidFlat;
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

NSColor *ItemCollection::selectionColor()
{
    Item *first = firstSelectedItem();
    if (first)
        return first->selectionColor();
    return nil;
}

void ItemCollection::setSelectionColor(NSColor *color)
{
    for (uint i = 0; i < items.size(); i++)
	{
        Item *item = items[i];
		if (item->selected)
            item->setSelectionColor([color copy]);
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
