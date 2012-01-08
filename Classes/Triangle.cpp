//
//  Triangle.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#include "MeshHelpers.h"

Triangle2::Triangle2() : selected(false)
{
    for (int i = 0; i < 3; i++)
    {
        _vertices[i] = NULL;
        _vertexEdges[i] = NULL;
        _texCoords[i] = NULL;
        _texCoordEdges[i] = NULL;
    }
}

Triangle2::Triangle2(VertexNode *vertices[3]) : selected(false)
{
    for (int i = 0; i < 3; i++)
    {
        _vertices[i] = vertices[i];
        _vertexEdges[i] = NULL;
        _texCoords[i] = NULL;
        _texCoordEdges[i] = NULL;
    }
}

Triangle2::Triangle2(VertexNode *vertices[3], TexCoordNode *texCoords[3]) : selected(false)
{
    for (int i = 0; i < 3; i++)
    {
        _vertices[i] = vertices[i];
        _vertexEdges[i] = NULL;
        _texCoords[i] = texCoords[i];
        _texCoordEdges[i] = NULL;
    }
}

void TriangleNode::addToVerticesAndTexCoords()
{
    for (int i = 0; i < 3; i++)
    {
        if (data._vertices[i])
            data._vertices[i]->addTriangle(this);
    }
    
    for (int i = 0; i < 3; i++)
    {
        if (data._texCoords[i])
            data._texCoords[i]->addTriangle(this);
    }
}

void TriangleNode::removeFromVerticesAndTexCoords()
{
    for (int i = 0; i < 3; i++)
    {
        if (data._vertices[i])
        {
            data._vertices[i]->removeTriangle(this);
            data._vertices[i] = NULL;
        }
    }
    
    for (int i = 0; i < 3; i++)
    {
        if (data._texCoords[i])
        {
            data._texCoords[i]->removeTriangle(this);
            data._texCoords[i] = NULL;
        }
    }
}

void TriangleNode::removeFromEdges()
{
    for (int i = 0; i < 3; i++)
    {
        if (data._vertexEdges[i])
        {
            data._vertexEdges[i]->data.removeTriangle(this);
            data._vertexEdges[i] = NULL;
        }
    }
    
    for (int i = 0; i < 3; i++)
    {
        if (data._texCoordEdges[i])
        {
            data._texCoordEdges[i]->data.removeTriangle(this);
            data._texCoordEdges[i] = NULL;
        }
    }
}

void TriangleNode::replaceVertex(VertexNode *currentVertex, VertexNode *newVertex)
{
    for (int i = 0; i < 3; i++)
    {
        if (data._vertices[i] == currentVertex)
        {
            data._vertices[i] = newVertex;
            newVertex->addTriangle(this);
            break;
        }
    }
}

void Triangle2::removeVertex(VertexNode *vertex)
{
    for (int i = 0; i < 3; i++)
    {
        if (_vertices[i] == vertex)
        {
            _vertices[i] = NULL;
            break;
        }
    }
}

void Triangle2::removeTexCoord(TexCoordNode *texCoord)
{
    for (int i = 0; i < 3; i++)
    {
        if (_texCoords[i] == texCoord)
        {
            _texCoords[i] = NULL;
            break;
        }
    }
}

void Triangle2::removeEdge(VertexEdgeNode *edge)
{
    for (int i = 0; i < 3; i++)
    {
        if (_vertexEdges[i] == edge)
        {
            _vertexEdges[i] = NULL;
            break;
        }
    }
}

void Triangle2::removeEdges()
{
    for (int i = 0; i < 3; i++)
        _vertexEdges[i] = NULL;
}

bool Triangle2::isDegenerated() const
{
    if (containsVertex(NULL))
        return true;
    
    if (containsEdge(NULL))
        return true;
    
    if (_vertices[0] == _vertices[1])
		return true;
	if (_vertices[0] == _vertices[2])
		return true;
	if (_vertices[1] == _vertices[2])
		return true;
	
	return false;
}

bool Triangle2::containsVertex(const VertexNode *vertex) const
{
    for (int i = 0; i < 3; i++)
	{
		if (_vertices[i] == vertex)
			return true;
	}
	return false;
}

bool Triangle2::containsEdge(const VertexEdgeNode *edge) const
{
    for (int i = 0; i < 3; i++)
    {
        if (_vertexEdges[i] == edge)
            return true;
    }
    return false;
}

void Triangle2::flip()
{
    swap(_vertices[0], _vertices[2]);    
}

int Triangle2::indexOfVertex(const VertexNode *vertex) const
{
    for (int i = 0; i < 3; i++)
    {
        if (_vertices[i] == vertex)
            return i;
    }
    return -1;
}

void Triangle2::sortVertices(VertexNode *&v1, VertexNode *&v2) const
{
    int index1 = indexOfVertex(v1);
    int index2 = indexOfVertex(v2);
    
    if (index1 == 1 && index2 == 0)
        swap(v1, v2);
    else if (index1 == 2 && index2 == 1)
        swap(v1, v2);
    else if (index1 == 0 && index2 == 2)
        swap(v1, v2);
}

VertexNode *Triangle2::vertexNotInEdge(const VertexEdge *edge) const
{
    for (int i = 0; i < 3; i++)
    {
        if (_vertices[i] != edge->vertex(0) &&
            _vertices[i] != edge->vertex(1))
            return _vertices[i];
    }
    return NULL;
}

void Triangle2::computeNormal()
{
    Vector3D u = _vertices[0]->data.position - _vertices[1]->data.position;
	Vector3D v = _vertices[1]->data.position - _vertices[2]->data.position;
	normal = u.Cross(v);    
}
