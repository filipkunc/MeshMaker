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

@property (readonly) Vector3D selectionCenter;
@property (readonly) Quaternion selectionRotation;
@property (readonly) Vector3D selectionScale;
@property (readwrite) BOOL toggleWhenSelecting;
@property (readonly) NSUInteger selectedCount;

- (void)draw;
- (void)moveSelectedByOffset:(Vector3D)offset;
- (void)rotateSelectedByOffset:(Quaternion)offset;
- (void)scaleSelectedByOffset:(Vector3D)offset;
- (void)changeSelection:(BOOL)isSelected;
- (void)invertSelection;

- (void)removeSelected;
- (void)cloneSelected;

@end
