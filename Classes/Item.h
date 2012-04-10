//
//  Item.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "Mesh.h"
#import "MemoryStreaming.h"

@interface Item : NSObject <MemoryStreaming>
{
	Vector3D *position;
	Quaternion *rotation;
	Vector3D *scale;
	Mesh *mesh;
	BOOL selected;
	BOOL visible;
}

@property (readwrite, assign) Vector3D position;
@property (readwrite, assign) Quaternion rotation;
@property (readwrite, assign) Vector3D scale;
@property (readwrite, assign) BOOL selected;
@property (readwrite, assign) BOOL visible;
@property (readonly) Mesh *mesh;
@property (readonly) Matrix4x4 transform;

- (id)initWithPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale;
- (void)drawWithMode:(enum ViewMode)mode forSelection:(BOOL)forSelection;
- (void)moveByOffset:(Vector3D)offset;
- (void)rotateByOffset:(Quaternion)offset;
- (void)scaleByOffset:(Vector3D)offset;
- (Item *)duplicate;

@end
