//
//  Mesh2.make.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#include "Mesh2.h"

void Mesh2::makeCube()
{
    _vertices->removeAll();
	_triangles->removeAll();
	
	// back vertices
	VertexNode *v0 = _vertices->add(Vector3D(-1, -1, -1)); // 0
	VertexNode *v1 = _vertices->add(Vector3D( 1, -1, -1)); // 1
    VertexNode *v2 = _vertices->add(Vector3D( 1,  1, -1)); // 2
	VertexNode *v3 = _vertices->add(Vector3D(-1,  1, -1)); // 3
	
	// front vertices
	VertexNode *v4 = _vertices->add(Vector3D(-1, -1,  1)); // 4
	VertexNode *v5 = _vertices->add(Vector3D( 1, -1,  1)); // 5
	VertexNode *v6 = _vertices->add(Vector3D( 1,  1,  1)); // 6
	VertexNode *v7 = _vertices->add(Vector3D(-1,  1,  1)); // 7
	
	// back triangles
    addQuad(v0, v1, v2, v3);
	
	// front triangles
    addQuad(v7, v6, v5, v4);
	
	// bottom triangles
    addQuad(v1, v0, v4, v5);
	
	// top triangles
    addQuad(v3, v2, v6, v7);
	
	// left triangles
    addQuad(v7, v4, v0, v3);
	
	// right triangles
    addQuad(v2, v1, v5, v6);
	
    setSelectionMode(_selectionMode);
	//[self setSelectionMode:[self selectionMode]];
}

