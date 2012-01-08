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
        if (node->data == triangle)
            _triangles.remove(node);
    }
}

void TexCoordNode::removeFromTriangles()
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        node->data->data.removeTexCoord(this);
    }
    
    _triangles.removeAll();
}

void TexCoordNode::addEdge(VertexEdgeNode *edge)
{
    _edges.add(edge);
}

void TexCoordNode::removeEdge(VertexEdgeNode *edge)
{
    for (SimpleNode<VertexEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data == edge)
            _edges.remove(node);
    }
}

void TexCoordNode::removeEdges()
{
    _edges.removeAll();
}

void TexCoordNode::removeFromEdges()
{
    for (SimpleNode<VertexEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        node->data->data.removeTexCoord(this);
    }
    
    _edges.removeAll();
}

