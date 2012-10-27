//
//  JSWrappers.h
//  MeshMaker
//
//  Created by Filip Kunc on 10/27/12.
//
//

#import "ItemCollection.h"

@class VertexWrapper;
@class TriangleWrapper;
@class EdgeWrapper;
@class VertexNodeIterator;
@class TriangleNodeIterator;
@class EdgeNodeIterator;

@interface ItemCollection (JSWrappers)

@end

@interface Item (JSWrappers)

@property (readonly) VertexNodeIterator *vertexIterator;
@property (readonly) TriangleNodeIterator *triQuadIterator;
@property (readonly) EdgeNodeIterator *edgeIterator;

@end

@interface VertexWrapper : NSObject

@property (readwrite, assign) VertexNode *node;
@property (readwrite, assign) BOOL selected;
@property (readwrite, assign) float x;
@property (readwrite, assign) float y;
@property (readwrite, assign) float z;

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

@end

@interface EdgeWrapper : NSObject

@property (readwrite, assign) VertexEdgeNode *node;
@property (readwrite, assign) BOOL selected;

- (id)initWithNode:(VertexEdgeNode *)edgeNode;

- (VertexWrapper *)vertexAtIndex:(uint)index;
- (void)setVertex:(VertexWrapper *)vertex atIndex:(uint)index;

- (TriangleWrapper *)triangleAtIndex:(uint)index;
- (void)setTriangle:(TriangleWrapper *)triangle atIndex:(uint)index;

@end

@interface VertexNodeIterator : VertexWrapper
{
    VertexNode *begin;
    VertexNode *end;
}

@property (readonly) BOOL finished;

- (id)initWithBegin:(VertexNode *)theBegin end:(VertexNode *)theEnd;
- (void)moveStart;
- (void)moveNext;

@end

@interface TriangleNodeIterator : TriangleWrapper
{
    TriangleNode *begin;
    TriangleNode *end;
}

@property (readonly) BOOL finished;

- (id)initWithBegin:(TriangleNode *)theBegin end:(TriangleNode *)theEnd;
- (void)moveStart;
- (void)moveNext;

@end

@interface EdgeNodeIterator : EdgeWrapper
{
    VertexEdgeNode *begin;
    VertexEdgeNode *end;
}

@property (readonly) BOOL finished;

- (id)initWithBegin:(VertexEdgeNode *)theBegin end:(VertexEdgeNode *)theEnd;
- (void)moveStart;
- (void)moveNext;

@end
