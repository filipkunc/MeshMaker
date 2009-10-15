//
//  OpenGLManipulatingController.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 8/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "OpenGLManipulating.h"

@protocol OpenGLManipulatingModel

- (uint)count;
- (BOOL)isSelectedAtIndex:(uint)index;
- (void)setSelected:(BOOL)selected atIndex:(uint)index;
- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection;
- (void)cloneSelected;
- (void)removeSelected;

@optional // better for Mesh-like objects
- (void)willSelect;

- (void)getSelectionCenter:(Vector3D *)center 
				  rotation:(Quaternion *)rotation
					 scale:(Vector3D *)scale;

- (void)moveSelectedByOffset:(Vector3D)offset;
- (void)rotateSelectedByOffset:(Quaternion)offset;
- (void)scaleSelectedByOffset:(Vector3D)offset;

@optional // better for Item-like objects

- (Vector3D)positionAtIndex:(uint)index;
- (Quaternion)rotationAtIndex:(uint)index;
- (Vector3D)scaleAtIndex:(uint)index;
- (void)setPosition:(Vector3D)position atIndex:(uint)index;
- (void)setRotation:(Quaternion)rotation atIndex:(uint)index;
- (void)setScale:(Vector3D)scale atIndex:(uint)index;
- (void)moveByOffset:(Vector3D)offset atIndex:(uint)index;
- (void)rotateByOffset:(Quaternion)offset atIndex:(uint)index;
- (void)scaleByOffset:(Vector3D)offset atIndex:(uint)index;

@end

@interface OpenGLManipulatingController : NSObject <OpenGLManipulating>
{
	id<OpenGLManipulatingModel> model;
	Vector3D *selectionCenter;
	Quaternion *selectionRotation;
	Vector3D *selectionEuler;
	Vector3D *selectionScale;
	uint selectedCount;
	NSInteger lastSelectedIndex;
	enum ManipulatorType currentManipulator;
	
	Matrix4x4 *modelTransform;
	Vector3D *modelPosition;
	Quaternion *modelRotation;
	Vector3D *modelScale;
}

@property (readwrite, assign) id<OpenGLManipulatingModel> model;
@property (readonly, assign) NSInteger lastSelectedIndex;

- (float)selectionValueAtIndex:(uint)index;
- (void)setSelectionValue:(float)value atIndex:(uint)index;
- (void)willChangeSelection;
- (void)didChangeSelection;
- (void)setPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale;

@end
