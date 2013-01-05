//
//  OpenGLSceneView.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//


#import "OpenGLDrawing.h"
#import "OpenGLSceneView.h"

const float perspectiveAngle = 45.0f;
const float minDistance = 1.0f;
const float maxDistance = 500.0f;

NSOpenGLPixelFormat *globalPixelFormat = nil;
NSOpenGLContext *globalGLContext = nil;

@implementation OpenGLSceneView

@synthesize manipulated, displayed, delegate;

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
    displayed = nil;
    manipulated = nil;
    delegate = nil;
    
    selectionOffset = new Vector3D();
    isManipulating = NO;
    isSelecting = NO;
    highlightCameraMode = NO;
    
    camera = new Camera();
    camera->SetRadians(Vector2D(-45.0f * DEG_TO_RAD, 45.0f * DEG_TO_RAD));
    camera->SetZoom(20.0f);
    
    perspectiveRadians = new Vector2D(camera->GetRadians());
    
    lastPoint = NSMakePoint(0, 0);
    
    defaultManipulator = new Manipulator(ManipulatorTypeDefault);
    translationManipulator = new Manipulator(ManipulatorTypeTranslation);
    rotationManipulator = new Manipulator(ManipulatorTypeRotation);
    scaleManipulator = new Manipulator(ManipulatorTypeScale);

    currentManipulator = defaultManipulator;
    
    manipulatorWrapper = [[OpenGLSelectingWrapper alloc] init];
    
    cameraMode = CameraModePerspective;
    
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
    
    [ShaderProgram initShaders];
}

- (void)dealloc
{
	delete perspectiveRadians;
	delete selectionOffset;
	delete camera;
    
    delete defaultManipulator;
    delete translationManipulator;
    delete rotationManipulator;
	delete scaleManipulator;
}

- (ManipulatorType)currentManipulator
{
	if (currentManipulator == defaultManipulator)
		return ManipulatorTypeDefault;
	if (currentManipulator == translationManipulator)
		return ManipulatorTypeTranslation;
	if (currentManipulator == rotationManipulator)
		return ManipulatorTypeRotation;
	if (currentManipulator == scaleManipulator)
		return ManipulatorTypeScale;
	return ManipulatorTypeDefault;
}

- (void)setCurrentManipulator:(ManipulatorType)value
{
	switch (value)
	{
		case ManipulatorTypeDefault:
			currentManipulator = defaultManipulator;
			break;
		case ManipulatorTypeTranslation:
			currentManipulator = translationManipulator;
			break;
		case ManipulatorTypeRotation:
			currentManipulator = rotationManipulator;
			break;
		case ManipulatorTypeScale:
			currentManipulator = scaleManipulator;
			break;
		default:
			break;
	}
	[self setNeedsDisplay:YES];
}

- (Camera)camera
{
	return *camera;
}

- (void)setCamera:(Camera)aCamera
{
	*camera = aCamera;
}

- (enum CameraMode)cameraMode
{
	return cameraMode;
}

- (void)setCameraMode:(enum CameraMode)value
{	
	if (cameraMode == CameraModePerspective)
	{
		*perspectiveRadians = camera->GetRadians();
	}
	cameraMode = value;
	switch (cameraMode)
	{
		case CameraModePerspective:
			camera->SetRadians(*perspectiveRadians);
			break;
		case CameraModeTop:
			camera->SetRadians(Vector2D(-90.0f * DEG_TO_RAD, 0));
			break;
		case CameraModeBottom:
			camera->SetRadians(Vector2D(90.0f * DEG_TO_RAD, 0));
			break;
		case CameraModeLeft:
			camera->SetRadians(Vector2D(0, -90.0f * DEG_TO_RAD));
			break;
		case CameraModeRight:
			camera->SetRadians(Vector2D(0, 90.0f * DEG_TO_RAD));
			break;
		case CameraModeFront:
			camera->SetRadians(Vector2D());
			break;
		case CameraModeBack:
			camera->SetRadians(Vector2D(0, 180.0f * DEG_TO_RAD));
			break;
		default:
			break;
	}
	[self setNeedsDisplay:YES];
}

- (void)reshape
{
	[self setNeedsDisplay:YES];
}

