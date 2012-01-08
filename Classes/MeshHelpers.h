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

void AddTriangle(vector<Triangle> &triangles, int index1, int index2, int index3);
void AddQuad(vector<Triangle> &triangles, int index1, int index2, int index3, int index4);
