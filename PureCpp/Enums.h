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

// CocoaBool on Mac
typedef signed char CocoaBool;

#ifndef YES
#define YES  ((CocoaBool)1)
#endif

#ifndef NO
#define NO   ((CocoaBool)0)
#endif

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
	ViewModeSolid = 0,
	ViewModeWireframe = 1
};

enum Widget
{
	WidgetLine,
	WidgetArrow,
	WidgetPlane,
	WidgetCircle,
	WidgetCube,
};

enum EditMode
{
	EditModeItems = 0,
	EditModeVertices = 1,
	EditModeTriangles = 2,
	EditModeEdges = 3
};

enum ItemWithSteps
{
	ItemWithStepsCylinder = 0,
	ItemWithStepsSphere = 1
};

enum OpenGLSelectionMode
{
	OpenGLSelectionModeAdd,
	OpenGLSelectionModeSubtract,
	OpenGLSelectionModeInvert,
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
