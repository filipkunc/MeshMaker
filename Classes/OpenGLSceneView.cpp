//
//  OpenGLSceneView.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//


#include "OpenGLDrawing.h"
#include "OpenGLSceneView.h"

NSOpenGLPixelFormat *globalPixelFormat = nil;
NSOpenGLContext *globalGLContext = nil;

class OpenGLSceneViewCoreDelegate : public IOpenGLSceneViewCoreDelegate
{
public:
    OpenGLSceneView *_sceneView;
    
    OpenGLSceneViewCoreDelegate(OpenGLSceneView *sceneView)
    {
        _sceneView = sceneView;
    }
    
    virtual ~OpenGLSceneViewCoreDelegate() { }
    
    virtual NSRect bounds() { return _sceneView.bounds; }
    virtual void setNeedsDisplay() { [_sceneView setNeedsDisplay:YES]; }
    virtual void manipulationStarted() { [_sceneView.delegate manipulationStartedInView:_sceneView]; }
    virtual void manipulationEnded() { [_sceneView.delegate manipulationEndedInView:_sceneView]; }
    virtual void selectionChanged() { [_sceneView.delegate selectionChangedInView:_sceneView]; }
    virtual bool texturePaintEnabled() { return [_sceneView.delegate texturePaintEnabled]; }
    virtual bool vertexToolEnabled() { return [_sceneView.delegate vertexToolEnabled]; }
    virtual void vertexAddOrConnect(Vector3D position, Camera *camera) { [_sceneView.delegate vertexAddOrConnect:position fromCamera:camera]; }
    virtual void vertexAddOrConnectHint(Vector3D position, Camera *camera, vector<Vector3D> &vertices) { [_sceneView.delegate vertexAddOrConnectHint:position fromCamera:camera vertices:&vertices]; }
    virtual void makeCurrentContext() { [[_sceneView openGLContext] makeCurrentContext]; }
    virtual void updateCameraZoomAndCenter(Camera *camera) { [_sceneView.delegate updateCameraZoomAndCenter:camera fromView:_sceneView]; }
};

@implementation OpenGLSceneView

@synthesize delegate;

- (IOpenGLManipulating *)manipulated
{
    return _coreView->manipulated();
}

- (void)setManipulated:(IOpenGLManipulating *)manipulated
{
    _coreView->setManipulated(manipulated);
}

- (IOpenGLManipulating *)displayed
{
    return _coreView->displayed();
}

- (void)setDisplayed:(IOpenGLManipulating *)displayed
{
    _coreView->setDisplayed(displayed);
}

- (enum ManipulatorType)currentManipulator
{
    return _coreView->currentManipulator();
}

- (void)setCurrentManipulator:(enum ManipulatorType)currentManipulator
{
    _coreView->setCurrentManipulator(currentManipulator);
}

- (enum CameraMode)cameraMode
{
    return _coreView->cameraMode();
}

- (void)setCameraMode:(enum CameraMode)cameraMode
{
    _coreView->setCameraMode(cameraMode);
}

- (Camera *)camera
{
    return _coreView->camera();
}

+ (NSOpenGLPixelFormat *)sharedPixelFormat
{
	if (!globalPixelFormat)
	{
		NSOpenGLPixelFormatAttribute attribs[] = 
		{
            //NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
			NSOpenGLPFAAccelerated,
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFAColorSize, 1,
			NSOpenGLPFADepthSize, 1,
			0 
		};
		
		globalPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
	}
	return globalPixelFormat;
}

+ (NSOpenGLContext *)sharedContext
{
	if (!globalGLContext)
	{
		globalGLContext = [[NSOpenGLContext alloc] initWithFormat:[OpenGLSceneView sharedPixelFormat]
												   shareContext:nil];
	}
	return globalGLContext;
}

