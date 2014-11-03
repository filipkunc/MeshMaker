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

void AddTriangle(vector<TriQuad> &triangles, unsigned int index1, unsigned int index2, unsigned int index3);
void AddTriangle(vector<TriQuad> &triangles, unsigned int vertexIndices[3], unsigned int texCoordIndices[3]);
void AddQuad(vector<TriQuad> &triangles, unsigned int index1, unsigned int index2, unsigned int index3, unsigned int index4);
void FlipTriangle(TriQuad &triangle);