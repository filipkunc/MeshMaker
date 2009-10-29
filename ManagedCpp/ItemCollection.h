//
//  ItemCollection.h
//  OpenGLWidgets
//
//  Created by Filip Kunc on 10/25/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "../PureCpp/MathCore/MathDeclaration.h"
#include "Item.h"

using namespace System::Collections::Generic;

namespace ManagedCpp
{
	public ref class ItemCollection
	{
	private:
		List<Item ^> ^items;
	public:
		ItemCollection();

		property uint Count { uint get(); }

		Item ^GetItem(uint index);
		void AddItem(Item ^item);
		void RemoveItem(Item ^item);
		void MergeSelectedItems();

		Vector3D GetPosition(uint index);
		Quaternion GetRotation(uint index);
		Vector3D GetScale(uint index);
		void SetPosition(Vector3D position, uint index);
		void SetRotation(Quaternion rotation, uint index);
		void SetScale(Vector3D scale, uint index);
		void MoveBy(Vector3D offset, uint index);
		void RotateBy(Quaternion offset, uint index);
		void ScaleBy(Vector3D offset, uint index);
		CocoaBool IsSelected(uint index);
		void SetSelected(CocoaBool selected, uint index);
		void Draw(uint index, CocoaBool forSelection, ViewMode mode);

		void CloneSelected();
		void RemoveSelected();
	};
}
