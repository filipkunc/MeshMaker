/*
 *  OpenGLManipulating.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 8/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#import "OpenGLSelecting.h"

enum ManipulatorType 
{
	ManipulatorTypeDefault = 0,
	ManipulatorTypeTranslation = 1, 
	ManipulatorTypeRotation = 2,
	ManipulatorTypeScale = 3
};

@protocol OpenGLManipulating <OpenGLSelecting>

@property (readwrite, assign) enum ManipulatorType currentManipulator;
@property (readwrite, assign) Vector3D selectionCenter;
@property (readwrite, assign) Quaternion selectionRotation;
@property (readwrite, assign) Vector3D selectionScale;
@property (readwrite, assign) float selectionX, selectionY, selectionZ;
@property (readonly) NSUInteger selectedCount;

- (void)moveSelectedByOffset:(Vector3D)offset;
- (void)rotateSelectedByOffset:(Quaternion)offset;
- (void)scaleSelectedByOffset:(Vector3D)offset;
- (void)updateSelection;
- (void)draw;
- (void)changeSelection:(BOOL)isSelected;
- (void)invertSelection;
- (void)removeSelected;

@end
