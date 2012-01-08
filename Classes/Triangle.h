//
//  Triangle.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#pragma once

class Triangle2
{
private:
    VertexNode *_vertices[3];
    VertexEdgeNode *_edges[3];
    TexCoordNode *_TexCoords[3];
public:
    bool selected;
    Vector3D normal;
    
    Triangle2();
    Triangle2(VertexNode *vertices[3]);
    Triangle2(VertexNode *vertices[3], TexCoordNode *texutreCoordinates[3]);
    
    VertexNode *vertex(int index) const { return _vertices[index]; }
    VertexEdgeNode *edge(int index) const { return _edges[index]; }
    TexCoordNode *texCoord(int index) const { return _TexCoords[index]; }
    
    void setVertex(int index, VertexNode *value) { _vertices[index] = value; }
    void setEdge(int index, VertexEdgeNode *value) { _edges[index] = value; }
    
    void removeVertex(VertexNode *vertex);
    void removeTexCoord(TexCoordNode *TexCoord);
    void removeEdge(VertexEdgeNode *edge);
    void removeEdges();
    
    bool isDegenerated() const;
    bool containsVertex(const VertexNode *vertex) const;
    bool containsEdge(const VertexEdgeNode *edge) const;
    void flip();
    
    int indexOfVertex(const VertexNode *vertex) const;
    void sortVertices(VertexNode *&v1, VertexNode *&v2) const;
    VertexNode *vertexNotInEdge(const VertexEdge *edge) const;
    
    void computeNormal();
    
    friend class TriangleNode;
};

class TriangleNode : public FPNode<TriangleNode, Triangle2>
{
public:
    TriangleNode() : FPNode<TriangleNode, Triangle2>() { }
    TriangleNode(const Triangle2 &triangle) : FPNode<TriangleNode, Triangle2>(triangle)
    {
        addToVertices();
    }
    virtual ~TriangleNode()
    {
        removeFromVertices();
        removeFromEdges();
    }
    
    void addToVertices();
    void removeFromVertices();
    void removeFromEdges();
    void replaceVertex(VertexNode *currentVertex, VertexNode *newVertex);
};
