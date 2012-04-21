//
//  TexCoordEdge.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#include "MeshHelpers.h"

TexCoordEdge::TexCoordEdge() : selected(false), visible(true), halfTexCoord(NULL)
{
    for (int i = 0; i < 2; i++)
    {
        _texCoords[i] = NULL;
        _triangles[i] = NULL;
    }
}

TexCoordEdge::TexCoordEdge(TexCoordNode *texCoords[2]) : selected(false), visible(true), halfTexCoord(NULL)
{
    for (int i = 0; i < 2; i++)
    {
        _texCoords[i] = texCoords[i];
        _triangles[i] = NULL;
    }
}

bool TexCoordEdge::isQuadEdge() const
{
    if (_triangles[0] != NULL && _triangles[1] != NULL)
    {
        Triangle2 &t0 = _triangles[0]->data();
        Triangle2 &t1 = _triangles[1]->data();
        
        TexCoordNode *otherTexCoords[2];
        otherTexCoords[0] = t0.texCoordNotInEdge(this);
        otherTexCoords[1] = t1.texCoordNotInEdge(this);
        
        float squaredDistanceInEdge = _texCoords[0]->data().position.SqDistance(_texCoords[1]->data().position);
        float sqDistances[4];
        
        for (int i = 0; i < 2; i++)
        {
            sqDistances[i] = otherTexCoords[i]->data().position.SqDistance(_texCoords[0]->data().position);
            sqDistances[i + 2] = otherTexCoords[i]->data().position.SqDistance(_texCoords[1]->data().position); 
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

bool TexCoordEdge::isDegenerated() const
{
    if (_triangles[0] == NULL && _triangles[1] == NULL)
        return true;
    
    if (containsTexCoord(NULL))
        return true;
    
    if (_texCoords[0] == _texCoords[1])
        return true;
    
    return false;    
}

bool TexCoordEdge::containsTexCoord(const TexCoordNode *texCoord) const
{
    for (int i = 0; i < 2; i++)
    {
        if (_texCoords[i] == texCoord)
            return true;        
    }
    return false;
}

void TexCoordEdgeNode::addToTexCoords()
{
    for (int i = 0; i < 2; i++)
    {
        if (data()._texCoords[i])
            data()._texCoords[i]->addEdge(this);
    }
}

void TexCoordEdgeNode::removeFromTexCoords()
{
    for (int i = 0; i < 2; i++)
    {
        if (data()._texCoords[i])
        {
            data()._texCoords[i]->removeEdge(this);
            data()._texCoords[i] = NULL;
        }
    }
}

void TexCoordEdgeNode::removeFromTriangles()
{
    for (int i = 0; i < 2; i++)
    {
        if (data()._triangles[i])
        {
            data()._triangles[i]->data().removeTexCoordEdge(this);
            data()._triangles[i] = NULL;
        }
    }   
}

void TexCoordEdge::removeTexCoord(TexCoordNode *texCoord)
{
    for (int i = 0; i < 2; i++)
    {
        if (_texCoords[i] == texCoord)
        {
            _texCoords[i] = NULL;
            break;
        }
    }
}

void TexCoordEdge::removeTriangle(TriangleNode *triangle)
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

void TexCoordEdgeNode::replaceTexCoord(TexCoordNode *currentTexCoord, TexCoordNode *newTexCoord)
{
    for (int i = 0; i < 2; i++)
    {
        if (data()._texCoords[i] == currentTexCoord)
        {
            data()._texCoords[i] = newTexCoord;
            newTexCoord->addEdge(this);
            break;
        }
    }   
}

bool TexCoordEdge::isNotShared() const
{
    for (int i = 0; i < 2; i++)
    {
        if (_triangles[i] == NULL || !_triangles[i]->data().selected)
            return true;
    }
    return false;    
}

void TexCoordEdge::turn()
{
    if (_triangles[0] == NULL || _triangles[1] == NULL)
        return;
    
    Triangle2 &t0 = _triangles[0]->data();
    Triangle2 &t1 = _triangles[1]->data();
    
    TexCoordNode *v0 = t0.texCoordNotInEdge(this);
    TexCoordNode *v1 = t1.texCoordNotInEdge(this);
    
    _triangles[0]->removeFromVertices();
    _triangles[1]->removeFromVertices();
    
    t0.setTexCoord(0, v1);
    t0.setTexCoord(1, v0);
    t0.setTexCoord(2, _texCoords[0]);
    
    _triangles[0]->addToVertices();
    
    t1.setTexCoord(0, v0);
    t1.setTexCoord(1, v1);
    t1.setTexCoord(2, _texCoords[1]);
    
    _triangles[1]->addToVertices();
    
    _texCoords[0] = v0;
    _texCoords[1] = v1;
}

TexCoordNode *TexCoordEdge::opposite(TexCoordNode *texCoord) const
{
    if (_texCoords[0] == texCoord)
        return _texCoords[1];
    
    return _texCoords[0];
}
