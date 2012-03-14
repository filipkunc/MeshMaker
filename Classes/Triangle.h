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
    Vector3D vertexNormal;
    Vector3D texCoordNormal;
    
    Triangle2();
    Triangle2(VertexNode *vertices[3]);
    Triangle2(VertexNode *vertices[3], TexCoordNode *texCoords[3]);
    
    VertexNode *vertex(int index) const { return _vertices[index]; }
    VertexEdgeNode *vertexEdge(int index) const { return _vertexEdges[index]; }
    TexCoordNode *texCoord(int index) const { return _texCoords[index]; }
    TexCoordEdgeNode *texCoordEdge(int index) const { return _texCoordEdges[index]; }
    
    void setVertex(int index, VertexNode *value) { _vertices[index] = value; }
    void setTexCoordByVertex(TexCoordNode * texCoord, VertexNode *vertex);
    void setVertexEdge(int index, VertexEdgeNode *value) { _vertexEdges[index] = value; }
    void setTexCoordEdge(int index, TexCoordEdgeNode *value) { _texCoordEdges[index] = value; }
    
    void removeVertex(VertexNode *vertex);
    void removeTexCoord(TexCoordNode *texCoord);
    void removeVertexEdge(VertexEdgeNode *edge);
    void removeTexCoordEdge(TexCoordEdgeNode *edge);
    void removeEdges();
    
    bool isDegenerated() const;
    bool containsVertex(const VertexNode *vertex) const;
    bool containsVertexEdge(const VertexEdgeNode *edge) const;
    bool containsTexCoordEdge(const TexCoordEdgeNode *edge) const;
    void flip();
    
    int indexOfVertex(const VertexNode *vertex) const;
    void sortVertices(VertexNode *&v1, VertexNode *&v2) const;
    VertexNode *vertexNotInEdge(const VertexEdge *edge) const;
    TexCoordEdgeNode *texCoordNotInEdge(const TexCoordEdge *edge) const;
    
    void computeNormal();
    bool rayIntersect(const Vector3D &origin, const Vector3D &direction, float &u, float &v, Vector3D &intersect);
    void convertBarycentricToUVs(float &u, float &v);
    
    friend class TriangleNode;
};

class TriangleNode : public FPNode<TriangleNode, Triangle2>
{
public:
    TriangleNode() : FPNode<TriangleNode, Triangle2>() { }
    TriangleNode(const Triangle2 &triangle) : FPNode<TriangleNode, Triangle2>(triangle)
    {
        addToVertices();
        addToTexCoords();
    }
    virtual ~TriangleNode()
    {
        removeFromVertices();
        removeFromTexCoords();
        removeFromEdges();
    }    
    virtual void setData(const Triangle2 &data) 
    { 
        FPNode::setData(data);
        addToVertices();
        addToTexCoords();
    }    
    void addToVertices();
    void addToTexCoords();
    void removeFromVertices();
    void removeFromTexCoords();
    void removeFromEdges();
    void replaceVertex(VertexNode *currentVertex, VertexNode *newVertex);
    void replaceTexCoord(TexCoordNode *currentTexCoord, TexCoordNode *newTexCoord);
};
