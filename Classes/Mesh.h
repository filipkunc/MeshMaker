//
//  Mesh.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/29/09.
//  For license see LICENSE.TXT
//

#import "OpenGLDrawing.h"
#import "MathDeclaration.h"
#import "OpenGLManipulatingController.h"
#import "MeshHelpers.h"
#import "CppFileStreaming.h"
#import "Shader.h"
#import "ShaderProgram.h"

@interface Mesh : NSObject <OpenGLManipulatingModelMesh, NSCoding, CppFileStreaming>
{
@public
	FPList<VertexNode, Vertex2> *vertices;
	FPList<TriangleNode, Triangle2> *triangles;
	NSColor *color;
	enum MeshSelectionMode selectionMode;
	
    vector<VertexNode *> *cachedVertexSelection;
    vector<TriangleNode *> *cachedTriangleSelection;
    
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
- (void)addQuadWithNode1:(VertexNode *)node1
				   node2:(VertexNode *)node2
				   node3:(VertexNode *)node3 
				   node4:(VertexNode *)node4;
- (void)drawFillAsDarker:(BOOL)darker forSelection:(BOOL)forSelection;
- (void)drawWire;
- (void)drawWithMode:(enum ViewMode)mode scale:(Vector3D)scale selected:(BOOL)isSelected forSelection:(BOOL)forSelection;
- (void)makeMeshWithType:(enum MeshType)type steps:(uint)steps;
- (void)makeCube;
- (void)makeCylinderWithSteps:(uint)steps;
- (void)makeSphereWithSteps:(uint)steps;
- (void)makeEdges;
- (void)removeDegeneratedTriangles;
- (void)removeSelectedVertices;
- (void)fastMergeVertexFirst:(uint)firstIndex second:(uint)secondIndex;
- (void)fastMergeSelectedVertices;
- (void)mergeSelectedVertices;
- (void)transformWithMatrix:(Matrix4x4 *)matrix;
- (void)mergeWithMesh:(Mesh *)mesh;
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
- (void)extrudeSelectedTriangles;
- (void)resetCache;
- (void)fillCache;
- (void)updateColorCacheAsDarker:(BOOL)darker;
- (void)useShader:(BOOL)flipped;

@end
