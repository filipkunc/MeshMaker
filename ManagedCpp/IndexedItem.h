//
//  IndexedItem.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/12/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "Item.h"

namespace ManagedCpp
{
	public ref class IndexedItem
	{
	private:
		uint index;
		Item ^item;
	public:
		IndexedItem(uint index, Item ^item);
		~IndexedItem();
	
		property uint Index { uint get(); }
		Item ^GetItem();
	};
}