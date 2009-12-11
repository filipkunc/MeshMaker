//
//  OpenGLManipulatingController.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/26/09.
//  For license see LICENSE.TXT
//

#include "OpenGLManipulatingController.h"

namespace ManagedCpp
{
	OpenGLManipulatingController::OpenGLManipulatingController()
	{
		selectionCenter = new Vector3D();
		selectionRotation = new Quaternion();
		selectionEuler = new Vector3D();
		selectionScale = new Vector3D(1, 1, 1);
		selectedCount = 0;
		lastSelectedIndex = -1;
		currentManipulator = ManipulatorType::ManipulatorTypeDefault;
		modelTransform = new Matrix4x4();
		modelPosition = new Vector3D();
		modelRotation = new Quaternion();
		modelScale = new Vector3D(1, 1, 1);
		model = nullptr;
		modelMesh = nullptr;
		modelItem = nullptr;
		
		ModelObserver ^observer = BindingsExtensions::Observe(this);
		observerSelectionX = observer->CreateOrGetObserver<float>("SelectionX");
		observerSelectionY = observer->CreateOrGetObserver<float>("SelectionY");
		observerSelectionZ = observer->CreateOrGetObserver<float>("SelectionZ");
	}

	OpenGLManipulatingController::~OpenGLManipulatingController()
	{
		delete selectionCenter;
		delete selectionRotation;
		delete selectionEuler;
		delete selectionScale;
		delete modelTransform;
		delete modelPosition;
		delete modelRotation;
		delete modelScale;
		model = nullptr;
		modelMesh = nullptr;
		modelItem = nullptr;
		observerSelectionX = nullptr;
		observerSelectionY = nullptr;
		observerSelectionZ = nullptr;

		BindingsExtensions::ClearAll(this);
	}

	PropertyObserver<float> ^OpenGLManipulatingController::ObserverSelectionX::get()
	{
		return observerSelectionX;
	}

	PropertyObserver<float> ^OpenGLManipulatingController::ObserverSelectionY::get()
	{
		return observerSelectionY;
	}

	PropertyObserver<float> ^OpenGLManipulatingController::ObserverSelectionZ::get()
	{
		return observerSelectionZ;
	}

	ManipulatorType OpenGLManipulatingController::CurrentManipulator::get()
	{
		return currentManipulator;
	}

	void OpenGLManipulatingController::CurrentManipulator::set(ManipulatorType value)
	{
		this->WillChangeSelection();
		currentManipulator = value;
		this->DidChangeSelection();
	}

	uint OpenGLManipulatingController::SelectedCount::get()
	{
		return selectedCount;
	}
	
	OpenGLManipulatingModel ^OpenGLManipulatingController::Model::get()
	{
		return model;
	}

	void OpenGLManipulatingController::Model::set(OpenGLManipulatingModel ^value)
	{
		model = value;
		modelMesh = dynamic_cast<OpenGLManipulatingModelMesh ^>(value);
		modelItem = dynamic_cast<OpenGLManipulatingModelItem ^>(value);
	}

	int OpenGLManipulatingController::LastSelectedIndex::get()
	{
		return lastSelectedIndex;
	}

	void OpenGLManipulatingController::SetTransform(Vector3D position, Quaternion rotation, Vector3D scale)
	{
		*modelPosition = position;
		*modelRotation = rotation;
		*modelScale = scale;
		modelTransform->TranslateRotateScale(position, rotation, scale);
	}

	void OpenGLManipulatingController::SetTransform(Item ^item)
	{
		if (item == nullptr)
			SetTransform(Vector3D(), Quaternion(), Vector3D(1, 1, 1));
		else
			SetTransform(item->Position, item->Rotation, item->Scale);
	}

	float OpenGLManipulatingController::SelectionX::get()
	{
		return GetSelectionValue(0);
	}

	float OpenGLManipulatingController::SelectionY::get()
	{
		return GetSelectionValue(1);
	}

