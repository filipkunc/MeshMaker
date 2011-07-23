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
    return 0;
	//return edges->size();
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
		case MeshTypeCube:
			[self makeCube];
			break;
		case MeshTypeCylinder:
			[self makeCylinderWithSteps:steps];
			break;
		case MeshTypeSphere:
			[self makeSphereWithSteps:steps];
			break;
		default:
			break;
	}
}

- (void)makeCube
{
	mesh->makeCube();
}

- (void)makeCylinderWithSteps:(uint)steps
{
    mesh->makeCylinder(steps);
}

- (void)makeSphereWithSteps:(uint)steps
{
	mesh->makeSphere(steps);
}

- (void)mergeVertexPairs
{
    /*
	[self resetCache];
	
	for (int i = 0; i < (int)selected->size(); i++)
	{
		if (selected->at(i).selected)
		{
			Vector3D firstVertex = [self vertexAtIndex:i];
			float smallestDistance = 10.0f; // maximum distance between vertices in pair
			int secondIndex = -1;
			for (int j = i + 1; j < (int)selected->size(); j++)
			{
				if (selected->at(j).selected)
				{
					Vector3D secondVertex = [self vertexAtIndex:j];
					float currentDistance = firstVertex.Distance(secondVertex);
					if (currentDistance < smallestDistance)
					{
						secondIndex = j;
						smallestDistance = currentDistance;
					}
				}
			}
			
			if (secondIndex > -1)
			{
				[self fastMergeVertexFirst:i second:secondIndex];
				i--;
			}
		}
	}
		
	[self removeDegeneratedTriangles];
	[self removeNonUsedVertices];
	
	NSAssert(vertices->size() == selected->size(), @"vertices->size() == selected->size()");*/
}

- (void)transformWithMatrix:(Matrix4x4 *)matrix
{
    mesh->transformAll(*matrix);
}

- (void)mergeWithMesh:(Mesh *)aMesh
{
    mesh->merge(aMesh->mesh);
}

- (void)splitEdgeAtIndex:(uint)index
{
	/*
	Edge edge = [self edgeAtIndex:index];
	[self removeEdgeAtIndex:index];
	Vector3D firstVertex = [self vertexAtIndex:edge.vertexIndices[0]];
	Vector3D secondVertex = [self vertexAtIndex:edge.vertexIndices[1]];
	Vector3D centerVertex = firstVertex + secondVertex;
	centerVertex /= 2.0f;
	vertices->push_back(centerVertex);
	uint centerIndex = vertices->size() - 1;
	
	Vector3D triangleVertices[3];
		
	for (int i = 0; i < (int)triangles->size(); i++)
	{
		Triangle triangle = [self triangleAtIndex:i];
		if (IsEdgeInTriangle(triangle, edge))
		{
			uint oppositeIndex = NonEdgeIndexInTriangle(triangle, edge);
			
			[self removeTriangleAtIndex:i];
			i--;
			
			[self addEdgeWithIndex1:centerIndex index2:oppositeIndex];
			
			[self getTriangleVertices:triangleVertices fromTriangle:triangle];
			Vector3D splittedTriangleNormal = NormalFromTriangleVertices(triangleVertices);
			
			Triangle firstTriangle = MakeTriangle(edge.vertexIndices[0], oppositeIndex, centerIndex);
			[self getTriangleVertices:triangleVertices fromTriangle:firstTriangle];
			Vector3D firstTriangleNormal = NormalFromTriangleVertices(triangleVertices);
			
			Triangle secondTriangle = MakeTriangle(edge.vertexIndices[1], oppositeIndex, centerIndex);
			[self getTriangleVertices:triangleVertices fromTriangle:secondTriangle];
			Vector3D secondTriangleNormal = NormalFromTriangleVertices(triangleVertices);
			
			if (firstTriangleNormal.Dot(splittedTriangleNormal) < 0.0f)
				firstTriangle = FlipTriangle(firstTriangle);
			
			if (secondTriangleNormal.Dot(splittedTriangleNormal) < 0.0f)
				secondTriangle = FlipTriangle(secondTriangle);
			
			[self addTriangle:firstTriangle];
			[self addTriangle:secondTriangle];
		}
	}
	
	[self addEdgeWithIndex1:centerIndex index2:edge.vertexIndices[1]];
	[self addEdgeWithIndex1:centerIndex index2:edge.vertexIndices[0]];*/
}

- (void)splitSelectedEdges
{
	/*
	[self resetCache];
	
	for (int i = 0; i < (int)selected->size(); i++)
	{
		if (selected->at(i).selected)
		{
			[self splitEdgeAtIndex:i];
			i--;
		}
	}*/
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

- (void)didSelect
{
    mesh->resetTriangleColorCache();
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

- (void)duplicateSelected
{
	if (mesh->selectionMode() == MeshSelectionModeTriangles)
        mesh->extrudeSelectedTriangles();
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
				 
#pragma mark CppFileStreaming implementation

- (id)initWithFileStream:(ifstream *)fin
{
	self = [self init];
	if (self)
	{
		uint verticesSize;
		uint trianglesSize;
		fin->read((char *)&verticesSize, sizeof(uint));
		fin->read((char *)&trianglesSize, sizeof(uint));
        
        vector<Vector3D> vertices;
        vector<Triangle> triangles;
		
		for (uint i = 0; i < verticesSize; i++)
		{
			Vector3D vertex;
			fin->read((char *)&vertex, sizeof(Vector3D));
			vertices.push_back(vertex);
		}
		
		for (uint i = 0; i < trianglesSize; i++)
		{
			Triangle triangle;
			fin->read((char *)&triangle, sizeof(Triangle));
			triangles.push_back(triangle);
		}
        
        mesh->fromIndexRepresentation(vertices, triangles);
	}
	return self;
}

- (void)encodeWithFileStream:(ofstream *)fout
{
    vector<Vector3D> vertices;
    vector<Triangle> triangles;
    
    mesh->toIndexRepresentation(vertices, triangles);
    
	uint size = vertices.size();
	fout->write((char *)&size, sizeof(uint));
	size = triangles.size();
	fout->write((char *)&size, sizeof(uint));
    	
	if (vertices.size() > 0)
		fout->write((char *)&vertices.at(0), vertices.size() * sizeof(Vector3D));
	
	if (triangles.size() > 0)
		fout->write((char *)&triangles.at(0), triangles.size() * sizeof(Triangle));
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

@end
