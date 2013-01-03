//
//  Manipulator.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#import "OpenGLDrawing.h"
#import "Manipulator.h"

Manipulator::Manipulator(ManipulatorType type)
{
    Manipulator();
    
    switch (type)
    {
        case ManipulatorTypeDefault:
            addWidgetWithAxis(WidgetLine, AxisX);
            addWidgetWithAxis(WidgetLine, AxisY);
            addWidgetWithAxis(WidgetLine, AxisZ);
            break;
        case ManipulatorTypeTranslation:
            addWidgetWithAxis(WidgetArrow, AxisX);
            addWidgetWithAxis(WidgetArrow, AxisY);
            addWidgetWithAxis(WidgetArrow, AxisZ);
            
            addWidgetWithAxis(WidgetPlane, AxisX);
            addWidgetWithAxis(WidgetPlane, AxisY);
            addWidgetWithAxis(WidgetPlane, AxisZ);
            break;
        case ManipulatorTypeRotation:
            addWidgetWithAxis(WidgetCircle, AxisX);
            addWidgetWithAxis(WidgetCircle, AxisY);
            addWidgetWithAxis(WidgetCircle, AxisZ);
            break;
        case ManipulatorTypeScale:
            addWidgetWithAxis(WidgetCube, Center);
            addWidgetWithAxis(WidgetCube, AxisX);
            addWidgetWithAxis(WidgetCube, AxisY);
            addWidgetWithAxis(WidgetCube, AxisZ);
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

ManipulatorWidget &Manipulator::widgetAtIndex(uint index)
{
    return widgets.at(index);
}

void Manipulator::draw(Vector3D axisZ, Vector3D center, bool highlightAll)
{
	uint widgetsCount = widgets.size();
	
	if (widgetsCount == 0U)
		return;
	
	Matrix4x4 rotationMatrix = rotation.ToMatrix();
	
	if (widgets.at(0).widget == WidgetCircle)
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
		for (uint i = 0; i < widgetsCount; i++)
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
		for (uint i = 0; i < widgetsCount; i++)
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
		for (uint i = 0; i < widgetsCount; i++)
		{
            widgets[i].draw(size, highlightAll || i == selectedIndex, false, false);
		}
		glPopMatrix();
	}
}

#pragma mark OpenGLSelecting

uint Manipulator::selectableCount()
{
    return widgets.size();
}

void Manipulator::drawForSelectionAtIndex(uint index)
{
	ManipulatorWidget &widget = widgets.at(index);
	
    if (widget.widget == WidgetLine)
		return;
    
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	Matrix4x4 rotationMatrix = rotation.ToMatrix();
	glMultMatrixf(rotationMatrix);
    widget.draw(size, false, false, true);
    glPopMatrix();
}

void Manipulator::selectObjectAtIndex(uint index, OpenGLSelectionMode selectionMode)
{
    selectedIndex = index;
}
