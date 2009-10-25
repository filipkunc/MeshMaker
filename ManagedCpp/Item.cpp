/*
 *  Item.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Item.h"

namespace ManagedCpp
{
	Item::Item()
	{
		position = new Vector3D();
		rotation = new Quaternion();
		scale = new Vector3D(1, 1, 1);
		mesh = gcnew Mesh();
		selected = NO;
	}
	
	Item::Item(Vector3D aPosition, Quaternion aRotation, Vector3D aScale)
	{
		Item();
		*position = aPosition;
		*rotation = aRotation;
		*scale = aScale;
	}

	Item::~Item()
	{
		delete position;
		delete rotation;
		delete scale;
		mesh = nullptr;
	}

	Vector3D Item::Position::get()
	{
		return *position;
	}

	void Item::Position::set(Vector3D value)
	{
		*position = value;
	}

	Quaternion Item::Rotation::get()
	{
		return *rotation;
	}

	void Item::Rotation::set(Quaternion value)
	{
		*rotation = value;
	}

	Vector3D Item::Scale::get()
	{
		return *scale;
	}

	void Item::Scale::set(Vector3D value)
	{
		*scale = value;
	}
	
	void Item::Draw(ViewMode mode)
	{
		glPushMatrix();
		glTranslatef(position->x, position->y, position->z);
		Matrix4x4 rotationMatrix;
		rotation->ToMatrix(rotationMatrix);
		glMultMatrixf(rotationMatrix);
		if (mode == ViewModeSolid)
			mesh->Draw(*scale, selected);
		else
			mesh->DrawWire(*scale, selected);
		glPopMatrix();
	}
	
	void Item::MoveBy(Vector3D offset)
	{
		*position += offset;
	}
	
	void Item::RotateBy(Quaternion offset)
	{
		*rotation = offset * *rotation;
	}
	
	void Item::ScaleBy(Vector3D offset)
	{
		*scale += offset;
	}
	
	Item^ Item::Clone()
	{
		Item ^newItem = gcnew Item(this->Position, this->Rotation, this->Scale);

		newItem->mesh->Merge(this->mesh);
		newItem->selected = NO;
		this->selected = YES;
		
		return newItem;
	}
}
