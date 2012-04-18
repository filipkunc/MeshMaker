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
		float hue = (random() % 10) / 10.0f;
		color = [NSColor colorWithCalibratedHue:hue 
									 saturation:0.5f
									 brightness:0.6f 
										  alpha:1.0f];
        
        CGFloat colorComponents[4];
        [color getComponents:colorComponents];
        float floatComponents[4];
        for (int i = 0; i < 4; i++)
            floatComponents[i] = (float)colorComponents[i];
        
        mesh = new Mesh2(floatComponents);
    }
	return self;
}

- (void)dealloc
{
	delete mesh;
}

- (NSColor *)selectionColor
{
    return self.color;
}

- (void)setSelectionColor:(NSColor *)selectionColor
{
    self.color = selectionColor;
}

- (NSColor *)color
{
    return color;
}

- (void)setColor:(NSColor *)value
{
    color = value;
    CGFloat colorComponents[4];
    [color getComponents:colorComponents];
    float floatComponents[4];
    for (int i = 0; i < 4; i++)
        floatComponents[i] = (float)colorComponents[i];
    mesh->setColorComponents(floatComponents);
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

- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection withMode:(enum ViewMode)mode
{
	//mesh->drawAtIndex(index, forSelection, mode);
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
	/*if (selectionMode == MeshSelectionModeEdges)
	{
		for (uint i = 0; i < selected->size(); i++)
		{
			if ((*selected)[i].selected)
			{
				(*selected)[i].visible = NO;
				(*selected)[i].selected = NO;
				[self setEdgeMarked:NO atIndex:i];
			}
		}
	}
	else if (selectionMode == MeshSelectionModeTriangles)
	{
		for (uint i = 0; i < selected->size(); i++)
		{
			if ((*selected)[i].selected)
			{
				(*selected)[i].visible = NO;
				(*selected)[i].selected = NO;
				[self setTriangleMarked:NO atIndex:i];
			}
		}
		[self resetIndexCache];
	}
	else
	{
		for (uint i = 0; i < selected->size(); i++)
		{
			if ((*selected)[i].selected)
			{
				(*selected)[i].visible = NO;
				(*selected)[i].selected = NO;
				markedVertices->at(i) = NO;
			}
		}
	}*/	
}

- (void)unhideAll
{
	/*for (uint i = 0; i < selected->size(); i++)
	{
		(*selected)[i].visible = YES;
	}
	[self resetIndexCache];*/
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
        if (stream.version > 1)
        {
            Vector4D baseColor;           
            [stream readBytes:&baseColor length:sizeof(Vector4D)];
            color = [NSColor colorWithCalibratedRed:baseColor.x green:baseColor.y blue:baseColor.z alpha:baseColor.w];
        }
        
		uint verticesSize;
        uint texCoordsSize;
		uint trianglesSize;
        
        [stream readBytes:&verticesSize length:sizeof(uint)];
        [stream readBytes:&texCoordsSize length:sizeof(uint)];
        [stream readBytes:&trianglesSize length:sizeof(uint)];
        
        vector<Vector3D> vertices;
        vector<Vector3D> texCoords;
        vector<Triangle> triangles;
		
		for (uint i = 0; i < verticesSize; i++)
		{
			Vector3D vertex;
            [stream readBytes:&vertex length:sizeof(Vector3D)];
			vertices.push_back(vertex);
		}
        
        for (uint i = 0; i < texCoordsSize; i++)
        {
            Vector3D texCoord;
            [stream readBytes:&texCoord length:sizeof(Vector3D)];
            texCoords.push_back(texCoord);
        }
		
		for (uint i = 0; i < trianglesSize; i++)
		{
			Triangle triangle;
            [stream readBytes:&triangle length:sizeof(Triangle)];
			triangles.push_back(triangle);
		}
        
        mesh->fromIndexRepresentation(vertices, texCoords, triangles);
        
        CGFloat colorComponents[4];
        [color getComponents:colorComponents];
        float floatComponents[4];
        for (int i = 0; i < 4; i++)
            floatComponents[i] = (float)colorComponents[i];
        mesh->setColorComponents(floatComponents);
	}
	return self;
}

- (void)encodeWithWriteStream:(MemoryWriteStream *)stream
{
    if (stream.version > 1)
    {
        Vector4D baseColor;
        baseColor.x = color.redComponent;
        baseColor.y = color.greenComponent;
        baseColor.z = color.blueComponent;
        baseColor.w = color.alphaComponent;
        [stream writeBytes:&baseColor length:sizeof(Vector4D)];
    }
    
    vector<Vector3D> vertices;
    vector<Vector3D> texCoords;
    vector<Triangle> triangles;
    
    mesh->toIndexRepresentation(vertices, texCoords, triangles);
    
	uint size = vertices.size();
    [stream writeBytes:&size length:sizeof(uint)];
    size = texCoords.size();
    [stream writeBytes:&size length:sizeof(uint)];
	size = triangles.size();
    [stream writeBytes:&size length:sizeof(uint)];
    	
	if (vertices.size() > 0)
        [stream writeBytes:&vertices.at(0) length:vertices.size() * sizeof(Vector3D)];
    
    if (texCoords.size() > 0)
        [stream writeBytes:&texCoords.at(0) length:texCoords.size() * sizeof(Vector3D)];
	
	if (triangles.size() > 0)
        [stream writeBytes:&triangles.at(0) length:triangles.size() * sizeof(Triangle)];
}

- (NSString *)nameAtIndex:(uint)index
{
	switch (mesh->selectionMode())
	{
		case MeshSelectionModeVertices:
			return [NSString stringWithFormat:@"Vertex %i", index];
		case MeshSelectionModeEdges:
			return [NSString stringWithFormat:@"Edge %i", index];
		case MeshSelectionModeTriangles:
			return [NSString stringWithFormat:@"Triangle %i", index];	
		default:
			return nil;
	}
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
