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
class Edge2;
class TextureCoordinate;

class VertexNode;
class TriangleNode;
class EdgeNode;
class TextureCoordinateNode;

class TextureCoordinate
{
public:
    Vector2D position;
    
    bool selected;
    
    TextureCoordinate() : selected(false) { }
    TextureCoordinate(const Vector2D &v) : position(v), selected(false) { }
};

class TextureCoordinateNode : public FPNode<TextureCoordinateNode, TextureCoordinate>
{
public:
    SimpleList<TriangleNode *> _triangles;
    SimpleList<EdgeNode *> _edges;
public:
    TextureCoordinateNode() : FPNode<TextureCoordinateNode, TextureCoordinate>() { }
    TextureCoordinateNode(const TextureCoordinate &textureCoordinate) : FPNode<TextureCoordinateNode, TextureCoordinate>(textureCoordinate) { } 
    virtual ~TextureCoordinateNode() 
    { 
        removeFromTriangles();
        removeFromEdges();
    }
    
    bool isUsed() const { return _triangles.count() > 0; }
    void addTriangle(TriangleNode *triangle);
    void removeTriangle(TriangleNode *triangle);
    void removeFromTriangles();
    void addEdge(EdgeNode *edge);
    void removeEdge(EdgeNode *edge);
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
    SimpleList<EdgeNode *> _edges;
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
    void addEdge(EdgeNode *edge);
    void removeEdge(EdgeNode *edge);
    void removeEdges();
    void removeFromEdges();
    void replaceVertex(VertexNode *newVertex);
    void replaceVertexInSelectedTriangles(VertexNode *newVertex);
    EdgeNode *sharedEdge(VertexNode *otherVertex);
    void computeNormal();
};

class Triangle2
{
private:
    VertexNode *_vertices[3];
    EdgeNode *_edges[3];
    TextureCoordinateNode *_textureCoordinates[3];
public:
    bool selected;
    Vector3D normal;
    
    Triangle2();
    Triangle2(VertexNode *vertices[3]);
    Triangle2(VertexNode *vertices[3], TextureCoordinateNode *texutreCoordinates[3]);

    VertexNode *vertex(int index) const { return _vertices[index]; }
    EdgeNode *edge(int index) const { return _edges[index]; }
    TextureCoordinateNode *texCoord(int index) const { return _textureCoordinates[index]; }
    
    void setVertex(int index, VertexNode *value) { _vertices[index] = value; }
    void setEdge(int index, EdgeNode *value) { _edges[index] = value; }
    
    void removeVertex(VertexNode *vertex);
    void removeTextureCoordinate(TextureCoordinateNode *textureCoordinate);
    void removeEdge(EdgeNode *edge);
    void removeEdges();

    bool isDegenerated() const;
    bool containsVertex(const VertexNode *vertex) const;
    bool containsEdge(const EdgeNode *edge) const;
    void flip();
    
    int indexOfVertex(const VertexNode *vertex) const;
    void sortVertices(VertexNode *&v1, VertexNode *&v2) const;
    VertexNode *vertexNotInEdge(const Edge2 *edge) const;
    
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

class Edge2
{
private:
    VertexNode *_vertices[2];
    TriangleNode *_triangles[2];
    TextureCoordinateNode *_textureCoordinates[2];
public:
    bool selected;
    VertexNode *halfVertex;
    
    Edge2();
    Edge2(VertexNode *vertices[2]);
    
    bool isQuadEdge() const;
    bool isDegenerated() const;
    bool containsVertex(const VertexNode *vertex) const;
    bool isNotShared() const;
    
    VertexNode *vertex(int index) const { return _vertices[index]; }
    TriangleNode *triangle(int index) const { return _triangles[index]; }
    TextureCoordinateNode *texCoord(int index) const { return _textureCoordinates[index]; }
    
    void setTriangle(int index, TriangleNode *value) { _triangles[index] = value; }
    void removeVertex(VertexNode *vertex);
    void removeTriangle(TriangleNode *triangle);
    void removeTextureCoordinate(TextureCoordinateNode *textureCoordinate);
    void turn();
    
    VertexNode *opposite(VertexNode *vertex) const;
    
    friend class EdgeNode;
};

class EdgeNode : public FPNode<EdgeNode, Edge2>
{
public:
    EdgeNode() : FPNode<EdgeNode, Edge2>() { }
    EdgeNode(const Edge2 &edge) : FPNode<EdgeNode, Edge2>(edge)
    {
        addToVertices();
    }
    virtual ~EdgeNode()
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