	float OpenGLManipulatingController::SelectionZ::get()
	{
		return GetSelectionValue(2);
	}

	void OpenGLManipulatingController::SelectionX::set(float value)
	{
		SetSelectionValue(value, 0);
	}

	void OpenGLManipulatingController::SelectionY::set(float value)
	{
		SetSelectionValue(value, 1);
	}

	void OpenGLManipulatingController::SelectionZ::set(float value)
	{
		SetSelectionValue(value, 2);
	}

	float OpenGLManipulatingController::GetSelectionValue(uint index)
	{
		switch (currentManipulator)
		{
			case ManipulatorType::ManipulatorTypeTranslation:
				return (*selectionCenter)[index];
			case ManipulatorType::ManipulatorTypeRotation:
				return (*selectionEuler)[index] * RAD_TO_DEG;
			case ManipulatorType::ManipulatorTypeScale:
				return (*selectionScale)[index];
			case ManipulatorType::ManipulatorTypeDefault:
			default:
				return 0.0f;
		}
	}

	void OpenGLManipulatingController::SetSelectionValue(float value, uint index)
	{
		switch (currentManipulator)
		{
			case ManipulatorType::ManipulatorTypeTranslation:
			{
				Vector3D offset = Vector3D();
				offset[index] = value - (*selectionCenter)[index];
				this->MoveSelectedBy(offset);
			}break;
			case ManipulatorType::ManipulatorTypeRotation:
			{
				(*selectionEuler)[index] = value * DEG_TO_RAD;
				if (selectedCount == 1)
				{
					selectionRotation->FromEulerAngles(*selectionEuler);
					if (lastSelectedIndex > -1 && modelItem != nullptr)
						modelItem->SetRotation(*selectionRotation, lastSelectedIndex);
				}
				else
				{
					Quaternion offset = Quaternion();
					offset.FromEulerAngles(*selectionEuler);
					offset = offset * selectionRotation->Conjugate();
					this->RotateSelectedBy(offset);
				}
			}break;
			case ManipulatorType::ManipulatorTypeScale:
			{
				if (selectedCount == 1)
				{
					(*selectionScale)[index] = value;
					if (lastSelectedIndex > -1 && modelItem != nullptr)
						modelItem->SetScale(*selectionScale, lastSelectedIndex);
				}
				else
				{
					Vector3D offset = Vector3D();
					offset[index] = value - (*selectionScale)[index];
					this->ScaleSelectedBy(offset);
				}
			}break;
			default:
				break;
		}
	}
	
	void OpenGLManipulatingController::UpdateSelection()
	{
		this->WillChangeSelection();

		if (modelMesh != nullptr)
		{
			modelMesh->GetSelectionCenter(selectionCenter, selectionRotation, selectionScale);
			selectedCount = 0;
			for (uint i = 0; i < model->Count; i++)
			{
				if (model->IsSelected(i))
					selectedCount++;
			}
		}
		else if (modelItem != nullptr)
		{
			*selectionCenter = Vector3D();
			*selectionRotation = Quaternion();
			*selectionScale = Vector3D(1, 1, 1);
			selectedCount = 0;
			lastSelectedIndex = -1;
			for (uint i = 0; i < model->Count; i++)
			{
				if (model->IsSelected(i))
				{
					selectedCount++;
					*selectionCenter += modelItem->GetPosition(i);
					lastSelectedIndex = i;
				}
			}
			if (selectedCount > 0)
			{
				*selectionCenter /= (float)selectedCount;
				if (selectedCount == 1 && lastSelectedIndex > -1)
				{
					*selectionRotation = modelItem->GetRotation(lastSelectedIndex); 
					*selectionScale = modelItem->GetScale(lastSelectedIndex); 
				}
			}
			else
			{
				*selectionCenter = Vector3D();
			}
		}
		selectionRotation->ToEulerAngles(*selectionEuler);
		selectionCenter->Transform(*modelTransform);
		this->DidChangeSelection();
	}

