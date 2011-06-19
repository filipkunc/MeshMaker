//
//  Mesh2.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#include "Mesh2.h"

Mesh2::Mesh2()
{
    vertices = new FPList<VertexNode, Vertex2>();
    triangles = new FPList<TriangleNode, Triangle2>();
    
    cachedVertexSelection = new vector<VertexNode *>();
    cachedTriangleSelection = new vector<TriangleNode *>();
    
    cachedVertices = NULL;
    cachedNormals = NULL;
    cachedColors = NULL;
}

Mesh2::~Mesh2()
{
    delete vertices;
    delete triangles;
    
    delete cachedVertexSelection;
    delete cachedTriangleSelection;
}

void Mesh2::addQuad(VertexNode *v1, VertexNode *v2, VertexNode *v3, VertexNode *v4)
{
    Triangle2 triangle1(v1, v2, v3);
    Triangle2 triangle2(v1, v3, v4);
  	triangles->add(triangle1);
    triangles->add(triangle2);
}

void Mesh2::setSelectionMode(MeshSelectionMode value)
{
    selectionMode = value;
    cachedVertexSelection->clear();
    cachedTriangleSelection->clear();
    
    switch (selectionMode)
    {
        case MeshSelectionModeVertices:
        {
            for (VertexNode *node = vertices->begin(), *end = vertices->end(); node != end; node = node->next())
                cachedVertexSelection->push_back(node);
        } break;
        case MeshSelectionModeTriangles:
        {
            for (TriangleNode *node = triangles->begin(), *end = triangles->end(); node != end; node = node->next())
                cachedTriangleSelection->push_back(node);
        } break;            
        default:
            break;
    }
}

void Mesh2::getSelectionCenterRotationScale(Vector3D &center, Quaternion &rotation, Vector3D &scale)
{
    center = Vector3D();
	rotation = Quaternion();
	scale = Vector3D(1, 1, 1);
    
	uint selectedCount = 0;
    
    for (VertexNode *node = vertices->begin(), *end = vertices->end(); node != end; node = node->next())
    {
        if (node->data.selected)
        {
            center += node->data.position;
            selectedCount++;
        }
    }
	if (selectedCount > 0)
		center /= (float)selectedCount;
}

void Mesh2::resetCache()
{
    if (cachedVertices)
	{
		delete [] cachedVertices;
		cachedVertices = NULL;
	}
	if (cachedNormals)
	{
		delete [] cachedNormals;
		cachedNormals = NULL;
	}
	if (cachedColors)
	{
		delete [] cachedColors;
		cachedColors = NULL;
	}
}

void Mesh2::fillCache()
{
    if (!cachedVertices)
	{
		cachedVertices = new Vector3D[triangles->count() * 3];
		cachedNormals = new Vector3D[triangles->count() * 3];
		cachedColors = new Vector3D[triangles->count() * 3];
		Vector3D triangleVertices[3];
        
        uint i = 0;
		
		for (TriangleNode *node = triangles->begin(), *end = triangles->end(); node != end; node = node->next())
		{
			Triangle2 currentTriangle = node->data;
            currentTriangle.GetVertexPositions(triangleVertices);
			
			Vector3D n = NormalFromTriangleVertices(triangleVertices);
			
			for (uint j = 0; j < 3; j++)
			{
				cachedVertices[i * 3 + j] = triangleVertices[j];
				cachedNormals[i * 3 + j] = n;
			}
            
            i++;
		}
	}
}

void Mesh2::fillColorCacheAsDarker(float colorComponents[4], bool darker)
{
	float selectedComponents[] = { 0.7f, 0.0f, 0.0f };
	
	if (darker)
	{
		for (uint k = 0; k < 3; k++)
			colorComponents[k] -= 0.2f;
		
		selectedComponents[0] -= 0.2f;
	}
    
    uint i = 0;
	
    for (TriangleNode *node = triangles->begin(), *end = triangles->end(); node != end; node = node->next())
	{
		if (node->data.selected)
		{
			for (uint j = 0; j < 3; j++)
			{
				for (uint k = 0; k < 3; k++)
				{
					cachedColors[i * 3 + j][k] = selectedComponents[k];
				}
			}				
		}
		else
		{
			for (uint j = 0; j < 3; j++)
			{
				for (uint k = 0; k < 3; k++)
				{
					cachedColors[i * 3 + j][k] = colorComponents[k];
				}
			}	
		}
        
        i++;
	}
}

void Mesh2::makeCube()
{
    vertices->removeAll();
	triangles->removeAll();
	
	// back vertices
	VertexNode *v0 = vertices->add(Vector3D(-1, -1, -1)); // 0
	VertexNode *v1 = vertices->add(Vector3D( 1, -1, -1)); // 1
    VertexNode *v2 = vertices->add(Vector3D( 1,  1, -1)); // 2
	VertexNode *v3 = vertices->add(Vector3D(-1,  1, -1)); // 3
	
	// front vertices
	VertexNode *v4 = vertices->add(Vector3D(-1, -1,  1)); // 4
	VertexNode *v5 = vertices->add(Vector3D( 1, -1,  1)); // 5
	VertexNode *v6 = vertices->add(Vector3D( 1,  1,  1)); // 6
	VertexNode *v7 = vertices->add(Vector3D(-1,  1,  1)); // 7
	
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
	
    setSelectionMode(selectionMode);
	//[self setSelectionMode:[self selectionMode]];
}
