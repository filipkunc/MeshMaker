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
    VertexEdgeNode *_vertexEdges[3];
    TexCoordNode *_texCoords[3];
    TexCoordEdgeNode *_texCoordEdges[3];
public:
    bool selected;
    Vector3D normal;
    
    Triangle2();
    Triangle2(VertexNode *vertices[3]);
    Triangle2(VertexNode *vertices[3], TexCoordNode *texCoords[3]);
    
    VertexNode *vertex(int index) const { return _vertices[index]; }
    VertexEdgeNode *vertexEdge(int index) const { return _vertexEdges[index]; }
    TexCoordNode *texCoord(int index) const { return _texCoords[index]; }
    TexCoordEdgeNode *texCoordEdge(int index) const { return _texCoordEdges[index]; }
    
    void setVertex(int index, VertexNode *value) { _vertices[index] = value; }
    void setEdge(int index, VertexEdgeNode *value) { _vertexEdges[index] = value; }
    
    void removeVertex(VertexNode *vertex);
    void removeTexCoord(TexCoordNode *texCoord);
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
        addToVerticesAndTexCoords();
    }
    virtual ~TriangleNode()
    {
        removeFromVerticesAndTexCoords();
        removeFromEdges();
    }
    
    void addToVerticesAndTexCoords();
    void removeFromVerticesAndTexCoords();
    void removeFromEdges();
    void replaceVertex(VertexNode *currentVertex, VertexNode *newVertex);
};
