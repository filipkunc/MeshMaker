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

void TriangleNode::addToVertices()
{
    for (int i = 0; i < 3; i++)
    {
        if (data()._vertices[i])
            data()._vertices[i]->addTriangle(this);
    }
}

void TriangleNode::addToTexCoords()
{
    for (int i = 0; i < 3; i++)
    {
        if (data()._texCoords[i])
            data()._texCoords[i]->addTriangle(this);
    }
}

void TriangleNode::removeFromVertices()
{
    for (int i = 0; i < 3; i++)
    {
        if (data()._vertices[i])
        {
            data()._vertices[i]->removeTriangle(this);
            data()._vertices[i] = NULL;
        }
    }
}

void TriangleNode::removeFromTexCoords()
{
    for (int i = 0; i < 3; i++)
    {
        if (data()._texCoords[i])
        {
            data()._texCoords[i]->removeTriangle(this);
            data()._texCoords[i] = NULL;
        }
    }
}

void TriangleNode::removeFromEdges()
{
    for (int i = 0; i < 3; i++)
    {
        if (data()._vertexEdges[i])
        {
            data()._vertexEdges[i]->data().removeTriangle(this);
            data()._vertexEdges[i] = NULL;
        }
    }
    
    for (int i = 0; i < 3; i++)
    {
        if (data()._texCoordEdges[i])
        {
            data()._texCoordEdges[i]->data().removeTriangle(this);
            data()._texCoordEdges[i] = NULL;
        }
    }
}

void TriangleNode::replaceVertex(VertexNode *currentVertex, VertexNode *newVertex)
{
    for (int i = 0; i < 3; i++)
    {
        if (data()._vertices[i] == currentVertex)
        {
            data()._vertices[i] = newVertex;
            newVertex->addTriangle(this);
            break;
        }
    }
}

void TriangleNode::replaceTexCoord(TexCoordNode *currentTexCoord, TexCoordNode *newTexCoord)
{
    for (int i = 0; i < 3; i++)
    {
        if (data()._texCoords[i] == currentTexCoord)
        {
            data()._texCoords[i] = newTexCoord;
            newTexCoord->addTriangle(this);
            break;
        }
    }
}

void Triangle2::setTexCoordByVertex(TexCoordNode *texCoord, VertexNode *vertex)
{
    for (int i = 0; i < 3; i++)
    { 
        if (_vertices[i] == vertex) 
        {
            _texCoords[i] = texCoord; 
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

void Triangle2::removeVertexEdge(VertexEdgeNode *edge)
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

void Triangle2::removeTexCoordEdge(TexCoordEdgeNode *edge)
{
    for (int i = 0; i < 3; i++)
    {
        if (_texCoordEdges[i] == edge)
        {
            _texCoordEdges[i] = NULL;
            break;
        }
    }
}

void Triangle2::removeEdges()
{
    for (int i = 0; i < 3; i++)
    {
        _vertexEdges[i] = NULL;
        _texCoordEdges[i] = NULL;
    }    
}

bool Triangle2::isDegenerated() const
{
    if (containsVertex(NULL))
        return true;
    
    if (containsVertexEdge(NULL))
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

bool Triangle2::containsVertexEdge(const VertexEdgeNode *edge) const
{
    for (int i = 0; i < 3; i++)
    {
        if (_vertexEdges[i] == edge)
            return true;
    }
    return false;
}

bool Triangle2::containsTexCoordEdge(const TexCoordEdgeNode *edge) const
{
    for (int i = 0; i < 3; i++)
    {
        if (_texCoordEdges[i] == edge)
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
    Vector3D u, v;
    
    u = _texCoords[0]->data().position - _texCoords[1]->data().position;
    v = _texCoords[1]->data().position - _texCoords[2]->data().position;
    texCoordNormal = u.Cross(v);    
    
    u = _vertices[0]->data().position - _vertices[1]->data().position;
    v = _vertices[1]->data().position - _vertices[2]->data().position;
    vertexNormal = u.Cross(v);    
}

bool Triangle2::rayIntersect(const Vector3D &origin, const Vector3D &direction, float &u, float &v, Vector3D &intersect)
{
    Vector3D v0 = _vertices[0]->data().position;
    Vector3D v1 = _vertices[1]->data().position;
    Vector3D v2 = _vertices[2]->data().position;
    
    Vector3D e1 = v1 - v0;
    Vector3D e2 = v2 - v0;
    Vector3D p = direction.Cross(e2);
    float a = e1.Dot(p);
    
    if (fabsf(a) < FLOAT_EPS)
        return false;
    
    float f = 1.0f / a;
    
    Vector3D s = origin - v0;
    u = f * s.Dot(p);
    if (u < 0.0f || u > 1.0f)
        return false;
    
    Vector3D q = s.Cross(e1);
    v = f * direction.Dot(q);
    if (v < 0.0f || u + v > 1.0f)
        return false;
    
    float t = f * e2.Dot(q);
    if (t >= 0.0f)
    {
        intersect = v0 + e1 * u + e2 * v;
        return true;
    }
    return false;
}

void Triangle2::convertBarycentricToUVs(float &u, float &v)
{
    Vector3D t0 = _texCoords[0]->data().position;
    Vector3D t1 = _texCoords[1]->data().position;
    Vector3D t2 = _texCoords[2]->data().position;    
    
    Vector3D e1 = t1 - t0;
    Vector3D e2 = t2 - t0;
   
    Vector3D final = t0 + e1 * u + e2 * v;
    
    u = final.x;
    v = final.y;
}
