//
//  OpenGLSceneView.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//


#import <GLUT/glut.h>
#import "OpenGLSceneView.h"

const float perspectiveAngle = 45.0f;
const float minDistance = 0.2f;
const float maxDistance = 1000.0f;

@implementation OpenGLSceneView

@synthesize manipulated, displayed;

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
		
		// The GL context must be active for these functions to have an effect
		NSOpenGLContext *glcontext = [self openGLContext];
		[glcontext makeCurrentContext];
		
		// Configure the view
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHT0);
		
		selectionOffset = new Vector3D();
		isManipulating = NO;
		isSelecting = NO;
		
		camera = new Camera();
		camera->SetRadX(-45.0f);
		camera->SetRadY(45.0f);
		camera->SetZoom(20.0f);
		
		lastPoint = NSMakePoint(0, 0);
		
		defaultManipulator = [[Manipulator alloc] init];
		[defaultManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisX widget:WidgetLine]];
		[defaultManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisY widget:WidgetLine]];
		[defaultManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisZ widget:WidgetLine]];
		
		translationManipulator = [[Manipulator alloc] init];
		[translationManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisX widget:WidgetArrow]];
		[translationManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisY widget:WidgetArrow]];
		[translationManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisZ widget:WidgetArrow]];
		[translationManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisX widget:WidgetPlane]];
		[translationManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisY widget:WidgetPlane]];
		[translationManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisZ widget:WidgetPlane]];
		
		rotationManipulator = [[Manipulator alloc] init];
		[rotationManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisX widget:WidgetCircle]];
		[rotationManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisY widget:WidgetCircle]];
		[rotationManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisZ widget:WidgetCircle]];
		
		scaleManipulator = [[Manipulator alloc] init];
		[scaleManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:Center widget:WidgetCube]];
		[scaleManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisX widget:WidgetCube]];
		[scaleManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisY widget:WidgetCube]];
		[scaleManipulator addWidget:[[ManipulatorWidget alloc] initWithAxis:AxisZ widget:WidgetCube]];
		
		currentManipulator = defaultManipulator;
	}
	return self;
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

- (void)dealloc
{
	delete selectionOffset;
	delete camera;
	[defaultManipulator release];
	[translationManipulator release];
	[rotationManipulator release];
	[super dealloc];
}

- (void)reshape
{
	// Convert up to window space, which is in pixel units.
	NSRect baseRect = [self convertRectToBase:[self bounds]];
	// Now the result is glViewport()-compatible.
	glViewport(0, 0, baseRect.size.width, baseRect.size.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(perspectiveAngle, baseRect.size.width / baseRect.size.height, minDistance, maxDistance);
}

- (void)drawGridWithSize:(int)size step:(int)step
{
	glBegin(GL_LINES);
	for (int x = -size; x <= size; x += step)
    {
        glVertex3i(x, 0, -size);
        glVertex3i(x, 0, size);
	}
    for (int z = -size; z <= size; z += step)
    {
		glVertex3i(-size, 0, z);
        glVertex3i(size, 0, z);
    }
	glEnd();
}

- (void)drawRect:(NSRect)rect
{
	// Clear the background
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Set the viewpoint
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera->GetViewMatrix());
	
	glDisable(GL_LIGHTING);
	
	glColor3f(0.3f, 0.3f, 0.3f);
	[self drawGridWithSize:10 step:2];
	
	glEnable(GL_LIGHTING);
	
	if (displayed != manipulated)
		[displayed draw];
	
	[manipulated draw];
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	
	if ([manipulated selectedCount] > 0)
	{
		[currentManipulator setPosition:[manipulated selectionCenter]];
		[currentManipulator setSize:camera->GetPosition().Distance([currentManipulator position]) * 0.15f];
		[scaleManipulator setRotation:[manipulated selectionRotation]];
		[currentManipulator drawWithAxisZ:camera->GetAxisZ() center:[manipulated selectionCenter]];
	}
		
	if (isSelecting)
	{
		[self beginOrtho];
		glDisable(GL_TEXTURE_2D);
		float color[4] = { 0.2f, 0.5f, 1.0f, 0.0f };
		color[3] = 0.3f;
		glColor4fv(color);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glRecti(lastPoint.x, lastPoint.y, currentPoint.x, currentPoint.y);
		color[3] = 0.9f;
		glColor4fv(color);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glRecti(lastPoint.x, lastPoint.y, currentPoint.x, currentPoint.y);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		[self endOrtho];
	}
	
	glEnable(GL_DEPTH_TEST);
	
	glFinish();
	
	[[self openGLContext] flushBuffer];
}

#pragma mark Mouse Events

- (void)mouseDown:(NSEvent *)e
{
	lastPoint = [self convertPoint:[e locationInWindow] fromView:nil];
	
	if ([manipulated selectedCount] > 0 && [currentManipulator selectedIndex] >= 0)
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
	}
	else
	{
		isSelecting = YES;
	}
}

