/*
 *  Enums.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/24/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

enum class MeshType
{
    Plane = 0,
	Cube,
	Cylinder,
	Sphere,
    Icosahedron
};
	
enum class MeshSelectionMode
{
	Vertices = 0,
	Triangles,
	Edges,
};

enum class ManipulatorType
{
	Default = 0,
	Translation = 1,
	Rotation = 2,
	Scale = 3
};

enum class ViewMode
{
	SolidFlat = 0,
    SolidSmooth = 1,
    MixedWireSolid = 2,
	Wireframe = 3,
    Unwrap = 4
};

enum class EditMode
{
	Items = 0,
	Vertices = 1,
	Triangles = 2,
	Edges = 3
};

enum class CameraMode
{
	Perspective = 0,
	Left = 1,
	Right = 2,
	Top = 3,
	Bottom = 4,
	Front = 5,
	Back = 6
};

enum class Axis
{
	Center = -1,
	X = 0,
	Y = 1,
	Z = 2,
};

enum class PlaneAxis
{
	X = 3,
	Y = 4,
	Z = 5,
};

enum class Widget
{
	Line,
	Arrow,
	Plane,
	Circle,
	Cube,
};

enum class OpenGLSelectionMode
{
	Add,
	Subtract,
	Invert,
    Expand,
    InvertExpand,
};
    
enum class ModelVersion : unsigned int
{
    First = 1U,
    Colors = 2U,
    TriQuads = 3U,
    CrossPlatform = 4U,
    TextureNames = 5U,

    Latest = TextureNames
};

enum class VertexWindowMode
{
    Add = 0,
    TriangleConnect = 1,
    QuadConnect = 2,
};

