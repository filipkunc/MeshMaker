//
//  OpenGLSceneView.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//


#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import "OpenGLSceneView.h"

const float perspectiveAngle = 45.0f;
const float minDistance = 0.2f;
const float maxDistance = 1000.0f;

@implementation OpenGLSceneView

@synthesize manipulated, displayed, delegate;

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
		displayed = nil;
		manipulated = nil;
		delegate = nil;
		
		// The GL context must be active for these functions to have an effect
		NSOpenGLContext *glcontext = [self openGLContext];
		[glcontext makeCurrentContext];
		
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glShadeModel(GL_SMOOTH);
		
		selectionOffset = new Vector3D();
		isManipulating = NO;
		isSelecting = NO;
		highlightCameraMode = NO;
		
		camera = new Camera();
		camera->SetRadX(-45.0f * DEG_TO_RAD);
		camera->SetRadY(45.0f * DEG_TO_RAD);
		camera->SetZoom(20.0f);
		
		perspectiveRadians = new Vector3D(camera->GetRadians());
		
		lastPoint = NSMakePoint(0, 0);
		
		defaultManipulator = [[Manipulator alloc] initWithManipulatorType:ManipulatorTypeDefault];		
		translationManipulator = [[Manipulator alloc] initWithManipulatorType:ManipulatorTypeTranslation];
		rotationManipulator = [[Manipulator alloc] initWithManipulatorType:ManipulatorTypeRotation];		
		scaleManipulator = [[Manipulator alloc] initWithManipulatorType:ManipulatorTypeScale];
				
		currentManipulator = defaultManipulator;
		
		cameraMode = CameraModePerspective;
		viewMode = ViewModeSolid;
		normalShader = [[ShaderProgram alloc] init];
		[normalShader attachShaderWithType:GL_VERTEX_SHADER resourceInBundle:@"twoSidedLighting"];
		
		flippedShader = [[ShaderProgram alloc] init];
		[flippedShader attachShaderWithType:GL_VERTEX_SHADER resourceInBundle:@"twoSidedLightingFlipped"];

		glEnable(GL_VERTEX_PROGRAM_TWO_SIDE);
		[normalShader linkProgram];
		[flippedShader linkProgram];
	}
	return self;
}

- (void)dealloc
{
	delete perspectiveRadians;
	delete selectionOffset;
	delete camera;
	[defaultManipulator release];
	[translationManipulator release];
	[rotationManipulator release];
	[scaleManipulator release];
	[normalShader release];
	[flippedShader release];
	[super dealloc];
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
			camera->SetRadians(Vector3D(-90.0f * DEG_TO_RAD, 0, 0));
			break;
		case CameraModeBottom:
			camera->SetRadians(Vector3D(90.0f * DEG_TO_RAD, 0, 0));
			break;
		case CameraModeLeft:
			camera->SetRadians(Vector3D(0, -90.0f * DEG_TO_RAD, 0));
			break;
		case CameraModeRight:
			camera->SetRadians(Vector3D(0, 90.0f * DEG_TO_RAD, 0));
			break;
		case CameraModeFront:
			camera->SetRadians(Vector3D(0, 0, 0));
			break;
		case CameraModeBack:
			camera->SetRadians(Vector3D(0, 180.0f * DEG_TO_RAD, 0));
			break;
		default:
			break;
	}
	[self setNeedsDisplay:YES];
}

- (enum ViewMode)viewMode
{
	return viewMode;
}

- (void)setViewMode:(enum ViewMode)value
{
	viewMode = value;
	[self setNeedsDisplay:YES];
}

