//
//  VertexEdge.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#pragma once

class VertexEdge
{
private:
    VertexNode *_vertices[2];
    TriangleNode *_triangles[2];
public:
    bool selected;
    VertexNode *halfVertex;
    
    VertexEdge();
    VertexEdge(VertexNode *vertices[2]);
    
    bool isQuadEdge() const;
    bool isDegenerated() const;
    bool containsVertex(const VertexNode *vertex) const;
    bool isNotShared() const;
    
    VertexNode *vertex(int index) const { return _vertices[index]; }
    TriangleNode *triangle(int index) const { return _triangles[index]; }
    
    void setTriangle(int index, TriangleNode *value) { _triangles[index] = value; }
    void removeVertex(VertexNode *vertex);
    void removeTriangle(TriangleNode *triangle);
    void turn();
    
    VertexNode *opposite(VertexNode *vertex) const;
    
    friend class VertexEdgeNode;
};

class VertexEdgeNode : public FPNode<VertexEdgeNode, VertexEdge>
{
public:
    VertexEdgeNode() : FPNode<VertexEdgeNode, VertexEdge>() { }
    VertexEdgeNode(const VertexEdge &edge) : FPNode<VertexEdgeNode, VertexEdge>(edge)
    {
        addToVertices();
    }
    virtual ~VertexEdgeNode()
    {
        removeFromVertices();
        removeFromTriangles();
    }
    
    void addToVertices();
    void removeFromVertices();
    void removeFromTriangles();
    void replaceVertex(VertexNode *currentVertex, VertexNode *newVertex);
};