- (NSRect)reshapeViewport
{
	// Convert up to window space, which is in pixel units.
	NSRect baseRect = [self convertRectToBase:[self bounds]];
	// Now the result is glViewport()-compatible.
	glViewport(0, 0, baseRect.size.width, baseRect.size.height);
	return baseRect;
}

- (void)applyProjectionWithRect:(NSRect)baseRect
{
	float w_h = baseRect.size.width / baseRect.size.height;
		
	if (cameraMode != CameraModePerspective)
	{
		float x = camera->GetZoom() * w_h;
		float y = camera->GetZoom(); 
		
		x /= 2.0f;
		y /= 2.0f;
		
		glOrtho(-x, x, -y, y, -maxDistance, maxDistance);
	}
	else 
	{
		gluPerspective(perspectiveAngle, w_h, minDistance, maxDistance);        
	}
}

- (void)setupViewportAndCamera
{
	NSRect baseRect = [self reshapeViewport];
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	[self applyProjectionWithRect:baseRect];	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera->GetViewMatrix());
}

#pragma mark Drawing

- (void)drawGridWithSize:(int)size step:(int)step
{
	float dark = 0.1f;
	float light = 0.4f;
	
	glPushMatrix();
	
	if (cameraMode == CameraModeFront || cameraMode == CameraModeBack)
		glRotatef(90.0f, 1, 0, 0);
	else if (cameraMode == CameraModeLeft || cameraMode == CameraModeRight)
		glRotatef(90.0f, 0, 0, 1);
	
	glBegin(GL_LINES);
	for (int x = -size; x <= size; x += step)
    {
		if (x == 0)
			glColor3f(dark, dark, dark);
		else
			glColor3f(light, light, light);
		
        glVertex3i(x, 0, -size);
        glVertex3i(x, 0, size);
	}
    for (int z = -size; z <= size; z += step)
    {
		if (z == 0)
			glColor3f(dark, dark, dark);
		else
			glColor3f(light, light, light);
		
		glVertex3i(-size, 0, z);
        glVertex3i(size, 0, z);
    }
	glEnd();
	
	glPopMatrix();
}

- (void)drawManipulatedAndDisplayedForSelection:(BOOL)forSelection
{
	if (displayed != manipulated)
		[displayed drawForSelection:forSelection];
	[manipulated drawForSelection:forSelection];
}

- (void)drawOrthoDefaultManipulator
{
	[self beginOrtho];
	glPushMatrix();
	glTranslatef(18.0f, 18.0f, 0.0f);
	glMultMatrixf(camera->GetRotationQuaternion().ToMatrix());
    defaultManipulator->position = Vector3D();
    defaultManipulator->size = 15.0f;
    defaultManipulator->draw(camera->GetAxisZ(), defaultManipulator->position, highlightCameraMode);
	glPopMatrix();
	[self endOrtho];
}

- (void)drawCurrentManipulator
{
	if ([manipulated selectedCount] > 0)
	{
        currentManipulator->position = manipulated.selectionCenter;
        
		if (cameraMode == CameraModePerspective)
        {
            Vector3D manipulatorPosition = [manipulated selectionCenter];
            Vector3D cameraPosition = -camera->GetCenter() + (camera->GetAxisZ() * camera->GetZoom());
            
            float distance = cameraPosition.Distance(manipulatorPosition);
            
            //debugString = [NSString stringWithFormat:@"%.2f", distance];            

            currentManipulator->size = distance * 0.15f;
        }
		else
        {
            currentManipulator->size = camera->GetZoom() * 0.17f;
        }
        
        scaleManipulator->rotation = manipulated.selectionRotation;
        if (currentManipulator != defaultManipulator)
            currentManipulator->draw(camera->GetAxisZ(), manipulated.selectionCenter);
	}
}

- (void)drawSelectionRect
{
	if (isSelecting)
	{
		[self beginOrtho];
		glDisable(GL_TEXTURE_2D);
		float color[4] = { 0.2f, 0.4f, 1.0f, 0.0f };
		color[3] = 0.2f;
		glColor4fv(color);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glRectf(lastPoint.x, lastPoint.y, currentPoint.x, currentPoint.y);
		color[3] = 0.9f;
		glColor4fv(color);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glRectf(lastPoint.x, lastPoint.y, currentPoint.x, currentPoint.y);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		[self endOrtho];
	}	
}

