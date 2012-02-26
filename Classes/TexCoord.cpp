//
//  TexCoord.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#include "MeshHelpers.h"

void TexCoordNode::addTriangle(TriangleNode *triangle)
{
    _triangles.add(triangle);
}

void TexCoordNode::removeTriangle(TriangleNode *triangle)
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (node->data() == triangle)
            _triangles.remove(node);
    }
}

void TexCoordNode::removeFromTriangles()
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        node->data()->data().removeTexCoord(this);
    }
    
    _triangles.removeAll();
}

void TexCoordNode::addEdge(TexCoordEdgeNode *edge)
{
    _edges.add(edge);
}

void TexCoordNode::removeEdge(TexCoordEdgeNode *edge)
{
    for (SimpleNode<TexCoordEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data() == edge)
            _edges.remove(node);
    }
}

void TexCoordNode::removeEdges()
{
    _edges.removeAll();
}

void TexCoordNode::removeFromEdges()
{
    for (SimpleNode<TexCoordEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        node->data()->data().removeTexCoord(this);
    }
    
    _edges.removeAll();
}

TexCoordEdgeNode *TexCoordNode::sharedEdge(TexCoordNode *otherTexCoord)
{
    for (SimpleNode<TexCoordEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data()->data().containsTexCoord(otherTexCoord))
            return node->data();
    }
    return NULL;
}

void TexCoordNode::replaceTexCoord(TexCoordNode *newTexCoord)
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        node->data()->replaceTexCoord(this, newTexCoord);
    }
    
    for (SimpleNode<TexCoordEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        node->data()->replaceTexCoord(this, newTexCoord);
    }
    
    _triangles.removeAll();
    _edges.removeAll();
}

void TexCoordNode::computeNormal()
{
    float count = 0;
    normal = Vector3D();
    
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        normal += node->data()->data().texCoordNormal;
        count++;
    }
    
    normal /= count;
}
