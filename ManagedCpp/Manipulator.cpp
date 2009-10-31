//
//  Manipulator.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#include <stdlib.h>
#include <GLUT/glut.h>
#include "Manipulator.h"


namespace ManagedCpp
{
	float Manipulator::Size::get()
	{
		return size;
	}

	void Manipulator::Size::set(float value)
	{
		size = value;
	}

	int Manipulator::SelectedIndex::get()
	{
		return selectedIndex;
	}

	void Manipulator::SelectedIndex::set(int value)
	{
		selectedIndex = value;
	}

	Vector3D Manipulator::Position::get()
	{
		return *position;
	}

	void Manipulator::Position::set(Vector3D value)
	{
		*position = value;
	}

	Quaternion Manipulator::Rotation::get()
	{
		return *rotation;
	}

	void Manipulator::Rotation::set(Quaternion value)
	{
		*rotation = value;
	}

	Manipulator::Manipulator()
	{
		widgets = gcnew List<ManipulatorWidget ^>();
		position = new Vector3D();
		rotation = new Quaternion();
		size = 10.0f;
		selectedIndex = -1;
	}

	Manipulator::~Manipulator()
	{
		widgets = nullptr;
		delete position;
		delete rotation;
	}

	void Manipulator::AddWidget(ManipulatorWidget ^widget)
	{
		widgets->Add(widget);
	}

	void Manipulator::Draw(Vector3D axisZ, Vector3D center)
	{
		if (widgets->Count <= 0)
			return;
		
		Matrix4x4 rotationMatrix;
		rotation->ToMatrix(rotationMatrix);
		
		ManipulatorWidget ^widget = widgets[0];
		if (widget->ManipWidget == WidgetCircle)
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
			for (int i = 0; i < widgets->Count; i++)
			{
				widget = widgets[i];
				widget->Draw(size, i == selectedIndex, YES);
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
			for (int i = 0; i < widgets->Count; i++)
			{
				widget = widgets[i];
				widget->Draw(size, i == selectedIndex, NO);
			}
			glPopMatrix();
			
			glDisable(GL_CLIP_PLANE0);
		}
		else
		{
			glPushMatrix();
			glTranslatef(position->x, position->y, position->z);
			glMultMatrixf(rotationMatrix);
			for (int i = 0; i < widgets->Count; i++)
			{
				widget = widgets[i];
				widget->Draw(size, i == selectedIndex, NO);
			}
			glPopMatrix();
		}
	}

	ManipulatorWidget ^Manipulator::GetWidget(int index)
	{
		return widgets[index];
	}

	#pragma region OpenGLSelecting

	uint Manipulator::SelectableCount::get()
	{
		return widgets->Count;
	}

	void Manipulator::DrawForSelection(uint index)
	{
		ManipulatorWidget ^widget = widgets[index];
		if (widget->ManipWidget == WidgetLine)
			return;
		glPushMatrix();
		glTranslatef(position->x, position->y, position->z);
		Matrix4x4 rotationMatrix;
		rotation->ToMatrix(rotationMatrix);
		glMultMatrixf(rotationMatrix);
		widget->Draw(size, NO, NO);
		glPopMatrix();
	}

	void Manipulator::SelectObject(uint index, OpenGLSelectionMode selectionMode)
	{
		selectedIndex = index;
	}

	void Manipulator::WillSelect()
	{

	}
	
	void Manipulator::DidSelect()
	{

	}

	#pragma endregion
}