- (void)reshape
{
	[super setNeedsDisplay:YES];
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

- (void)drawManipulatedAndDisplayed
{
	[Mesh setNormalShader:normalShader];
	[Mesh setFlippedShader:flippedShader];
	
	if (displayed != manipulated)
		[displayed drawWithMode:viewMode];
	[manipulated drawWithMode:viewMode];
}

- (void)drawDefaultManipulator
{
	[defaultManipulator setPosition:Vector3D()];
	if (cameraMode == CameraModePerspective)
		[defaultManipulator setSize:camera->GetPosition().Distance([defaultManipulator position]) * 0.07f];
	else
		[defaultManipulator setSize:camera->GetZoom() * 0.08f];
	[defaultManipulator drawWithAxisZ:camera->GetAxisZ() center:Vector3D()];
}

- (void)drawOrthoDefaultManipulator
{
	[self beginOrtho];
	glPushMatrix();
	glTranslatef(18.0f, 18.0f, 0.0f);
	glMultMatrixf(camera->GetRotationMatrix());
	[defaultManipulator setPosition:Vector3D()];
	[defaultManipulator setSize:15.0f];
	[defaultManipulator drawWithAxisZ:camera->GetAxisZ() 
							   center:[defaultManipulator position] 
						 highlightAll:highlightCameraMode];
	glPopMatrix();
	[self endOrtho];
}

- (void)drawCurrentManipulator
{
	if ([manipulated selectedCount] > 0)
	{
		[currentManipulator setPosition:[manipulated selectionCenter]];
		
		if (cameraMode == CameraModePerspective)
			[currentManipulator setSize:camera->GetPosition().Distance([currentManipulator position]) * 0.15f];
		else
			[currentManipulator setSize:camera->GetZoom() * 0.17f];
		
		[scaleManipulator setRotation:[manipulated selectionRotation]];
		[currentManipulator drawWithAxisZ:camera->GetAxisZ() center:[manipulated selectionCenter]];
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
	float clearColor = 0.6f;
	glClearColor(clearColor, clearColor, clearColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	[self setupViewportAndCamera];
	
	[self drawGridWithSize:10 step:2];
	
	[self drawManipulatedAndDisplayed];
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	
	// removed it is very disturbing
	//[self drawDefaultManipulator];
	
	[self drawCurrentManipulator];
	
	[self drawOrthoDefaultManipulator];
	[self drawSelectionRect];
		
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
	[trackingArea release];
}

- (void)mouseDown:(NSEvent *)e
{
	lastPoint = [self locationFromNSEvent:e];
	
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
	else if ([manipulated selectedCount] > 0 && [currentManipulator selectedIndex] >= 0)
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
			[currentManipulator setSelectedIndex:-1];
			[currentManipulator setPosition:[manipulated selectionCenter]];
			[self selectWithPoint:currentPoint selecting:currentManipulator selectionMode:OpenGLSelectionModeAdd];
			[self setNeedsDisplay:YES];
		}
	}
	
	if ([currentManipulator selectedIndex] < 0)
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
		
		if ([e modifierFlags] & NSControlKeyMask)
			selectionMode = OpenGLSelectionModeInvert;
		else if ([e modifierFlags] & NSShiftKeyMask)
			selectionMode = OpenGLSelectionModeAdd;
		else
			[manipulated changeSelection:NO];
		
		NSRect rect = [self currentRect];
		if (rect.size.width > 5.0f && rect.size.height > 5.0f)
		{
			[self selectWithRect:[self currentRect] 
					   selecting:manipulated 
						cullFace:NO
				   selectionMode:selectionMode];
		}
		else
		{
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
	
	if ([e modifierFlags] & NSAlternateKeyMask)
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
	
	if ([e modifierFlags] & NSAlternateKeyMask)
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
	
	if ([e modifierFlags] & NSAlternateKeyMask)
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
	
	if (([e modifierFlags] & (NSAlternateKeyMask | NSControlKeyMask)) == (NSAlternateKeyMask | NSControlKeyMask))
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
	else if ([e modifierFlags] & NSAlternateKeyMask)
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

- (void)selectWithX:(double)x
				  y:(double)y
			  width:(double)width
			 height:(double)height
		  selecting:(id<OpenGLSelecting>)selecting 
		nearestOnly:(BOOL)nearestOnly
		   cullFace:(BOOL)cullFace
	  selectionMode:(enum OpenGLSelectionMode)selectionMode
{
	if (selecting == nil || [selecting selectableCount] <= 0)
		return;
	
	id aSelecting = selecting;
	if ([aSelecting respondsToSelector:@selector(willSelect)])
	{
		[selecting willSelect];
	}
	
	int objectsFound = 0;
    int viewport[4];
	const unsigned int selectBufferSize = 65535;
	unsigned int selectBuffer[selectBufferSize];
	
	[[self openGLContext] makeCurrentContext];
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glSelectBuffer(selectBufferSize, selectBuffer);
	NSRect baseRect = [self reshapeViewport];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glRenderMode(GL_SELECT);
    glLoadIdentity();
    gluPickMatrix(x, y, width, height, viewport);
	[self applyProjectionWithRect:baseRect];
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera->GetViewMatrix());
    glInitNames();
    glPushName(0);
    glPushMatrix();
    
	if (cullFace)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
	
	for (uint i = 0; i < [selecting selectableCount]; i++)
	{
		glLoadName(i + 1);
		glPushMatrix();
		[selecting drawForSelectionAtIndex:i];
		glPopMatrix();
	}
	
	if (cullFace)
	{
		glDisable(GL_CULL_FACE);
	}
    
	glPopMatrix();
	glFinish();
    objectsFound = (int)glRenderMode(GL_RENDER);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
		
	if (objectsFound <= 0)
		return;
	
	if (nearestOnly)
	{
        unsigned int lowestDepth = selectBuffer[1];
        int selectedIndex = (int)selectBuffer[3];
        for (int i = 1; i < objectsFound; i++)
        {
            if (selectBuffer[(i * 4) + 1] < lowestDepth)
            {
                lowestDepth = selectBuffer[(i * 4) + 1];
                selectedIndex = (int)selectBuffer[(i * 4) + 3];
            }
        }
        selectedIndex--;
        if (selectedIndex >= 0)
			[selecting selectObjectAtIndex:selectedIndex withMode:selectionMode];
	}
	else
	{
		for (int i = 0; i < objectsFound; i++)
        {
            int selectedIndex = (int)selectBuffer[(i * 4) + 3];
            selectedIndex--;
            if (selectedIndex >= 0)
				[selecting selectObjectAtIndex:selectedIndex withMode:selectionMode];
        }
	}	
	
	if ([aSelecting respondsToSelector:@selector(didSelect)])
	{
		[selecting didSelect];
	}
}

- (void)selectWithPoint:(NSPoint)point 
			  selecting:(id<OpenGLSelecting>)selecting 
		  selectionMode:(enum OpenGLSelectionMode)selectionMode
{
	[self selectWithX:point.x
					y:point.y
				width:10.0
			   height:10.0
			selecting:selecting
		  nearestOnly:YES
			 cullFace:NO
		selectionMode:selectionMode];
}

- (void)selectWithRect:(NSRect)rect
			 selecting:(id<OpenGLSelecting>)selecting
			  cullFace:(BOOL)cullFace
		 selectionMode:(enum OpenGLSelectionMode)selectionMode
{
	[self selectWithX:rect.origin.x + rect.size.width / 2
					y:rect.origin.y + rect.size.height / 2 
				width:rect.size.width
			   height:rect.size.height
			selecting:selecting
		  nearestOnly:NO
			 cullFace:cullFace
		selectionMode:selectionMode];
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
	position.Transform(rotation.Conjugate());
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
	int selectedIndex = [currentManipulator selectedIndex];
	
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
	int selectedIndex = [currentManipulator selectedIndex];
	
	Vector3D scale = Vector3D();
	
	if (selectedIndex >= 0)
	{
		ManipulatorWidget *selectedWidget = [currentManipulator widgetAtIndex:selectedIndex];
		enum Axis selectedAxis = [selectedWidget axis];
		if (selectedAxis >= AxisX && selectedAxis <= AxisZ)
		{
			position = [self positionFromRotatedAxis:selectedAxis point:point rotation:[manipulated selectionRotation]];
			scale = position - *lastPosition;
		}
		else if (selectedAxis == Center)
		{
			position = [self positionFromAxis:AxisY point:point];
			scale = position - *lastPosition;
			scale.x = scale.y;
			scale.z = scale.y;
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
	
	int selectedIndex = [currentManipulator selectedIndex];
	
	position = [self positionFromPlaneAxis:(PlaneAxis)(selectedIndex + 3) point:point];
	position -= [manipulated selectionCenter];
	
	switch(selectedIndex)
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
		default: break;
	}
	
	*lastPosition = position;
	return quaternion;
}

- (IBAction)redraw:(id)sender
{
	[self setNeedsDisplay:YES];
}

@end
