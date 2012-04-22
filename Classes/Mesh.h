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
#import "MemoryStreaming.h"
#import "Shader.h"
#import "ShaderProgram.h"
#import "Mesh2.h"

@interface Mesh : NSObject <MemoryStreaming>
{
@public
    NSColor *color;    
    Mesh2 *mesh;    
}

@property (readwrite, assign) enum MeshSelectionMode selectionMode; 
@property (readonly) uint vertexCount;
@property (readonly) uint triangleCount;
@property (readonly) uint edgeCount;
@property (readwrite, copy) NSImage *image;
@property (readwrite, copy) NSColor *color;

+ (NSString *)descriptionOfMeshType:(enum MeshType)type;
- (void)drawWithMode:(enum ViewMode)mode scale:(Vector3D)scale selected:(BOOL)isSelected forSelection:(BOOL)forSelection;
- (void)makeMeshWithType:(enum MeshType)type steps:(uint)steps;
- (void)transformWithMatrix:(Matrix4x4 *)matrix;
- (void)mergeWithMesh:(Mesh *)aMesh;
- (void)mergeSelected;
- (void)splitSelected;
- (void)flipSelected;
- (void)flipAllTriangles;
- (void)loopSubdivision;
- (void)detachSelected;
- (void)extrudeSelected;
- (void)cleanTexture;
- (void)resetTexCooords;

- (uint)count;
- (void)willSelectThrough:(BOOL)selectThrough;
- (BOOL)needsCullFace;
- (void)didSelect;
- (void)getSelectionCenter:(Vector3D *)center 
				  rotation:(Quaternion *)rotation
					 scale:(Vector3D *)scale;
- (void)transformSelectedByMatrix:(Matrix4x4 *)matrix;
- (BOOL)isSelectedAtIndex:(uint)index;
- (void)setSelected:(BOOL)isSelected atIndex:(uint)index;
- (void)drawAllForSelection:(BOOL)forSelection withMode:(enum ViewMode)mode;
- (void)flipSelected;
- (void)flipAllTriangles;
- (void)loopSubdivision;
- (void)detachSelected;
- (void)extrudeSelected;
- (void)duplicateSelected;
- (void)removeSelected;
- (void)hideSelected;
- (void)unhideAll;
- (void)cleanTexture;

@end
