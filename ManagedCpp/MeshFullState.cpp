//
//  MeshFullState.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/12/09.
//  For license see LICENSE.TXT
//

#include "MeshFullState.h"

namespace ManagedCpp
{
	MeshFullState::MeshFullState(Mesh ^mesh, uint index)
	{
		itemIndex = index;
		vertices = new vector<Vector3D>(*mesh->vertices);
		triangles = new vector<Triangle>(*mesh->triangles);
		selected = new vector<CocoaBool>(*mesh->selected);
		selectionMode = mesh->SelectionMode;
	}

	MeshFullState::~MeshFullState()
	{
		delete vertices;
		delete triangles;
		delete selected;
	}

	uint MeshFullState::ItemIndex::get()
	{
		return itemIndex;
	}

	void MeshFullState::ItemIndex::set(uint value)
	{
		itemIndex = value;
	}

	void MeshFullState::ApplyFullToMesh(Mesh ^mesh)
	{
		*(mesh->vertices) = *vertices;
		*(mesh->triangles) = *triangles;
		*(mesh->selected) = *selected;
		mesh->selectionMode = selectionMode;
		if (selectionMode == MeshSelectionMode::MeshSelectionModeEdges)
			mesh->MakeEdges();
		mesh->MakeMarkedVertices();
	}
}