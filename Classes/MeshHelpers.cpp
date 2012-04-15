/*
 *  MeshHelpers.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#import "MeshHelpers.h"

void AddTriangle(vector<Triangle> &triangles, uint index1, uint index2, uint index3)
{
    Triangle triangle;
    
    triangle.vertexIndices[0] = index1;
    triangle.vertexIndices[1] = index2;
    triangle.vertexIndices[2] = index3;
    
    triangles.push_back(triangle);
}

void AddTriangle(vector<Triangle> &triangles, uint vertexIndices[3], uint texCoordIndices[3])
{
    Triangle triangle;
    
    for (int i = 0; i < 3; i++)
    {
        triangle.vertexIndices[i] = vertexIndices[i];
        triangle.texCoordIndices[i] = texCoordIndices[i];
    }
    
    triangles.push_back(triangle);
}

void AddQuad(vector<Triangle> &triangles, uint index1, uint index2, uint index3, uint index4)
{
    Triangle triangle1, triangle2;
	triangle1.vertexIndices[0] = index1;
	triangle1.vertexIndices[1] = index2;
	triangle1.vertexIndices[2] = index3;
	
	triangle2.vertexIndices[0] = index1;
	triangle2.vertexIndices[1] = index3;
	triangle2.vertexIndices[2] = index4;
    
    triangles.push_back(triangle1);
    triangles.push_back(triangle2);
}

void FlipTriangle(Triangle &triangle)
{
    swap(triangle.vertexIndices[0], triangle.vertexIndices[2]);
    swap(triangle.texCoordIndices[0], triangle.texCoordIndices[2]);
}
