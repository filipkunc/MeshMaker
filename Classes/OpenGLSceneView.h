//
//  OpenGLSceneView.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#import "OpenGLSceneViewCore.h"

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
    OpenGLSceneViewCore *_coreView;
    IOpenGLSceneViewCoreDelegate *_coreDelegate;
    id<OpenGLSceneViewDelegate> __weak delegate;
}

@property (readwrite, assign) IOpenGLManipulating *displayed;
@property (readwrite, assign) IOpenGLManipulating *manipulated;
@property (readwrite, weak) id<OpenGLSceneViewDelegate> delegate;
@property (readwrite, assign) enum ManipulatorType currentManipulator;
@property (readwrite, assign) enum CameraMode cameraMode;
@property (readwrite, assign) Camera camera;

@end

