//
//  Manipulator.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/31/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "../PureCpp/MathCore/MathDeclaration.h"
#include "../PureCpp/MathCore/Camera.h"
#include "OpenGLSelecting.h"
#include "ManipulatorWidget.h"

using namespace System;
using namespace System::Collections::Generic;

namespace ManagedCpp
{
	public ref class Manipulator : OpenGLSelecting
	{
	private:
		List<ManipulatorWidget ^> ^widgets;
		Vector3D *position;
		Quaternion *rotation;
		float size;
		int selectedIndex;
	
	public:
		// OpenGLSelecting
		virtual property uint SelectableCount { uint get(); }
		virtual void DrawForSelection(uint index);
		virtual void SelectObject(uint index, OpenGLSelectionMode selectionMode);

		//@optional
		virtual void WillSelect();
		virtual void DidSelect();

		property Vector3D Position { Vector3D get(); void set(Vector3D value); }
		property Quaternion Rotation { Quaternion get(); void set(Quaternion value); }
		property float Size { float get(); void set(float value); }
		property int SelectedIndex { int get(); void set(int value); }

		Manipulator();
		~Manipulator();

		void AddWidget(ManipulatorWidget ^widget);
		void Draw(Vector3D axisZ, Vector3D center);
		ManipulatorWidget ^GetWidget(int index);
	};
}
