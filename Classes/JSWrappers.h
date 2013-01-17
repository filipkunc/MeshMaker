//
//  JSWrappers.h
//  MeshMaker
//
//  Created by Filip Kunc on 10/27/12.
//
//

#pragma once

#import "ItemCollection.h"

@class VertexWrapper;
@class TriangleWrapper;
@class EdgeWrapper;
@class SimpleNodeEdgeWrapper;
@class VertexNodeIterator;
@class TriangleNodeIterator;
@class EdgeNodeIterator;
@class SimpleNodeEdgeIterator;
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
- (void)extrudeSelectedTriangles;
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
@property (readonly) SimpleNodeEdgeIterator *edgeIterator;

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
