/*
 *  OpenGLManipulatingModel.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/28/09.
 *  For license see LICENSE.TXT
 *
 */

#import "OpenGLDrawing.h"
#import "MathDeclaration.h"
#import "OpenGLManipulating.h"

@protocol OpenGLManipulatingModel <NSObject>

@property (readwrite, assign) enum ViewMode viewMode;

- (uint)count;
- (BOOL)isSelectedAtIndex:(uint)index;
- (void)setSelected:(BOOL)selected atIndex:(uint)index;
- (void)expandSelectionFromIndex:(uint)index invert:(BOOL)invert;
- (void)duplicateSelected;
- (void)removeSelected;
- (void)hideSelected;
- (void)unhideAll;

@optional

@property (readwrite, copy) NSColor *selectionColor;

- (void)willSelectThrough:(BOOL)selectThrough;
- (void)didSelect;
- (BOOL)needsCullFace;

@end

@protocol OpenGLManipulatingModelMesh <OpenGLManipulatingModel>

- (void)getSelectionCenter:(Vector3D *)center 
				  rotation:(Quaternion *)rotation
					 scale:(Vector3D *)scale;

- (void)transformSelectedByMatrix:(Matrix4x4 *)matrix;
- (void)drawAllForSelection:(BOOL)forSelection;
- (BOOL)useGLProject;
- (void)glProjectSelectWithX:(int)x  
                           y:(int)y 
                       width:(int)width
                      height:(int)height
                   transform:(Matrix4x4 *)matrix
               selectionMode:(enum OpenGLSelectionMode)selectionMode;
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
- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection;

@end