	void OpenGLManipulatingController::WillChangeSelection()
	{
		observerSelectionX->RaiseWillChange();
		observerSelectionY->RaiseWillChange();
		observerSelectionZ->RaiseWillChange();
	}
	
	void OpenGLManipulatingController::DidChangeSelection()
	{
		observerSelectionX->RaiseDidChange();
		observerSelectionY->RaiseDidChange();
		observerSelectionZ->RaiseDidChange();
	}

	Vector3D OpenGLManipulatingController::SelectionCenter::get()
	{
		return *selectionCenter;
	}

	void OpenGLManipulatingController::SelectionCenter::set(Vector3D value)
	{
		this->WillChangeSelection();
		*selectionCenter = value;
		this->DidChangeSelection();
	}

	Quaternion OpenGLManipulatingController::SelectionRotation::get()
	{
		return *selectionRotation;
	}

	void OpenGLManipulatingController::SelectionRotation::set(Quaternion value)
	{
		this->WillChangeSelection();
		*selectionRotation = value;
		selectionRotation->ToEulerAngles(*selectionEuler);
		this->DidChangeSelection();
	}

	Vector3D OpenGLManipulatingController::SelectionScale::get()
	{
		return *selectionScale;
	}

	void OpenGLManipulatingController::SelectionScale::set(Vector3D value)
	{
		this->WillChangeSelection();
		*selectionScale = value;
		this->DidChangeSelection();
	}

	void OpenGLManipulatingController::MoveSelectedBy(Vector3D offset)
	{
		Vector3D transformedOffset = offset;
		Matrix4x4 m, r, s;
		Quaternion inverseRotation = modelRotation->Conjugate();
		
		Vector3D inverseScale = *modelScale;
		for (int i = 0; i < 3; i++)
			inverseScale[i] = 1.0f / inverseScale[i];
		
		inverseRotation.ToMatrix(r);
		s.Scale(inverseScale);
		m = s * r;
		transformedOffset.Transform(m);
		
		if (modelMesh != nullptr)
		{
			modelMesh->MoveSelectedBy(transformedOffset);
		}
		else if (modelItem != nullptr)
		{
			for (uint i = 0; i < model->Count; i++)
			{
				if (model->IsSelected(i))
					modelItem->MoveBy(offset, i);
			}
		}
	
		this->SelectionCenter = *selectionCenter + offset;
	}

	void OpenGLManipulatingController::RotateSelectedBy(Quaternion offset)
	{
		if (modelMesh != nullptr)
		{
			Vector3D rotationCenter = *selectionCenter;
			rotationCenter.Transform(modelTransform->Inverse());
			modelMesh->MoveSelectedBy(-rotationCenter);
			modelMesh->RotateSelectedBy(offset);
			modelMesh->MoveSelectedBy(rotationCenter);
			this->SelectionRotation = offset * (*selectionRotation);
		}
		else if (modelItem != nullptr)
		{
			if (this->SelectedCount > 1)
			{
				Vector3D rotationCenter = *selectionCenter;
				rotationCenter.Transform(modelTransform->Inverse());
				for (uint i = 0; i < model->Count; i++)
				{
					if (model->IsSelected(i))
					{
						Vector3D itemPosition = modelItem->GetPosition(i);
						itemPosition -= rotationCenter;
						itemPosition.Transform(offset);
						itemPosition += rotationCenter;
						modelItem->SetPosition(itemPosition, i);
						modelItem->RotateBy(offset, i);
					}
				}
				this->SelectionRotation = offset * (*selectionRotation);
			}
			else if (lastSelectedIndex > -1)
			{
				modelItem->RotateBy(offset, lastSelectedIndex);
				this->SelectionRotation = modelItem->GetRotation(lastSelectedIndex);
			}		
		}
	}

