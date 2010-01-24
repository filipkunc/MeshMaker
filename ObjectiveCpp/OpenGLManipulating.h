/*
 *  OpenGLManipulating.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 8/3/09.
 *  For license see LICENSE.TXT
 *
 */
#import "OpenGLSelecting.h"

// for PropertyReflector
@protocol OpenGLTransforming

@property (readwrite, assign) float positionX, positionY, positionZ;
@property (readwrite, assign) float rotationX, rotationY, rotationZ;
@property (readwrite, assign) float scaleX, scaleY, scaleZ;

@end

@protocol OpenGLManipulating <OpenGLSelecting, OpenGLTransforming>

@property (readwrite, assign) Vector3D selectionCenter;
@property (readwrite, assign) Quaternion selectionRotation;
@property (readwrite, assign) Vector3D selectionScale;
@property (readonly) uint selectedCount;

- (void)moveSelectedByOffset:(Vector3D)offset;
- (void)rotateSelectedByOffset:(Quaternion)offset;
- (void)scaleSelectedByOffset:(Vector3D)offset;
- (void)updateSelection;
- (void)drawWithMode:(enum ViewMode)mode;
- (void)changeSelection:(BOOL)isSelected;
- (void)invertSelection;
- (void)cloneSelected;
- (void)removeSelected;

@end
