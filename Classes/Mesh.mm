//
//  Mesh.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/29/09.
//  For license see LICENSE.TXT
//

#import "Mesh.h"

static ShaderProgram *normalShader;
static ShaderProgram *flippedShader;

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

+ (void)setNormalShader:(ShaderProgram *)shaderProgram
{
	normalShader = shaderProgram;
}

+ (void)setFlippedShader:(ShaderProgram *)shaderProgram
{
	flippedShader = shaderProgram;
}

- (uint)vertexCount
{
	return vertices->Count();
}

- (uint)triangleCount
{
	return triangles->Count();
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
        vertices = new FPList<VertexNode, Vertex2>();
        triangles = new FPList<TriangleNode, Triangle2>();
		selectionMode = MeshSelectionModeVertices;
		float hue = (random() % 10) / 10.0f;
		color = [NSColor colorWithCalibratedHue:hue 
									 saturation:0.5f
									 brightness:0.6f 
										  alpha:1.0f];
		[color retain];
		
		cachedIndices = NULL;
		cachedVertices = NULL;
		cachedNormals = NULL;
		cachedColors = NULL;
        
        cachedVertexSelection = new vector<VertexNode *>();
        cachedTriangleSelection = new vector<TriangleNode *>();
	}
	return self;
}

- (void)dealloc
{
	delete vertices;
	delete triangles;
	[color release];
	[self resetCache];
	[super dealloc];
}

- (enum MeshSelectionMode)selectionMode
{
    return selectionMode;
}

- (void)setSelectionMode:(enum MeshSelectionMode)value
{
	selectionMode = value;
    cachedVertexSelection->clear();
    cachedTriangleSelection->clear();
    
    switch (selectionMode)
    {
        case MeshSelectionModeVertices:
        {
            for (VertexNode *node = vertices->Begin(), *end = vertices->End(); node != end; node = node->Next())
                cachedVertexSelection->push_back(node);
        } break;
        case MeshSelectionModeTriangles:
        {
            for (TriangleNode *node = triangles->Begin(), *end = triangles->End(); node != end; node = node->Next())
                cachedTriangleSelection->push_back(node);
        } break;            
        default:
            break;
    }
}

- (void)addVertex:(Vector3D)aVertex
{
	vertices->Add(Vertex2(aVertex));
}

- (void)addTriangle:(Triangle2)aTriangle
{
	triangles->Add(aTriangle);
}

- (void)addTriangleWithNode1:(VertexNode *)node1
					   node2:(VertexNode *)node2
					   node3:(VertexNode *)node3
{
	Triangle2 triangle(node1, node2, node3);
	[self addTriangle:triangle];
}

- (void)addQuadWithNode1:(VertexNode *)node1
				   node2:(VertexNode *)node2
				   node3:(VertexNode *)node3 
				   node4:(VertexNode *)node4
{
	Triangle2 triangle1(node1, node2, node3);
    Triangle2 triangle2(node1, node3, node4);
	[self addTriangle:triangle1];
	[self addTriangle:triangle2];
}

- (void)resetIndexCache
{
	if (cachedIndices)
	{
		delete cachedIndices;
		cachedIndices = NULL;
	}
}

- (void)resetCache
{
	[self resetIndexCache];
	if (cachedVertices)
	{
		delete [] cachedVertices;
		cachedVertices = NULL;
	}
	if (cachedNormals)
	{
		delete [] cachedNormals;
		cachedNormals = NULL;
	}
	if (cachedColors)
	{
		delete [] cachedColors;
		cachedColors = NULL;
	}
}

- (void)fillCache
{
	if (!cachedVertices)
	{
		cachedVertices = new Vector3D[triangles->Count() * 3];
		cachedNormals = new Vector3D[triangles->Count() * 3];
		cachedColors = new Vector3D[triangles->Count() * 3];
		Vector3D triangleVertices[3];
        
        uint i = 0;
		
		for (TriangleNode *node = triangles->Begin(), *end = triangles->End(); node != end; node = node->Next())
		{
			Triangle2 currentTriangle = node->data;
            currentTriangle.GetVertexPositions(triangleVertices);
			
			Vector3D n = NormalFromTriangleVertices(triangleVertices);
			
			for (uint j = 0; j < 3; j++)
			{
				cachedVertices[i * 3 + j] = triangleVertices[j];
				cachedNormals[i * 3 + j] = n;
			}
            
            i++;
		}
	}
}

