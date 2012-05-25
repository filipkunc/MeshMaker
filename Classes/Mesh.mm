//
//  Mesh.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/29/09.
//  For license see LICENSE.TXT
//

#import "Mesh.h"

@implementation Mesh

+ (NSString *)descriptionOfMeshType:(enum MeshType)type
{
	switch (type)
	{
		case MeshTypeCube:
			return @"Cube";
		case MeshTypeCylinder:
			return @"Cylinder";
		case MeshTypeSphere:
			return @"Sphere";
        case MeshTypePlane:
            return @"Plane";
        case MeshTypeIcosahedron:
            return @"Icosahedron";
		default:
			return nil;
	}
}

- (uint)vertexCount
{
	return mesh->vertexCount();
}

- (uint)triangleCount
{
	return mesh->triangleCount();
}

- (uint)edgeCount
{
    return mesh->vertexEdgeCount();
}

- (id)init
{
	self = [super init];
	if (self)
	{
		mesh = new Mesh2();
    }
	return self;
}

- (void)dealloc
{
	delete mesh;
}

- (NSColor *)color
{
    return mesh->color();
}

- (void)setColor:(NSColor *)value
{
    mesh->setColor(value);
}

- (enum MeshSelectionMode)selectionMode
{
    return mesh->selectionMode();
}

- (void)setSelectionMode:(enum MeshSelectionMode)value
{
    mesh->setSelectionMode(value);
}

- (void)drawWithMode:(enum ViewMode)mode scale:(Vector3D)scale selected:(BOOL)isSelected forSelection:(BOOL)forSelection
{
    mesh->draw(mode, scale, isSelected, forSelection);
}

- (void)makeMeshWithType:(enum MeshType)type steps:(uint)steps
{
	switch (type) 
	{
        case MeshTypePlane:
            mesh->makePlane();
            break;
		case MeshTypeCube:
            mesh->makeCube();
			break;
		case MeshTypeCylinder:
            mesh->makeCylinder(steps);
			break;
		case MeshTypeSphere:
            mesh->makeSphere(steps);
			break;
        case MeshTypeIcosahedron:
            mesh->makeIcosahedron();
            break;
		default:
			break;
	}
}

- (void)transformWithMatrix:(Matrix4x4 *)matrix
{
    mesh->transformAll(*matrix);
}

- (void)mergeWithMesh:(Mesh *)aMesh
{
    mesh->merge(aMesh->mesh);
}

- (void)mergeSelected
{
	mesh->mergeSelected();
}

- (void)splitSelected
{
	mesh->splitSelected();
}

#pragma mark OpenGLManipulatingModel implementation

- (uint)count
{
    return mesh->selectedCount();
}

- (void)willSelectThrough:(BOOL)selectThrough
{
    Mesh2::setSelectThrough(selectThrough);
}

- (BOOL)needsCullFace
{
    if (mesh->selectionMode() == MeshSelectionModeTriangles && Mesh2::selectThrough())
        return YES;
    return NO;
}

- (void)didSelect
{
    mesh->resetTriangleCache();
    mesh->computeSoftSelection();
}

- (void)getSelectionCenter:(Vector3D *)center 
				  rotation:(Quaternion *)rotation
					 scale:(Vector3D *)scale
{
	mesh->getSelectionCenterRotationScale(*center, *rotation, *scale);
}

- (void)transformSelectedByMatrix:(Matrix4x4 *)matrix
{
    mesh->transformSelected(*matrix);
}

- (BOOL)isSelectedAtIndex:(uint)index
{
    return mesh->isSelectedAtIndex(index);
}

- (void)setSelected:(BOOL)isSelected atIndex:(uint)index 
{
    mesh->setSelectedAtIndex(isSelected, index);
}

- (void)drawAllForSelection:(BOOL)forSelection withMode:(enum ViewMode)mode
{
    mesh->drawAll(mode, forSelection);
}

- (void)flipSelected
{
    mesh->flipSelected();
}

- (void)flipAllTriangles
{
    mesh->flipAllTriangles();
}

- (void)loopSubdivision
{
    mesh->loopSubdivision();
}

- (void)detachSelected
{
    mesh->detachSelected();
}

- (void)extrudeSelected
{
    if (mesh->selectionMode() == MeshSelectionModeTriangles)
        mesh->extrudeSelectedTriangles();
}

- (void)duplicateSelected
{
	if (mesh->selectionMode() == MeshSelectionModeTriangles)
        mesh->duplicateSelectedTriangles();
}

- (void)removeSelected
{
    mesh->removeSelected();
}

- (void)hideSelected
{
    mesh->hideSelected();
}

- (void)unhideAll
{
	mesh->unhideAll();
}

- (void)cleanTexture
{
    mesh->cleanTexture();
}

#pragma mark CppFileStreaming implementation

- (id)initWithReadStream:(MemoryReadStream *)stream
{
	self = [self init];
	if (self)
	{
        mesh = new Mesh2(stream);
	}
	return self;
}

- (void)encodeWithWriteStream:(MemoryWriteStream *)stream
{
    mesh->encode(stream);
}

- (void)resetTexCooords
{
    mesh->resetTriangleCache();
    mesh->makeTexCoords();
    mesh->makeEdges();
}

- (NSImage *)image
{
    return mesh->texture().canvas;
}

- (void)setImage:(NSImage *)image
{
    [mesh->texture() setCanvas:image];
}

@end
