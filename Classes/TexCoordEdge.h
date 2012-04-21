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
    TriangleNode *_triangles[2];
    TexCoordNode *_texCoords[2];
public:
    bool selected;
    bool visible;
    TexCoordNode *halfTexCoord;
    
    TexCoordEdge();
    TexCoordEdge(TexCoordNode *texCoords[2]);
    
    bool isQuadEdge() const;
    bool isDegenerated() const;
    bool containsTexCoord(const TexCoordNode *texCoord) const;
    bool isNotShared() const;
    
    TriangleNode *triangle(int index) const { return _triangles[index]; }
    TexCoordNode *texCoord(int index) const { return _texCoords[index]; }
    
    void setTriangle(int index, TriangleNode *value) { _triangles[index] = value; }
    void removeTriangle(TriangleNode *triangle);
    void removeTexCoord(TexCoordNode *texCoord);
    void turn();
    
    TexCoordNode *opposite(TexCoordNode *texCoord) const;
    
    friend class TexCoordEdgeNode;
};

class TexCoordEdgeNode : public FPNode<TexCoordEdgeNode, TexCoordEdge>
{
public:
    TexCoordEdgeNode() : FPNode<TexCoordEdgeNode, TexCoordEdge>() { }
    TexCoordEdgeNode(const TexCoordEdge &edge) : FPNode<TexCoordEdgeNode, TexCoordEdge>(edge)
    {
        addToTexCoords();
    }
    virtual ~TexCoordEdgeNode()
    {
        removeFromTexCoords();
        removeFromTriangles();
    }
    virtual void setData(const TexCoordEdge &data) 
    { 
        FPNode::setData(data);
        addToTexCoords();
    }
    void addToTexCoords();
    void removeFromTexCoords();
    void removeFromTriangles();
    void replaceTexCoord(TexCoordNode *currentTexCoord, TexCoordNode *newTexCoord);
};
