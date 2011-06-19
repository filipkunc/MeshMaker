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
			NSLog(@"Unknown mesh type: %i", type);
			break;
	}
}

- (void)makeCube
{
	NSLog(@"makeCube");
	
	mesh->makeCube();
}

- (void)makeCylinderWithSteps:(uint)steps
{
	NSLog(@"makeCylinderWithSteps:%i", steps);
	/*
	vertices->removeAll();
	triangles->removeAll();
	
	VertexNode *node0 = vertices->add(Vector3D(0, -1, 0)); // 0
 	VertexNode *node1 = vertices->add(Vector3D(0,  1, 0)); // 1
	
	VertexNode *node2 = vertices->add(Vector3D(cosf(0.0f), -1, sinf(0.0f))); // 2
	VertexNode *node3 = vertices->add(Vector3D(cosf(0.0f),  1, sinf(0.0f))); // 3
		
	uint max = steps;
	float step = (FLOAT_PI * 2.0f) / max;
	float angle = step;
	for (uint i = 1; i < max; i++)
	{
		VertexNode *last2 = vertices->add(Vector3D(cosf(angle), -1, sinf(angle))); // 4
		VertexNode *last1 = vertices->add(Vector3D(cosf(angle),  1, sinf(angle))); // 5
        
        VertexNode *last3 = last2->previous();
        VertexNode *last4 = last3->previous();
		
		Triangle2 triangle1(last3, last2, last1);
        Triangle2 triangle2(last2, last3, last4);
		
		triangles->add(triangle1);
		triangles->add(triangle2);
		
		Triangle2 triangle3(last4, node0, last2);
        Triangle2 triangle4(last3, last1, node1);
		
		triangles->add(triangle3);
		triangles->add(triangle4);
		
		angle += step;
	}
    
    VertexNode *last1 = vertices->last();
    VertexNode *last2 = last1->previous();
	
	Triangle2 triangle1(node2, node3, last1);
    Triangle2 triangle2(last1, last2, node2);
	
	triangles->add(triangle1);
	triangles->add(triangle2);
	
	Triangle2 triangle3(node0, node2, last2);
    Triangle2 triangle4(node3, node1, last1);
	
	triangles->add(triangle3);
	triangles->add(triangle4);
	
	[self setSelectionMode:[self selectionMode]];*/
}

- (void)makeSphereWithSteps:(uint)steps
{
    NSLog(@"makeSphereWithSteps:%i", steps);
	
	/*vertices->removeAll();
	triangles->removeAll();
		
	uint max = steps;
    
    vector<VertexNode *> tempVertices;
    vector<Triangle> tempTriangles;
     
    tempVertices.push_back(vertices->add(Vector3D(0, 1, 0)));
    tempVertices.push_back(vertices->add(Vector3D(0, -1, 0)));
     
    float step = FLOAT_PI / max;
     
    for (uint i = 0; i < max; i++)
    {
        float beta = i * step * 2.0f;
     
        for (uint j = 1; j < max; j++)
        {
            float alpha = 0.5f * FLOAT_PI + j * step;
            float y0 = sinf(alpha);
            float w0 = cosf(alpha);                
     
            float x0 = sinf(beta) * w0;
            float z0 = cosf(beta) * w0;
     
            tempVertices.push_back(vertices->add(Vector3D(x0, y0, z0)));
     
            if (i > 0 && j < max - 1)
            {
                int index = (i - 1) * (max - 1);
                
                AddQuad(tempTriangles, 
                        1 + max + j + index, 
                        2 + j + index,
                        1 + j + index,
                        max + j + index);                
            }
        }
     
        int index = i * (max - 1);
        if (i < max - 1)
        {
            AddTriangle(tempTriangles,
                        0,
                        2 + index + max - 1,
                        2 + index);
     
            AddTriangle(tempTriangles,
                        1,
                        index + max,
                        index + 2 * max - 1);
        }
        else 
        {
            
            AddTriangle(tempTriangles,
                        0,
                        2,
                        2 + index);
            
            AddTriangle(tempTriangles,
                        1,
                        index + max,
                        max);
        }
    }
     
    for (uint j = 1; j < max - 1; j++)
    {
        int index = (max - 1) * (max - 1);
        AddQuad(tempTriangles,
                1 + j + index,
                1 + j,
                2 + j,
                2 + j + index);
    }
    
    VertexNode *triangleVertices[3];
    
    for (uint i = 0; i < tempTriangles.size(); i++)
    {
        Triangle indexTriangle = tempTriangles[i];
        for (uint j = 0; j < 3; j++)
        {
            VertexNode *node = tempVertices.at(indexTriangle.vertexIndices[j]);
            triangleVertices[j] = node;
        }
        Triangle2 nodeTriangle(triangleVertices[0], triangleVertices[1], triangleVertices[2]);        
        triangles->add(nodeTriangle);
    }    
     
    [self setSelectionMode:[self selectionMode]];*/
}

