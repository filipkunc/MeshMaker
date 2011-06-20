//
//  Mesh2.make.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#include "Mesh2.h"

void Mesh2::addQuad(VertexNode *v1, VertexNode *v2, VertexNode *v3, VertexNode *v4)
{
    VertexNode *vertices1[3] = { v1, v2, v3 };
    VertexNode *vertices2[3] = { v1, v3, v4 };
    
    Triangle2 triangle1(vertices1);
    Triangle2 triangle2(vertices2);
  	_triangles->add(triangle1);
    _triangles->add(triangle2);
}

EdgeNode *Mesh2::findOrCreateEdge(VertexNode *v1, VertexNode *v2, TriangleNode *triangle)
{
    for (EdgeNode *node = _edges->begin(), *end = _edges->end(); node != end; node = node->next())
    {
        Edge2 &edge = node->data;
        
        if (edge.containsVertex(v1) && edge.containsVertex(v2))
        {
            edge.setTriangle(1, triangle);
            return node;
        }
    }

    VertexNode *vertices[2] = { v1, v2 };
    EdgeNode *node = _edges->add(vertices);
    node->data.setTriangle(0, triangle);
    return node;
}

void Mesh2::makeEdges()
{
    for (TriangleNode *node = _triangles->begin(), *end = _triangles->end(); node != end; node = node->next())
    {
        Triangle2 &triangle = node->data;
        
        VertexNode *v0 = triangle.vertex(0);
        VertexNode *v1 = triangle.vertex(1);
        VertexNode *v2 = triangle.vertex(2);
        
        EdgeNode *e0 = findOrCreateEdge(v0, v1, node);
        EdgeNode *e1 = findOrCreateEdge(v1, v2, node);
        EdgeNode *e2 = findOrCreateEdge(v2, v0, node);
        
        triangle.setEdge(0, e0);
        triangle.setEdge(1, e1);
        triangle.setEdge(2, e2);        
    }
}

void Mesh2::makeCube()
{
    _vertices->removeAll();
	_triangles->removeAll();
    _edges->removeAll();
	
	// back vertices
	VertexNode *v0 = _vertices->add(Vector3D(-1, -1, -1));
	VertexNode *v1 = _vertices->add(Vector3D( 1, -1, -1));
    VertexNode *v2 = _vertices->add(Vector3D( 1,  1, -1));
	VertexNode *v3 = _vertices->add(Vector3D(-1,  1, -1));
	
	// front vertices
	VertexNode *v4 = _vertices->add(Vector3D(-1, -1,  1));
	VertexNode *v5 = _vertices->add(Vector3D( 1, -1,  1));
	VertexNode *v6 = _vertices->add(Vector3D( 1,  1,  1));
	VertexNode *v7 = _vertices->add(Vector3D(-1,  1,  1));
    
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
    
    makeEdges();
	
    setSelectionMode(_selectionMode);
}

