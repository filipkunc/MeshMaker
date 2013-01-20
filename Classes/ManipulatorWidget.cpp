//
//  ManipulatorWidget.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/22/09.
//  For license see LICENSE.TXT
//

#include "ManipulatorWidget.h"

void ManipulatorWidget::drawCore(float size)
{
	switch (widget)
	{
		case Widget::Line:
			DrawLine(size);
			break;
		case Widget::Arrow:
			DrawArrow(size);
			break;
		case Widget::Circle:
			DrawCircle(size * 0.7f);
			break;
		case Widget::Plane:
			DrawPlane(size * 0.3f, size * 0.2f);
			break;
		case Widget::Cube:
			DrawCubeArrow(size);
			break;
		default:
			break;
	}
}

void ManipulatorWidget::draw(float size, bool isSelected, bool isGray, bool forSelection)
{
	float alpha, angle;
	if (widget == Widget::Plane)
	{
		alpha = 0.5f;
		angle = -90.0f;
	}
	else
	{
		alpha = 0.8f;
		angle = 90.0f;
	}
    if (!forSelection)
    {
        if (isSelected)
            glColor4f(1, 1, 0, alpha);
        else if (isGray)
            glColor4f(0.3f, 0.3f, 0.3f, 0.8f);
    }
	switch (axis)
	{
		case Axis::X:
			if (!isSelected && !isGray && !forSelection)
				glColor4f(1, 0, 0, alpha);
			glPushMatrix();
			glRotatef(-angle, 0, 0, 1);
            drawCore(size);
			glPopMatrix();
			break;
		case Axis::Y:
			if (!isSelected && !isGray && !forSelection)
				glColor4f(0, 1, 0, alpha);
            drawCore(size);
			break;
		case Axis::Z:
			if (!isSelected && !isGray && !forSelection)
				glColor4f(0, 0, 1, alpha);
			glPushMatrix();
			glRotatef(angle, 1, 0, 0);
            drawCore(size);
			glPopMatrix();
			break;
		case Axis::Center:
			if (!isSelected && !isGray && !forSelection)
				glColor4f(1, 1, 1, 0.4f);
			DrawCenterCube(size);
			break;
		default:
			break;
	}
}
