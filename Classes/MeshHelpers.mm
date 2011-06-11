/*
 *  MeshHelpers.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#include "MeshHelpers.h"

void VertexNode::AddTriangle(Triangle2 *triangle)
{
    triangles.Add(triangle);
}

void VertexNode::RemoveTriangle(Triangle2 *triangle)
{
    for (SimpleNode<Triangle2 *> *node = triangles.Begin(), *end = triangles.End(); node != end; node = node->Next())
    {
        if (node->data == triangle)
            triangles.Remove(node);
    }
}

void VertexNode::RemoveFromTriangles()
{
    for (SimpleNode<Triangle2 *> *node = triangles.Begin(), *end = triangles.End(); node != end; node = node->Next())
    {
        node->data->RemoveVertex(this);
    }
    
    triangles.RemoveAll();
}

void VertexNode::ReplaceVertex(VertexNode *newVertex)
{
    for (SimpleNode<Triangle2 *> *node = triangles.Begin(), *end = triangles.End(); node != end; node = node->Next())
    {
        node->data->ReplaceVertex(this, newVertex);
    }
    
    triangles.RemoveAll();
}

Triangle2::Triangle2() : selected(false)
{
    vertices[0] = NULL;
    vertices[1] = NULL;
    vertices[2] = NULL;
}

Triangle2::Triangle2(VertexNode *v1, VertexNode *v2, VertexNode *v3) : selected(false)
{
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;    
}

void Triangle2::AddToVertices()
{
    for (uint i = 0; i < 3; i++)
    {
        if (vertices[i])
            vertices[i]->AddTriangle(this);
    }
}

void Triangle2::RemoveFromVertices()
{
    for (uint i = 0; i < 3; i++)
    {
        if (vertices[i])
        {
            vertices[i]->RemoveTriangle(this);
            vertices[i] = NULL;
        }
    }
}

void Triangle2::ReplaceVertex(VertexNode *currentVertex, VertexNode *newVertex)
{
    for (uint i = 0; i < 3; i++)
    {
        if (vertices[i] == currentVertex)
        {
            vertices[i] = newVertex;
            newVertex->AddTriangle(this);
            break;
        }
    }
}

void Triangle2::RemoveVertex(VertexNode *vertex)
{
    for (uint i = 0; i < 3; i++)
    {
        if (vertices[i] == vertex)
        {
            vertices[i] = NULL;
            break;
        }
    }
}

bool Triangle2::IsDegenerated() const
{
    if (vertices[0] == vertices[1])
		return true;
	if (vertices[0] == vertices[2])
		return true;
	if (vertices[1] == vertices[2])
		return true;
	
	return false;
}

bool Triangle2::IsVertexInTriangle(VertexNode *vertex) const
{
    for (uint i = 0; i < 3; i++)
	{
		if (vertices[i] == vertex)
			return true;
	}
	return false;
}

void Triangle2::GetVertexPositions(Vector3D vertexPositions[3]) const
{
    for (uint i = 0; i < 3; i++)
        vertexPositions[i] = vertices[i]->data.position;
}

void Triangle2::Flip()
{
    swap(vertices[0], vertices[2]);    
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

