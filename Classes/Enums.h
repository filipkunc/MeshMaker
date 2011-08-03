/*
 *  Enums.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/24/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef BOOL
typedef signed char BOOL;
#endif

#ifndef YES
#define YES             (BOOL)1
#endif

#ifndef NO
#define NO              (BOOL)0
#endif

enum MeshType
{
	MeshTypeCube = 0,
	MeshTypeCylinder,
	MeshTypeSphere
};
	
enum MeshSelectionMode
{
	MeshSelectionModeVertices = 0,
	MeshSelectionModeTriangles,
	MeshSelectionModeEdges
};

enum ManipulatorType
{
	ManipulatorTypeDefault = 0,
	ManipulatorTypeTranslation = 1, 
	ManipulatorTypeRotation = 2,
	ManipulatorTypeScale = 3
};

enum ViewMode
{
	ViewModeSolidFlat = 0,
    ViewModeSolidSmooth = 1,
	ViewModeWireframe = 2,
};

enum EditMode
{
	EditModeItems = 0,
	EditModeVertices = 1,
	EditModeTriangles = 2,
	EditModeEdges = 3
};

enum CameraMode
{
	CameraModePerspective = 0,
	CameraModeLeft = 1,
	CameraModeRight = 2,
	CameraModeTop = 3,
	CameraModeBottom = 4,
	CameraModeFront = 5,
	CameraModeBack = 6
};

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
