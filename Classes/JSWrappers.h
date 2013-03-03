//
//  JSWrappers.h
//  MeshMaker
//
//  Created by Filip Kunc on 10/27/12.
//
//

#pragma once

#include "ItemCollection.h"

#if defined (__APPLE__)

@class VertexWrapper;
@class TriangleWrapper;
@class EdgeWrapper;
@class SimpleNodeEdgeWrapper;
@class VertexNodeIterator;
@class TriangleNodeIterator;
@class EdgeNodeIterator;
@class VertexNodeEdgeIterator;
@class ItemWrapper;

@interface ItemCollectionWrapper : NSObject
{
    ItemCollection *_itemCollection;
}

- (id)initWithItemCollection:(ItemCollection *)itemCollection;

@property (readonly) uint count;

- (ItemWrapper *)at:(uint)index;

@end

@interface ItemWrapper : NSObject
{
    Item *_item;
}

- (id)initWithItem:(Item *)item;

@property (readonly) VertexNodeIterator *vertexIterator;
@property (readonly) TriangleNodeIterator *triQuadIterator;
@property (readonly) EdgeNodeIterator *edgeIterator;

@property (readonly) uint vertexCount;
@property (readonly) uint triQuadCount;
@property (readwrite, assign) BOOL selected;

- (VertexWrapper *)addVertexWithX:(float)x y:(float)y z:(float)z;
- (TriangleWrapper *)addTriangleWithFirst:(VertexWrapper *)v0 second:(VertexWrapper *)v1 third:(VertexWrapper *)v2;
- (TriangleWrapper *)addQuadWithFirst:(VertexWrapper *)v0 second:(VertexWrapper *)v1 third:(VertexWrapper *)v2 fourth:(VertexWrapper *)v3;
- (void)removeTriQuad:(TriangleWrapper *)triQuad;

- (void)removeDegeneratedTriangles;
- (void)removeNonUsedVertices;
- (void)removeNonUsedTexCoords;
- (void)mergeSelected;
- (void)splitSelected;
- (void)detachSelected;
- (void)duplicateSelectedTriangles;
- (void)flipSelected;
- (void)flipAllTriangles;
- (void)extrudeSelected;
- (void)triangulate;
- (void)triangulateSelectedQuads;
- (void)openSubdivision;
- (void)loopSubdivision;
- (void)makeTexCoords;
- (void)makeEdges;
- (void)updateSelection;
- (void)setSelectionModeVertices;
- (void)setSelectionModeTriQuads;
- (void)setSelectionModeEdges;

@end

@interface VertexWrapper : NSObject

@property (readwrite, assign) VertexNode *node;
@property (readwrite, assign) BOOL selected;
@property (readwrite, assign) float x;
@property (readwrite, assign) float y;
@property (readwrite, assign) float z;
@property (readwrite, assign) uint index;
@property (readonly) uint edgeCount;
@property (readonly) VertexNodeEdgeIterator *edgeIterator;

- (id)initWithNode:(VertexNode *)vertexNode;

@end

@interface TriangleWrapper : NSObject

@property (readwrite, assign) TriangleNode *node;
@property (readwrite, assign) BOOL selected;
@property (readonly) BOOL isQuad;
@property (readonly) uint count;

- (id)initWithNode:(TriangleNode *)triangleNode;

- (VertexWrapper *)vertexAtIndex:(uint)index;
- (void)setVertex:(VertexWrapper *)vertex atIndex:(uint)index;

- (EdgeWrapper *)edgeAtIndex:(uint)index;
- (void)setEdge:(EdgeWrapper *)edge atIndex:(uint)index;

- (VertexWrapper *)vertexNotInEdge:(EdgeWrapper *)edge;

@end

@interface EdgeWrapper : NSObject

@property (readwrite, assign) VertexEdgeNode *node;
@property (readwrite, assign) BOOL selected;
@property (readwrite, assign) VertexWrapper *half;

- (id)initWithNode:(VertexEdgeNode *)edgeNode;

- (VertexWrapper *)vertexAtIndex:(uint)index;
- (void)setVertex:(VertexWrapper *)vertex atIndex:(uint)index;

- (TriangleWrapper *)triangleAtIndex:(uint)index;
- (void)setTriangle:(TriangleWrapper *)triangle atIndex:(uint)index;

- (VertexWrapper *)oppositeVertex:(VertexWrapper *)vertex;

@end

@interface VertexNodeEdgeWrapper : EdgeWrapper
{
    Vertex2VEdgeNode *_simpleNode;
}

@property (readwrite, assign) Vertex2VEdgeNode *simpleNode;

- (id)initWithSimpleNode:(Vertex2VEdgeNode *)edgeNode;

@end

#define DeclareIterator(Name, TBase, TNode) \
@interface Name : TBase \
{ \
    TNode *begin; \
    TNode *end; \
} \
\
@property (readonly) BOOL finished; \
\
- (id)initWithBegin:(TNode *)theBegin end:(TNode *)theEnd; \
- (void)moveStart; \
- (void)moveNext; \
\
@end

DeclareIterator(VertexNodeIterator, VertexWrapper, VertexNode)
DeclareIterator(TriangleNodeIterator, TriangleWrapper, TriangleNode)
DeclareIterator(EdgeNodeIterator, EdgeWrapper, VertexEdgeNode)
DeclareIterator(VertexNodeEdgeIterator, VertexNodeEdgeWrapper, Vertex2VEdgeNode)

