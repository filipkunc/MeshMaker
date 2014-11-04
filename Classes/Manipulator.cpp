//
//  Manipulator.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#include "OpenGLDrawing.h"
#include "Manipulator.h"

Manipulator::Manipulator(ManipulatorType type)
{
    Manipulator();
    
    switch (type)
    {
        case ManipulatorType::Default:
            addWidgetWithAxis(Widget::Line, Axis::X);
            addWidgetWithAxis(Widget::Line, Axis::Y);
            addWidgetWithAxis(Widget::Line, Axis::Z);
            break;
        case ManipulatorType::Translation:
            addWidgetWithAxis(Widget::Arrow, Axis::X);
            addWidgetWithAxis(Widget::Arrow, Axis::Y);
            addWidgetWithAxis(Widget::Arrow, Axis::Z);
            
            addWidgetWithAxis(Widget::Plane, Axis::X);
            addWidgetWithAxis(Widget::Plane, Axis::Y);
            addWidgetWithAxis(Widget::Plane, Axis::Z);
            break;
        case ManipulatorType::Rotation:
            addWidgetWithAxis(Widget::Circle, Axis::X);
            addWidgetWithAxis(Widget::Circle, Axis::Y);
            addWidgetWithAxis(Widget::Circle, Axis::Z);
            break;
        case ManipulatorType::Scale:
            addWidgetWithAxis(Widget::Cube, Axis::Center);
            addWidgetWithAxis(Widget::Cube, Axis::X);
            addWidgetWithAxis(Widget::Cube, Axis::Y);
            addWidgetWithAxis(Widget::Cube, Axis::Z);
            break;
        default:
            break;
    }
}

void Manipulator::addWidget(const ManipulatorWidget &widget)
{
    widgets.push_back(widget);
}

void Manipulator::addWidgetWithAxis(Widget widget, Axis axis)
{
    widgets.push_back(ManipulatorWidget(widget, axis));
}

ManipulatorWidget &Manipulator::widgetAtIndex(unsigned int index)
{
    return widgets.at(index);
}

void Manipulator::draw(Vector3D axisZ, Vector3D center, bool highlightAll)
{
    unsigned int widgetsCount = static_cast<unsigned int>(widgets.size());
	
	if (widgetsCount == 0U)
		return;
	
	Matrix4x4 rotationMatrix = rotation.ToMatrix();
	
	if (widgets.at(0).widget == Widget::Circle)
	{
		double eq[4] = { 0, 0, 0, 0 };
		for (int j = 0; j < 3; j++)
			eq[j] = -axisZ[j];
		eq[3] = -(eq[0] * center.x + eq[1] * center.y + eq[2] * center.z);
		glClipPlane(GL_CLIP_PLANE0, eq);
		glEnable(GL_CLIP_PLANE0);
		
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		glMultMatrixf(rotationMatrix);
		for (unsigned int i = 0; i < widgetsCount; i++)
		{
            widgets[i].draw(size, highlightAll || i == selectedIndex, true, false);
		}
		glPopMatrix();
		
		glDisable(GL_CLIP_PLANE0);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glColor4f(0.8f, 0.8f, 0.8f, 0.25f);
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		glMultMatrixf(rotationMatrix);
		DrawSphere(size * 0.7f, 20, 20);
		glPopMatrix();
		
		glDisable(GL_BLEND);
		
		center -= axisZ * size * 0.02f;
		
		for (int j = 0; j < 3; j++)
			eq[j] = axisZ[j];
		eq[3] = -(eq[0] * center.x + eq[1] * center.y + eq[2] * center.z);
		glClipPlane(GL_CLIP_PLANE0, eq);
		glEnable(GL_CLIP_PLANE0);
		
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		glMultMatrixf(rotationMatrix);
		for (unsigned int i = 0; i < widgetsCount; i++)
		{
			widgets[i].draw(size, highlightAll || i == selectedIndex, false, false);
		}
		glPopMatrix();
		
		glDisable(GL_CLIP_PLANE0);
	}
	else
	{
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		glMultMatrixf(rotationMatrix);
		for (unsigned int i = 0; i < widgetsCount; i++)
		{
            widgets[i].draw(size, highlightAll || i == selectedIndex, false, false);
		}
		glPopMatrix();
	}
}

#pragma mark OpenGLSelecting

unsigned int Manipulator::selectableCount()
{
    return static_cast<unsigned int>(widgets.size());
}

void Manipulator::drawForSelectionAtIndex(unsigned int index)
{
	ManipulatorWidget &widget = widgets.at(index);
	
    if (widget.widget == Widget::Line)
		return;
    
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	Matrix4x4 rotationMatrix = rotation.ToMatrix();
	glMultMatrixf(rotationMatrix);
    widget.draw(size, false, false, true);
    glPopMatrix();
}

void Manipulator::selectObjectAtIndex(unsigned int index, OpenGLSelectionMode selectionMode)
{
    selectedIndex = index;
}
