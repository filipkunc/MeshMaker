/*
 *  OpenGLDrawing.h
 *  OpenGLWidgets
 *
 *  Created by Filip Kunc on 6/22/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "MathDeclaration.h"

enum Axis
{
	Center = -1,
	AxisX = 0,
	AxisY = 1,
	AxisZ = 2,
};

enum PlaneAxis
{
	PlaneAxisX = 3,
	PlaneAxisY = 4,
	PlaneAxisZ = 5,
};

void DrawCone(float width, float height, float offset);
void DrawLine(float size);
void DrawArrow(float size);
void DrawCubeArrow(float size);
void DrawCenterCube(float size);
void DrawPlane(float size, float sizeOffset);
void DrawCircle(float size);
void DrawPlane(Vector3D a, Vector3D b, float size);
void DrawSelectionPlane(PlaneAxis plane);