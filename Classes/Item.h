//
//  Item.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "MeshHelpers.h"
#import "Mesh2.h"
#import "OpenGLManipulatingController.h"
#import "MemoryStream.h"
#import "MemoryStreaming.h"

@class VertexWrapper;
@class VertexNodeIterator;
@class TriangleNodeIterator;

@interface Item : NSObject <OpenGLManipulatingModelMesh, MemoryStreaming>
{
	Vector3D *position;
	Quaternion *rotation;
	Vector3D *scale;
	Mesh2 *mesh;
	BOOL selected;
	BOOL visible;
    enum ViewMode viewMode;
}

@property (readwrite, copy) NSColor *selectionColor;
@property (readwrite, assign) Vector3D position;
@property (readwrite, assign) Quaternion rotation;
@property (readwrite, assign) Vector3D scale;
@property (readwrite, assign) BOOL selected;
@property (readwrite, assign) BOOL visible;
@property (readwrite, assign) enum ViewMode viewMode;
@property (readonly) Mesh2 *mesh;
@property (readonly) Matrix4x4 transform;
@property (readonly) VertexNodeIterator *vertexIterator;
@property (readonly) TriangleNodeIterator *triQuadIterator;

- (id)initFromSelectedTrianglesInMesh:(Mesh2 *)aMesh;
- (id)initWithMesh:(Mesh2 *)aMesh;
- (id)initWithPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale;
- (void)drawForSelection:(BOOL)forSelection;
- (void)moveByOffset:(Vector3D)offset;
- (void)rotateByOffset:(Quaternion)offset;
- (void)scaleByOffset:(Vector3D)offset;
- (Item *)duplicate;

@end

@interface VertexNodeIterator : NSObject
{
    VertexNode *begin;
    VertexNode *end;
    VertexNode *current;
}

@property (readonly) BOOL finished;

@property (readwrite, assign) BOOL selected;
@property (readwrite, assign) float positionX;
@property (readwrite, assign) float positionY;
@property (readwrite, assign) float positionZ;

- (id)initWithBegin:(VertexNode *)theBegin end:(VertexNode *)theEnd;
- (void)moveStart;
- (void)moveNext;

@end

@interface TriangleNodeIterator : NSObject
{
    TriangleNode *begin;
    TriangleNode *end;
    TriangleNode *current;
}

@property (readwrite, assign) TriangleNode *current;
@property (readonly) BOOL finished;
@property (readonly) BOOL isQuad;
@property (readonly) uint count;

@property (readwrite, assign) BOOL selected;

- (id)initWithBegin:(TriangleNode *)theBegin end:(TriangleNode *)theEnd;
- (void)moveStart;
- (void)moveNext;

- (VertexWrapper *)vertexAtIndex:(uint)index;
- (void)setVertex:(VertexWrapper *)vertex atIndex:(uint)index;

@end

@interface VertexWrapper : NSObject
{
    VertexNode *vertex;
}

@property (readonly) VertexNode *vertex;
@property (readwrite, assign) BOOL selected;
@property (readwrite, assign) float positionX;
@property (readwrite, assign) float positionY;
@property (readwrite, assign) float positionZ;

- (id)initWithVertex:(VertexNode *)aVertexNode;

@end
