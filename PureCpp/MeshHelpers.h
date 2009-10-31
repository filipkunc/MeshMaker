/*
 *  MeshHelpers.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include "Enums.h"
#include "MathCore/MathDeclaration.h"

typedef struct {
	uint vertexIndices[3];
} Triangle;

typedef struct {
	uint vertexIndices[2];
} Edge;

CocoaBool IsTriangleDegenerated(Triangle triangle);
CocoaBool AreEdgesSame(Edge a, Edge b);
CocoaBool IsIndexInTriangle(Triangle triangle, uint index);
CocoaBool IsEdgeInTriangle(Triangle triangle, Edge edge);
uint NonEdgeIndexInTriangle(Triangle triangle, Edge edge);
Vector3D NormalFromTriangleVertices(Vector3D triangleVertices[3]);
Triangle MakeTriangle(uint a, uint b, uint c);
Triangle FlipTriangle(Triangle triangle);
