/*
 *  MeshHelpers.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#include "MeshHelpers.h"

void Triangle2::AddToVertices()
{
    
}

void Triangle2::RemoveFromVertices()
{
    
}

bool Triangle2::IsDegenerated()
{
    if (vertices[0] == vertices[1])
		return true;
	if (vertices[0] == vertices[2])
		return true;
	if (vertices[1] == vertices[2])
		return true;
	
	return false;
}

bool Triangle2::IsVertexInTriangle(VertexNode vertex)
{
    for (uint i = 0; i < 3; i++)
	{
		if (vertices[i] == vertex)
			return true;
	}
	return false;
}

Triangle2 Triangle2::Flip()
{
    Triangle2 opposite;
	opposite.vertices[0] = vertices[2];
	opposite.vertices[1] = vertices[1];
	opposite.vertices[2] = vertices[0];
	return opposite;
}

Vector3D NormalFromTriangleVertices(Vector3D triangleVertices[3])
{
	// now is same as RedBook (OpenGL Programming Guide)
	Vector3D u = triangleVertices[0] - triangleVertices[1];
	Vector3D v = triangleVertices[1] - triangleVertices[2];
	return u.Cross(v);
}

void AddTriangle(vector<Triangle> &triangles, int index1, int index2, int index3)
{
    Triangle triangle;
    
    triangle.vertexIndices[0] = index1;
    triangle.vertexIndices[1] = index2;
    triangle.vertexIndices[2] = index3;
    
    triangles.push_back(triangle);
}

void AddQuad(vector<Triangle> &triangles, int index1, int index2, int index3, int index4)
{
    Triangle triangle1, triangle2;
	triangle1.vertexIndices[0] = index1;
	triangle1.vertexIndices[1] = index2;
	triangle1.vertexIndices[2] = index3;
	
	triangle2.vertexIndices[0] = index1;
	triangle2.vertexIndices[1] = index3;
	triangle2.vertexIndices[2] = index4;
    
    triangles.push_back(triangle1);
    triangles.push_back(triangle2);
}

