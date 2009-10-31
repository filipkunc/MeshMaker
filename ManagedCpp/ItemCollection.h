//
//  ItemCollection.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/25/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "../PureCpp/MathCore/MathDeclaration.h"
#include "Item.h"

using namespace System::Collections::Generic;

namespace ManagedCpp
{
	public ref class ItemCollection : OpenGLManipulatingModelItem
	{
	private:
		List<Item ^> ^items;
	public:
		ItemCollection();

		virtual property uint Count { uint get(); }

		Item ^GetItem(uint index);
		void AddItem(Item ^item);
		void RemoveItem(Item ^item);
		void MergeSelectedItems();

		virtual Vector3D GetPosition(uint index);
		virtual Quaternion GetRotation(uint index);
		virtual Vector3D GetScale(uint index);
		virtual void SetPosition(Vector3D position, uint index);
		virtual void SetRotation(Quaternion rotation, uint index);
		virtual void SetScale(Vector3D scale, uint index);
		virtual void MoveBy(Vector3D offset, uint index);
		virtual void RotateBy(Quaternion offset, uint index);
		virtual void ScaleBy(Vector3D offset, uint index);
		virtual CocoaBool IsSelected(uint index);
		virtual void SetSelected(CocoaBool selected, uint index);
		virtual void Draw(uint index, CocoaBool forSelection, ViewMode mode);
		virtual void WillSelect();
		virtual void DidSelect();
		virtual void CloneSelected();
		virtual void RemoveSelected();
	};
}
