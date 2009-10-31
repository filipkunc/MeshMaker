//
//  OpenGLManipulatingController.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/25/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "../PureCpp/MathCore/MathDeclaration.h"
#include "OpenGLManipulating.h"

namespace ManagedCpp
{
	public interface class OpenGLManipulatingModel
	{
		property uint Count { uint get(); }
		CocoaBool IsSelected(uint index);
		void SetSelected(CocoaBool selected, uint index);
		void Draw(uint index, CocoaBool forSelection, ViewMode mode);
		void CloneSelected();
		void RemoveSelected();
	
		//@optional
		void WillSelect();
		void DidSelect();
	};

	public interface class OpenGLManipulatingModelMesh : OpenGLManipulatingModel
	{
		void GetSelectionCenter(Vector3D *center,
								Quaternion *rotation,
								Vector3D *scale);
		
		void MoveSelectedBy(Vector3D offset);
		void RotateSelectedBy(Quaternion offset);
		void ScaleSelectedBy(Vector3D offset);
	};

	public interface class OpenGLManipulatingModelItem : OpenGLManipulatingModel
	{
		//@optional // better for Item-like objects
		Vector3D GetPosition(uint index);
		Quaternion GetRotation(uint index);
		Vector3D GetScale(uint index);
		void SetPosition(Vector3D position, uint index);
		void SetRotation(Quaternion rotation, uint index);
		void SetScale(Vector3D scale, uint index);
		void MoveBy(Vector3D offset, uint index);
		void RotateBy(Quaternion offset, uint index);
		void ScaleBy(Vector3D offset, uint index);
	};
}