- (void)drawRect:(NSRect)rect
{
    [ShaderProgram resetProgram];
    
	float clearColor = 0.6f;
	glClearColor(clearColor, clearColor, clearColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
	
	[self setupViewportAndCamera];

	[self drawGridWithSize:10 step:2];
    
	[self drawManipulatedAndDisplayedForSelection:NO];
    
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	
	[self drawCurrentManipulator];
	
	[self drawOrthoDefaultManipulator];
	[self drawSelectionRect];
    
    if (debugString)
    {
        [self beginOrtho];
        [FPTexture drawString:debugString atPoint:CGPointMake(5.0f, [self bounds].size.height)];
        [self endOrtho];
    }
    
    glEnable(GL_DEPTH_TEST);
		
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
/*
- (void)getCurrentTransform:(Matrix4x4 *)matrix
{
    OpenGLManipulatingController *controller = (OpenGLManipulatingController *)manipulated;
    
    if ([[controller model] isKindOfClass:[Item class]])
    {
        *matrix = *[controller modelTransform];
    }
    else 
    {
        ItemCollection * itemCollection = (ItemCollection *)[controller model];
        Item *item = [itemCollection firstSelectedItem];
        if (item != nil)
            matrix->TranslateRotateScale(item.position, item.rotation, item.scale);            
    }
}

- (Mesh2 *)currentMesh
{
    OpenGLManipulatingController *controller = (OpenGLManipulatingController *)manipulated;
    
    Mesh2 *mesh = NULL;
    
    if ([[controller model] isKindOfClass:[Item class]])
    {
        mesh = [(Item *)[controller model] mesh];
    }
    else 
    {
        ItemCollection * itemCollection = (ItemCollection *)[controller model];
        mesh = [itemCollection currentMesh];
    }
    
    return mesh;
}

- (void)paintOnTextureWithFirstPoint:(NSPoint)firstPoint secondPoint:(NSPoint)secondPoint
{
    Mesh2 *mesh = [self currentMesh];
    
    if (!mesh)
        return;
    
    int viewport[4];
    double modelview[16];
    double projection[16];
    double posX = 0.0, posY = 0.0, posZ = 0.0;
    
    [[self openGLContext] makeCurrentContext];
    
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    Vector3D cameraOrigin = camera->GetAxisZ() * camera->GetZoom();
    const float z = 1.0f;
    
    Matrix4x4 modelTransform;
    [self getCurrentTransform:&modelTransform];
    
    Matrix4x4 transform;
    transform.Translate(camera->GetCenter());
    transform = transform * modelTransform;
    transform = transform.Inverse();
    
    cameraOrigin = transform.Transform(cameraOrigin);
    
    Vector2D startPoint = Vector2D(firstPoint.x, firstPoint.y);
    Vector2D endPoint = Vector2D(secondPoint.x, secondPoint.y);
    
    Vector2D move = endPoint - startPoint;
    float advance = 1.0f / move.GetLength();
    
    vector<Vector2D> *UVs = new vector<Vector2D>();
    float u, v;

    for (float t = 0.0f; t < 1.0f; t += advance)
    {
        Vector2D current = startPoint + move * t;
        
        gluUnProject(current.x, current.y, z, modelview, projection, viewport, &posX, &posY, &posZ);
        Vector3D unprojectedMousePosition = Vector3D((float)posX, (float)posY, (float)posZ);
        Vector3D mouseDirection = unprojectedMousePosition - cameraOrigin;
        mouseDirection = transform.Transform(mouseDirection);
       
        TriangleNode *nearest = mesh->rayToUV(cameraOrigin, mouseDirection, u, v);
        if (nearest)
            UVs->push_back(Vector2D(u, v));
    }
    
    gluUnProject(endPoint.x, endPoint.y, z, modelview, projection, viewport, &posX, &posY, &posZ);
    Vector3D unprojectedMousePosition = Vector3D((float)posX, (float)posY, (float)posZ);
    Vector3D mouseDirection = unprojectedMousePosition - cameraOrigin;
    mouseDirection = transform.Transform(mouseDirection);
    
    TriangleNode *nearest = mesh->rayToUV(cameraOrigin, mouseDirection, u, v);
    if (nearest)
        UVs->push_back(Vector2D(u, v));
    
    [[mesh->texture() canvas] lockFocus];
    
    NSColor *color = [[delegate brushColor] colorWithAlphaComponent:0.3f];
    [color setStroke];
    
    NSBezierPath *bezierPath = [NSBezierPath bezierPath];
    [bezierPath setLineWidth:[delegate brushSize]];
    
    Vector2D lastUV;
    
    for (uint i = 0; i < UVs->size(); i++)
    {
        Vector2D uv = UVs->at(i);
        NSPoint point = NSMakePoint(uv.x, uv.y);
        if (i == 0)
            [bezierPath moveToPoint:point];
        else
        {
            float distance = lastUV.Distance(uv);
            if (distance > 10.0f)
                [bezierPath moveToPoint:point];
            else
                [bezierPath lineToPoint:point];
        }
        
        lastUV = uv;
    }
    
    delete UVs;

    [bezierPath stroke];
    
    [[mesh->texture() canvas] unlockFocus];        
    [mesh->texture() updateTexture];
    [self setNeedsDisplay:YES];
}
*/
- (void)mouseDown:(NSEvent *)e
{
	lastPoint = [self locationFromNSEvent:e];
    isPainting = NO;
    
    if ([e modifierFlags] & NSAlternateKeyMask)
	{
		isManipulating = isSelecting = NO;
		return;
	}    
    	
	if (highlightCameraMode)
	{
		switch (cameraMode)
		{
			case CameraModeTop:
				self.cameraMode = CameraModeBottom;
				break;
			case CameraModeBottom:
				self.cameraMode = CameraModeTop;
				break;
			case CameraModeLeft:
				self.cameraMode = CameraModeRight;
				break;
			case CameraModeRight:
				self.cameraMode = CameraModeLeft;
				break;
			case CameraModeFront:
				self.cameraMode = CameraModeBack;
				break;
			case CameraModeBack:
				self.cameraMode = CameraModeFront;
				break;
			default:
				break;
		}
	}
	else if (delegate.texturePaintEnabled)
    {
        isPainting = YES;
        //[self paintOnTextureWithFirstPoint:lastPoint secondPoint:lastPoint];
        return;
    }
    else if ([manipulated selectedCount] > 0 && currentManipulator->selectedIndex < UINT_MAX)
	{
		if (currentManipulator == translationManipulator)
		{
			*selectionOffset = [self translationFromPoint:lastPoint];
			*selectionOffset -= [manipulated selectionCenter];
			isManipulating = YES;
		}
		else if (currentManipulator == rotationManipulator)
		{
			[self rotationFromPoint:lastPoint lastPosition:selectionOffset];
			isManipulating = YES;
		}
		else if (currentManipulator == scaleManipulator)
		{
			[self scaleFromPoint:lastPoint lastPosition:selectionOffset];
			isManipulating = YES;
		}
		if (isManipulating)
			[delegate manipulationStartedInView:self];
	}
	else
	{		
		isSelecting = YES;
	}
}

- (NSRect)orthoManipulatorRect
{
	return NSMakeRect(3.0f, 3.0f, 30.0f, 30.0f);
}

- (void)mouseMoved:(NSEvent *)e
{
	highlightCameraMode = NO;
	currentPoint = [self locationFromNSEvent:e];
	if ([manipulated selectedCount] > 0)
	{
		if (!isManipulating)
		{
            currentManipulator->selectedIndex = UINT_MAX;
            currentManipulator->position = manipulated.selectionCenter;
            manipulatorWrapper.cppSelecting = currentManipulator;
			[self selectWithPoint:currentPoint selecting:manipulatorWrapper selectionMode:OpenGLSelectionModeAdd];
			[self setNeedsDisplay:YES];
		}
	}
	
	if (currentManipulator-> selectedIndex == UINT_MAX)
	{
		if (NSPointInRect(currentPoint, [self orthoManipulatorRect]))
			highlightCameraMode = YES;
		[self setNeedsDisplay:YES];
	}
}

- (void)mouseExited:(NSEvent *)e
{
	highlightCameraMode = NO;
	[self setNeedsDisplay:YES];
}

- (void)mouseUp:(NSEvent *)e
{
    isPainting = NO;
    
	currentPoint = [self locationFromNSEvent:e];
	if (isManipulating)
	{
		[delegate manipulationEndedInView:self];
		isManipulating = NO;
	}
	if (isSelecting)
	{
		isSelecting = NO;
		OpenGLSelectionMode selectionMode = OpenGLSelectionModeAdd;
        
		NSUInteger flags = [e modifierFlags];
				
		if ((flags & NSCommandKeyMask) == NSCommandKeyMask)
			selectionMode = OpenGLSelectionModeInvert;
		else if ((flags & NSShiftKeyMask) == NSShiftKeyMask)
			selectionMode = OpenGLSelectionModeAdd;
		else
			[manipulated changeSelection:NO];
		
		NSRect rect = [self currentRect];
		if ([e clickCount] <= 1 && rect.size.width > 5.0f && rect.size.height > 5.0f)
		{
            [self selectWithRect:[self currentRect] 
					   selecting:manipulated 
				   selectionMode:selectionMode
                   selectThrough:(flags & NSControlKeyMask) == NSControlKeyMask];
		}
		else
        {
            if ([e clickCount] == 2)
            {
                if (selectionMode == OpenGLSelectionModeInvert)
                    selectionMode = OpenGLSelectionModeInvertExpand;
                else
                    selectionMode = OpenGLSelectionModeExpand;
            }
            
        	[self selectWithPoint:currentPoint 
						selecting:manipulated
					selectionMode:selectionMode];
        }
		
		[delegate selectionChangedInView:self];
		[self setNeedsDisplay:YES];
	}
}

- (void)mouseDragged:(NSEvent *)e
{
	currentPoint = [self locationFromNSEvent:e];
	float deltaX = currentPoint.x - lastPoint.x;
	float deltaY = currentPoint.y - lastPoint.y;
    
	NSUInteger flags = [e modifierFlags];
	NSUInteger combinedFlags = NSAlternateKeyMask | NSCommandKeyMask;
	
	if ((flags & combinedFlags) == combinedFlags)
	{
		NSRect bounds = [self bounds];
		float w = bounds.size.width;
		float h = bounds.size.height;
		float sensitivity = (w + h) / 2.0f;
		sensitivity = 1.0f / sensitivity;
        sensitivity *= camera->GetZoom() * 1.12f;
		camera->LeftRight(-deltaX * sensitivity);
		camera->UpDown(deltaY * sensitivity);
		
		lastPoint = currentPoint;
		[self setNeedsDisplay:YES];
	}
	else if ((flags & NSAlternateKeyMask) == NSAlternateKeyMask)
	{
		if (cameraMode == CameraModePerspective)
		{
			lastPoint = currentPoint;
			const float sensitivity = 0.005f;
			camera->RotateLeftRight(deltaX * sensitivity);
			camera->RotateUpDown(-deltaY * sensitivity);
			[self setNeedsDisplay:YES];
		}
	}
	else if (delegate.texturePaintEnabled)
    {
        //[self paintOnTextureWithFirstPoint:lastPoint secondPoint:currentPoint];
        lastPoint = currentPoint;
    }
    else if (isManipulating)
	{
		lastPoint = currentPoint;
		if (currentManipulator == translationManipulator)
		{
			Vector3D move = [self translationFromPoint:currentPoint];
			move -= *selectionOffset;
			move -= [manipulated selectionCenter];
			[manipulated moveSelectedByOffset:move];
			[self setNeedsDisplay:YES];
		}
		else if (currentManipulator == rotationManipulator)
		{
			Quaternion rotation = [self rotationFromPoint:currentPoint lastPosition:selectionOffset];
			[manipulated rotateSelectedByOffset:rotation];
			[self setNeedsDisplay:YES];
		}
		else if (currentManipulator == scaleManipulator)
		{
			Vector3D scale = [self scaleFromPoint:currentPoint lastPosition:selectionOffset];
			[manipulated scaleSelectedByOffset:scale];
			[self setNeedsDisplay:YES];
		}
	}
	else if (isSelecting)
	{
		[self setNeedsDisplay:YES];
	}
}

- (void)otherMouseDown:(NSEvent *)e
{
	lastPoint = [self locationFromNSEvent:e];
}

- (void)otherMouseDragged:(NSEvent *)e
{
	currentPoint = [self locationFromNSEvent:e];	
	float deltaX = currentPoint.x - lastPoint.x;
	float deltaY = currentPoint.y - lastPoint.y;
	
	if (([e modifierFlags] & NSAlternateKeyMask) == NSAlternateKeyMask)
	{
		NSRect bounds = [self bounds];
		float w = bounds.size.width;
		float h = bounds.size.height;
		float sensitivity = (w + h) / 2.0f;
		sensitivity = 1.0f / sensitivity;
		camera->LeftRight(-deltaX * camera->GetZoom() * sensitivity);
		camera->UpDown(deltaY * camera->GetZoom() * sensitivity);
		
		lastPoint = currentPoint;
		[self setNeedsDisplay:YES];
	}
}

- (void)rightMouseDown:(NSEvent *)e
{
	lastPoint = [self locationFromNSEvent:e];
}

- (void)rightMouseDragged:(NSEvent *)e
{	
	currentPoint = [self locationFromNSEvent:e];	
	float deltaY = currentPoint.y - lastPoint.y;
	
	if (([e modifierFlags] & NSAlternateKeyMask) == NSAlternateKeyMask)
	{
		float sensitivity = camera->GetZoom() * 0.02f;
		
		camera->Zoom(-deltaY * sensitivity);
		
		lastPoint = currentPoint;
		[self setNeedsDisplay:YES];
	}
}

- (void)scrollWheel:(NSEvent *)e
{	
	float deltaX = [e deltaX];
	float deltaY = [e deltaY];
	
	NSUInteger flags = [e modifierFlags];
	NSUInteger combinedFlags = NSAlternateKeyMask | NSCommandKeyMask;
	
	if ((flags & combinedFlags) == combinedFlags)
	{
		NSRect bounds = [self bounds];
		float w = bounds.size.width;
		float h = bounds.size.height;
		float sensitivity = (w + h) / 6.0f;
		sensitivity = 1.0f / sensitivity;
		camera->LeftRight(-deltaX * camera->GetZoom() * sensitivity);
		camera->UpDown(-deltaY * camera->GetZoom() * sensitivity);
		[self setNeedsDisplay:YES];
	}
	else if ((flags & NSAlternateKeyMask) == NSAlternateKeyMask)
	{
		if (cameraMode == CameraModePerspective)
		{
			const float sensitivity = 0.02f;
			camera->RotateLeftRight(-deltaX * sensitivity);
			camera->RotateUpDown(-deltaY * sensitivity);
			[self setNeedsDisplay:YES];
		}
	}
	else
	{
		float sensitivity = camera->GetZoom() * 0.02f;
		camera->Zoom(deltaY * sensitivity);
		[self setNeedsDisplay:YES];
	}
}

- (NSRect)currentRect
{
	float minX = Min(lastPoint.x, currentPoint.x);
	float maxX = Max(lastPoint.x, currentPoint.x);
	float minY = Min(lastPoint.y, currentPoint.y);
	float maxY = Max(lastPoint.y, currentPoint.y);
	
	return NSMakeRect(minX, minY, maxX - minX, maxY - minY);
}

#pragma mark Ortho

- (void)beginOrtho
{
	NSRect rect = [self bounds];
	glDepthMask(GL_FALSE);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();			
	glOrtho(0, rect.size.width, 0, rect.size.height, -maxDistance, maxDistance);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();			
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 1, 1, 1);
}

- (void)endOrtho
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
	glPopMatrix();				
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();				
	glMatrixMode(GL_MODELVIEW);
	glDepthMask(GL_TRUE);
}

