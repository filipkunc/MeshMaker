//
//  OpenGLSceneView.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "OpenGLSceneViewCore.h"

@class OpenGLSceneView;

@protocol OpenGLSceneViewDelegate

- (void)manipulationStartedInView:(OpenGLSceneView *)view;
- (void)manipulationEndedInView:(OpenGLSceneView *)view;
- (void)selectionChangedInView:(OpenGLSceneView *)view;

@property (readonly) NSColor *brushColor;
@property (readonly) float brushSize;
@property (readonly) BOOL texturePaintEnabled;
@property (readonly) BOOL vertexToolEnabled;

- (void)vertexAddOrConnect:(Vector3D)position fromCamera:(Camera *)camera;
- (void)vertexAddOrConnectHint:(Vector3D)position fromCamera:(Camera *)camera vertices:(vector<Vector3D> *)vertices;
- (void)updateCameraZoomAndCenter:(Camera *)camera fromView:(OpenGLSceneView *)view;

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
@property (readonly, assign) Camera *camera;

@end
