//
//  TexCoordEdge.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#pragma once

class TexCoordEdge
{
private:
    VertexNode *_vertices[2];
    TriangleNode *_triangles[2];
    TexCoordNode *_TexCoords[2];
public:
    bool selected;
    VertexNode *halfVertex;
    TexCoordNode *halfTexCoord;
    
    TexCoordEdge();
    TexCoordEdge(VertexNode *vertices[2], TexCoordNode *texCoords[2]);
    
    bool isQuadEdge() const;
    bool isDegenerated() const;
    bool containsVertex(const VertexNode *vertex) const;
    bool isNotShared() const;
    
    VertexNode *vertex(int index) const { return _vertices[index]; }
    TriangleNode *triangle(int index) const { return _triangles[index]; }
    TexCoordNode *texCoord(int index) const { return _TexCoords[index]; }
    
    void setTriangle(int index, TriangleNode *value) { _triangles[index] = value; }
    void removeVertex(VertexNode *vertex);
    void removeTriangle(TriangleNode *triangle);
    void removeTexCoord(TexCoordNode *TexCoord);
    void turn();
    
    VertexNode *opposite(VertexNode *vertex) const;
    
    friend class TexCoordEdgeNode;
};

class TexCoordEdgeNode : public FPNode<TexCoordEdgeNode, TexCoordEdge>
{
public:
    TexCoordEdgeNode() : FPNode<TexCoordEdgeNode, TexCoordEdge>() { }
    TexCoordEdgeNode(const TexCoordEdge &edge) : FPNode<TexCoordEdgeNode, TexCoordEdge>(edge)
    {
        addToVertices();
    }
    virtual ~TexCoordEdgeNode()
    {
        removeFromVertices();
        removeFromTriangles();
    }
    
    void addToVertices();
    void removeFromVertices();
    void removeFromTriangles();
    void replaceVertex(VertexNode *currentVertex, VertexNode *newVertex);
};