#pragma mark Selection

const uint kMaxSelectedIndicesCount = 2000 * 2000;  // max width * max height resolution

uint selectedIndices[kMaxSelectedIndicesCount];

- (NSMutableIndexSet *)selectWithX:(int)x
                                 y:(int)y
                             width:(int)width
                            height:(int)height
                         selecting:(id<OpenGLSelecting>)selecting 
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    [self setupViewportAndCamera];
    
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    if ([selecting respondsToSelector:@selector(drawAllForSelection)])
    {
        [selecting drawAllForSelection];
    }
    else
    {
        for (uint i = 0; i < [selecting selectableCount]; i++)
        {
            uint colorIndex = i + 1;
            glColor4ubv((GLubyte *)&colorIndex);
            [selecting drawForSelectionAtIndex:i];
        }
    }
    
	glFinish();
    
    uint selectedIndicesCount = (uint)width * (uint)height;

    if (selectedIndicesCount >= kMaxSelectedIndicesCount)
        return nil;
    
    if (selectedIndicesCount > 0)
    {
    	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, selectedIndices);
        
        NSMutableIndexSet *uniqueIndices = [NSMutableIndexSet indexSet];
 
        for (uint i = 0; i < selectedIndicesCount; i++)
        {
            uint selectedIndex = selectedIndices[i];
            if (selectedIndex > 0 && selectedIndex - 1 < [selecting selectableCount])
                [uniqueIndices addIndex:selectedIndex - 1];
        }
        
        return uniqueIndices;        
    }

    return nil;    
}

