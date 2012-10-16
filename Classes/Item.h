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

- (id)initFromSelectedTrianglesInMesh:(Mesh2 *)aMesh;
- (id)initWithMesh:(Mesh2 *)aMesh;
- (id)initWithPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale;
- (void)drawForSelection:(BOOL)forSelection;
- (void)moveByOffset:(Vector3D)offset;
- (void)rotateByOffset:(Quaternion)offset;
- (void)scaleByOffset:(Vector3D)offset;
- (Item *)duplicate;

@end
