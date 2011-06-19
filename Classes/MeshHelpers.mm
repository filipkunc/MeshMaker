/*
 *  MeshHelpers.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#import "MeshHelpers.h"

void VertexNode::addTriangle(Triangle2 *triangle)
{
    triangles.add(triangle);
}

void VertexNode::removeTriangle(Triangle2 *triangle)
{
    for (SimpleNode<Triangle2 *> *node = triangles.begin(), *end = triangles.end(); node != end; node = node->next())
    {
        if (node->data == triangle)
            triangles.remove(node);
    }
}

void VertexNode::removeFromTriangles()
{
    for (SimpleNode<Triangle2 *> *node = triangles.begin(), *end = triangles.end(); node != end; node = node->next())
    {
        node->data->removeVertex(this);
    }
    
    triangles.removeAll();
}

void VertexNode::replaceVertex(VertexNode *newVertex)
{
    for (SimpleNode<Triangle2 *> *node = triangles.begin(), *end = triangles.end(); node != end; node = node->next())
    {
        node->data->replaceVertex(this, newVertex);
    }
    
    triangles.removeAll();
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

void Triangle2::addToVertices()
{
    for (uint i = 0; i < 3; i++)
    {
        if (vertices[i])
            vertices[i]->addTriangle(this);
    }
}

void Triangle2::removeFromVertices()
{
    for (uint i = 0; i < 3; i++)
    {
        if (vertices[i])
        {
            vertices[i]->removeTriangle(this);
            vertices[i] = NULL;
        }
    }
}

void Triangle2::replaceVertex(VertexNode *currentVertex, VertexNode *newVertex)
{
    for (uint i = 0; i < 3; i++)
    {
        if (vertices[i] == currentVertex)
        {
            vertices[i] = newVertex;
            newVertex->addTriangle(this);
            break;
        }
    }
}

void Triangle2::removeVertex(VertexNode *vertex)
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

bool Triangle2::isDegenerated() const
{
    if (isVertexInTriangle(NULL))
        return true;
    
    if (vertices[0] == vertices[1])
		return true;
	if (vertices[0] == vertices[2])
		return true;
	if (vertices[1] == vertices[2])
		return true;
	
	return false;
}

bool Triangle2::isVertexInTriangle(VertexNode *vertex) const
{
    for (uint i = 0; i < 3; i++)
	{
		if (vertices[i] == vertex)
			return true;
	}
	return false;
}

void Triangle2::getVertexPositions(Vector3D vertexPositions[3]) const
{
    for (uint i = 0; i < 3; i++)
        vertexPositions[i] = vertices[i]->data.position;
}

void Triangle2::flip()
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

