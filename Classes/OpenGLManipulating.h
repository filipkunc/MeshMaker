/*
 *  OpenGLManipulating.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 8/3/09.
 *  For license see LICENSE.TXT
 *
 */
#import "OpenGLSelecting.h"

@protocol OpenGLTransforming <NSObject>

@property (readwrite, assign) float selectionX, selectionY, selectionZ;
@property (readwrite, assign) enum ManipulatorType currentManipulator;

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
- (void)drawWithMode:(enum ViewMode)mode forSelection:(BOOL)forSelection;
- (void)changeSelection:(BOOL)isSelected;
- (void)invertSelection;
- (void)duplicateSelected;
- (void)removeSelected;
- (void)hideSelected;
- (void)unhideAll;
- (NSString *)nameAtIndex:(uint)index;

@end
