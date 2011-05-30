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
#import "CppFileStreaming.h"
#import "Shader.h"
#import "ShaderProgram.h"
#import "FPList.h"
#import <vector>
using namespace std;

class Vertex2
{
public:
    Vector3D position;
    
    bool selected;
    
    Vertex2() : selected(false) { }
    Vertex2(const Vector3D &v) : position(v), selected(false) { }
};

class Triangle2
{
public:
    FPNode<Vertex2> *vertices[3];
    
    bool selected;
    
    Triangle2() : selected(false) { }
};

typedef FPNode<Vertex2> *VertexNode;
typedef FPNode<Triangle2> *TriangleNode;

@interface Mesh : NSObject <OpenGLManipulatingModelMesh, NSCoding, CppFileStreaming>
{
@public
	FPList<Vertex2> *vertices;
	FPList<Triangle2> *triangles;
	NSColor *color;
	enum MeshSelectionMode selectionMode;
	
    vector<VertexNode> *cachedVertexSelection;
    vector<TriangleNode> *cachedTriangleSelection;
    
    vector<uint> *cachedIndices;
	Vector3D *cachedVertices;
	Vector3D *cachedNormals;
	Vector3D *cachedColors;
}

@property (readwrite, assign) enum MeshSelectionMode selectionMode; 
@property (readonly) uint vertexCount;
@property (readonly) uint triangleCount;
@property (readonly) uint edgeCount;

+ (NSString *)descriptionOfMeshType:(enum MeshType)type;
+ (void)setNormalShader:(ShaderProgram *)shaderProgram;
+ (void)setFlippedShader:(ShaderProgram *)shaderProgram;

- (void)addVertex:(Vector3D)aVertex;
- (void)addTriangle:(Triangle2)aTriangle;
- (void)addTriangleWithNode1:(VertexNode)node1
					   node2:(VertexNode)node2
					   node3:(VertexNode)node3;
- (void)addQuadWithNode1:(VertexNode)node1
				   node2:(VertexNode)node2
				   node3:(VertexNode)node3 
				   node4:(VertexNode)node4;
/*- (void)removeVertexAtIndex:(uint)index;
- (void)removeTriangleAtIndex:(uint)index;
- (void)removeEdgeAtIndex:(uint)index;*/
- (void)drawFillAsDarker:(BOOL)darker forSelection:(BOOL)forSelection;
- (void)drawWire;
- (void)drawWithMode:(enum ViewMode)mode scale:(Vector3D)scale selected:(BOOL)isSelected forSelection:(BOOL)forSelection;
- (void)makeMeshWithType:(enum MeshType)type steps:(uint)steps;
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
- (void)getTriangleVertices:(Vector3D *)triangleVertices fromTriangle:(Triangle2)triangle;
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
- (void)resetIndexCache;
- (void)resetCache;
- (void)fillIndexCache;
- (void)fillCache;
- (void)updateColorCacheAsDarker:(BOOL)darker;
- (void)useShader:(BOOL)flipped;

@end
