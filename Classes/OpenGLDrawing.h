/*
 *  OpenGLDrawing.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 6/22/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

#if defined(__APPLE__)
#include <Cocoa/Cocoa.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif defined (WIN32)
#define NOMINMAX
#include <windows.h>
#if defined(SHADERS)
#include "../MeshMakerCppCLI/glew/include/GL/glew.h"
#pragma comment(lib, "../MeshMakerCppCLI/glew/lib/glew32.lib")
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#elif defined(__linux__)
#define SHADERS 1
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "MathDeclaration.h"
#include "Enums.h"

void DrawCube(float size);
void DrawSphere(float radius, int lats, int longs);
void DrawCone(float width, float height, float offset);
void DrawLine(float size);
void DrawArrow(float size);
void DrawCubeArrow(float size);
void DrawCenterCube(float size);
void DrawPlane(float size, float sizeOffset);
void DrawCircle(float size);
void DrawPlane(Vector3D a, Vector3D b, float size);
void DrawSelectionPlane(PlaneAxis plane);
void ColorIndex(uint colorIndex);