- (void)updateColorCacheAsDarker:(BOOL)darker
{
	CGFloat components[4];
	[color getComponents:components];
	
	CGFloat selectedComponents[] = { 0.7f, 0.0f, 0.0f };
	
	if (darker)
	{
		for (uint k = 0; k < 3; k++)
			components[k] -= 0.2f;
		
		selectedComponents[0] -= 0.2f;
	}
    
    uint i = 0;
	
    for (TriangleNode *node = triangles->Begin(), *end = triangles->End(); node != end; node = node->Next())
	{
		if (node->data.selected)
		{
			for (uint j = 0; j < 3; j++)
			{
				for (uint k = 0; k < 3; k++)
				{
					cachedColors[i * 3 + j][k] = selectedComponents[k];
				}
			}				
		}
		else
		{
			for (uint j = 0; j < 3; j++)
			{
				for (uint k = 0; k < 3; k++)
				{
					cachedColors[i * 3 + j][k] = components[k];
				}
			}	
		}
        
        i++;
	}
}

- (void)drawFillAsDarker:(BOOL)darker forSelection:(BOOL)forSelection
{
	[self fillCache];
	if (selectionMode == MeshSelectionModeTriangles && !forSelection)
		[self updateColorCacheAsDarker:darker];
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (selectionMode == MeshSelectionModeTriangles && !forSelection)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		float *colorPtr = (float *)cachedColors;
		glColorPointer(3, GL_FLOAT, 0, colorPtr);
	}
	
	float *vertexPtr = (float *)cachedVertices;
	float *normalPtr = (float *)cachedNormals;
	
	glNormalPointer(GL_FLOAT, 0, normalPtr);
	glVertexPointer(3, GL_FLOAT, 0, vertexPtr);
	
	if (cachedIndices != NULL)
	{
		if (cachedIndices->size() > 0)
		{
			uint *indexPtr = &(*cachedIndices)[0];
			glDrawElements(GL_TRIANGLES, cachedIndices->size(), GL_UNSIGNED_INT, indexPtr);
		}
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, triangles->Count() * 3);
	}
	
	if (selectionMode == MeshSelectionModeTriangles && !forSelection)
		glDisableClientState(GL_COLOR_ARRAY);
	
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