- (void)makeEdges
{
	/*
    [self resetCache];
	edges->clear();
	for (uint i = 0; i < triangles->size(); i++)
	{
		Triangle triangle = triangles->at(i);
		Edge triangleEdges[3];
		BOOL addTriangleEdges[3];
		
		for (uint j = 0; j < 3; j++)
		{
			addTriangleEdges[j] = YES;
			if (j == 2)
			{
				triangleEdges[j].vertexIndices[0] = triangle.vertexIndices[2];
				triangleEdges[j].vertexIndices[1] = triangle.vertexIndices[0];
			}
			else
			{
				triangleEdges[j].vertexIndices[0] = triangle.vertexIndices[j];
				triangleEdges[j].vertexIndices[1] = triangle.vertexIndices[j + 1];
			}
		}

		int falseCounter = 0;
		
		for (uint j = 0; j < edges->size(); j++)
		{
			Edge edge = edges->at(j);
			for (uint k = 0; k < 3; k++)
			{
				if (addTriangleEdges[k] && 
					AreEdgesSame(triangleEdges[k], edge))
				{
					addTriangleEdges[k] = NO;
					falseCounter++;
					break;
				}
			}
			if (falseCounter == 3)
				break;
		}
		
		for (uint j = 0; j < 3; j++)
		{
			if (addTriangleEdges[j])
				edges->push_back(triangleEdges[j]);
		}
	}
	
	NSLog(@"edgeCount:%i", [self edgeCount]);*/
}

- (void)fastMergeVertexFirst:(uint)firstIndex second:(uint)secondIndex
{
    /*
	NSLog(@"fastMergeVertexFirst:%i second:%i", firstIndex, secondIndex);
	NSAssert(vertices->size() == selected->size(), @"vertices->size() == selected->size()");
	
	Vector3D first = [self vertexAtIndex:firstIndex];
	Vector3D second = [self vertexAtIndex:secondIndex];
	Vector3D center = first + second;
	center /= 2;
	
	vertices->push_back(center);
	selected->push_back((SelectionInfo){ NO, YES });
	
	uint centerIndex = vertices->size() - 1;
	
	for (uint i = 0; i < triangles->size(); i++)
	{
		for (uint j = 0; j < 3; j++)
		{
			if (triangles->at(i).vertexIndices[j] == firstIndex ||
				triangles->at(i).vertexIndices[j] == secondIndex)
			{
				triangles->at(i).vertexIndices[j] = centerIndex;
			}
		}
	}
	
	// erasing should happen from the back of STL vector 
	if (firstIndex > secondIndex)
	{
		[self removeVertexAtIndex:firstIndex];
		[self removeVertexAtIndex:secondIndex];
	}
	else
	{
		[self removeVertexAtIndex:secondIndex];
		[self removeVertexAtIndex:firstIndex];
	}
	
	NSAssert(vertices->size() == selected->size(), @"vertices->size() == selected->size()");*/
}

- (void)mergeVertexPairs
{
    /*
	NSLog(@"mergeVertexPairs");
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

- (void)mergeWithMesh:(Mesh *)mesh
{
    /*
	NSLog(@"mergeWithMesh:");
	[self resetCache];
	
	uint vertexCount = vertices->size();
	for (uint i = 0; i < mesh->vertices->size(); i++)
	{
		vertices->push_back(mesh->vertices->at(i));
	}
	for (uint i = 0; i < mesh->triangles->size(); i++)
	{
		Triangle triangle = mesh->triangles->at(i);
		triangle.vertexIndices[0] += vertexCount;
		triangle.vertexIndices[1] += vertexCount;
		triangle.vertexIndices[2] += vertexCount;
		triangles->push_back(triangle);
	}
	selected->clear();
	for (uint i = 0; i < vertices->size(); i++)
		selected->push_back((SelectionInfo){ NO, YES });*/
}

