//
//  Triangle.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#include "MeshHelpers.h"

Triangle2::Triangle2() : selected(false), visible(true)
{
    for (int i = 0; i < count(); i++)
    {
        setVertex(i, NULL);
        setVertexEdge(i, NULL);
        setTexCoord(i, NULL);
        setTexCoordEdge(i, NULL);
    }
}

Triangle2::Triangle2(VertexNode *vertices[3]) : selected(false), visible(true)
{
    for (int i = 0; i < count(); i++)
    {
        setVertex(i, vertices[i]);
        setVertexEdge(i, NULL);
        setTexCoord(i, NULL);
        setTexCoordEdge(i, NULL);
    }
}

Triangle2::Triangle2(VertexNode *vertices[3], TexCoordNode *texCoords[3]) : selected(false), visible(true)
{
    for (int i = 0; i < count(); i++)
    {
        setVertex(i, vertices[i]);
        setVertexEdge(i, NULL);
        setTexCoord(i, texCoords[i]);
        setTexCoordEdge(i, NULL);
    }
}

void TriangleNode::addToVertices()
{
    for (int i = 0; i < data().count(); i++)
    {
        if (data().vertex(i))
            data().vertex(i)->addTriangle(this);
    }
}

void TriangleNode::addToTexCoords()
{
    for (int i = 0; i < data().count(); i++)
    {
        if (data().texCoord(i))
            data().texCoord(i)->addTriangle(this);
    }
}

void TriangleNode::removeFromVertices()
{
    for (int i = 0; i < data().count(); i++)
    {
        if (data().vertex(i))
        {
            data().vertex(i)->removeTriangle(this);
            data().setVertex(i, NULL);
        }
    }
}

void TriangleNode::removeFromTexCoords()
{
    for (int i = 0; i < data().count(); i++)
    {
        if (data().texCoord(i))
        {
            data().texCoord(i)->removeTriangle(this);
            data().setTexCoord(i, NULL);
        }
    }
}

void TriangleNode::removeFromEdges()
{
    for (int i = 0; i < data().count(); i++)
    {
        if (data().vertexEdge(i))
        {
            data().vertexEdge(i)->data().removeTriangle(this);
            data().setVertexEdge(i, NULL);
        }
        
        if (data().texCoordEdge(i))
        {
            data().texCoordEdge(i)->data().removeTriangle(this);
            data().setTexCoordEdge(i, NULL);
        }
    }
}

void TriangleNode::replaceVertex(VertexNode *currentVertex, VertexNode *newVertex)
{
    for (int i = 0; i < data().count(); i++)
    {
        if (data().vertex(i) == currentVertex)
        {
            data().setVertex(i, newVertex);
            newVertex->addTriangle(this);
            break;
        }
    }
}

void TriangleNode::replaceTexCoord(TexCoordNode *currentTexCoord, TexCoordNode *newTexCoord)
{
    for (int i = 0; i < data().count(); i++)
    {
        if (data().texCoord(i) == currentTexCoord)
        {
            data().setTexCoord(i, newTexCoord);
            newTexCoord->addTriangle(this);
            break;
        }
    }
}

void Triangle2::setTexCoordByVertex(TexCoordNode *texCoord, VertexNode *vertex)
{
    for (int i = 0; i < count(); i++)
    { 
        if (this->vertex(i) == vertex)
        {
            setTexCoord(i, texCoord);
            break;
        }
    }
}

void Triangle2::removeVertex(VertexNode *vertex)
{
    for (int i = 0; i < count(); i++)
    {
        if (this->vertex(i) == vertex)
        {
            setVertex(i, NULL);
            break;
        }
    }
}

void Triangle2::removeTexCoord(TexCoordNode *texCoord)
{
    for (int i = 0; i < count(); i++)
    {
        if (this->texCoord(i) == texCoord)
        {
            this->setTexCoord(i, NULL);
            break;
        }
    }
}

void Triangle2::removeVertexEdge(VertexEdgeNode *edge)
{
    for (int i = 0; i < count(); i++)
    {
        if (vertexEdge(i) == edge)
        {
            setVertexEdge(i, NULL);
            break;
        }
    }
}

