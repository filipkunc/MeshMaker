/*
 *  OpenGLManipulatingModel.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/28/09.
 *  For license see LICENSE.TXT
 *
 */

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "OpenGLManipulating.h"

@protocol OpenGLManipulatingModel

- (uint)count;
- (BOOL)isSelectedAtIndex:(uint)index;
- (void)setSelected:(BOOL)selected atIndex:(uint)index;
- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection withMode:(enum ViewMode)mode;
- (void)cloneSelected;
- (void)removeSelected;

@optional

- (void)willSelect;
- (void)didSelect;

@end

@protocol OpenGLManipulatingModelMesh <OpenGLManipulatingModel>

- (void)getSelectionCenter:(Vector3D *)center 
				  rotation:(Quaternion *)rotation
					 scale:(Vector3D *)scale;

- (void)moveSelectedByOffset:(Vector3D)offset;
- (void)rotateSelectedByOffset:(Quaternion)offset;
- (void)scaleSelectedByOffset:(Vector3D)offset;

@end

@protocol OpenGLManipulatingModelItem <OpenGLManipulatingModel>

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


