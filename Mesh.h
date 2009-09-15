//
//  Mesh.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/29/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "OpenGLManipulatingController.h"
#import <vector>
using namespace std;

typedef struct {
	NSUInteger vertexIndices[3];
} Triangle;

typedef struct {
	NSUInteger vertexIndices[2];
} Edge;

BOOL IsTriangleDegenerated(Triangle triangle);
BOOL AreEdgesSame(Edge a, Edge b);
BOOL IsIndexInTriangle(Triangle triangle, NSUInteger index);
BOOL IsEdgeInTriangle(Triangle triangle, Edge edge);
NSUInteger NonEdgeIndexInTriangle(Triangle triangle, Edge edge);
Vector3D NormalFromTriangle(Vector3D triangleVertices[3]);
Triangle MakeTriangle(NSUInteger a, NSUInteger b, NSUInteger c);
Triangle MakeTriangleOpposite(Triangle triangle);

enum MeshSelectionMode
{
	MeshSelectionModeVertices = 0,
	MeshSelectionModeTriangles,
	MeshSelectionModeEdges
};

@interface Mesh : NSObject <OpenGLManipulatingModel>
{
	vector<Vector3D> *vertices;
	vector<Triangle> *triangles;
	vector<Edge> *edges;
	vector<BOOL> *selected;
	enum MeshSelectionMode selectionMode;
}

@property (readwrite, assign) enum MeshSelectionMode selectionMode; 
@property (readonly) NSUInteger vertexCount;
@property (readonly) NSUInteger triangleCount;
@property (readonly) NSUInteger edgeCount;

- (Vector3D)vertexAtIndex:(NSUInteger)anIndex;
- (Triangle)triangleAtIndex:(NSUInteger)anIndex;
- (Edge)edgeAtIndex:(NSUInteger)anIndex;

- (void)addVertex:(Vector3D)aVertex;

- (void)addTriangle:(Triangle)aTriangle;

- (void)addTriangleWithIndex1:(NSUInteger)index1
					   index2:(NSUInteger)index2
					   index3:(NSUInteger)index3;

- (void)addQuadWithIndex1:(NSUInteger)index1
				   index2:(NSUInteger)index2
				   index3:(NSUInteger)index3 
				   index4:(NSUInteger)index4;

- (void)addEdgeWithIndex1:(NSUInteger)index1
				   index2:(NSUInteger)index2;

- (void)removeVertexAtIndex:(NSUInteger)index;
- (void)removeTriangleAtIndex:(NSUInteger)index;
- (void)removeEdgeAtIndex:(NSUInteger)index;
- (void)drawFillWithScale:(Vector3D)scale;
- (void)drawWireWithScale:(Vector3D)scale;
- (void)drawWithScale:(Vector3D)scale selected:(BOOL)selected;
- (void)makeCube;
- (void)makeCylinderWithSteps:(NSUInteger)steps;
- (void)makeSphere;
- (void)makeEdges;
- (void)removeDegeneratedTriangles;
- (void)removeSelectedVertices;
- (void)fastMergeVertexFirst:(NSUInteger)firstIndex second:(NSUInteger)secondIndex;
- (void)fastMergeSelectedVertices;
- (void)mergeSelectedVertices;
- (void)transformWithMatrix:(Matrix4x4)matrix;
- (void)mergeWithMesh:(Mesh *)mesh;
- (void)getTriangleVertices:(Vector3D *)triangleVertices fromTriangle:(Triangle)triangle;
- (void)splitTriangleAtIndex:(NSUInteger)index;
- (void)splitEdgeAtIndex:(NSUInteger)index;
- (void)splitSelectedEdges;
- (void)turnEdgeAtIndex:(NSUInteger)index;
- (void)turnSelectedEdges;
- (void)mergeVertexPairs;
- (void)mergeSelected;
- (void)splitSelected;

@end