- (void)mouseMoved:(NSEvent *)e
{
	currentPoint = [self convertPoint:[e locationInWindow] fromView:nil];
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
}

- (void)mouseUp:(NSEvent *)e
{
	currentPoint = [self convertPoint:[e locationInWindow] fromView:nil];	
	isManipulating = NO;
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
			[self selectWithRect:[self currentRect] selecting:manipulated selectionMode:selectionMode];
		else
			[self selectWithPoint:currentPoint selecting:manipulated selectionMode:selectionMode];
		[self setNeedsDisplay:YES];
	}
}

- (void)mouseDragged:(NSEvent *)e
{
	currentPoint = [self convertPoint:[e locationInWindow] fromView:nil];	
	
	if (isManipulating)
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
	lastPoint = [self convertPoint:[e locationInWindow] fromView:nil];
}

- (void)otherMouseDragged:(NSEvent *)e
{	
	NSPoint point = [self convertPoint:[e locationInWindow] fromView:nil];
	float diffX = point.x - lastPoint.x;
	float diffY = point.y - lastPoint.y;
	
	if ([e modifierFlags] & NSAlternateKeyMask)
	{
		const float sensitivity = 0.005f;
		camera->RotateLeftRight(diffX * sensitivity);
		camera->RotateUpDown(-diffY * sensitivity);
	}
	else
	{
		NSRect bounds = [self bounds];
		float w = bounds.size.width;
		float h = bounds.size.height;
		float sensitivity = (w + h) / 2.0f;
		sensitivity = 1.0f / sensitivity;
		camera->LeftRight(-diffX * camera->GetZoom() * sensitivity);
		camera->UpDown(diffY * camera->GetZoom() * sensitivity);
	}
	
	lastPoint = point;
	
	[self setNeedsDisplay:YES];
}

- (void)scrollWheel:(NSEvent *)e
{
	float zoom = [e deltaY];
	float sensitivity = camera->GetZoom() * 0.02f;
	
	camera->Zoom(zoom * sensitivity);
	
	[self setNeedsDisplay:YES];
}

#pragma mark Keyboard Events

- (void)keyDown:(NSEvent *)e
{
	//NSLog([e description]);
	switch ([e keyCode]) {
		case 51: // delete
		{
			//NSLog(@"removeSelected not implemented");
			[manipulated removeSelected];
			[self setNeedsDisplay:YES];
		}break;
		case 8: // cmd + C
		{
			if ([e modifierFlags] & NSCommandKeyMask)
			{
				NSLog(@"cloneSelected not implemented");
				//[manipulated cloneSelected];
				[self setNeedsDisplay:YES];
			}
		}break;
		default:
			break;
	}
}

- (void)keyUp:(NSEvent *)e
{

}

-(NSRect)currentRect
{
	float minX = MIN(lastPoint.x, currentPoint.x);
	float maxX = MAX(lastPoint.x, currentPoint.x);
	float minY = MIN(lastPoint.y, currentPoint.y);
	float maxY = MAX(lastPoint.y, currentPoint.y);
	
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
	gluOrtho2D(0, rect.size.width, 0, rect.size.height);
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
	  selectionMode:(enum OpenGLSelectionMode)selectionMode
{
	if (selecting == nil || [selecting selectableCount] <= 0)
		return;
	
	int objectsFound = 0;
    int viewport[4];
	unsigned int selectBuffer[16535];
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glSelectBuffer(16535, selectBuffer);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glRenderMode(GL_SELECT);
    glLoadIdentity();
    gluPickMatrix(x, y, width, height, viewport);
	NSRect baseRect = [self convertRectToBase:[self bounds]];
	gluPerspective(perspectiveAngle, baseRect.size.width / baseRect.size.height, minDistance, maxDistance);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera->GetViewMatrix());
    glInitNames();
    glPushName(0);
    glPushMatrix();
    
	for (int i = 0; i < [selecting selectableCount]; i++)
	{
		glLoadName((unsigned int)(i + 1));
		glPushMatrix();
		[selecting drawForSelectionAtIndex:i];
		glPopMatrix();
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
}

- (void)selectWithPoint:(NSPoint)point 
			  selecting:(id<OpenGLSelecting>)selecting 
		  selectionMode:(enum OpenGLSelectionMode)selectionMode
{
	[self selectWithX:point.x
					y:point.y
				width:8.0
			   height:8.0
			selecting:selecting
		  nearestOnly:YES
		selectionMode:selectionMode];
}

- (void)selectWithRect:(NSRect)rect
			 selecting:(id<OpenGLSelecting>)selecting
		 selectionMode:(enum OpenGLSelectionMode)selectionMode
{
	[self selectWithX:rect.origin.x + rect.size.width / 2
					y:rect.origin.y + rect.size.height / 2 
				width:rect.size.width
			   height:rect.size.height
			selecting:selecting
		  nearestOnly:NO
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
