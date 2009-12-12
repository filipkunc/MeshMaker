//
//  ItemManipulationState.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/12/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "../PureCpp/MathCore/MathDeclaration.h"
#include "Item.h"

using namespace System::Collections::Generic;

namespace ManagedCpp
{
	public ref class ItemManipulationState
	{
	private:
		uint itemIndex;
		Vector3D *position;
		Quaternion *rotation;
		Vector3D *scale;
	public:
		property uint ItemIndex { uint get(); }
		
		ItemManipulationState(Item ^item, uint index);
		~ItemManipulationState();

		void ApplyManipulationToItem(Item ^item);
	};
}