- (void)splitTriangleAtIndex:(uint)index
{
	/*NSLog(@"splitTriangleAtIndex:%i", index);
	
	Triangle triangle = [self triangleAtIndex:index];
	Vector3D triangleVertices[3];
	[self getTriangleVertices:triangleVertices fromTriangle:triangle];
	
	Vector3D centerVertex = Vector3D();
	
	for (uint i = 0; i < 3; i++)
		centerVertex +=	triangleVertices[i];
	
	centerVertex /= 3;
	
	vertices->push_back(centerVertex);
	
	uint centerVertexIndex = vertices->size() - 1;
	
	Vector3D triangleNormal = NormalFromTriangleVertices(triangleVertices);
		
	for (uint i = 0; i < 3; i++)
	{
		Triangle newTriangle;
		
		if (i == 2)
		{
			newTriangle = MakeTriangle(triangle.vertexIndices[2], 
									   triangle.vertexIndices[0], 
									   centerVertexIndex);
		}
		else
		{
			newTriangle = MakeTriangle(triangle.vertexIndices[i], 
									   triangle.vertexIndices[i + 1], 
									   centerVertexIndex);
		}
		
		[self getTriangleVertices:triangleVertices fromTriangle:newTriangle];
		
		Vector3D newTriangleNormal = NormalFromTriangleVertices(triangleVertices);
		
		if (triangleNormal.Dot(newTriangleNormal) < 0)
			newTriangle = FlipTriangle(newTriangle);
		
		[self addTriangle:newTriangle];
	}
	
	[self removeTriangleAtIndex:index];*/
}

- (void)splitEdgeAtIndex:(uint)index
{
	/*NSLog(@"splitEdgeAtIndex:%i", index);
	
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
	/*NSLog(@"splitSelectedEdges");
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

- (void)splitSelectedTriangles
{
	/*NSLog(@"splitSelectedTriangles");
	[self resetCache];
	
	for (int i = 0; i < (int)selected->size(); i++)
	{
		if (selected->at(i).selected)
		{
			[self splitTriangleAtIndex:i];
			i--;
		}
	}*/
}

- (void)turnEdgeAtIndex:(uint)index
{
	/*NSLog(@"turnEdgeAtIndex:%i", index);
	
	Edge edge = [self edgeAtIndex:index];
	uint counter = 0;
	uint oldTriangleIndices[2];
	Triangle oldTriangles[2];
	
	for (uint i = 0; i < triangles->size(); i++)
	{
		Triangle triangle = [self triangleAtIndex:i];
		if (IsEdgeInTriangle(triangle, edge))
		{
			oldTriangleIndices[counter] = i;
			oldTriangles[counter] = triangle;
			counter++;
			if (counter == 2)
			{
				Edge turned;
				turned.vertexIndices[0] = NonEdgeIndexInTriangle(oldTriangles[0], edge);
				turned.vertexIndices[1] = NonEdgeIndexInTriangle(oldTriangles[1], edge);
				
				Vector3D triangleVertices[3];
				
				[self getTriangleVertices:triangleVertices fromTriangle:oldTriangles[0]];
				Vector3D oldTriangleNormal1 = NormalFromTriangleVertices(triangleVertices);
				
				[self getTriangleVertices:triangleVertices fromTriangle:oldTriangles[1]];
				Vector3D oldTriangleNormal2 = NormalFromTriangleVertices(triangleVertices);
				
				for (int j = 0; j < 2; j++)
				{
					Triangle newTriangle = MakeTriangle(edge.vertexIndices[j], 
														turned.vertexIndices[0], 
														turned.vertexIndices[1]);
										
					[self getTriangleVertices:triangleVertices fromTriangle:newTriangle];
					Vector3D newTriangleNormal = NormalFromTriangleVertices(triangleVertices);
					
					// two dot products, it is working, but not in all cases
					if (newTriangleNormal.Dot(oldTriangleNormal1) < 0.0f ||
						newTriangleNormal.Dot(oldTriangleNormal2) < 0.0f)
					{
						newTriangle = FlipTriangle(newTriangle);
						NSLog(@"opposite in turnEdgeAtIndex	");
					}
					
					triangles->at(oldTriangleIndices[j]) = newTriangle;
				}
								
				edges->at(index) = turned;
				
				return;
			}
		}
	}*/
}

- (void)turnSelectedEdges
{
	/*NSLog(@"turnSelectedEdges");
	
	for (uint i = 0; i < selected->size(); i++)
	{
		if (selected->at(i).selected)
		{
			[self turnEdgeAtIndex:i];
			
			// uncomment this line to deselect after edge turn
			//selected->at(i) = NO;
		}
	}*/
}