void Triangle2::removeTexCoordEdge(TexCoordEdgeNode *edge)
{
    for (int i = 0; i < count(); i++)
    {
        if (texCoordEdge(i) == edge)
        {
            setTexCoordEdge(i, NULL);
            break;
        }
    }
}

void Triangle2::removeEdges()
{
    for (int i = 0; i < count(); i++)
    {
        setVertexEdge(i, NULL);
        setTexCoordEdge(i, NULL);
    }
}

bool Triangle2::isDegenerated() const
{
    if (containsVertex(NULL))
        return true;
    
    if (containsVertexEdge(NULL))
        return true;
    
    for (int i = 0; i < count() - 1; i++)
    {
        for (int j = i + 1; j < count(); j++)
        {
            if (vertex(i) == vertex(j))
                return true;
        }
    }
    	
	return false;
}

bool Triangle2::containsVertex(const VertexNode *vertex) const
{
    for (int i = 0; i < count(); i++)
	{
		if (this->vertex(i) == vertex)
			return true;
	}
	return false;
}

bool Triangle2::containsVertexEdge(const VertexEdgeNode *edge) const
{
    for (int i = 0; i < count(); i++)
    {
        if (vertexEdge(i) == edge)
            return true;
    }
    return false;
}

bool Triangle2::containsTexCoordEdge(const TexCoordEdgeNode *edge) const
{
    for (int i = 0; i < count(); i++)
    {
        if (texCoordEdge(i) == edge)
            return true;
    }
    return false;
}

void Triangle2::flip()
{
    // TODO: Make it work for polygon (tri/quad).
    
    swap(_nodes[0].vertex, _nodes[2].vertex);
}

int Triangle2::indexOfVertex(const VertexNode *vertex) const
{
    for (int i = 0; i < count(); i++)
    {
        if (this->vertex(i) == vertex)
            return i;
    }
    return -1;
}

void Triangle2::sortVertices(VertexNode *&v1, VertexNode *&v2) const
{
    // TODO: Make it work for polygon (tri/quad).
    
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
    for (int i = 0; i < count(); i++)
    {
        VertexNode *currentVertex = vertex(i);
        if (currentVertex != edge->vertex(0) &&
            currentVertex != edge->vertex(1))
            return currentVertex;
    }
    return NULL;
}

TexCoordNode *Triangle2::texCoordNotInEdge(const TexCoordEdge *edge) const
{
    for (int i = 0; i < count(); i++)
    {
        TexCoordNode *currentTexCoord = texCoord(i);
        if (currentTexCoord != edge->texCoord(0) &&
            currentTexCoord != edge->texCoord(1))
            return currentTexCoord;
    }
    return NULL;
}

void Triangle2::computeNormal()
{
    // TODO: Make it work for polygon (tri/quad).
    
    Vector3D u, v;
    
    u = texCoord(0)->data().position - texCoord(1)->data().position;
    v = texCoord(1)->data().position - texCoord(2)->data().position;
    texCoordNormal = u.Cross(v);    
    
    u = vertex(0)->data().position - vertex(1)->data().position;
    v = vertex(1)->data().position - vertex(2)->data().position;
    vertexNormal = u.Cross(v);    
}

bool Triangle2::rayIntersect(const Vector3D &origin, const Vector3D &direction, float &u, float &v, Vector3D &intersect)
{
    // TODO: Make it work for polygon (tri/quad).
    
    Vector3D v0 = vertex(0)->data().position;
    Vector3D v1 = vertex(1)->data().position;
    Vector3D v2 = vertex(2)->data().position;
    
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
    // TODO: Make it work for polygon (tri/quad).
    
    Vector3D t0 = texCoord(0)->data().position;
    Vector3D t1 = texCoord(1)->data().position;
    Vector3D t2 = texCoord(2)->data().position;
    
    Vector3D e1 = t1 - t0;
    Vector3D e2 = t2 - t0;
   
    Vector3D final = t0 + e1 * u + e2 * v;
    
    u = final.x;
    v = final.y;
}