	void OpenGLManipulatingController::ScaleSelectedBy(Vector3D offset)
	{
		if (modelMesh != nullptr)
		{
			Vector3D rotationCenter = *selectionCenter;
			rotationCenter.Transform(modelTransform->Inverse());
			modelMesh->MoveSelectedBy(-rotationCenter);
			modelMesh->ScaleSelectedBy(offset + Vector3D(1, 1, 1));
			modelMesh->MoveSelectedBy(rotationCenter);
		}
		else if (modelItem != nullptr)
		{
			if (this->SelectedCount > 1)
			{
				Vector3D rotationCenter = *selectionCenter;
				rotationCenter.Transform(modelTransform->Inverse());
				for (uint i = 0; i < model->Count; i++)
				{
					if (model->IsSelected(i))
					{
						Vector3D itemPosition = modelItem->GetPosition(i);
						itemPosition -= rotationCenter;
						itemPosition.x *= 1.0f + offset.x;
						itemPosition.y *= 1.0f + offset.y;
						itemPosition.z *= 1.0f + offset.z;
						itemPosition += rotationCenter;
						modelItem->SetPosition(itemPosition, i);
						modelItem->ScaleBy(offset, i);
					}
				}
			}
			else if (lastSelectedIndex > -1)
			{
				modelItem->ScaleBy(offset, lastSelectedIndex);
			}
		}
		this->SelectionScale = *selectionScale + offset;
	}

	void OpenGLManipulatingController::Draw(ViewMode mode)
	{
		glPushMatrix();
		glMultMatrixf(modelTransform->m);
		for (uint i = 0; i < model->Count; i++)
		{
			model->Draw(i, NO, mode);
		}
		glPopMatrix();
	}

	void OpenGLManipulatingController::WillSelect()
	{
		/*id aModel = model;
		if ([aModel respondsToSelector:@selector(willSelect)])
		{
			[model willSelect];
		}*/
		model->WillSelect();
	}

	void OpenGLManipulatingController::DidSelect()
	{
		/*id aModel = model;
		if ([aModel respondsToSelector:@selector(didSelect)])
		{
			[model didSelect];
		}*/
		model->DidSelect();
		this->UpdateSelection();
	}

	uint OpenGLManipulatingController::SelectableCount::get()
	{
		return model->Count;
	}

	void OpenGLManipulatingController::DrawForSelection(uint index)
	{
		glPushMatrix();
		glMultMatrixf(modelTransform->m);
		model->Draw(index, YES, ViewMode::ViewModeSolid);
		glPopMatrix();
	}

	void OpenGLManipulatingController::SelectObject(uint index, OpenGLSelectionMode selectionMode)
	{
		switch (selectionMode) 
		{
			case OpenGLSelectionModeAdd:
				model->SetSelected(YES, index);
				break;
			case OpenGLSelectionModeSubtract:
				model->SetSelected(NO, index);
				break;
			case OpenGLSelectionModeInvert:
				model->SetSelected(!model->IsSelected(index), index);
				break;
			default:
				break;
		}
	}

	void OpenGLManipulatingController::ChangeSelection(CocoaBool isSelected)
	{
		this->WillSelect();
		for (uint i = 0; i < model->Count; i++)
			model->SetSelected(isSelected, i);
		this->DidSelect();
		this->UpdateSelection();
	}

	void OpenGLManipulatingController::InvertSelection()
	{
		this->WillSelect();
		for (uint i = 0; i < model->Count; i++)
			model->SetSelected(!model->IsSelected(i), i);
		this->DidSelect();
		this->UpdateSelection();
	}

	void OpenGLManipulatingController::CloneSelected()
	{
		model->CloneSelected();
		this->UpdateSelection();
	}

	void OpenGLManipulatingController::RemoveSelected()
	{
		model->RemoveSelected();
		this->UpdateSelection();
	}
}
