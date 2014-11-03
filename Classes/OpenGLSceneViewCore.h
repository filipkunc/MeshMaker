//
//  OpenGLSceneViewCore.h
//  MeshMaker
//
//  Created by Filip Kunc on 1/6/13.
//
//

#pragma once

#include <Cocoa/Cocoa.h>
#include "MathDeclaration.h"
#include "Camera.h"
#include "OpenGLSelecting.h"
#include "Manipulator.h"
#include "OpenGLManipulating.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Mesh2.h"
#include "ItemCollection.h"

class IOpenGLSceneViewCoreDelegate
{
public:
    virtual ~IOpenGLSceneViewCoreDelegate() { }

    virtual NSRect bounds() = 0;
    virtual void setNeedsDisplay() = 0;
    virtual void manipulationStarted() = 0;
    virtual void manipulationEnded() = 0;
    virtual void selectionChanged() = 0;
    virtual bool texturePaintEnabled() = 0;
    virtual bool vertexToolEnabled() = 0;
    virtual void vertexAddOrConnect(Vector3D position, Camera *camera) = 0;
    virtual void vertexAddOrConnectHint(Vector3D position, Camera *camera, vector<Vector3D> &vertices) = 0;
    virtual void makeCurrentContext() = 0;
    virtual void updateCameraZoomAndCenter(Camera *camera) = 0;
};

class OpenGLSceneViewCore
{
public:
    IOpenGLSceneViewCoreDelegate *_delegate;
    IOpenGLManipulating *_displayed;
    IOpenGLManipulating *_manipulated;
    
	Vector3D _selectionOffset;
	Camera *_camera;
	Vector2D _perspectiveRadians;
	NSPoint _lastPoint;
	NSPoint _currentPoint;
    bool _isManipulating;
    bool _isSelecting;
    bool _isPainting;
    bool _highlightCameraMode;
	Manipulator *_defaultManipulator;
	Manipulator *_translationManipulator;
	Manipulator *_rotationManipulator;
	Manipulator *_scaleManipulator;
	Manipulator *_currentManipulator;
	CameraMode _cameraMode;
    vector<Vector3D> _vertexHints;
    
    static bool _alwaysSelectThrough;    
public:
    OpenGLSceneViewCore(IOpenGLSceneViewCoreDelegate *delegate);
    ~OpenGLSceneViewCore();
    
    IOpenGLManipulating *displayed();
    void setDisplayed(IOpenGLManipulating *displayed);
    IOpenGLManipulating *manipulated();
    void setManipulated(IOpenGLManipulating *manipulated);
    Camera *camera();
    void drawGrid(int size, int step);
    NSRect orthoManipulatorRect();
    NSRect currentRect();
    void beginOrtho();
    void endOrtho();
    void applyProjection();
    bool *select(int x, int y, int width, int height, IOpenGLSelecting *selecting);
    void select(NSPoint point, IOpenGLSelecting *selecting, OpenGLSelectionMode selectionMode);
    void select(NSRect rect, IOpenGLSelecting *selecting, OpenGLSelectionMode selectionMode, bool selectThrough);
    Vector3D positionInSpaceByPoint(NSPoint point);
    void drawSelectionPlane(int index);
    Vector3D positionFromAxisPoint(Axis axis, NSPoint point);
    Vector3D positionFromRotatedAxisPoint(Axis axis, NSPoint point, Quaternion rotation);
    Vector3D positionFromPlaneAxis(PlaneAxis plane, NSPoint point);
    Vector3D addVertexPositionFromPoint(NSPoint point);
    Vector3D translationFromPoint(NSPoint point);
    Vector3D scaleFromPoint(NSPoint point, Vector3D &lastPosition);
    Quaternion rotationFromPoint(NSPoint point, Vector3D &lastPosition);
    void reshapeViewport();
    void setupViewportAndCamera();
    ManipulatorType currentManipulator();
    void setCurrentManipulator(ManipulatorType manipulator);
    CameraMode cameraMode();
    void setCameraMode(CameraMode value);
    void drawManipulatedAndDisplatedForSelection(bool forSelection);
    void drawOrthoDefaultManipulator();
    void drawCurrentManipulator();
    void drawSelectionRect();
    void draw();
    void mouseDown(NSPoint point, bool alt);
    void mouseMoved(NSPoint point);
    void mouseExited();
    void mouseUp(NSPoint point, bool alt, bool cmd, bool ctrl, bool shift, int clickCount);
    void mouseDragged(NSPoint point, bool alt, bool cmd);
    void otherMouseDown(NSPoint point);
    void otherMouseDragged(NSPoint point, bool alt);
    void rightMouseDown(NSPoint point);
    void rightMouseDragged(NSPoint point, bool alt);
    void scrollWheel(float deltaX, float deltaY, bool alt, bool cmd);
};
