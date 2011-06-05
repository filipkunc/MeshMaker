/*
 *  MeshHelpers.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#import "Enums.h"
#import "MathDeclaration.h"
#import "FPList.h"
#import "FPNode.h"
#import <vector>
using namespace std;

struct Triangle
{
	uint vertexIndices[3];
};

struct Edge
{
	uint vertexIndices[2];
};

struct SelectionInfo
{
    bool selected;
    bool visible;
};

class Vertex2;
class Triangle2;

typedef FPNode<Vertex2> VertexNode;
typedef FPNode<Triangle2> TriangleNode;

class Vertex2
{
public:
    Vector3D position;
    FPList<FPNode<Triangle2>, Triangle2> triangles;
    
    bool selected;
    
    Vertex2() : selected(false) { }
    Vertex2(const Vector3D &v) : position(v), selected(false) { }    
};

class Triangle2
{
public:
    VertexNode *vertices[3];
    
    bool selected;
    
    Triangle2() : selected(false) { }

    void AddToVertices();
    void RemoveFromVertices();
    bool IsDegenerated();
    bool IsVertexInTriangle(VertexNode *vertex);
    Triangle2 Flip();
};

//class Vertex2Node : public FPNode<Vertex2>
//{
//    void RemoveFromTriangles()
//    {
//        FPList<Triangle2> &triangles = data.triangles;
//        for (TriangleNode node = triangles.Begin(), end = triangles.End(); node != end; node = node->Next())
//        {
//            Triangle2 triangle = node->data;
//            for (uint i = 0; i < 3; i++)
//            {
//                if (triangle.vertices[i] == this)
//                {
//                    triangle.vertices[i] = NULL;
//                    break;
//                }
//            }
//        }
//    }
//};

Vector3D NormalFromTriangleVertices(Vector3D triangleVertices[3]);

void AddTriangle(vector<Triangle> &triangles, int index1, int index2, int index3);
void AddQuad(vector<Triangle> &triangles, int index1, int index2, int index3, int index4);