- (void)selectWithPoint:(NSPoint)point 
			  selecting:(id<OpenGLSelecting>)selecting 
		  selectionMode:(enum OpenGLSelectionMode)selectionMode
{
    if (selecting == nil || [selecting selectableCount] <= 0)
		return;
    
    if ([selecting respondsToSelector:@selector(willSelectThrough:)])
		[selecting willSelectThrough:NO];
    
    [[self openGLContext] makeCurrentContext];
        
	NSMutableIndexSet *uniqueIndices = [self selectWithX:point.x - 5
                                                       y:point.y - 5
                                                   width:10
                                                  height:10
                                               selecting:selecting];
    
    [uniqueIndices enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) 
    {
         [selecting selectObjectAtIndex:idx withMode:selectionMode];
         *stop = YES;
    }];
    
    if ([selecting respondsToSelector:@selector(didSelect)])
		[selecting didSelect];
}

- (void)selectWithRect:(NSRect)rect
			 selecting:(id<OpenGLSelecting>)selecting
		 selectionMode:(enum OpenGLSelectionMode)selectionMode
         selectThrough:(BOOL)selectThrough
{
    if (selecting == nil || [selecting selectableCount] <= 0)
		return;
    
    if ([selecting respondsToSelector:@selector(willSelectThrough:)])
		[selecting willSelectThrough:selectThrough];
    
    [[self openGLContext] makeCurrentContext];

    if ([selecting respondsToSelector:@selector(useGLProject)])
    {
        if ([selecting useGLProject])
        {
            [selecting glProjectSelectWithX:rect.origin.x
                                          y:rect.origin.y
                                      width:rect.size.width
                                     height:rect.size.height
                              selectionMode:selectionMode];
            
            if ([selecting respondsToSelector:@selector(didSelect)])
                [selecting didSelect];
            
            return;
        }
    }    
    
    NSMutableIndexSet *uniqueIndices;
    
    if ([selecting respondsToSelector:@selector(needsCullFace)] && [selecting needsCullFace])
    {
        glCullFace(GL_CCW);
        glEnable(GL_CULL_FACE);
        
        NSMutableIndexSet *uniqueIndices1 = [self selectWithX:rect.origin.x
                                                            y:rect.origin.y
                                                        width:rect.size.width
                                                       height:rect.size.height
                                                    selecting:selecting];
        glDisable(GL_CULL_FACE);
        
        NSMutableIndexSet *uniqueIndices2 = [self selectWithX:rect.origin.x
                                                            y:rect.origin.y
                                                        width:rect.size.width
                                                       height:rect.size.height
                                                    selecting:selecting];
        
        uniqueIndices = [NSMutableIndexSet indexSet];
        if (uniqueIndices1 != nil)
            [uniqueIndices addIndexes:uniqueIndices1];
        if (uniqueIndices2 != nil)
            [uniqueIndices addIndexes:uniqueIndices2];
    }
    else
    {
        uniqueIndices = [self selectWithX:rect.origin.x
                                        y:rect.origin.y
                                    width:rect.size.width
                                   height:rect.size.height
                                selecting:selecting];
    }
    
    [uniqueIndices enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) 
    {
        [selecting selectObjectAtIndex:idx withMode:selectionMode];
    }];
    
    if ([selecting respondsToSelector:@selector(didSelect)])
		[selecting didSelect];
}

