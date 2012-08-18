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
    struct PackedNode
    {
        VertexNode *vertex;
        VertexEdgeNode *vertexEdge;
        TexCoordNode *texCoord;
        TexCoordEdgeNode *texCoordEdge;
        
        PackedNode() : vertex(NULL), vertexEdge(NULL), texCoord(NULL), texCoordEdge(NULL) { }
    };
    
    PackedNode _nodes[3];
public:
    bool selected;
    bool visible;
    Vector3D vertexNormal;
    Vector3D texCoordNormal;
    
    Triangle2();
    Triangle2(VertexNode *vertices[3]);
    Triangle2(VertexNode *vertices[3], TexCoordNode *texCoords[3]);
    
    int count() const { return 3; }
    
    VertexNode *vertex(int index) const { return _nodes[index].vertex; }
    VertexEdgeNode *vertexEdge(int index) const { return _nodes[index].vertexEdge; }
    TexCoordNode *texCoord(int index) const { return _nodes[index].texCoord; }
    TexCoordEdgeNode *texCoordEdge(int index) const { return _nodes[index].texCoordEdge; }
    
    void setVertex(int index, VertexNode *value) { _nodes[index].vertex = value; }
    void setTexCoord(int index, TexCoordNode *value) { _nodes[index].texCoord = value; }
    void setTexCoordByVertex(TexCoordNode * texCoord, VertexNode *vertex);
    void setVertexEdge(int index, VertexEdgeNode *value) { _nodes[index].vertexEdge = value; }
    void setTexCoordEdge(int index, TexCoordEdgeNode *value) { _nodes[index].texCoordEdge = value; }
    
    void removeVertex(TexCoordNode *texCoord) { removeTexCoord(texCoord); }
    void removeEdge(VertexEdgeNode *edge) { removeVertexEdge(edge); }
    void removeEdge(TexCoordEdgeNode *edge) { removeTexCoordEdge(edge); }
    
    TexCoordNode *vertexNotInEdge(const TexCoordEdge *edge) const { return texCoordNotInEdge(edge); }
    
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
    TexCoordNode *texCoordNotInEdge(const TexCoordEdge *edge) const;
    
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
    
    void replaceVertex(TexCoordNode *currentTexCoord, TexCoordNode *newTexCoord) { replaceTexCoord(currentTexCoord, newTexCoord); }
    
    void replaceVertex(VertexNode *currentVertex, VertexNode *newVertex);
    void replaceTexCoord(TexCoordNode *currentTexCoord, TexCoordNode *newTexCoord);
};

