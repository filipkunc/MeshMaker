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

BOOL IsTriangleDegenerated(Triangle triangle);

enum MeshSelectionMode
{
	MeshSelectionModeVertices = 0,
	MeshSelectionModeTriangles
};

@interface Mesh : NSObject <OpenGLManipulatingModel>
{
	vector<Vector3D> *vertices;
	vector<Triangle> *triangles;
	vector<BOOL> *selectedIndices;
	enum MeshSelectionMode selectionMode;
}

@property (readwrite, assign) enum MeshSelectionMode selectionMode; 
@property (readonly) NSUInteger vertexCount;
@property (readonly) NSUInteger triangleCount;

- (Vector3D)vertexAtIndex:(NSUInteger)anIndex;
- (Triangle)triangleAtIndex:(NSUInteger)anIndex;
- (void)addVertex:(Vector3D)aVertex;
- (void)addTriangleWithIndex1:(NSUInteger)index1
					   index2:(NSUInteger)index2
					   index3:(NSUInteger)index3;
- (void)addQuadWithIndex1:(NSUInteger)index1
				   index2:(NSUInteger)index2
				   index3:(NSUInteger)index3 
				   index4:(NSUInteger)index4;
- (void)removeVertexAtIndex:(NSUInteger)index;
- (void)removeTriangleAtIndex:(NSUInteger)index;
- (void)drawFillWithScale:(Vector3D)scale;
- (void)drawWireWithScale:(Vector3D)scale;
- (void)drawWithScale:(Vector3D)scale selected:(BOOL)selected;
- (void)makeCube;
- (void)makeCylinderWithSteps:(int)steps;
- (void)removeDegeneratedTriangles;
- (void)removeSelectedVertices;
- (void)fastCollapseSelectedVertices;
- (void)collapseSelectedVertices;
- (void)transformWithMatrix:(Matrix4x4)matrix;
- (void)mergeWithMesh:(Mesh *)mesh;

@end
