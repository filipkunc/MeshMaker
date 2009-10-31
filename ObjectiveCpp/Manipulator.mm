//
//  Manipulators.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#import <GLUT/glut.h>
#import "Manipulator.h"

@implementation Manipulator

@synthesize size, selectedIndex;

- (id)init
{
	self = [super init];
	if (self)
	{
		widgets = [[NSMutableArray alloc] init];
		position = new Vector3D();
		rotation = new Quaternion();
		size = 10.0f;
		selectedIndex = -1;
	}
	return self;
}

- (void)dealloc
{
	[widgets release];
	delete position;
	delete rotation;
	[super dealloc];
}

- (Vector3D)position
{
	return *position;
}

- (void)setPosition:(Vector3D)aPosition
{
	*position = aPosition;
}

- (Quaternion)rotation
{
	return *rotation;
}

- (void)setRotation:(Quaternion)aRotation
{
	*rotation = aRotation;
}

- (void)addWidget:(ManipulatorWidget *)widget
{
	[widgets addObject:widget];
}

- (void)drawWithAxisZ:(Vector3D)axisZ center:(Vector3D)center
{
	if ([widgets count] <= 0)
		return;
	
	Matrix4x4 rotationMatrix;
	rotation->ToMatrix(rotationMatrix);
	
	ManipulatorWidget *widget = (ManipulatorWidget *)[widgets objectAtIndex:0];
	if ([widget widget] == WidgetCircle)
	{
		double eq[4] = { 0, 0, 0, 0 };
		for (int j = 0; j < 3; j++)
			eq[j] = -axisZ[j];
		eq[3] = -(eq[0] * center.x + eq[1] * center.y + eq[2] * center.z);
		glClipPlane(GL_CLIP_PLANE0, eq);
		glEnable(GL_CLIP_PLANE0);
		
		glPushMatrix();
		glTranslatef(position->x, position->y, position->z);
		glMultMatrixf(rotationMatrix);
		for (int i = 0; i < [widgets count]; i++)
		{
			widget = (ManipulatorWidget *)[widgets objectAtIndex:i];
			[widget drawWithSize:size isSelected:i == selectedIndex isGray:YES];
		}
		glPopMatrix();
		
		glDisable(GL_CLIP_PLANE0);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glColor4f(0.8f, 0.8f, 0.8f, 0.25f);
		glPushMatrix();
		glTranslatef(position->x, position->y, position->z);
		glMultMatrixf(rotationMatrix);
		glutSolidSphere(size * 0.7f, 20, 20);
		glPopMatrix();
		
		glDisable(GL_BLEND);
		
		center -= axisZ * size * 0.02f;
		
		for (int j = 0; j < 3; j++)
			eq[j] = axisZ[j];
		eq[3] = -(eq[0] * center.x + eq[1] * center.y + eq[2] * center.z);
		glClipPlane(GL_CLIP_PLANE0, eq);
		glEnable(GL_CLIP_PLANE0);
		
		glPushMatrix();
		glTranslatef(position->x, position->y, position->z);
		glMultMatrixf(rotationMatrix);
		for (int i = 0; i < [widgets count]; i++)
		{
			widget = (ManipulatorWidget *)[widgets objectAtIndex:i];
			[widget drawWithSize:size isSelected:i == selectedIndex isGray:NO];
		}
		glPopMatrix();
		
		glDisable(GL_CLIP_PLANE0);
	}
	else
	{
		glPushMatrix();
		glTranslatef(position->x, position->y, position->z);
		glMultMatrixf(rotationMatrix);
		for (int i = 0; i < [widgets count]; i++)
		{
			widget = (ManipulatorWidget *)[widgets objectAtIndex:i];
			[widget drawWithSize:size isSelected:i == selectedIndex isGray:NO];
		}
		glPopMatrix();
	}
}

- (ManipulatorWidget *)widgetAtIndex:(int)index
{
	return [widgets objectAtIndex:index];
}

#pragma mark OpenGLSelecting

- (uint)selectableCount
{
	return [widgets count];
}

- (void)drawForSelectionAtIndex:(uint)index
{
	ManipulatorWidget *widget = (ManipulatorWidget *)[widgets objectAtIndex:index];
	if ([widget widget] == WidgetLine)
		return;
	glPushMatrix();
	glTranslatef(position->x, position->y, position->z);
	Matrix4x4 rotationMatrix;
	rotation->ToMatrix(rotationMatrix);
	glMultMatrixf(rotationMatrix);
	[widget drawWithSize:size isSelected:NO isGray:NO];
	glPopMatrix();
}

- (void)selectObjectAtIndex:(uint)index 
				   withMode:(enum OpenGLSelectionMode)selectionMode
{
	selectedIndex = index;
}

@end
