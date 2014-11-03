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

#include <Cocoa/Cocoa.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "MathDeclaration.h"
#include "Enums.h"
#include "Exceptions.h"
#include <vector>
using namespace std;

static inline const char * GetGLErrorString(GLenum error)
{
	const char *str;
	switch( error )
	{
		case GL_NO_ERROR:
			str = "GL_NO_ERROR";
			break;
		case GL_INVALID_ENUM:
			str = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			str = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			str = "GL_INVALID_OPERATION";
			break;
#if defined __gl_h_ || defined __gl3_h_
		case GL_OUT_OF_MEMORY:
			str = "GL_OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			str = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
#endif
#if defined __gl_h_
		case GL_STACK_OVERFLOW:
			str = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			str = "GL_STACK_UNDERFLOW";
			break;
		case GL_TABLE_TOO_LARGE:
			str = "GL_TABLE_TOO_LARGE";
			break;
#endif
		default:
			str = "(ERROR: Unknown Error Enum)";
			break;
	}
	return str;
}

#define GetGLError()									\
{														\
	GLenum err = glGetError();							\
	while (err != GL_NO_ERROR) {						\
		printf("GLError %s set in File:%s Line:%d\n",	\
				GetGLErrorString(err),					\
				__FILE__,								\
				__LINE__);								\
		err = glGetError();								\
	}													\
}

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
void ColorIndex(unsigned int colorIndex);
void ColorIndices(vector<unsigned int> &colorIndices);
void ReadSelectedIndices(int x, int y, int width, int height, unsigned int *selectedIndices);
