/*
 *  OpenGLManipulating.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 8/3/09.
 *  For license see LICENSE.TXT
 *
 */
#import "OpenGLSelecting.h"

@protocol OpenGLManipulating <OpenGLSelecting>

@property (readwrite, assign) enum ManipulatorType currentManipulator;
@property (readwrite, assign) Vector3D selectionCenter;
@property (readwrite, assign) Quaternion selectionRotation;
@property (readwrite, assign) Vector3D selectionScale;
@property (readwrite, assign) float selectionX, selectionY, selectionZ;
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
