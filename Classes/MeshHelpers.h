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
#include "Vertex.h"
#include "TexCoord.h"
#include "Triangle.h"
#include "VertexEdge.h"
#include "TexCoordEdge.h"

void AddTriangle(vector<Triangle> &triangles, uint index1, uint index2, uint index3);
void AddTriangle(vector<Triangle> &triangles, uint vertexIndices[3], uint texCoordIndices[3]);
void AddQuad(vector<Triangle> &triangles, uint index1, uint index2, uint index3, uint index4);
