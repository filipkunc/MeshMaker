//
//  TexCoord.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#pragma once

class TexCoord
{
public:
    Vector3D position;
    
    bool selected;
    
    TexCoord() : selected(false) { }
    TexCoord(const Vector3D &v) : position(v), selected(false) { }
};

class TexCoordNode : public FPNode<TexCoordNode, TexCoord>
{
public:
    SimpleList<TriangleNode *> _triangles;
    SimpleList<TexCoordEdgeNode *> _edges;
    int index;
    Vector3D normal;
public:
    TexCoordNode() : FPNode<TexCoordNode, TexCoord>() { }
    TexCoordNode(const TexCoord &texCoord) : FPNode<TexCoordNode, TexCoord>(texCoord) { } 
    virtual ~TexCoordNode() 
    { 
        removeFromTriangles();
        removeFromEdges();
    }
    
    bool isUsed() const { return _triangles.count() > 0; }
    void addTriangle(TriangleNode *triangle);
    void removeTriangle(TriangleNode *triangle);
    void removeFromTriangles();
    void addEdge(TexCoordEdgeNode *edge);
    void removeEdge(TexCoordEdgeNode *edge);
    void removeEdges();
    void removeFromEdges(); 
    TexCoordEdgeNode *sharedEdge(TexCoordNode *otherTexCoord);
    void replaceTexCoord(TexCoordNode *newVertex);
    void computeNormal();    
};
