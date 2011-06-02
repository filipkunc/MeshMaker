/*
 *  MeshHelpers.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#include "MeshHelpers.h"

BOOL IsTriangleDegenerated(Triangle triangle)
{
	if (triangle.vertexIndices[0] == triangle.vertexIndices[1])
		return YES;
	if (triangle.vertexIndices[0] == triangle.vertexIndices[2])
		return YES;
	if (triangle.vertexIndices[1] == triangle.vertexIndices[2])
		return YES;
	
	return NO;
}

BOOL AreEdgesSame(Edge first, Edge second)
{
	if (first.vertexIndices[0] == second.vertexIndices[0] &&
		first.vertexIndices[1] == second.vertexIndices[1])
		return YES;
	
	if (first.vertexIndices[0] == second.vertexIndices[1] &&
		first.vertexIndices[1] == second.vertexIndices[0])
		return YES;
	
	return NO;
}

BOOL IsIndexInTriangle(Triangle triangle, uint index)
{
	for (uint i = 0; i < 3; i++)
	{
		if (triangle.vertexIndices[i] == index)
			return YES;
	}
	return NO;
}

BOOL IsEdgeInTriangle(Triangle triangle, Edge edge)
{
	if (IsIndexInTriangle(triangle, edge.vertexIndices[0]) &&
		IsIndexInTriangle(triangle, edge.vertexIndices[1]))
	{
		return YES;
	}
	return NO;
}

uint NonEdgeIndexInTriangle(Triangle triangle, Edge edge)
{
	for (uint i = 0; i < 3; i++)
	{
		if (triangle.vertexIndices[i] != edge.vertexIndices[0] &&
			triangle.vertexIndices[i] != edge.vertexIndices[1])
		{
			return triangle.vertexIndices[i];
		}
	}
	return 0;
}

Vector3D NormalFromTriangleVertices(Vector3D triangleVertices[3])
{
	// now is same as RedBook (OpenGL Programming Guide)
	Vector3D u = triangleVertices[0] - triangleVertices[1];
	Vector3D v = triangleVertices[1] - triangleVertices[2];
	return u.Cross(v);
}

Triangle MakeTriangle(uint first, uint second, uint third)
{
	Triangle triangle;
	triangle.vertexIndices[0] = first;
	triangle.vertexIndices[1] = second;
	triangle.vertexIndices[2] = third;
	return triangle;
}

Triangle FlipTriangle(Triangle triangle)
{
	Triangle opposite;
	opposite.vertexIndices[0] = triangle.vertexIndices[2];
	opposite.vertexIndices[1] = triangle.vertexIndices[1];
	opposite.vertexIndices[2] = triangle.vertexIndices[0];
	return opposite;
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

