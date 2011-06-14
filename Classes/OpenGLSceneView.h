//
//  OpenGLSceneView.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "Camera.h"
#import "OpenGLSelecting.h"
#import "Manipulator.h"
#import "OpenGLManipulating.h"
#import "Shader.h"
#import "ShaderProgram.h"
#import "Mesh.h"

@class OpenGLSceneView;

@protocol OpenGLSceneViewDelegate

- (void)manipulationStartedInView:(OpenGLSceneView *)view;
- (void)manipulationEndedInView:(OpenGLSceneView *)view;
- (void)selectionChangedInView:(OpenGLSceneView *)view;

@end

@interface OpenGLSceneView : NSOpenGLView 
{
	id<OpenGLManipulating> __weak displayed;
	id<OpenGLManipulating> __weak manipulated;
	id<OpenGLSceneViewDelegate> __weak delegate;
	
	Vector3D *selectionOffset;
	Camera *camera;
	Vector2D *perspectiveRadians;
	NSPoint lastPoint;
	NSPoint currentPoint;
	BOOL isManipulating;
	BOOL isSelecting;
	BOOL highlightCameraMode;
	Manipulator *defaultManipulator;
	Manipulator *translationManipulator;
	Manipulator *rotationManipulator;
	Manipulator *scaleManipulator;
	Manipulator *currentManipulator;
	enum CameraMode cameraMode;
	enum ViewMode viewMode;
}

@property (readwrite, weak) id<OpenGLManipulating> displayed;
@property (readwrite, weak) id<OpenGLManipulating> manipulated;
@property (readwrite, weak) id<OpenGLSceneViewDelegate> delegate;
@property (readwrite, assign) enum ManipulatorType currentManipulator;
@property (readwrite, assign) enum CameraMode cameraMode;
@property (readwrite, assign) enum ViewMode viewMode;
@property (readwrite, assign) Camera camera;

- (void)drawGridWithSize:(int)size step:(int)step;
- (NSRect)orthoManipulatorRect;
- (NSRect)currentRect;
- (void)beginOrtho;
- (void)endOrtho;
- (void)applyProjectionWithRect:(NSRect)baseRect;

- (void)selectWithX:(int)x 
				  y:(int)y
			  width:(int)width 
			 height:(int)height
		  selecting:(id<OpenGLSelecting>)selecting 
		nearestOnly:(BOOL)nearestOnly
		   cullFace:(BOOL)cullFace
		selectionMode:(enum OpenGLSelectionMode)selectionMode;

- (void)selectWithPoint:(NSPoint)point 
			  selecting:(id<OpenGLSelecting>)selecting
		  selectionMode:(enum OpenGLSelectionMode)selectionMode;

- (void)selectWithRect:(NSRect)rect 
			 selecting:(id<OpenGLSelecting>)selecting
			  cullFace:(BOOL)cullFace
		 selectionMode:(enum OpenGLSelectionMode)selectionMode;

- (Vector3D)positionInSpaceByPoint:(NSPoint)point;
- (void)drawSelectionPlaneWithIndex:(int)index;
- (Vector3D)positionFromAxis:(Axis)axis point:(NSPoint)point;
- (Vector3D)positionFromRotatedAxis:(Axis)axis point:(NSPoint)point rotation:(Quaternion)rotation;
- (Vector3D)positionFromPlaneAxis:(PlaneAxis)plane point:(NSPoint)point;
- (Vector3D)translationFromPoint:(NSPoint)point;
- (Vector3D)scaleFromPoint:(NSPoint)point lastPosition:(Vector3D *)lastPosition;
- (Quaternion)rotationFromPoint:(NSPoint)point lastPosition:(Vector3D *)lastPosition;
- (NSRect)reshapeViewport;

@end
