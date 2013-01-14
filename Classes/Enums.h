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

#if defined(__APPLE__)
#define EnumClass enum class
#elif defined(WIN32)
#include <windows.h>
using namespace System;
#define EnumClass public enum struct
#endif

EnumClass MeshType
{
    Plane = 0,
	Cube,
	Cylinder,
	Sphere,
    Icosahedron
};
	
EnumClass MeshSelectionMode
{
	Vertices = 0,
	Triangles,
	Edges,
};

EnumClass ManipulatorType
{
	Default = 0,
	Translation = 1,
	Rotation = 2,
	Scale = 3
};

EnumClass ViewMode
{
	SolidFlat = 0,
    SolidSmooth = 1,
    MixedWireSolid = 2,
	Wireframe = 3,
    Unwrap = 4
};

EnumClass EditMode
{
	Items = 0,
	Vertices = 1,
	Triangles = 2,
	Edges = 3
};

EnumClass CameraMode
{
	Perspective = 0,
	Left = 1,
	Right = 2,
	Top = 3,
	Bottom = 4,
	Front = 5,
	Back = 6
};

EnumClass Axis
{
	Center = -1,
	X = 0,
	Y = 1,
	Z = 2,
};

EnumClass PlaneAxis
{
	X = 3,
	Y = 4,
	Z = 5,
};

EnumClass Widget
{
	Line,
	Arrow,
	Plane,
	Circle,
	Cube,
};

EnumClass OpenGLSelectionMode
{
	Add,
	Subtract,
	Invert,
    Expand,
    InvertExpand,
};
    
EnumClass ModelVersion : uint
{
    First = 1U,
    Colors = 2U,
    TriQuads = 3U,
    CrossPlatform = 4U,

    Latest = CrossPlatform
};