- (void)mergeSelected
{
	NSLog(@"mergeSelected");
    mesh->mergeSelected();
}

- (void)splitSelected
{
	NSLog(@"splitSelected");
	
	switch (mesh->selectionMode())
	{
		case MeshSelectionModeEdges:
			[self splitSelectedEdges];
			break;
		case MeshSelectionModeTriangles:
			[self splitSelectedTriangles];
			break;
		default:
			break;
	}
}

#pragma mark OpenGLManipulatingModel implementation

- (uint)count
{
    return mesh->selectedCount();
}

- (void)didSelect
{
    
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
	mesh->drawAtIndex(index, forSelection, mode);
}

- (void)flipSelected
{
    mesh->flipSelected();
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

- (void)flipSelectedTriangles
{
//	if (selectionMode == MeshSelectionModeTriangles)
//	{	
//		[self resetCache];
//        for (TriangleNode *node = triangles->begin(), *end = triangles->end(); node != end; node = node->next())
//        {
//            if (node->data.selected)
//                node->data.Flip();
//        }
//	}
}

- (void)flipAllTriangles
{
//	[self resetCache];
//	for (TriangleNode *node = triangles->begin(), *end = triangles->end(); node != end; node = node->next())
//    {
//        node->data.Flip();
//    }
}
				 
#pragma mark NSCoding implementation

- (id)initWithCoder:(NSCoder *)aDecoder
{
	self = [self init];
	if (self)
	{		
		/*NSUInteger tempLength = 0;
		
		const Vector3D *tempVertices = (const Vector3D *)[aDecoder decodeBytesForKey:@"vertices"
																	  returnedLength:&tempLength];
		tempLength /= sizeof(Vector3D);
		
		for (uint i = 0; i < tempLength; i++)
			vertices->push_back(tempVertices[i]);
		
		const Triangle *tempTriangles = (const Triangle *)[aDecoder decodeBytesForKey:@"triangles"
																	   returnedLength:&tempLength];
		tempLength /= sizeof(Triangle);
		
		for (uint i = 0; i < tempLength; i++)
			triangles->push_back(tempTriangles[i]);*/
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	// problem with zero size, should be handled in code for Item
	//[aCoder encodeBytes:(uint8_t *)&vertices->at(0) length:vertices->size() * sizeof(Vector3D) forKey:@"vertices"];
	//[aCoder encodeBytes:(uint8_t *)&triangles->at(0) length:triangles->size() * sizeof(Triangle) forKey:@"triangles"];
}

#pragma mark CppFileStreaming implementation

- (id)initWithFileStream:(ifstream *)fin
{
	self = [self init];
	if (self)
	{
		/*uint verticesSize;
		uint trianglesSize;
		fin->read((char *)&verticesSize, sizeof(uint));
		fin->read((char *)&trianglesSize, sizeof(uint));
		
		for (uint i = 0; i < verticesSize; i++)
		{
			Vector3D vertex;
			fin->read((char *)&vertex, sizeof(Vector3D));
			vertices->push_back(vertex);
		}
		
		for (uint i = 0; i < trianglesSize; i++)
		{
			Triangle triangle;
			fin->read((char *)&triangle, sizeof(Triangle));
			triangles->push_back(triangle);
		}*/
	}
	return self;
}

- (void)encodeWithFileStream:(ofstream *)fout
{
	/*uint size = vertices->size(); 
	fout->write((char *)&size, sizeof(uint));
	size = triangles->size();
	fout->write((char *)&size, sizeof(uint));
	
	if (vertices->size() > 0)
		fout->write((char *)&vertices->at(0), vertices->size() * sizeof(Vector3D));
	
	if (triangles->size() > 0)
		fout->write((char *)&triangles->at(0), triangles->size() * sizeof(Triangle));*/
}

- (NSString *)nameAtIndex:(uint)index
{
	/*switch (selectionMode)
	{
		case MeshSelectionModeVertices:
			return [NSString stringWithFormat:@"Vertex %i", index];
		case MeshSelectionModeEdges:
			return [NSString stringWithFormat:@"Edge %i", index];
		case MeshSelectionModeTriangles:
			return [NSString stringWithFormat:@"Triangle %i", index];	
		default:
			return nil;
	}*/
    return nil;
}

@end
