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

		[Browsable(false)]
		virtual property uint SelectableCount { uint get(); }
		virtual void DrawForSelection(uint index);
		virtual void SelectObject(uint index, OpenGLSelectionMode selectionMode);

		//@optional
		virtual void WillSelect();
		virtual void DidSelect();

		// OpenGLManipulating

		[Browsable(false)]
		virtual property ManipulatorType CurrentManipulator { ManipulatorType get(); void set(ManipulatorType value); }
		[Browsable(false)]
		virtual property Vector3D SelectionCenter { Vector3D get(); void set(Vector3D value); }
		[Browsable(false)]
		virtual property Quaternion SelectionRotation { Quaternion get(); void set(Quaternion value); }
		[Browsable(false)]
		virtual property Vector3D SelectionScale { Vector3D get(); void set(Vector3D value); }
		[Browsable(false)]
		virtual property float SelectionX { float get(); void set(float value); }
		[Browsable(false)]
		virtual property float SelectionY { float get(); void set(float value); }
		[Browsable(false)]
		virtual property float SelectionZ { float get(); void set(float value); }
		[Browsable(false)]
		virtual property uint SelectedCount { uint get(); }

		[Browsable(true)]
		property float PositionX { float get(); void set(float value); }
		[Browsable(true)]
		property float PositionY { float get(); void set(float value); }
		[Browsable(true)]
		property float PositionZ { float get(); void set(float value); }

		[Browsable(true)]
		property float RotationX { float get(); void set(float value); }
		[Browsable(true)]
		property float RotationY { float get(); void set(float value); }
		[Browsable(true)]
		property float RotationZ { float get(); void set(float value); }

		[Browsable(true)]
		property float ScaleX { float get(); void set(float value); }
		[Browsable(true)]
		property float ScaleY { float get(); void set(float value); }
		[Browsable(true)]
		property float ScaleZ { float get(); void set(float value); }

		virtual void MoveSelectedBy(Vector3D offset);
		virtual void RotateSelectedBy(Quaternion offset);
		virtual void ScaleSelectedBy(Vector3D offset);
		virtual void UpdateSelection();
		virtual void Draw(ViewMode mode);
		virtual void ChangeSelection(CocoaBool isSelected);
		virtual void InvertSelection();
		virtual void CloneSelected();
		virtual void RemoveSelected();

		[Browsable(false)]
		property OpenGLManipulatingModel ^Model { OpenGLManipulatingModel ^get(); 
												  void set(OpenGLManipulatingModel ^value); }
		[Browsable(false)]
		property int LastSelectedIndex { int get(); }

		[Browsable(false)]
		property PropertyObserver<float> ^ObserverSelectionX { PropertyObserver<float> ^get(); }
		[Browsable(false)]
		property PropertyObserver<float> ^ObserverSelectionY { PropertyObserver<float> ^get(); }
		[Browsable(false)]
		property PropertyObserver<float> ^ObserverSelectionZ { PropertyObserver<float> ^get(); }

		float GetSelectionValue(uint index, ManipulatorType manipulatorType);
		void SetSelectionValue(float value, uint index, ManipulatorType manipulatorType);
		void WillChangeSelection();
		void DidChangeSelection();
		void SetTransform(Vector3D position, Quaternion rotation, Vector3D scale);

		// needed for C#, can't consume native types
		void SetTransform(Item ^item);
	};
}
