//
//  OpenGLManipulatingController.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/26/09.
//  For license see LICENSE.TXT
//

#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../PureCpp/MathCore/MathDeclaration.h"
#include "OpenGLManipulating.h"
#include "OpenGLManipulatingModel.h"
#include "Item.h"

using namespace HotChocolate::Bindings;

namespace ManagedCpp
{
	public ref class OpenGLManipulatingController : OpenGLManipulating
	{
	private:
		OpenGLManipulatingModel ^model;
		OpenGLManipulatingModelMesh ^modelMesh;
		OpenGLManipulatingModelItem ^modelItem;
		Vector3D *selectionCenter;
		Quaternion *selectionRotation;
		Vector3D *selectionEuler;
		Vector3D *selectionScale;
		uint selectedCount;
		int lastSelectedIndex;
		ManipulatorType currentManipulator;
		
		Matrix4x4 *modelTransform;
		Vector3D *modelPosition;
		Quaternion *modelRotation;
		Vector3D *modelScale;

		PropertyObserver<float> ^observerSelectionX;
		PropertyObserver<float> ^observerSelectionY;
		PropertyObserver<float> ^observerSelectionZ;
	public:
		OpenGLManipulatingController();
		~OpenGLManipulatingController();

		// OpenGLSelecting

		virtual property uint SelectableCount { uint get(); }
		virtual void DrawForSelection(uint index);
		virtual void SelectObject(uint index, OpenGLSelectionMode selectionMode);

		//@optional
		virtual void WillSelect();
		virtual void DidSelect();

		// OpenGLManipulating

		virtual property ManipulatorType CurrentManipulator { ManipulatorType get(); void set(ManipulatorType value); }
		virtual property Vector3D SelectionCenter { Vector3D get(); void set(Vector3D value); }
		virtual property Quaternion SelectionRotation { Quaternion get(); void set(Quaternion value); }
		virtual property Vector3D SelectionScale { Vector3D get(); void set(Vector3D value); }
		virtual property float SelectionX { float get(); void set(float value); }
		virtual property float SelectionY { float get(); void set(float value); }
		virtual property float SelectionZ { float get(); void set(float value); }
		virtual property uint SelectedCount { uint get(); }

		virtual void MoveSelectedBy(Vector3D offset);
		virtual void RotateSelectedBy(Quaternion offset);
		virtual void ScaleSelectedBy(Vector3D offset);
		virtual void UpdateSelection();
		virtual void Draw(ViewMode mode);
		virtual void ChangeSelection(CocoaBool isSelected);
		virtual void InvertSelection();
		virtual void CloneSelected();
		virtual void RemoveSelected();

		property OpenGLManipulatingModel ^Model { OpenGLManipulatingModel ^get(); 
												  void set(OpenGLManipulatingModel ^value); }
		property int LastSelectedIndex { int get(); }

		property PropertyObserver<float> ^ObserverSelectionX { PropertyObserver<float> ^get(); }
		property PropertyObserver<float> ^ObserverSelectionY { PropertyObserver<float> ^get(); }
		property PropertyObserver<float> ^ObserverSelectionZ { PropertyObserver<float> ^get(); }

		float GetSelectionValue(uint index);
		void SetSelectionValue(float value, uint index);
		void WillChangeSelection();
		void DidChangeSelection();
		void SetTransform(Vector3D position, Quaternion rotation, Vector3D scale);

		// needed for C#, can't consume native types
		void SetTransform(Item ^item);
	};
}
