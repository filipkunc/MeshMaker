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
    
    PackedNode _nodes[4];
    bool _isQuad;
    
    const PackedNode &node(uint index) const;
    PackedNode &node(uint index);
public:
    bool selected;
    bool visible;
    bool normalsAreValid;
    Vector3D vertexNormal;
    Vector3D texCoordNormal;
    
    static const uint twoTriIndices[6];
    
    Triangle2();
    Triangle2(VertexNode *vertices[], bool isQuad=false);
    Triangle2(VertexNode *vertices[], TexCoordNode *texCoords[], bool isQuad=false);
    Triangle2(const vector<VertexNode *> &vertices);

    bool isQuad() const { return _isQuad; }
    uint count() const { return isQuad() ? 4U : 3U; }
    
    VertexNode *vertex(uint index) const { return node(index).vertex; }
    VertexEdgeNode *vertexEdge(uint index) const { return node(index).vertexEdge; }
    TexCoordNode *texCoord(uint index) const { return node(index).texCoord; }
    TexCoordEdgeNode *texCoordEdge(uint index) const { return node(index).texCoordEdge; }
    
    void setVertex(uint index, VertexNode *value) { node(index).vertex = value; }
    void setTexCoord(uint index, TexCoordNode *value) { node(index).texCoord = value; }
    void setTexCoordByVertex(TexCoordNode * texCoord, VertexNode *vertex);
    void setVertexEdge(uint index, VertexEdgeNode *value) { node(index).vertexEdge = value; }
    void setTexCoordEdge(uint index, TexCoordEdgeNode *value) { node(index).texCoordEdge = value; }
    
    void removeVertex(TexCoordNode *texCoord) { removeTexCoord(texCoord); }
    void removeEdge(VertexEdgeNode *edge) { removeVertexEdge(edge); }
    void removeEdge(TexCoordEdgeNode *edge) { removeTexCoordEdge(edge); }
    
    TexCoordNode *vertexNotInEdge(const TexCoordEdge *edge) const { return texCoordNotInEdge(edge); }
    
    void removeVertex(VertexNode *vertex);
    void removeTexCoord(TexCoordNode *texCoord);
    void removeVertexEdge(VertexEdgeNode *edge);
    void removeTexCoordEdge(TexCoordEdgeNode *edge);
    void removeEdges();
    
    bool isDegeneratedAfterCollapseToTriangle();
    bool containsVertex(const VertexNode *vertex) const;
    bool containsVertexEdge(const VertexEdgeNode *edge) const;
    bool containsTexCoordEdge(const TexCoordEdgeNode *edge) const;
    void flip();
    
    uint indexOfVertex(const VertexNode *vertex) const;
    bool shouldSwapVertices(const VertexNode *v1, const VertexNode *v2) const;
    VertexNode *vertexNotInEdge(const VertexEdge *edge) const;
    TexCoordNode *texCoordNotInEdge(const TexCoordEdge *edge) const;
    
    VertexEdgeNode *findSecondSplitEdgeNode(const VertexEdge &firstEdge) const;
    VertexEdgeNode *nextEdgeInQuadLoop(const VertexEdge &edge) const;
    
    void computeNormalsIfNeeded();
    bool rayIntersect(const Vector3D &origin, const Vector3D &direction, float &u, float &v, Vector3D &intersect);
    void convertBarycentricToUVs(float &u, float &v);
    
    friend class TriangleNode;
};

class TriangleNode : public FPNode<TriangleNode, Triangle2>
{
public:
    float selectionWeight;
    
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
    
    void softSelect(const vector<float> &weights);
};

