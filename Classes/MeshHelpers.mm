/*
 *  MeshHelpers.mm
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#import "MeshHelpers.h"

void AddTriangle(vector<TriQuad> &triangles, uint index1, uint index2, uint index3)
{
    TriQuad triangle;
    triangle.isQuad = false;
    
    triangle.vertexIndices[0] = index1;
    triangle.vertexIndices[1] = index2;
    triangle.vertexIndices[2] = index3;
    
    triangles.push_back(triangle);
}

void AddTriangle(vector<TriQuad> &triangles, uint vertexIndices[3], uint texCoordIndices[3])
{
    TriQuad triangle;
    triangle.isQuad = false;
    
    for (int i = 0; i < 3; i++)
    {
        triangle.vertexIndices[i] = vertexIndices[i];
        triangle.texCoordIndices[i] = texCoordIndices[i];
    }
    
    triangles.push_back(triangle);
}

void AddQuad(vector<TriQuad> &triangles, uint index1, uint index2, uint index3, uint index4)
{
    TriQuad triangle;
    triangle.isQuad = true;
    
	triangle.vertexIndices[0] = index1;
	triangle.vertexIndices[1] = index2;
	triangle.vertexIndices[2] = index3;
    triangle.vertexIndices[3] = index4;
	
    triangles.push_back(triangle);
}

void FlipTriangle(TriQuad &triangle)
{
    swap(triangle.vertexIndices[0], triangle.vertexIndices[2]);
    swap(triangle.texCoordIndices[0], triangle.texCoordIndices[2]);
}
