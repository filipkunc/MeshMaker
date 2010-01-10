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
#include "ItemManipulationState.h"
#include "MeshManipulationState.h"
#include "MeshFullState.h"
#include "IndexedItem.h"
#include "CppFileStreaming.h"

using namespace System::Collections::Generic;
using namespace System::ComponentModel;

namespace ManagedCpp
{
	public ref class ItemCollection : OpenGLManipulatingModelItem, CppFileStreaming
	{
	private:
		List<Item ^> ^items;
	public:
		ItemCollection();

		virtual property uint Count { uint get(); }

		Item ^GetItem(uint index);
		void AddItem(Item ^item);
		void RemoveItem(Item ^item);
		void RemoveAt(int index);
		void RemoveRange(int index, int count);
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

		[Browsable(false)]
		property List<ItemManipulationState ^> ^CurrentManipulations 
		{ 
			List<ItemManipulationState ^> ^get();
			void set(List<ItemManipulationState ^> ^manipulations);
		}

		[Browsable(false)]
		property MeshManipulationState ^CurrentMeshManipulation
		{
			MeshManipulationState ^get();
			void set(MeshManipulationState ^manipulation);
		}

		[Browsable(false)]
		property MeshFullState ^CurrentMeshFull
		{
			MeshFullState ^get();
			void set(MeshFullState ^full);
		}

		[Browsable(false)]
		property List<uint> ^CurrentSelection
		{
			List<uint> ^get();
			void set(List<uint> ^selection);
		}

		[Browsable(false)]
		property List<IndexedItem ^> ^CurrentItems
		{
			List<IndexedItem ^> ^get();
			void set(List<IndexedItem ^> ^anItems);
		}

		[Browsable(false)]
		property List<Item ^> ^AllItems
		{
			List<Item ^> ^get();
			void set(List<Item ^> ^anItems);
		}

		void SetSelectionFromIndexedItems(List<IndexedItem ^> ^anItems);

		void DeselectAll();

		virtual void Decode(ifstream *fin);
		virtual void Encode(ofstream *fout);

		bool ReadFromFile(String ^fileName);
		void WriteToFile(String ^fileName);
	};
}
