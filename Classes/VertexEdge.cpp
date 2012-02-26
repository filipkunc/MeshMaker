//
//  VertexEdge.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#include "MeshHelpers.h"

VertexEdge::VertexEdge() : selected(false), halfVertex(NULL)
{
    for (int i = 0; i < 2; i++)
    {
        _vertices[i] = NULL;
        _triangles[i] = NULL;
    }
}

VertexEdge::VertexEdge(VertexNode *vertices[2]) : selected(false), halfVertex(NULL)
{
    for (int i = 0; i < 2; i++)
    {
        _vertices[i] = vertices[i];
        _triangles[i] = NULL;
    }
}

bool VertexEdge::isQuadEdge() const
{
    if (_triangles[0] != NULL && _triangles[1] != NULL)
    {
        Triangle2 &t0 = _triangles[0]->data();
        Triangle2 &t1 = _triangles[1]->data();
        
        VertexNode *otherVertices[2];
        otherVertices[0] = t0.vertexNotInEdge(this);
        otherVertices[1] = t1.vertexNotInEdge(this);
        
        float squaredDistanceInEdge = _vertices[0]->data().position.SqDistance(_vertices[1]->data().position);
        float sqDistances[4];
        
        for (int i = 0; i < 2; i++)
        {
            sqDistances[i] = otherVertices[i]->data().position.SqDistance(_vertices[0]->data().position);
            sqDistances[i + 2] = otherVertices[i]->data().position.SqDistance(_vertices[1]->data().position); 
        }
        
        for (int i = 0; i < 4; i++)
        {
            if (squaredDistanceInEdge < sqDistances[i])
                return false;
            
            if (fabsf(squaredDistanceInEdge - sqDistances[i]) < FLOAT_EPS)
                return false;                
        }
        
        return true;
    }
    return false;
}

bool VertexEdge::isDegenerated() const
{
    if (_triangles[0] == NULL && _triangles[1] == NULL)
        return true;
    
    if (containsVertex(NULL))
        return true;
    
    if (_vertices[0] == _vertices[1])
        return true;
    
    return false;    
}

bool VertexEdge::containsVertex(const VertexNode *vertex) const
{
    for (int i = 0; i < 2; i++)
    {
        if (_vertices[i] == vertex)
            return true;        
    }
    return false;
}

void VertexEdgeNode::addToVertices()
{
    for (int i = 0; i < 2; i++)
    {
        if (data()._vertices[i])
            data()._vertices[i]->addEdge(this);
    }
}

void VertexEdgeNode::removeFromVertices()
{
    for (int i = 0; i < 2; i++)
    {
        if (data()._vertices[i])
        {
            data()._vertices[i]->removeEdge(this);
            data()._vertices[i] = NULL;
        }
    }
}

void VertexEdgeNode::removeFromTriangles()
{
    for (int i = 0; i < 2; i++)
    {
        if (data()._triangles[i])
        {
            data()._triangles[i]->data().removeVertexEdge(this);
            data()._triangles[i] = NULL;
        }
    }    
}

void VertexEdge::removeVertex(VertexNode *vertex)
{
    for (int i = 0; i < 2; i++)
    {
        if (_vertices[i] == vertex)
        {
            _vertices[i] = NULL;
            break;
        }
    }
}

void VertexEdge::removeTriangle(TriangleNode *triangle)
{
    for (int i = 0; i < 2; i++)
    {
        if (_triangles[i] == triangle)
        {
            _triangles[i] = NULL;
            break;
        }
    }
}

void VertexEdgeNode::replaceVertex(VertexNode *currentVertex, VertexNode *newVertex)
{
    for (int i = 0; i < 2; i++)
    {
        if (data()._vertices[i] == currentVertex)
        {
            data()._vertices[i] = newVertex;
            newVertex->addEdge(this);
            break;
        }
    }   
}

bool VertexEdge::isNotShared() const
{
    for (int i = 0; i < 2; i++)
    {
        if (_triangles[i] == NULL || !_triangles[i]->data().selected)
            return true;
    }
    return false;    
}

void VertexEdge::turn()
{
    if (_triangles[0] == NULL || _triangles[1] == NULL)
        return;
    
    Triangle2 &t0 = _triangles[0]->data();
    Triangle2 &t1 = _triangles[1]->data();
    
    VertexNode *v0 = t0.vertexNotInEdge(this);
    VertexNode *v1 = t1.vertexNotInEdge(this);
    
    //    Vector3D t0_normal = t0.computeNormal();
    //    Vector3D t1_normal = t1.computeNormal();
    
    _triangles[0]->removeFromVertices();
    _triangles[1]->removeFromVertices();
    
    t0.setVertex(0, v1);
    t0.setVertex(1, v0);
    t0.setVertex(2, _vertices[0]);
    
    _triangles[0]->addToVertices();
    
    //    if (t0.computeNormal().Dot(t0_normal) < 0.0f)
    //        t0.flip();
    
    t1.setVertex(0, v0);
    t1.setVertex(1, v1);
    t1.setVertex(2, _vertices[1]);
    
    _triangles[1]->addToVertices();
    
    //    if (t1.computeNormal().Dot(t1_normal) < 0.0f)
    //        t1.flip();
    
    _vertices[0] = v0;
    _vertices[1] = v1;
}

VertexNode *VertexEdge::opposite(VertexNode *vertex) const
{
    if (_vertices[0] == vertex)
        return _vertices[1];
    
    return _vertices[0];
}
