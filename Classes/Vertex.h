//
//  Vertex.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#pragma once

class Vertex2
{
public:
    Vector3D position;
    
    bool selected;
    
    Vertex2() : selected(false) { }
    Vertex2(const Vector3D &v) : position(v), selected(false) { }    
};

class VertexNode : public FPNode<VertexNode, Vertex2>
{
public:
    SimpleList<TriangleNode *> _triangles;
    SimpleList<VertexEdgeNode *> _edges;
public:
    int index;
    Vector3D normal;
    float selectionWeight;
    
    VertexNode() : FPNode<VertexNode, Vertex2>() { }
    VertexNode(const Vertex2 &vertex) : FPNode<VertexNode, Vertex2>(vertex) { } 
    virtual ~VertexNode() 
    { 
        removeFromTriangles();
        removeFromEdges();
    }
    
    bool isUsed() const { return _triangles.count() > 0; }
    void addTriangle(TriangleNode *triangle);
    void removeTriangle(TriangleNode *triangle);
    void removeFromTriangles();
    void addEdge(VertexEdgeNode *edge);
    void removeEdge(VertexEdgeNode *edge);
    void removeEdges();
    void removeFromEdges();
    void replaceVertex(VertexNode *newVertex);
    void replaceVertexInSelectedTriangles(VertexNode *newVertex);
    VertexEdgeNode *sharedEdge(VertexNode *otherVertex);
    void computeNormal();
};