#pragma mark Position retrieve

- (Vector3D)positionInSpaceByPoint:(NSPoint)point
{
	int viewport[4];
    double modelview[16];
    double projection[16];
    float winX, winY, winZ;
    double posX = 0.0, posY = 0.0, posZ = 0.0;
	
	[[self openGLContext] makeCurrentContext];
	
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
	
    winX = point.x;
    winY = point.y;
    glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	
	return Vector3D((float)posX, (float)posY, (float)posZ);
}

- (void)drawSelectionPlaneWithIndex:(int)index
{
	Vector3D position = [manipulated selectionCenter];
	
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	DrawSelectionPlane((PlaneAxis)(PlaneAxisX + index));
	glPopMatrix();
}

- (Vector3D)positionFromAxis:(Axis)axis point:(NSPoint)point
{
	const float size = 4000.0f;
	DrawPlane(camera->GetAxisX(), camera->GetAxisY(), size);
	
	Vector3D position = [self positionInSpaceByPoint:point];
	Vector3D result = [manipulated selectionCenter];
	result[axis] = position[axis];
	return result;
}

- (Vector3D)positionFromRotatedAxis:(Axis)axis point:(NSPoint)point rotation:(Quaternion)rotation
{
	const float size = 4000.0f;
	DrawPlane(camera->GetAxisX(), camera->GetAxisY(), size);
	
	Vector3D position = [self positionInSpaceByPoint:point];
	Vector3D result = [manipulated selectionCenter];
	position = rotation.Conjugate().ToMatrix().Transform(position);
	result[axis] = position[axis];
	return result;
}

