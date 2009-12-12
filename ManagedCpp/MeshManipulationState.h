//
//  MeshManipulationState.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/12/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "../PureCpp/MathCore/MathDeclaration.h"
#include "Mesh.h"

namespace ManagedCpp
{
	public ref class MeshManipulationState
	{
	private:
		uint itemIndex;
		vector<Vector3D> *vertices;
		vector<CocoaBool> *selected;
		MeshSelectionMode selectionMode;
	public:
		property uint ItemIndex { uint get(); void set(uint value); }
		MeshManipulationState(Mesh ^mesh, uint index);
		~MeshManipulationState();

		void ApplyManipulationToMesh(Mesh ^mesh);
	};
}