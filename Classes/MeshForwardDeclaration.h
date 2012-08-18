//
//  MeshForwardDeclaration.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#import "Enums.h"
#import "MathDeclaration.h"
#import "FPArrayCache.h"
#import "SimpleNodeAndList.h"
#import <vector>
using namespace std;

enum ModelVersion : uint
{
    ModelVersionFirst = 1U,
    ModelVersionColors = 2U,
    ModelVersionTriQuads = 3U,
    
    ModelVersionLatest = ModelVersionTriQuads
};

struct Triangle
{
    uint vertexIndices[3];
    uint texCoordIndices[3];
};

struct TriQuad
{
	uint vertexIndices[4];
    uint texCoordIndices[4];
    bool isQuad;
};

struct Edge
{
	uint vertexIndices[2];
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
