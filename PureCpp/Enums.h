/*
 *  Enums.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/24/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

// This isn't Pure C++ as I wanted, but I needed enum export for C#.
// If you find better name than MyEnum that doesn't clash with M$ or Fruit,
// write me e-mail.
#ifdef WIN32
#include <windows.h>
using namespace System;
#define MyEnum public enum struct
#else
#define MyEnum enum
#endif

#ifndef uint
typedef unsigned int uint;
#endif

// CocoaBool on Mac
typedef signed char CocoaBool;

#ifndef YES
#define YES  ((CocoaBool)1)
#endif

#ifndef NO
#define NO   ((CocoaBool)0)
#endif

#ifdef WIN32
namespace ManagedCpp {
#endif

MyEnum MeshType
{
	MeshTypeCube = 0,
	MeshTypeCylinder,
	MeshTypeSphere
};
	
MyEnum MeshSelectionMode
{
	MeshSelectionModeVertices = 0,
	MeshSelectionModeTriangles,
	MeshSelectionModeEdges
};

MyEnum ManipulatorType
{
	ManipulatorTypeDefault = 0,
	ManipulatorTypeTranslation = 1, 
	ManipulatorTypeRotation = 2,
	ManipulatorTypeScale = 3
};

MyEnum ViewMode
{
	ViewModeSolid = 0,
	ViewModeWireframe = 1
};

MyEnum EditMode
{
	EditModeItems = 0,
	EditModeVertices = 1,
	EditModeTriangles = 2,
	EditModeEdges = 3
};

MyEnum CameraMode
{
	CameraModePerspective = 0,
	CameraModeLeft = 1,
	CameraModeRight = 2,
	CameraModeTop = 3,
	CameraModeBottom = 4,
	CameraModeFront = 5,
	CameraModeBack = 6
};

#ifdef WIN32
} // namespace ManagedCpp
#endif 

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

enum Widget
{
	WidgetLine,
	WidgetArrow,
	WidgetPlane,
	WidgetCircle,
	WidgetCube,
};

enum OpenGLSelectionMode
{
	OpenGLSelectionModeAdd,
	OpenGLSelectionModeSubtract,
	OpenGLSelectionModeInvert,
};
