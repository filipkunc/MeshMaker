//
//  OpenGLSceneView.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//


#include "OpenGLDrawing.h"
#include "OpenGLSceneView.h"

#if defined(__APPLE__)

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
    virtual bool addVertexEnabled() { return [_sceneView.delegate addVertexEnabled]; }
    virtual void addVertex(Vector3D position) { [_sceneView.delegate addVertex:position]; }
    virtual void makeCurrentContext() { [[_sceneView openGLContext] makeCurrentContext]; }
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

- (Camera)camera
{
    return *_coreView->camera();
}

- (void)setCamera:(Camera)camera
{
    *_coreView->camera() = camera;
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

#elif defined(WIN32)

namespace MeshMakerCppCLI
{
	class OpenGLSceneViewCoreDelegate : public IOpenGLSceneViewCoreDelegate
	{
	public:
		gcroot<OpenGLSceneView ^> _sceneView;
	    
		OpenGLSceneViewCoreDelegate(OpenGLSceneView ^sceneView)
		{
			_sceneView = sceneView;
		}
	    
		virtual ~OpenGLSceneViewCoreDelegate() { }
	    
		virtual NSRect bounds() 
		{ 
			return NSMakeRect(0.0f, 0.0f, _sceneView->Width, _sceneView->Height);
		}

		virtual void setNeedsDisplay() 
		{ 
			_sceneView->Refresh();
		}

		virtual void manipulationStarted() 
		{ 
			_sceneView->Delegate->manipulationStartedInView(_sceneView);			
		}
		
		virtual void manipulationEnded() 
		{ 
			_sceneView->Delegate->manipulationEndedInView(_sceneView);		
		}
		
		virtual void selectionChanged() 
		{ 
			_sceneView->Delegate->selectionChangedInView(_sceneView);
		}

		virtual bool texturePaintEnabled() { return false; }
		
		virtual void makeCurrentContext() 
		{ 
			_sceneView->BeginGL();
		}
	};

	OpenGLSceneView::OpenGLSceneView()
	{
		InitializeComponent();
		_coreDelegate = new OpenGLSceneViewCoreDelegate(this);
		_coreView = new OpenGLSceneViewCore(_coreDelegate);
	}

	OpenGLSceneView::~OpenGLSceneView()
	{
		EndGL();
		
		delete _coreView;
		delete _coreDelegate;
		
		if (components)
			delete components;
	}

	OpenGLSceneViewCore *OpenGLSceneView::coreView()
	{
		return _coreView;
	}

	void OpenGLSceneView::OnLoad(EventArgs ^e)
	{
		UserControl::OnLoad(e);
		if (Tools::SafeDesignMode)
			return;
		
		InitializeGL();
	}

    void OpenGLSceneView::OnSizeChanged(EventArgs ^e)
	{
		UserControl::OnSizeChanged(e);
		Invalidate();
	}

	void OpenGLSceneView::OnPaintBackground(PaintEventArgs ^e)
	{
		if (Tools::SafeDesignMode)
			UserControl::OnPaintBackground(e);
	}

	void OpenGLSceneView::OnPaint(PaintEventArgs ^e)
	{
		if (Tools::SafeDesignMode)
			return;

		if (!_deviceContext || !_glRenderingContext)
			return;

		BeginGL();
		_coreView->draw();
		SwapBuffers(_deviceContext);
		EndGL();
	}

	NSPoint OpenGLSceneView::LocationFromEvent(MouseEventArgs ^e)
	{
		return NSMakePoint(e->X, this->Height - e->Y);
	}

	void OpenGLSceneView::OnMouseDown(MouseEventArgs ^e)
	{
		UserControl::OnMouseDown(e);
		if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			_coreView->mouseDown(LocationFromEvent(e),
				(Control::ModifierKeys & Keys::Alt) == Keys::Alt);
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			_coreView->otherMouseDown(LocationFromEvent(e));
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Right)
		{
			_coreView->rightMouseDown(LocationFromEvent(e));
		}
	}

	void OpenGLSceneView::OnMouseMove(MouseEventArgs ^e)
	{
		UserControl::OnMouseMove(e);
		if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			_coreView->mouseDragged(LocationFromEvent(e),
				(Control::ModifierKeys & Keys::Alt) == Keys::Alt,
				(Control::ModifierKeys & Keys::Control) == Keys::Control); // Command
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			_coreView->otherMouseDragged(LocationFromEvent(e),
				(Control::ModifierKeys & Keys::Alt) == Keys::Alt);
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Right)
		{
			_coreView->rightMouseDragged(LocationFromEvent(e),
				(Control::ModifierKeys & Keys::Alt) == Keys::Alt);
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::None)
		{
			_coreView->mouseMoved(LocationFromEvent(e));
		}
	}

	void OpenGLSceneView::OnMouseUp(MouseEventArgs ^e)
	{
		UserControl::OnMouseUp(e);
		_coreView->mouseUp(LocationFromEvent(e),
			(Control::ModifierKeys & Keys::Alt) == Keys::Alt,
			(Control::ModifierKeys & Keys::Control) == Keys::Control, // Command
			false, // Control is disabled (select through)
			(Control::ModifierKeys & Keys::Shift) == Keys::Shift,
			1);
	}

	void OpenGLSceneView::OnMouseDoubleClick(MouseEventArgs ^e)
	{
		_coreView->mouseUp(LocationFromEvent(e),
			(Control::ModifierKeys & Keys::Alt) == Keys::Alt,
			(Control::ModifierKeys & Keys::Control) == Keys::Control, // Command
			false, // Control is disabled (select through)
			(Control::ModifierKeys & Keys::Shift) == Keys::Shift,
			2);
	}

	void OpenGLSceneView::OnMouseWheel(MouseEventArgs ^e)
	{
		UserControl::OnMouseWheel(e);
		_coreView->scrollWheel(0.0f,
			e->Delta / 10.0f,
			(Control::ModifierKeys & Keys::Alt) == Keys::Alt,
			(Control::ModifierKeys & Keys::Control) == Keys::Control); // Command
	}

	void OpenGLSceneView::BeginGL()
	{
		 wglMakeCurrent(_deviceContext, _glRenderingContext);
	}
	
	void OpenGLSceneView::EndGL()
	{
		wglMakeCurrent(NULL, NULL);
	}

	void OpenGLSceneView::InitializeGL()
	{
		if (Tools::SafeDesignMode)
			return;

		_deviceContext = GetDC((HWND)this->Handle.ToPointer());
        // CAUTION: Not doing the following SwapBuffers() on the DC will
        // result in a failure to subsequently create the RC.
		SwapBuffers(_deviceContext);

        //Get the pixel format		
        PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 32;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int pixelFormatIndex = ChoosePixelFormat(_deviceContext, &pfd);
        if (pixelFormatIndex == 0)
        {
			Trace::WriteLine("Unable to retrieve pixel format");
            return;
        }

        if (SetPixelFormat(_deviceContext, pixelFormatIndex, &pfd) == 0)
        {
			Trace::WriteLine("Unable to set pixel format");
            return;
        }
		
		EndGL();

        //Create rendering context
        _glRenderingContext = wglCreateContext(_deviceContext);

		if (SharedContextView != nullptr)
			wglShareLists(SharedContextView->_glRenderingContext, _glRenderingContext); 

        if (!_glRenderingContext)
        {
			Trace::WriteLine("Unable to get rendering context");
            return;
        }
		if (wglMakeCurrent(_deviceContext, _glRenderingContext) == 0)
        {
			Trace::WriteLine("Unable to make rendering context current");
            return;
        }
        //Set up OpenGL related characteristics
        BeginGL();
#if defined(SHADERS)
		glewInit();
#endif

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

		// Configure the view
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		glShadeModel(GL_SMOOTH);
#if defined(SHADERS)
		ShaderProgram::initShaders();
#endif
	}	
}

