//
//  MeshManipulationState.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/12/09.
//  For license see LICENSE.TXT
//

#include "MeshManipulationState.h"

namespace ManagedCpp
{
	MeshManipulationState::MeshManipulationState(Mesh ^mesh, uint index)
	{
		itemIndex = index;
		vertices = new vector<Vector3D>(*mesh->vertices);
		selected = new vector<CocoaBool>(*mesh->selected);
		selectionMode = mesh->SelectionMode;
	}

	MeshManipulationState::~MeshManipulationState()
	{
		delete vertices;
		delete selected;
	}

	uint MeshManipulationState::ItemIndex::get()
	{
		return itemIndex;
	}

	void MeshManipulationState::ItemIndex::set(uint value)
	{
		itemIndex = value;
	}

	void MeshManipulationState::ApplyManipulationToMesh(Mesh ^mesh)
	{
		*(mesh->vertices) = *vertices;
		*(mesh->selected) = *selected;
		mesh->selectionMode = selectionMode;
		mesh->MakeMarkedVertices();
	}
}