- (void)awakeFromNib
{
	NSWindow *window = [self window];
	[window setAcceptsMouseMovedEvents:YES];
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (id)initWithCoder:(NSCoder *)c
{
	self = [super initWithCoder:c];
	if (self)
	{
        [self initView];
	}
	return self;
}

- (id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self)
    {
        [self initView];
    }
    return self;
}

- (void)initView
{
    delegate = nil;
    _coreDelegate = new OpenGLSceneViewCoreDelegate(self);
    _coreView = new OpenGLSceneViewCore(_coreDelegate);
    
    [self setupGL];
}

- (void)setupGL
{
    //[super prepareOpenGL];
    
    // The GL context must be active for these functions to have an effect
    [self clearGLContext];
    NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:[OpenGLSceneView sharedPixelFormat]
                                                          shareContext:[OpenGLSceneView sharedContext]];
    [self setOpenGLContext:context];
    [[self openGLContext] makeCurrentContext];
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    
    ShaderProgram::initShaders();
}

- (void)dealloc
{
	delete _coreView;
    delete _coreDelegate;
}

- (void)reshape
{
	[self setNeedsDisplay:YES];
}

#pragma mark Drawing

- (void)drawRect:(NSRect)rect
{
    _coreView->draw();
		
	[[self openGLContext] flushBuffer];
}

#pragma mark Mouse Events

- (NSPoint)locationFromNSEvent:(NSEvent *)e
{
	return [self convertPoint:[e locationInWindow] fromView:nil];
}

- (void)viewDidMoveToWindow
{
	NSUInteger options = NSTrackingMouseMoved |
						 NSTrackingActiveAlways |
						 NSTrackingInVisibleRect | 
						 NSTrackingMouseEnteredAndExited;
	
	NSTrackingArea *trackingArea;
	trackingArea = [[NSTrackingArea alloc] initWithRect:NSZeroRect
												options:options 
												  owner:self
											   userInfo:nil];
	[self addTrackingArea:trackingArea];
}

- (void)mouseDown:(NSEvent *)e
{
	_coreView->mouseDown([self locationFromNSEvent:e],
                         [e modifierFlags] & NSAlternateKeyMask);
}

- (void)mouseMoved:(NSEvent *)e
{
	_coreView->mouseMoved([self locationFromNSEvent:e]);
}

- (void)mouseExited:(NSEvent *)e
{
    _coreView->mouseExited();
}

- (void)mouseUp:(NSEvent *)e
{
    _coreView->mouseUp([self locationFromNSEvent:e],
                       [e modifierFlags] & NSAlternateKeyMask,
                       [e modifierFlags] & NSCommandKeyMask,
                       [e modifierFlags] & NSControlKeyMask,
                       [e modifierFlags] & NSShiftKeyMask,
                       [e clickCount]);
}

- (void)mouseDragged:(NSEvent *)e
{
	_coreView->mouseDragged([self locationFromNSEvent:e],
                            [e modifierFlags] & NSAlternateKeyMask,
                            [e modifierFlags] & NSCommandKeyMask);
}

- (void)otherMouseDown:(NSEvent *)e
{
    _coreView->otherMouseDown([self locationFromNSEvent:e]);
}

- (void)otherMouseDragged:(NSEvent *)e
{
	_coreView->otherMouseDragged([self locationFromNSEvent:e],
                                 [e modifierFlags] & NSAlternateKeyMask);
}

- (void)rightMouseDown:(NSEvent *)e
{
    _coreView->rightMouseDown([self locationFromNSEvent:e]);
}

- (void)rightMouseDragged:(NSEvent *)e
{	
	_coreView->rightMouseDragged([self locationFromNSEvent:e],
                                 [e modifierFlags] & NSAlternateKeyMask);
}

- (void)scrollWheel:(NSEvent *)e
{	
	_coreView->scrollWheel([e deltaX],
                           [e deltaY],
                           [e modifierFlags] & NSAlternateKeyMask,
                           [e modifierFlags] & NSCommandKeyMask);
}

@end
