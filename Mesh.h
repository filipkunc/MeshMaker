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
	uint vertexIndices[3];
} Triangle;

typedef struct {
	uint vertexIndices[2];
} Edge;

BOOL IsTriangleDegenerated(Triangle triangle);
BOOL AreEdgesSame(Edge a, Edge b);
BOOL IsIndexInTriangle(Triangle triangle, uint index);
BOOL IsEdgeInTriangle(Triangle triangle, Edge edge);
uint NonEdgeIndexInTriangle(Triangle triangle, Edge edge);
Vector3D NormalFromTriangle(Vector3D triangleVertices[3]);
Triangle MakeTriangle(uint a, uint b, uint c);
Triangle FlipTriangle(Triangle triangle);

enum MeshSelectionMode
{
	MeshSelectionModeVertices = 0,
	MeshSelectionModeTriangles,
	MeshSelectionModeEdges
};

@interface Mesh : NSObject <OpenGLManipulatingModel, NSCoding>
{
@public
	vector<Vector3D> *vertices;
	vector<Triangle> *triangles;
	vector<Edge> *edges;
	vector<BOOL> *selected;
	vector<BOOL> *markedVertices;
	NSColor *color;
	enum MeshSelectionMode selectionMode;
}

@property (readwrite, assign) enum MeshSelectionMode selectionMode; 
@property (readonly) uint vertexCount;
@property (readonly) uint triangleCount;
@property (readonly) uint edgeCount;

- (Vector3D)vertexAtIndex:(uint)anIndex;
- (Triangle)triangleAtIndex:(uint)anIndex;
- (Edge)edgeAtIndex:(uint)anIndex;
- (BOOL)isVertexMarkedAtIndex:(uint)anIndex;
- (void)setVertexMarked:(BOOL)isMarked atIndex:(uint)anIndex;
- (void)setEdgeMarked:(BOOL)isMarked atIndex:(uint)index;
- (void)setTriangleMarked:(BOOL)isMarked atIndex:(uint)index;
- (void)addVertex:(Vector3D)aVertex;
- (void)addTriangle:(Triangle)aTriangle;
- (void)addTriangleWithIndex1:(uint)index1
					   index2:(uint)index2
					   index3:(uint)index3;
- (void)addQuadWithIndex1:(uint)index1
				   index2:(uint)index2
				   index3:(uint)index3 
				   index4:(uint)index4;
- (void)addEdgeWithIndex1:(uint)index1
				   index2:(uint)index2;
- (void)removeVertexAtIndex:(uint)index;
- (void)removeTriangleAtIndex:(uint)index;
- (void)removeEdgeAtIndex:(uint)index;
- (void)drawFillWithScale:(Vector3D)scale;
- (void)drawWireWithScale:(Vector3D)scale;
- (void)drawWithScale:(Vector3D)scale selected:(BOOL)selected;
- (void)makeCube;
- (void)makeCylinderWithSteps:(uint)steps;
- (void)makeSphereWithSteps:(uint)steps;
- (void)makeEdges;
- (void)makeMarkedVertices;
- (void)removeDegeneratedTriangles;
- (void)removeSelectedVertices;
- (void)fastMergeVertexFirst:(uint)firstIndex second:(uint)secondIndex;
- (void)fastMergeSelectedVertices;
- (void)mergeSelectedVertices;
- (void)transformWithMatrix:(Matrix4x4)matrix;
- (void)mergeWithMesh:(Mesh *)mesh;
- (void)getTriangleVertices:(Vector3D *)triangleVertices fromTriangle:(Triangle)triangle;
- (void)splitTriangleAtIndex:(uint)index;
- (void)splitEdgeAtIndex:(uint)index;
- (void)splitSelectedEdges;
- (void)turnEdgeAtIndex:(uint)index;
- (void)turnSelectedEdges;
- (void)mergeVertexPairs;
- (void)mergeSelected;
- (void)splitSelected;
- (void)flipSelectedTriangles;
- (void)flipAllTriangles;
- (void)flipTriangleAtIndex:(uint)index;

@end
