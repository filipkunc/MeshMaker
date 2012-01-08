/*
 *  MeshHelpers.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#import "Enums.h"
#import "MathDeclaration.h"
#import "FPArrayCache.h"
#import "SimpleNodeAndList.h"
#import <vector>
using namespace std;

struct Triangle
{
	uint vertexIndices[3];
};

struct Edge
{
	uint vertexIndices[2];
};

struct SelectionInfo
{
    bool selected;
    bool visible;
};

class Vertex2;
class Triangle2;
class VertexEdge;
class TexCoord;

class VertexNode;
class TriangleNode;
class VertexEdgeNode;
class TexCoordNode;

class TexCoord
{
public:
    Vector2D position;
    
    bool selected;
    
    TexCoord() : selected(false) { }
    TexCoord(const Vector2D &v) : position(v), selected(false) { }
};

class TexCoordNode : public FPNode<TexCoordNode, TexCoord>
{
public:
    SimpleList<TriangleNode *> _triangles;
    SimpleList<VertexEdgeNode *> _edges;
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
    void addEdge(VertexEdgeNode *edge);
    void removeEdge(VertexEdgeNode *edge);
    void removeEdges();
    void removeFromEdges();   
};

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

class VertexEdge
{
private:
    VertexNode *_vertices[2];
    TriangleNode *_triangles[2];
    TexCoordNode *_TexCoords[2];
public:
    bool selected;
    VertexNode *halfVertex;
    TexCoordNode *halfTexCoord;
    
    VertexEdge();
    VertexEdge(VertexNode *vertices[2], TexCoordNode *texCoords[2]);
    
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

void AddTriangle(vector<Triangle> &triangles, int index1, int index2, int index3);
void AddQuad(vector<Triangle> &triangles, int index1, int index2, int index3, int index4);
