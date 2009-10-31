//
//  ManipulatorWidget.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/31/09.
//  For license see LICENSE.TXT
//

#include "ManipulatorWidget.h"

namespace ManagedCpp
{
	Axis ManipulatorWidget::ManipAxis::get()
	{
		return axis;
	}
	
	Widget ManipulatorWidget::ManipWidget::get()
	{
		return widget;
	}

	ManipulatorWidget::ManipulatorWidget(Axis anAxis, Widget aWidget)
	{
		axis = anAxis;
		widget = aWidget;
	}

	void ManipulatorWidget::DrawCore(float size)
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

	void ManipulatorWidget::Draw(float size, CocoaBool isSelected, CocoaBool isGray)
	{
		float alpha, angle;
		if (widget == WidgetPlane)
		{
			alpha = 0.5f;
			angle = -90.0f;
		}
		else
		{
			alpha = 1.0f;
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
				this->DrawCore(size);
				glPopMatrix();
				break;
			case AxisY:
				if (!isSelected && !isGray)
					glColor4f(0, 1, 0, alpha);
				this->DrawCore(size);
				break;
			case AxisZ:
				if (!isSelected && !isGray)
					glColor4f(0, 0, 1, alpha);
				glPushMatrix();
				glRotatef(angle, 1, 0, 0);
				this->DrawCore(size);
				glPopMatrix();
				break;
			case Center:
				if (!isSelected && !isGray)
					glColor4f(0.5f, 0.5f, 0.5f, 0.4f);
				DrawCenterCube(size);
				break;
			default:
				break;
		}
	}
}
