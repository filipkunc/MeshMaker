/*
 *  OpenGLManipulating.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 8/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "OpenGLSelecting.h"

namespace ManagedCpp
{
	public interface class OpenGLManipulating : OpenGLSelecting
	{
		property ManipulatorType CurrentManipulator { ManipulatorType get(); void set(ManipulatorType value); }
		property Vector3D SelectionCenter { Vector3D get(); void set(Vector3D value); }
		property Quaternion SelectionRotation { Quaternion get(); void set(Quaternion value); }
		property Vector3D SelectionScale { Vector3D get(); void set(Vector3D value); }
		property float SelectionX { float get(); void set(float value); }
		property float SelectionY { float get(); void set(float value); }
		property float SelectionZ { float get(); void set(float value); }
		property uint SelectedCount { uint get(); }

		void MoveSelectedBy(Vector3D offset);
		void RotateSelectedBy(Quaternion offset);
		void ScaleSelectedBy(Vector3D offset);
		void UpdateSelection();
		void Draw(ViewMode mode);
		void ChangeSelection(CocoaBool isSelected);
		void InvertSelection();
		void CloneSelected();
		void RemoveSelected();
	};
}