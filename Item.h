//
//  Item.h
//  OpenGLWidgets
//
//  Created by Filip Kunc on 6/28/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "Mesh.h"

@interface Item : NSObject 
{
	Vector3D *position;
	Quaternion *rotation;
	Vector3D *scale;
	Mesh *mesh;
	BOOL selected;
}

@property (readwrite, assign) Vector3D position;
@property (readwrite, assign) Quaternion rotation;
@property (readwrite, assign) Vector3D scale;
@property (readwrite, assign) BOOL selected;
@property (readonly) Mesh *mesh;

- (id)initWithPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale;
- (void)draw;
- (void)moveByOffset:(Vector3D)offset;
- (void)rotateByOffset:(Quaternion)offset;
- (void)scaleByOffset:(Vector3D)offset;

@end
