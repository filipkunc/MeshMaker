//
//  ItemCollection.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/25/09.
//  For license see LICENSE.TXT
//

#include "ItemCollection.h"

namespace ManagedCpp
{
	ItemCollection::ItemCollection()
	{
		items = gcnew List<Item ^>();
	}
	
	Item ^ItemCollection::GetItem(uint index)
	{
		return items[index];
	}

	void ItemCollection::AddItem(Item ^item)
	{
		items->Add(item);
	}

	void ItemCollection::RemoveItem(Item ^item)
	{
		items->Remove(item);
	}

	void ItemCollection::RemoveAt(int index)
	{
		items->RemoveAt(index);
	}

	void ItemCollection::RemoveRange(int index, int count)
	{
		items->RemoveRange(index, count);
	}

	uint ItemCollection::Count::get()
	{
		return items->Count;
	}

	Vector3D ItemCollection::GetPosition(uint index)
	{
		return GetItem(index)->Position;
	}

	Quaternion ItemCollection::GetRotation(uint index)
	{
		return GetItem(index)->Rotation;
	}

	Vector3D ItemCollection::GetScale(uint index)
	{
		return GetItem(index)->Scale;
	}

	void ItemCollection::SetPosition(Vector3D position, uint index)
	{
		GetItem(index)->Position = position;
	}

	void ItemCollection::SetRotation(Quaternion rotation, uint index)
	{
		GetItem(index)->Rotation = rotation;
	}

	void ItemCollection::SetScale(Vector3D scale, uint index)
	{
		GetItem(index)->Scale = scale;
	}

	void ItemCollection::MoveBy(Vector3D offset, uint index)
	{
		GetItem(index)->MoveBy(offset);
	}

	void ItemCollection::RotateBy(Quaternion offset, uint index)
	{
		GetItem(index)->RotateBy(offset);
	}

	void ItemCollection::ScaleBy(Vector3D offset, uint index)
	{
		GetItem(index)->ScaleBy(offset);
	}

	CocoaBool ItemCollection::IsSelected(uint index)
	{
		return GetItem(index)->Selected;
	}

	void ItemCollection::SetSelected(CocoaBool selected, uint index)
	{
		GetItem(index)->Selected = selected;
	}

	void ItemCollection::Draw(uint index, CocoaBool forSelection, ViewMode mode)
	{
		GetItem(index)->Draw(mode);
	
	}

	void ItemCollection::WillSelect()
	{

	}

	void ItemCollection::DidSelect()
	{

	}

	void ItemCollection::CloneSelected()
	{
		int count = this->Count;
		for (int i = 0; i < count; i++)
		{
			if (this->IsSelected(i))
			{
				Item ^newItem = GetItem(i)->Clone();
				items->Add(newItem);
			}
		}
	}

	void ItemCollection::RemoveSelected()
	{
		for (int i = 0; i < (int)this->Count; i++)
		{
			if (this->IsSelected(i))
			{
				items->RemoveAt(i);
				i--;
			}
		}
	}

	void ItemCollection::MergeSelectedItems()
	{
		Vector3D center = Vector3D();
		uint selectedCount = 0;
		
		for (uint i = 0; i < this->Count; i++)
		{
			if (this->IsSelected(i))
			{
				selectedCount++;
				center += GetItem(i)->Position;
			}
		}
		
		if (selectedCount < 2)
			return;
		
		center /= (float)selectedCount;
		
		Item ^newItem = gcnew Item(center, Quaternion(), Vector3D(1, 1, 1));
		Mesh ^mesh = newItem->GetMesh();
		
		Matrix4x4 firstMatrix, itemMatrix;
		
		firstMatrix.TranslateRotateScale(newItem->Position,
										 newItem->Rotation,
										 newItem->Scale);
										 
		firstMatrix = firstMatrix.Inverse();
		
		for (int i = 0; i < (int)this->Count; i++)
		{
			if (this->IsSelected(i))
			{
				Item ^item = GetItem(i);
				Vector3D scale = item->Scale;
				
				itemMatrix.TranslateRotateScale(item->Position,
												item->Rotation,
												scale);
				
				Matrix4x4 finalMatrix = firstMatrix * itemMatrix;
				Mesh ^itemMesh = item->GetMesh();
				
				itemMesh->Transform(finalMatrix);
				
				// mirror detection, some component of scale is negative
				if (scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f)
					itemMesh->FlipAllTriangles();
					
				mesh->Merge(itemMesh);
				items->RemoveAt(i);
				i--;
			}
		}
		
		newItem->Selected = YES;
		this->AddItem(newItem);
	}

