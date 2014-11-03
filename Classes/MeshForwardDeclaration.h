//
//  MeshForwardDeclaration.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#pragma once

#include "Enums.h"
#include "Exceptions.h"
#include "MathDeclaration.h"
#include "FPArrayCache.h"
#include "SimpleNodeAndList.h"
#include <vector>
using namespace std;

struct Triangle
{
    unsigned int vertexIndices[3];
    unsigned int texCoordIndices[3];
};

struct TriQuad
{
	unsigned int vertexIndices[4];
    unsigned int texCoordIndices[4];
    bool isQuad;
};

struct Edge
{
	unsigned int vertexIndices[2];
};

class Vertex2
{
public:
    Vector3D position;
    
    bool selected;
    bool visible;
    
    Vertex2() : selected(false), visible(true) { }
    Vertex2(const Vector3D &v) : position(v), selected(false), visible(true) { }    
};

class TexCoord
{
public:
    Vector3D position;
    
    bool selected;
    bool visible;
    
    TexCoord() : selected(false), visible(true) { }
    TexCoord(const Vector3D &v) : position(v), selected(false), visible(true) { }
};

template <class T>
class VNode;

template <class T>
class VEdge;

template <class T>
class VEdgeNode;

typedef VNode<Vertex2> VertexNode;
typedef VEdge<Vertex2> VertexEdge;
typedef VEdgeNode<Vertex2> VertexEdgeNode;

typedef VNode<TexCoord> TexCoordNode;
typedef VEdge<TexCoord> TexCoordEdge;
typedef VEdgeNode<TexCoord> TexCoordEdgeNode;
    
class Triangle2;
class TriangleNode;
    
class VertexTriangleNode : public FPNode<VertexTriangleNode, TriangleNode *>
{
public:
    int cacheIndices[2];
    
    VertexTriangleNode() : FPNode<VertexTriangleNode, TriangleNode *>()
    {
        cacheIndices[0] = -1;
        cacheIndices[1] = -1;
    }
    VertexTriangleNode(TriangleNode* const &data) : FPNode<VertexTriangleNode, TriangleNode *>(data)
    {
        cacheIndices[0] = -1;
        cacheIndices[1] = -1;
    }
    virtual ~VertexTriangleNode() { }
};

template <class T>
class VertexVEdgeNode : public FPNode<VertexVEdgeNode<T>, VEdgeNode<T> *>
{
public:
    int cacheIndex;
    
    VertexVEdgeNode() : FPNode<VertexVEdgeNode<T>, VEdgeNode<T> *>(), cacheIndex(-1) { }
    VertexVEdgeNode(VEdgeNode<T> * const &data) : FPNode<VertexVEdgeNode<T>, VEdgeNode<T> *>(data), cacheIndex(-1) { }
    virtual ~VertexVEdgeNode() { }
};
    
typedef VertexVEdgeNode<Vertex2> Vertex2VEdgeNode;
typedef VertexVEdgeNode<TexCoord> TexCoordVEdgeNode;