- (Vector3D)positionFromPlaneAxis:(PlaneAxis)plane point:(NSPoint)point
{
	int index = plane - PlaneAxisX;
	[self drawSelectionPlaneWithIndex:index];
	Vector3D position = [self positionInSpaceByPoint:point];
	Vector3D result = position;
	result[index] = [manipulated selectionCenter][index];
	return result;
}

#pragma mark Translation, Scale, Rotation

- (Vector3D)translationFromPoint:(NSPoint)point
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadMatrixf(camera->GetViewMatrix());
	
	Vector3D position = [manipulated selectionCenter];
	uint selectedIndex = currentManipulator->selectedIndex;
	
	if (selectedIndex >= AxisX && selectedIndex <= AxisZ)
		return [self positionFromAxis:(Axis)selectedIndex point:point];
	if (selectedIndex >= PlaneAxisX && selectedIndex <= PlaneAxisZ)
		return [self positionFromPlaneAxis:(PlaneAxis)selectedIndex point:point];
	
	return position;
}

- (Vector3D)scaleFromPoint:(NSPoint)point lastPosition:(Vector3D *)lastPosition
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadMatrixf(camera->GetViewMatrix());
	
	Vector3D position = [manipulated selectionCenter];
	uint selectedIndex = currentManipulator->selectedIndex;
	
	Vector3D scale = Vector3D();
	
	if (selectedIndex < UINT_MAX)
	{
		ManipulatorWidget &selectedWidget = currentManipulator->widgetAtIndex(selectedIndex);
		Axis selectedAxis = selectedWidget.axis;
		if (selectedAxis >= AxisX && selectedAxis <= AxisZ)
		{
			position = [self positionFromRotatedAxis:selectedAxis point:point rotation:[manipulated selectionRotation]];
			scale = position - *lastPosition;
		}
		else if (selectedAxis == Center)
		{
			position = [self positionFromPlaneAxis:PlaneAxisY point:point];
			scale = position - *lastPosition;
            scale.y = scale.x;
            scale.z = scale.x;
		}
		
		*lastPosition = position;
		scale *= 2.0f;
	}
			
	return scale;
}

- (Quaternion)rotationFromPoint:(NSPoint)point lastPosition:(Vector3D *)lastPosition
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadMatrixf(camera->GetViewMatrix());
	
	Quaternion quaternion;
	Vector3D position;
	float angle;
	
	uint selectedIndex = currentManipulator->selectedIndex;
	
	position = [self positionFromPlaneAxis:(PlaneAxis)(selectedIndex + 3) point:point];
	position -= [manipulated selectionCenter];
	
	switch (selectedIndex)
    {
		case AxisX:
			angle = atan2f(position.y, position.z) - atan2f(lastPosition->y, lastPosition->z);
			quaternion.FromAngleAxis(-angle, Vector3D(1, 0, 0));
			break;
		case AxisY:
			angle = atan2f(position.x, position.z) - atan2f(lastPosition->x, lastPosition->z);
			quaternion.FromAngleAxis(angle, Vector3D(0, 1, 0));
			break;
		case AxisZ:
			angle = atan2f(position.x, position.y) - atan2f(lastPosition->x, lastPosition->y);
			quaternion.FromAngleAxis(-angle, Vector3D(0, 0, 1));
			break;
	}
	
	*lastPosition = position;
	return quaternion;
}

@end
