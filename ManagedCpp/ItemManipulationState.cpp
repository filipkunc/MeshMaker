//
//  ItemManipulationState.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/12/09.
//  For license see LICENSE.TXT
//

#include "ItemManipulationState.h"

namespace ManagedCpp
{
	ItemManipulationState::ItemManipulationState(Item ^item, uint index)
	{
		position = new Vector3D(item->Position);
		rotation = new Quaternion(item->Rotation);
		scale = new Vector3D(item->Scale);
		itemIndex = index;
	}

	ItemManipulationState::~ItemManipulationState()
	{
		delete position;
		delete rotation;
		delete scale;
	}

	uint ItemManipulationState::ItemIndex::get()
	{
		return itemIndex;
	}

	void ItemManipulationState::ApplyManipulationToItem(Item ^item)
	{
		item->Position = *position;
		item->Rotation = *rotation;
		item->Scale = *scale;
		item->Selected = YES;
	}
}