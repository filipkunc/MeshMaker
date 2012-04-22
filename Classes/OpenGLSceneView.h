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
#import "Mesh2.h"
#import "FPTexture.h"
#import "ItemCollection.h"

@class OpenGLSceneView;

@protocol OpenGLSceneViewDelegate

- (void)manipulationStartedInView:(OpenGLSceneView *)view;
- (void)manipulationEndedInView:(OpenGLSceneView *)view;
- (void)selectionChangedInView:(OpenGLSceneView *)view;

@property (readonly) NSColor *brushColor;
@property (readonly) float brushSize;
@property (readonly) BOOL texturePaintEnabled;

@end

@interface OpenGLSceneView : NSOpenGLView 
{
	id<OpenGLManipulating> __weak displayed;
	id<OpenGLManipulating> __weak manipulated;
	id<OpenGLSceneViewDelegate> __weak delegate;

    NSString *debugString;
	Vector3D *selectionOffset;
	Camera *camera;
	Vector2D *perspectiveRadians;
	NSPoint lastPoint;
	NSPoint currentPoint;
	BOOL isManipulating;
	BOOL isSelecting;
    BOOL isPainting;
	BOOL highlightCameraMode;
	Manipulator *defaultManipulator;
	Manipulator *translationManipulator;
	Manipulator *rotationManipulator;
	Manipulator *scaleManipulator;
	Manipulator *currentManipulator;
	enum CameraMode cameraMode;
}

@property (readwrite, weak) id<OpenGLManipulating> displayed;
@property (readwrite, weak) id<OpenGLManipulating> manipulated;
@property (readwrite, weak) id<OpenGLSceneViewDelegate> delegate;
@property (readwrite, assign) enum ManipulatorType currentManipulator;
@property (readwrite, assign) enum CameraMode cameraMode;
@property (readwrite, assign) Camera camera;

- (void)drawGridWithSize:(int)size step:(int)step;
- (NSRect)orthoManipulatorRect;
- (NSRect)currentRect;
- (void)beginOrtho;
- (void)endOrtho;
- (void)applyProjectionWithRect:(NSRect)baseRect;

- (NSMutableIndexSet *)selectWithX:(int)x 
                                 y:(int)y
                             width:(int)width 
                            height:(int)height
                         selecting:(id<OpenGLSelecting>)selecting;

- (void)selectWithPoint:(NSPoint)point 
			  selecting:(id<OpenGLSelecting>)selecting
		  selectionMode:(enum OpenGLSelectionMode)selectionMode;

- (void)selectWithRect:(NSRect)rect 
			 selecting:(id<OpenGLSelecting>)selecting
		 selectionMode:(enum OpenGLSelectionMode)selectionMode
         selectThrough:(BOOL)selectThrough;

- (void)paintOnTextureWithFirstPoint:(NSPoint)firstPoint secondPoint:(NSPoint)secondPoint;
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
