//
//  Mesh.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/29/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "OpenGLManipulatingController.h"
#import "MeshHelpers.h"
#import <vector>
using namespace std;

@interface Mesh : NSObject <OpenGLManipulatingModelMesh, NSCoding>
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
- (void)drawWireWithScale:(Vector3D)scale selected:(BOOL)isSelected;
- (void)drawWithScale:(Vector3D)scale selected:(BOOL)isSelected;
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
- (void)flipSelected;
- (void)flipSelectedTriangles;
- (void)flipAllTriangles;
- (void)flipTriangleAtIndex:(uint)index;
- (void)extrudeSelectedTriangles;

@end
