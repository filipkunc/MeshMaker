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

- (NSUInteger)count;
- (Vector3D)positionAtIndex:(NSUInteger)index;
- (Quaternion)rotationAtIndex:(NSUInteger)index;
- (Vector3D)scaleAtIndex:(NSUInteger)index;
- (void)setPosition:(Vector3D)position atIndex:(NSUInteger)index;
- (void)setRotation:(Quaternion)rotation atIndex:(NSUInteger)index;
- (void)setScale:(Vector3D)scale atIndex:(NSUInteger)index;
- (void)moveByOffset:(Vector3D)offset atIndex:(NSUInteger)index;
- (void)rotateByOffset:(Quaternion)offset atIndex:(NSUInteger)index;
- (void)scaleByOffset:(Vector3D)offset atIndex:(NSUInteger)index;
- (BOOL)isSelectedAtIndex:(NSUInteger)index;
- (void)setSelected:(BOOL)selected atIndex:(NSUInteger)index;
- (void)drawAtIndex:(NSUInteger)index forSelection:(BOOL)forSelection;
- (void)removeAtIndex:(NSUInteger)index;

@end

@interface OpenGLManipulatingController : NSObject <OpenGLManipulating>
{
	id<OpenGLManipulatingModel> model;
	Vector3D *selectionCenter;
	Quaternion *selectionRotation;
	Vector3D *selectionEuler;
	Vector3D *selectionScale;
	NSUInteger selectedCount;
	NSInteger lastSelectedIndex;
	enum ManipulatorType currentManipulator;
	
	Matrix4x4 *modelTransform;
	Vector3D *modelPosition;
	Quaternion *modelRotation;
	Vector3D *modelScale;
}

@property (readwrite, assign) id<OpenGLManipulatingModel> model;
@property (readonly, assign) NSInteger lastSelectedIndex;

- (float)selectionValueAtIndex:(NSUInteger)index;
- (void)setSelectionValue:(float)value atIndex:(NSUInteger)index;
- (void)willChangeSelection;
- (void)didChangeSelection;
- (void)setPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale;

@end
