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
#import "Mesh2.h"

@interface Mesh : NSObject <OpenGLManipulatingModelMesh, CppFileStreaming>
{
@public
    NSColor *color;    
    Mesh2 *mesh;    
}

@property (readwrite, assign) enum MeshSelectionMode selectionMode; 
@property (readonly) uint vertexCount;
@property (readonly) uint triangleCount;
@property (readonly) uint edgeCount;

+ (NSString *)descriptionOfMeshType:(enum MeshType)type;
- (void)drawWithMode:(enum ViewMode)mode scale:(Vector3D)scale selected:(BOOL)isSelected forSelection:(BOOL)forSelection;
- (void)makeMeshWithType:(enum MeshType)type steps:(uint)steps;
- (void)transformWithMatrix:(Matrix4x4 *)matrix;
- (void)mergeWithMesh:(Mesh *)aMesh;
- (void)mergeVertexPairs;
- (void)mergeSelected;
- (void)splitSelected;
- (void)flipSelected;
- (void)flipAllTriangles;
- (void)loopSubdivision;
- (void)detachSelected;
- (void)cleanTexture;

@end