#elif defined(__linux__)

OpenGLSceneView::OpenGLSceneView(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    _coreView = new OpenGLSceneViewCore(this);
    this->setMouseTracking(true);
}

OpenGLSceneView::~OpenGLSceneView()
{
    delete _coreView;
}

OpenGLSceneViewCore *OpenGLSceneView::coreView()
{
    return _coreView;
}

QSize OpenGLSceneView::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize OpenGLSceneView::sizeHint() const
{
    return QSize(400, 400);
}

NSRect OpenGLSceneView::bounds()
{
    return NSMakeRect(0, 0, this->size().width(), this->size().height());
}

void OpenGLSceneView::setNeedsDisplay()
{
    this->update();
}

void OpenGLSceneView::manipulationStarted()
{

}

void OpenGLSceneView::manipulationEnded()
{

}

void OpenGLSceneView::selectionChanged()
{

}

bool OpenGLSceneView::texturePaintEnabled()
{
    return false;
}

void OpenGLSceneView::makeCurrentContext()
{
    this->makeCurrent();
}

void OpenGLSceneView::initializeGL()
{
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void OpenGLSceneView::paintGL()
{
    _coreView->draw();
}

void OpenGLSceneView::resizeGL(int width, int height)
{

}

NSPoint OpenGLSceneView::locationFromMouseEvent(QMouseEvent *event)
{
    NSPoint point = NSMakePoint(event->posF().x(), event->posF().y());
    point.y = this->bounds().size.height - point.y;
    return point;
}

void OpenGLSceneView::mousePressEvent(QMouseEvent *event)
{
    NSPoint point = locationFromMouseEvent(event);
    if (event->buttons() & Qt::LeftButton)
    {
        _coreView->mouseDown(point, event->modifiers() & Qt::AltModifier);
    }
    else if (event->buttons() & Qt::RightButton)
    {
        _coreView->rightMouseDown(point);
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        _coreView->otherMouseDown(point);
    }
}

void OpenGLSceneView::mouseMoveEvent(QMouseEvent *event)
{
    NSPoint point = locationFromMouseEvent(event);
    if (event->buttons() & Qt::LeftButton)
    {
        _coreView->mouseDragged(point,
                                event->modifiers() & Qt::AltModifier,
                                event->modifiers() & Qt::ControlModifier); // Command
    }
    else if (event->buttons() & Qt::RightButton)
    {
        _coreView->rightMouseDragged(point, event->modifiers() & Qt::AltModifier);
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        _coreView->otherMouseDragged(point, event->modifiers() & Qt::AltModifier);
    }
    else
    {
        _coreView->mouseMoved(point);
    }
}

void OpenGLSceneView::mouseReleaseEvent(QMouseEvent *event)
{
    NSPoint point = locationFromMouseEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        _coreView->mouseUp(point,
                           event->modifiers() & Qt::AltModifier,
                           event->modifiers() & Qt::ControlModifier, // Command
                           false, // Control select through is disabled
                           event->modifiers() & Qt::ShiftModifier,
                           1);
    }
}

void OpenGLSceneView::mouseDoubleClickEvent(QMouseEvent *event)
{
    NSPoint point = locationFromMouseEvent(event);
    _coreView->mouseUp(point,
                       event->modifiers() & Qt::AltModifier,
                       event->modifiers() & Qt::ControlModifier, // Command
                       false, // Control select through is disabled
                       event->modifiers() & Qt::ShiftModifier,
                       2);
}

#endif
