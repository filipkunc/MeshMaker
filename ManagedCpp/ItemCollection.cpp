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
		for (int i = 0; i < this->Count; i++)
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
		int selectedCount = 0;
		
		for (int i = 0; i < this->Count; i++)
		{
			if (this->IsSelected(i))
			{
				selectedCount++;
				center += GetItem(i)->Position;
			}
		}
		
		if (selectedCount < 2)
			return;
		
		center /= selectedCount;
		
		Item ^newItem = gcnew Item(center, Quaternion(), Vector3D(1, 1, 1));
		Mesh ^mesh = newItem->GetMesh();
		
		Matrix4x4 firstMatrix, itemMatrix;
		
		firstMatrix.TranslateRotateScale(newItem->Position,
										 newItem->Rotation,
										 newItem->Scale);
										 
		firstMatrix = firstMatrix.Inverse();
		
		for (int i = 0; i < this->Count; i++)
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
}
