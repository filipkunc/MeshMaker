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
	NSUInteger vertexIndices[2];
} Edge;

typedef struct {
	NSUInteger edgeIndices[3];
} Triangle;

Edge MakeEdge(NSUInteger index1, NSUInteger index2);
Triangle MakeTriangle(NSUInteger index1, NSUInteger index2, NSUInteger index3);

enum MeshSelectionMode
{
	MeshSelectionModeVertices = 0,
	MeshSelectionModeEdges,
	MeshSelectionModeTriangles
};

@interface Mesh : NSObject <OpenGLManipulatingModel>
{
	vector<Vector3D> *vertices;
	vector<Edge> *edges;
	vector<Triangle> *triangles;
	vector<BOOL> *selectedIndices;
	enum MeshSelectionMode selectionMode;
}

@property (readwrite, assign) enum MeshSelectionMode selectionMode; 

- (Vector3D)vertexAtIndex:(NSUInteger)anIndex;
- (Edge)edgeAtIndex:(NSUInteger)anIndex;
- (Triangle)triangleAtIndex:(NSUInteger)anIndex;
- (void)addVertex:(Vector3D)aVertex;
- (void)addEdge:(Edge)anEdge;
- (void)addTriangle:(Triangle)aTriangle;
- (void)drawFill;
- (void)drawWire;
- (void)draw:(BOOL)selected;
- (void)makeCube;
- (void)collapseSelected;
- (void)transformWithMatrix:(Matrix4x4)matrix;
- (void)mergeWithMesh:(Mesh *)mesh;

@end
