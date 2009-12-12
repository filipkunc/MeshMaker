//
//  IndexedItem.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/12/09.
//  For license see LICENSE.TXT
//

#include "IndexedItem.h"

namespace ManagedCpp
{
	IndexedItem::IndexedItem(uint index, Item ^item)
	{
		this->index = index;
		this->item = item;
	}

	IndexedItem::~IndexedItem()
	{

	}

	uint IndexedItem::Index::get()
	{
		return index;
	}

	Item ^IndexedItem::GetItem()
	{
		return item;
	}
}