#elif defined(WIN32)

namespace MeshMakerCppCLI
{

using namespace System;
using namespace System::Runtime::InteropServices;

ref class VertexWrapper;
ref class TriangleWrapper;
ref class EdgeWrapper;
ref class SimpleNodeEdgeWrapper;
ref class VertexNodeIterator;
ref class TriangleNodeIterator;
ref class EdgeNodeIterator;
ref class VertexNodeEdgeIterator;
ref class ItemWrapper;

[ComVisibleAttribute(true)]
public ref class ItemCollectionWrapper
{
private:
    ItemCollection *_itemCollection;
public:
	ItemCollectionWrapper(ItemCollection *itemCollection);
	int count();
	ItemWrapper ^at(int index);
};

[ComVisibleAttribute(true)]
public ref class ItemWrapper
{
private:
    Item *_item;
public:
	ItemWrapper(Item *item);
	VertexNodeIterator ^vertexIterator();
	TriangleNodeIterator ^triQuadIterator();
	EdgeNodeIterator ^edgeIterator();

	int vertexCount();
	int triQuadCount();
	bool selected();
	void setSelected(bool selected);

	VertexWrapper ^addVertex(float x, float y, float z);
	TriangleWrapper ^addTriangle(VertexWrapper ^v0, VertexWrapper ^v1, VertexWrapper ^v2);
	TriangleWrapper ^addQuad(VertexWrapper ^v0, VertexWrapper ^v1, VertexWrapper ^v2, VertexWrapper ^v3);
	void removeTriQuad(TriangleWrapper ^triQuad);

	void removeDegeneratedTriangles();
	void removeNonUsedVertices();
	void removeNonUsedTexCoords();
	void mergeSelected();
	void splitSelected();
	void detachSelected();
	void duplicateSelectedTriangles();
	void flipSelected();
	void flipAllTriangles();
	void extrudeSelected();
	void triangulate();
	void triangulateSelectedQuads();
	void openSubdivision();
	void loopSubdivision();
	void makeTexCoords();
	void makeEdges();
	void updateSelection();
	void setSelectionModeVertices();
	void setSelectionModeTriQuads();
	void setSelectionModeEdges();
	void removeSelected();
};

[ComVisibleAttribute(true)]
public ref class VertexWrapper
{
private:
	VertexNode *_node;
public:
	VertexWrapper(VertexNode *node);

	VertexNode *node();
	void setNode(VertexNode *node);

	bool selected();
	void setSelected(bool selected);

	float x();
	void setX(float x);

	float y();
	void setY(float y);

	float z();
	void setZ(float z);

	int index();
	void setIndex(int index);

	int edgeCount();
	VertexNodeEdgeIterator ^edgeIterator();
};

[ComVisibleAttribute(true)]
public ref class TriangleWrapper
{
private:
	TriangleNode *_node;
public:
	TriangleWrapper(TriangleNode *node);

	TriangleNode *node();
	void setNode(TriangleNode *node);

	bool selected();
	void setSelected(bool selected);

	bool isQuad();
	int count();

	VertexWrapper ^vertex(int index);
	void setVertex(VertexWrapper ^vertex, int index);

	EdgeWrapper ^edge(int index);
	void setEdge(EdgeWrapper ^edge, int index);

	VertexWrapper ^vertexNotInEdge(EdgeWrapper ^edge);
};

[ComVisibleAttribute(true)]
public ref class EdgeWrapper
{
private:
	VertexEdgeNode *_node;
public:
	EdgeWrapper(VertexEdgeNode *node);

	VertexEdgeNode *node();
	void setNode(VertexEdgeNode *node);

	bool selected();
	void setSelected(bool selected);

	VertexWrapper ^half();
	void setHalf(VertexWrapper ^half);

	VertexWrapper ^vertex(int index);
	void setVertex(VertexWrapper ^vertex, int index);

	TriangleWrapper ^triangle(int index);
	void setTriangle(TriangleWrapper ^triangle, int index);

	VertexWrapper ^oppositeVertex(VertexWrapper ^vertex);
};

[ComVisibleAttribute(true)]
public ref class VertexNodeEdgeWrapper : public EdgeWrapper
{
private:
    Vertex2VEdgeNode *_simpleNode;
public:
	VertexNodeEdgeWrapper(Vertex2VEdgeNode *edgeNode);

	Vertex2VEdgeNode *simpleNode();
	void setSimpleNode(Vertex2VEdgeNode *simpleNode);
};

#define DeclareIterator(Name, TBase, TNode) \
[ComVisibleAttribute(true)] \
public ref class Name : public TBase \
{ \
private: \
    TNode *begin; \
    TNode *end; \
public: \
	Name(TNode *theBegin, TNode *theEnd); \
	bool finished(); \
	void moveStart(); \
	void moveNext(); \
};

DeclareIterator(VertexNodeIterator, VertexWrapper, VertexNode)
DeclareIterator(TriangleNodeIterator, TriangleWrapper, TriangleNode)
DeclareIterator(EdgeNodeIterator, EdgeWrapper, VertexEdgeNode)
DeclareIterator(VertexNodeEdgeIterator, VertexNodeEdgeWrapper, Vertex2VEdgeNode)

}

#endif