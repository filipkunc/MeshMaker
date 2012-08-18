/*
 *  MeshHelpers.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include "MeshForwardDeclaration.h"
#include "Triangle.h"
#include "Vertex.h"
#include "VertexEdge.h"

void AddTriangle(vector<TriQuad> &triangles, uint index1, uint index2, uint index3);
void AddTriangle(vector<TriQuad> &triangles, uint vertexIndices[3], uint texCoordIndices[3]);
void AddQuad(vector<TriQuad> &triangles, uint index1, uint index2, uint index3, uint index4);
void FlipTriangle(TriQuad &triangle);