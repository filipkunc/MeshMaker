//
//  ManipulatorWidget.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/22/09.
//  For license see LICENSE.TXT
//

#import "ManipulatorWidget.h"

@implementation ManipulatorWidget

@synthesize axis, widget;

- (id)initWithAxis:(enum Axis)anAxis widget:(enum Widget)aWidget
{
	self = [super init];
	if (self)
	{
		axis = anAxis;
		widget = aWidget;
	}
	return self;
}

- (void)drawCoreWithSize:(float)size
{
	switch (widget)
	{
		case WidgetLine:
			DrawLine(size);
			break;
		case WidgetArrow:
			DrawArrow(size);
			break;
		case WidgetCircle:
			DrawCircle(size * 0.7f);
			break;
		case WidgetPlane:
			DrawPlane(size * 0.3f, size * 0.2f);
			break;
		case WidgetCube:
			DrawCubeArrow(size);
			break;
		default:
			break;
	}
}

- (void)drawWithSize:(float)size isSelected:(BOOL)isSelected isGray:(BOOL)isGray
{
	float alpha, angle;
	if (widget == WidgetPlane)
	{
		alpha = 0.5f;
		angle = -90.0f;
	}
	else
	{
		alpha = 0.8f;
		angle = 90.0f;
	}
	if (isSelected)
		glColor4f(1, 1, 0, alpha);
	else if (isGray)
		glColor4f(0.3f, 0.3f, 0.3f, 0.8f);
	switch (axis)
	{
		case AxisX:
			if (!isSelected && !isGray)
				glColor4f(1, 0, 0, alpha);
			glPushMatrix();
			glRotatef(-angle, 0, 0, 1);
			[self drawCoreWithSize:size];
			glPopMatrix();
			break;
		case AxisY:
			if (!isSelected && !isGray)
				glColor4f(0, 1, 0, alpha);
			[self drawCoreWithSize:size];
			break;
		case AxisZ:
			if (!isSelected && !isGray)
				glColor4f(0, 0, 1, alpha);
			glPushMatrix();
			glRotatef(angle, 1, 0, 0);
			[self drawCoreWithSize:size];
			glPopMatrix();
			break;
		case Center:
			if (!isSelected && !isGray)
				glColor4f(1, 1, 1, 0.4f);
			DrawCenterCube(size);
			break;
		default:
			break;
	}
}

@end