	List<ItemManipulationState ^> ^ItemCollection::CurrentManipulations::get()
	{
		List<ItemManipulationState ^> ^manipulations = gcnew List<ItemManipulationState ^>();

		for (uint i = 0; i < this->Count; i++)
		{
			Item ^item = this->GetItem(i);
			if (item->Selected)
			{
				ItemManipulationState ^itemState = gcnew ItemManipulationState(item, i);
				manipulations->Add(itemState);
			}
		}

		return manipulations;
	}

	void ItemCollection::CurrentManipulations::set(List<ItemManipulationState ^> ^manipulations)
	{
		this->DeselectAll();

		for each (ItemManipulationState ^manipulation in manipulations)
		{
			Item ^item = this->GetItem(manipulation->ItemIndex);
			manipulation->ApplyManipulationToItem(item);
		}
	}

	MeshManipulationState ^ItemCollection::CurrentMeshManipulation::get()
	{
		for (uint i = 0; i < this->Count; i++)
		{
			Item ^item = this->GetItem(i);
			if (item->Selected)
				return gcnew MeshManipulationState(item->GetMesh(), i);
		}
		return nullptr;
	}

	void ItemCollection::CurrentMeshManipulation::set(MeshManipulationState ^manipulation)
	{
		this->DeselectAll();

		Item ^item = this->GetItem(manipulation->ItemIndex);
		item->Selected = YES;
		manipulation->ApplyManipulationToMesh(item->GetMesh());
	}

	MeshFullState ^ItemCollection::CurrentMeshFull::get()
	{
		for (uint i = 0; i < this->Count; i++)
		{
			Item ^item = this->GetItem(i);
			if (item->Selected)
				return gcnew MeshFullState(item->GetMesh(), i);
		}
		return nullptr;
	}

	void ItemCollection::CurrentMeshFull::set(MeshFullState ^full)
	{
		this->DeselectAll();

		Item ^item = this->GetItem(full->ItemIndex);
		item->Selected = YES;
		full->ApplyFullToMesh(item->GetMesh());
	}

	List<uint> ^ItemCollection::CurrentSelection::get()
	{
		List<uint> ^selection = gcnew List<uint>();

		for (uint i = 0; i < this->Count; i++)
		{
			Item ^item = this->GetItem(i);
			if (item->Selected)
				selection->Add(i);
		}

		return selection;
	}

	void ItemCollection::CurrentSelection::set(List<uint> ^selection)
	{
		this->DeselectAll();

		for each (uint index in selection)
		{
			this->SetSelected(YES, index);
		}
	}

	List<IndexedItem ^> ^ItemCollection::CurrentItems::get()
	{
		List<IndexedItem ^> ^anItems = gcnew List<IndexedItem ^>();

		for (uint i = 0; i < this->Count; i++)
		{
			Item ^item = this->GetItem(i);
			if (item->Selected)
			{
				IndexedItem ^indexedItem = gcnew IndexedItem(i, item);
				anItems->Add(indexedItem);
			}
		}
		
		return anItems;
	}

	void ItemCollection::CurrentItems::set(List<IndexedItem ^> ^anItems)
	{
		this->DeselectAll();
		
		for each (IndexedItem ^indexedItem in anItems)
		{
			items->Insert(indexedItem->Index, indexedItem->GetItem());
		}
	}

	List<Item ^> ^ItemCollection::AllItems::get()
	{
		List<Item ^> ^anItems = gcnew List<Item ^>();
		
		for (uint i = 0; i < this->Count; i++)
		{
			Item ^clone = this->GetItem(i)->Clone();
			anItems->Add(clone);
		}

		return anItems;
	}

	void ItemCollection::AllItems::set(List<Item ^> ^anItems)
	{
		items = anItems;
	}

	void ItemCollection::SetSelectionFromIndexedItems(List<IndexedItem ^> ^anItems)
	{
		this->DeselectAll();
		
		for each(IndexedItem ^indexedItem in anItems)
		{
			this->SetSelected(YES, indexedItem->Index);
		}
	}
	
	void ItemCollection::DeselectAll()
	{
		for (uint i = 0; i < this->Count; i++)
			this->SetSelected(NO, i);
	}
}
