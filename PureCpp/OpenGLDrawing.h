/*
 *  OpenGLDrawing.h
 *  OpenGLWidgets
 *
 *  Created by Filip Kunc on 6/22/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "MathCore/MathDeclaration.h"
#include "Enums.h"

void DrawCone(float width, float height, float offset);
void DrawLine(float size);
void DrawArrow(float size);
void DrawCubeArrow(float size);
void DrawCenterCube(float size);
void DrawPlane(float size, float sizeOffset);
void DrawCircle(float size);
void DrawPlane(Vector3D a, Vector3D b, float size);
void DrawSelectionPlane(PlaneAxis plane);