- (void)drawWire
{
	if (selectionMode != MeshSelectionModeEdges)
	{
		glColor3f([color redComponent] - 0.2f, [color greenComponent] - 0.2f, [color blueComponent] - 0.2f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		[self drawFillAsDarker:YES forSelection:NO];
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

- (void)useShader:(BOOL)flipped
{
	if (flipped)
		[flippedShader useProgram];
	else
		[normalShader useProgram];
}

- (void)drawWithMode:(enum ViewMode)mode scale:(Vector3D)scale selected:(BOOL)isSelected forSelection:(BOOL)forSelection
{
	BOOL flipped = scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f;
	
	glPushMatrix();
	glScalef(scale.x, scale.y, scale.z);
	if (mode == ViewModeWireframe)
	{
        if (!forSelection)
            glColor3f([color redComponent] + 0.2f, [color greenComponent] + 0.2f, [color blueComponent] + 0.2f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		[self drawFillAsDarker:YES forSelection:forSelection];
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (forSelection)
	{
        [self drawFillAsDarker:YES forSelection:forSelection];
    }
    else
    {
		if (isSelected)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);
			[self useShader:flipped];
			if (selectionMode != MeshSelectionModeTriangles)
			{
				glColor3f([color redComponent], [color greenComponent], [color blueComponent]);
			}
			[self drawFillAsDarker:NO forSelection:forSelection];
			[ShaderProgram resetProgram];
			glDisable(GL_POLYGON_OFFSET_FILL);
			[self drawWire];
		}
		else
		{
            [self useShader:flipped];
            if (selectionMode != MeshSelectionModeTriangles)
            {
                glColor3f([color redComponent], [color greenComponent], [color blueComponent]);
            }
            [self drawFillAsDarker:NO forSelection:NO];
            [ShaderProgram resetProgram];
		}
	}
	glPopMatrix();
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
	
	vertices->RemoveAll();
	triangles->RemoveAll();
	
	// back vertices
	VertexNode *node0 = vertices->Add(Vector3D(-1, -1, -1)); // 0
	VertexNode *node1 = vertices->Add(Vector3D( 1, -1, -1)); // 1
    VertexNode *node2 = vertices->Add(Vector3D( 1,  1, -1)); // 2
	VertexNode *node3 = vertices->Add(Vector3D(-1,  1, -1)); // 3
	
	// front vertices
	VertexNode *node4 = vertices->Add(Vector3D(-1, -1,  1)); // 4
	VertexNode *node5 = vertices->Add(Vector3D( 1, -1,  1)); // 5
	VertexNode *node6 = vertices->Add(Vector3D( 1,  1,  1)); // 6
	VertexNode *node7 = vertices->Add(Vector3D(-1,  1,  1)); // 7
	
	// back triangles
	[self addQuadWithNode1:node0 node2:node1 node3:node2 node4:node3];
	
	// front triangles
	[self addQuadWithNode1:node7 node2:node6 node3:node5 node4:node4];
	
	// bottom triangles
	[self addQuadWithNode1:node1 node2:node0 node3:node4 node4:node5];
	
	// top triangles
	[self addQuadWithNode1:node3 node2:node2 node3:node6 node4:node7];
	
	// left triangles
	[self addQuadWithNode1:node7 node2:node4 node3:node0 node4:node3];
	
	// right triangles
	[self addQuadWithNode1:node2 node2:node1 node3:node5 node4:node6];
	
	[self setSelectionMode:[self selectionMode]];
}

- (void)makeCylinderWithSteps:(uint)steps
{
	NSLog(@"makeCylinderWithSteps:%i", steps);
	
	vertices->RemoveAll();
	triangles->RemoveAll();
	
	VertexNode *node0 = vertices->Add(Vector3D(0, -1, 0)); // 0
 	VertexNode *node1 = vertices->Add(Vector3D(0,  1, 0)); // 1
	
	VertexNode *node2 = vertices->Add(Vector3D(cosf(0.0f), -1, sinf(0.0f))); // 2
	VertexNode *node3 = vertices->Add(Vector3D(cosf(0.0f),  1, sinf(0.0f))); // 3
		
	uint max = steps;
	float step = (FLOAT_PI * 2.0f) / max;
	float angle = step;
	for (uint i = 1; i < max; i++)
	{
		VertexNode *last2 = vertices->Add(Vector3D(cosf(angle), -1, sinf(angle))); // 4
		VertexNode *last1 = vertices->Add(Vector3D(cosf(angle),  1, sinf(angle))); // 5
        
        VertexNode *last3 = last2->Previous();
        VertexNode *last4 = last3->Previous();
		
		Triangle2 triangle1(last3, last2, last1);
        Triangle2 triangle2(last2, last3, last4);
		
		triangles->Add(triangle1);
		triangles->Add(triangle2);
		
		Triangle2 triangle3(last4, node0, last2);
        Triangle2 triangle4(last3, last1, node1);
		
		triangles->Add(triangle3);
		triangles->Add(triangle4);
		
		angle += step;
	}
    
    VertexNode *last1 = vertices->Last();
    VertexNode *last2 = last1->Previous();
	
	Triangle2 triangle1(node2, node3, last1);
    Triangle2 triangle2(last1, last2, node2);
	
	triangles->Add(triangle1);
	triangles->Add(triangle2);
	
	Triangle2 triangle3(node0, node2, last2);
    Triangle2 triangle4(node3, node1, last1);
	
	triangles->Add(triangle3);
	triangles->Add(triangle4);
	
	[self setSelectionMode:[self selectionMode]];
}

- (void)makeSphereWithSteps:(uint)steps
{
    NSLog(@"makeSphereWithSteps:%i", steps);
	
	vertices->RemoveAll();
	triangles->RemoveAll();
		
	uint max = steps;
    
    vector<VertexNode *> tempVertices;
    vector<Triangle> tempTriangles;
     
    tempVertices.push_back(vertices->Add(Vector3D(0, 1, 0)));
    tempVertices.push_back(vertices->Add(Vector3D(0, -1, 0)));
     
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
     
            tempVertices.push_back(vertices->Add(Vector3D(x0, y0, z0)));
     
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
        triangles->Add(nodeTriangle);
    }    
     
    [self setSelectionMode:[self selectionMode]];
}

- (void)makeEdges
{
	[self resetCache];
	/*
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

- (void)removeDegeneratedTriangles
{
	NSLog(@"removeDegeneratedTriangles");
	[self resetCache];
    
    for (TriangleNode *node = triangles->Begin(), *end = triangles->End(); node != end; node = node->Next())
    {
        if (node->data.IsDegenerated())
            triangles->Remove(node);
    }
}

- (void)removeNonUsedVertices
{
	NSLog(@"removeNonUsedVertices");
	[self resetCache];
    
    for (VertexNode *node = vertices->Begin(), *end = vertices->End(); node != end; node = node->Next())
    {
        if (!node->IsUsed())
            vertices->Remove(node);
    }
}

- (void)removeSelectedVertices
{
	NSLog(@"removeSelectedVertices");
	[self resetCache];

    for (VertexNode *node = vertices->Begin(), *end = vertices->End(); node != end; node = node->Next())
    {
        if (node->data.selected)
            vertices->Remove(node);
    }
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

- (void)fastMergeSelectedVertices
{
	NSLog(@"fastMergeSelectedVertices");
	
	Vector3D center = Vector3D();
    
    SimpleList<VertexNode *> *selectedNodes = new SimpleList<VertexNode *>();
	
    for (VertexNode *node = vertices->Begin(), *end = vertices->End(); node != end; node = node->Next())
    {
        if (node->data.selected)
        {
            selectedNodes->Add(node);
            center += node->data.position;
        }
    }
    
	if (selectedNodes->Count() < 2)
    {
        delete selectedNodes;
		return;
    }
	
	center /= (float)selectedNodes->Count();
    
    VertexNode *centerNode = vertices->Add(center);
    
    for (SimpleNode<VertexNode *> *node = selectedNodes->Begin(), *end = selectedNodes->End(); node != end; node = node->Next())
    {
        node->data->ReplaceVertex(centerNode);        
        //vertices->Remove(node->data);        
    }
    
    delete selectedNodes;
}

- (void)mergeSelectedVertices
{
	NSLog(@"mergeSelectedVertices");
	[self resetCache];
	
	[self fastMergeSelectedVertices];
	
	//[self removeDegeneratedTriangles];
	//[self removeNonUsedVertices];
	
	//NSAssert(vertices->size() == selected->size(), @"vertices->size() == selected->size()");
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

- (void)transformWithMatrix:(Matrix4x4)matrix
{
	[self resetCache];
	/*for (uint i = 0; i < vertices->size(); i++)
		vertices->at(i).Transform(matrix);*/
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
	
	switch (selectionMode)
	{
		case MeshSelectionModeVertices:
			[self mergeSelectedVertices];
			break;
		default:
			break;
	}
}

- (void)splitSelected
{
	NSLog(@"splitSelected");
	
	switch (selectionMode)
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
    switch (selectionMode)
    {
        case MeshSelectionModeVertices:
            return cachedVertexSelection->size();
        case MeshSelectionModeTriangles:
            return cachedTriangleSelection->size();
        default:
            return 0;
    }
}

- (void)didSelect
{
    
}

- (void)getSelectionCenter:(Vector3D *)center 
				  rotation:(Quaternion *)rotation
					 scale:(Vector3D *)scale
{
	*center = Vector3D();
	*rotation = Quaternion();
	*scale = Vector3D(1, 1, 1);

	uint selectedCount = 0;
    
    for (VertexNode *node = vertices->Begin(), *end = vertices->End(); node != end; node = node->Next())
    {
        if (node->data.selected)
        {
            *center += node->data.position;
            selectedCount++;
        }
    }
	if (selectedCount > 0)
		*center /= (float)selectedCount;
	
	NSLog(@"selectedCount = %i", selectedCount);
}

- (void)moveSelectedByOffset:(Vector3D)offset
{
	[self resetCache];
    
    for (VertexNode *node = vertices->Begin(), *end = vertices->End(); node != end; node = node->Next())
    {
        if (node->data.selected)
            node->data.position += offset;
    }
}

- (void)rotateSelectedByOffset:(Quaternion)offset
{
	[self resetCache];
    
    for (VertexNode *node = vertices->Begin(), *end = vertices->End(); node != end; node = node->Next())
    {
        if (node->data.selected)
            node->data.position.Transform(offset);
    }	
}

- (void)scaleSelectedByOffset:(Vector3D)offset
{
	[self resetCache];
    
    for (VertexNode *node = vertices->Begin(), *end = vertices->End(); node != end; node = node->Next())
    {
        if (node->data.selected)
        {
            Vector3D &v = node->data.position;
            v.x *= offset.x;
			v.y *= offset.y;
			v.z *= offset.z;
        }
    }
}

- (BOOL)isSelectedAtIndex:(uint)index
{
    switch (selectionMode)
    {
        case MeshSelectionModeVertices:
            return cachedVertexSelection->at(index)->data.selected;
        case MeshSelectionModeTriangles:
            return cachedTriangleSelection->at(index)->data.selected;
        default:
            return NO;
    }
}

- (void)setSelected:(BOOL)isSelected atIndex:(uint)index 
{
    switch (selectionMode)
    {
        case MeshSelectionModeVertices:
            cachedVertexSelection->at(index)->data.selected = isSelected;
            break;
        case MeshSelectionModeTriangles:
        {
            Triangle2 &triangle = cachedTriangleSelection->at(index)->data;
            triangle.selected = isSelected;
            for (uint i = 0; i < 3; i++)
                triangle[i]->data.selected = isSelected;            
        } break;
        default:
            break;
    }
}

- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection withMode:(enum ViewMode)mode
{
	/*if (!selected->at(index).visible)
		return;*/
	
	switch (selectionMode) 
	{
		case MeshSelectionModeVertices:
		{
            const Vertex2 &vertex = cachedVertexSelection->at(index)->data;
			Vector3D v = vertex.position;
			if (!forSelection)
			{
				BOOL isSelected = vertex.selected;
				glPointSize(5.0f);
				if (isSelected)
					glColor3f(1.0f, 0.0f, 0.0f);
				else
					glColor3f(0.0f, 0.0f, 1.0f);
				glDisable(GL_LIGHTING);
			}
			glBegin(GL_POINTS);
			glVertex3f(v.x, v.y, v.z);
			glEnd();
		} break;
		case MeshSelectionModeTriangles:
		{
			if (forSelection)
			{
				Triangle2 triangle = cachedTriangleSelection->at(index)->data;
                Vector3D triangleVertices[3];
                triangle.GetVertexPositions(triangleVertices);
				glBegin(GL_TRIANGLES);
				for (uint i = 0; i < 3; i++)
				{
					Vector3D v = triangleVertices[i];
					glVertex3f(v.x, v.y, v.z);
				}
				glEnd();
			}
		} break;
        default:
            break;
		/*case MeshSelectionModeEdges:
		{
			Edge currentEdge = [self edgeAtIndex:index];
			if (!forSelection)
			{
				BOOL isSelected = selected->at(index).selected;
				if (isSelected)
					glColor3f(0.8f, 0.0f, 0.0f);
				else
					glColor3f([color redComponent] - 0.2f, [color greenComponent] - 0.2f, [color blueComponent] - 0.2f);
				glDisable(GL_LIGHTING);
			}
			glBegin(GL_LINES);
			for (uint i = 0; i < 2; i++)
			{
				Vector3D v = [self vertexAtIndex:currentEdge.vertexIndices[i]];
				glVertex3f(v.x, v.y, v.z);
			}
			glEnd();
		} break;*/
	}
}

- (void)extrudeSelectedTriangles
{/*
	[self resetCache];
	
	// This method finds all nonShared edges and copies all 
	// vertexIndices in selectedTriangles.
	// Then it makes quads between new and old edges.
	
	vector<uint> *vertexIndices = new vector<uint>();
	vector<Edge> *nonSharedEdges = new vector<Edge>();
	
	uint triCount = [self triangleCount];
	uint vertCount = [self vertexCount];
	
	for (uint i = 0; i < triCount; i++)
	{
		if (selected->at(i).selected)
		{
			[self setTriangleMarked:NO atIndex:i];
			Triangle &triangle = triangles->at(i);
			
			for (uint j = 0; j < 3; j++)
			{
				int foundIndex = -1;
				for (uint k = 0; k < vertexIndices->size(); k++)
				{
					if (vertexIndices->at(k) == triangle.vertexIndices[j])
					{
						foundIndex = k;
						break;
					}
				}
								
				uint &index = triangle.vertexIndices[j];
							
				if (foundIndex < 0)
				{
					vertexIndices->push_back(index);
					vertices->push_back(vertices->at(index));
					markedVertices->push_back(YES);
					index = vertCount + vertexIndices->size() - 1;
				}
				else
				{
					index = vertCount + foundIndex;
				}
			}
			
			for (uint j = 0; j < 3; j++)
			{
				Edge edge;
				edge.vertexIndices[0] = triangle.vertexIndices[j];
				edge.vertexIndices[1] = triangle.vertexIndices[j + 1 < 3 ? j + 1 : 0];

				BOOL foundEdge = NO;
				for (uint k = 0; k < nonSharedEdges->size(); k++)
				{
					if (AreEdgesSame(edge, nonSharedEdges->at(k)))
					{
						nonSharedEdges->erase(nonSharedEdges->begin() + k);
						foundEdge = YES;
						break;
					}
				}
				
				if (!foundEdge)
				{
					nonSharedEdges->push_back(edge);
				}
			}
		}
	}
	
	for (uint i = 0; i < nonSharedEdges->size(); i++)
	{
		Edge edge = nonSharedEdges->at(i);
		[self addQuadWithIndex1:edge.vertexIndices[0]
						 index2:vertexIndices->at(edge.vertexIndices[0] - vertCount)
						 index3:vertexIndices->at(edge.vertexIndices[1] - vertCount)
						 index4:edge.vertexIndices[1]];
	}
		
	delete vertexIndices;
	delete nonSharedEdges;
	
	[self removeNonUsedVertices]; // slow but sometimes neccessary*/
}

- (void)flipSelected
{
	if (selectionMode == MeshSelectionModeTriangles)
	{
		[self flipSelectedTriangles];
	}
	else if (selectionMode == MeshSelectionModeEdges)
	{
		[self turnSelectedEdges];
	}
}

- (void)duplicateSelected
{
	if (selectionMode == MeshSelectionModeTriangles)
	{
		[self extrudeSelectedTriangles];
	}	
}

- (void)removeSelected
{
    if (selectionMode == MeshSelectionModeTriangles)
	{
        for (TriangleNode *node = triangles->Begin(), *end = triangles->End(); node != end; node = node->Next())
        {
            if (node->data.selected)
                triangles->Remove(node);
        }
        
		[self removeNonUsedVertices];
        [self setSelectionMode:selectionMode];
	}
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
	/*if (selectionMode == MeshSelectionModeTriangles)
	{	
		[self resetCache];
		for (uint i = 0; i < [self triangleCount]; i++)
		{
			if (selected->at(i).selected)
				[self flipTriangleAtIndex:i];
		}
	}*/
}

- (void)flipAllTriangles
{
	[self resetCache];
	for (uint i = 0; i < [self triangleCount]; i++)
	{
		[self flipTriangleAtIndex:i];
	}
}
				 
- (void)flipTriangleAtIndex:(uint)index
{
	/*Triangle &triangle = triangles->at(index);
	triangle = FlipTriangle(triangle);*/
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
	switch (selectionMode)
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
