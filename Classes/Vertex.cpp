//
//  Vertex.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#include "MeshHelpers.h"

void VertexNode::addTriangle(TriangleNode *triangle)
{
    _triangles.add(triangle);
}

void VertexNode::removeTriangle(TriangleNode *triangle)
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (node->data() == triangle)
            _triangles.remove(node);
    }
}

void VertexNode::removeFromTriangles()
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        node->data()->data().removeVertex(this);
    }
    
    _triangles.removeAll();
}

void VertexNode::addEdge(VertexEdgeNode *edge)
{
    _edges.add(edge);
}

void VertexNode::removeEdge(VertexEdgeNode *edge)
{
    for (SimpleNode<VertexEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data() == edge)
            _edges.remove(node);
    }
}

void VertexNode::removeEdges()
{
    _edges.removeAll();
}

void VertexNode::removeFromEdges()
{
    for (SimpleNode<VertexEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        node->data()->data().removeVertex(this);
    }
    
    _edges.removeAll();
}

void VertexNode::replaceVertex(VertexNode *newVertex)
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        node->data()->replaceVertex(this, newVertex);
    }
    
    for (SimpleNode<VertexEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        node->data()->replaceVertex(this, newVertex);
    }
    
    _triangles.removeAll();
    _edges.removeAll();
}

void VertexNode::replaceVertexInSelectedTriangles(VertexNode *newVertex)
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (node->data()->data().selected)
        {
            node->data()->replaceVertex(this, newVertex);
            _triangles.remove(node);
        }
    }    
}

void VertexNode::computeNormal()
{
    float count = 0;
    normal = Vector3D();
    
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        normal += node->data()->data().vertexNormal;
        count++;
    }
    
    normal /= count;
}

VertexEdgeNode *VertexNode::sharedEdge(VertexNode *otherVertex)
{
    for (SimpleNode<VertexEdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data()->data().containsVertex(otherVertex))
            return node->data();
    }
    return NULL;
}
