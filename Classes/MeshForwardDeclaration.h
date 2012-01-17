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

struct Triangle
{
	uint vertexIndices[3];
    uint texCoordIndices[3];
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
class TexCoordEdge;

class VertexNode;
class TriangleNode;
class VertexEdgeNode;
class TexCoordNode;
class TexCoordEdgeNode;
