//
//  Mesh.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/29/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Mesh.h"

Edge MakeEdge(NSUInteger index1, NSUInteger index2)
{
	Edge edge;
	edge.vertexIndices[0] = index1;
	edge.vertexIndices[1] = index2;
	return edge;
}

Triangle MakeTriangle(NSUInteger index1, NSUInteger index2, NSUInteger index3)
{
	Triangle triangle;
	triangle.vertexIndices[0] = index1;
	triangle.vertexIndices[1] = index2;
	triangle.vertexIndices[2] = index3;
	return triangle;
}

BOOL IsTriangleDegenerated(Triangle triangle)
{
	if (triangle.vertexIndices[0] == triangle.vertexIndices[1])
		return YES;
	if (triangle.vertexIndices[0] == triangle.vertexIndices[2])
		return YES;
	if (triangle.vertexIndices[1] == triangle.vertexIndices[2])
		return YES;

	return NO;
}

@implementation Mesh

@synthesize selectionMode;

- (id)init
{
	self = [super init];
	if (self)
	{
		vertices = new vector<Vector3D>();
		triangles = new vector<Triangle>();
		selectedIndices = new vector<BOOL>();
	}
	return self;
}

- (void)dealloc
{
	delete vertices;
	delete triangles;
	delete selectedIndices;
	[super dealloc];
}

- (Vector3D)vertexAtIndex:(NSUInteger)anIndex
{
	return (*vertices)[anIndex];
}

- (Triangle)triangleAtIndex:(NSUInteger)anIndex
{
	return (*triangles)[anIndex];
}

- (void)addVertex:(Vector3D)aVertex
{
	vertices->push_back(aVertex);
}

- (void)addTriangle:(Triangle)aTriangle
{
	triangles->push_back(aTriangle);
}

- (void)drawFill
{
	Vector3D triangleVertices[3];
	
	glBegin(GL_TRIANGLES);
	
	for (NSUInteger i = 0; i < triangles->size(); i++)
	{
		Triangle currentTriangle = [self triangleAtIndex:i];
		for (NSUInteger j = 0; j < 3; j++)
		{
			Vector3D currentVertex = [self vertexAtIndex:currentTriangle.vertexIndices[j]];
			triangleVertices[j] = currentVertex;
		}
		Vector3D u = triangleVertices[1] - triangleVertices[0];
		Vector3D v = triangleVertices[2] - triangleVertices[0];
		Vector3D n = v.Cross(u);
		n.Normalize();
		for (NSUInteger j = 0; j < 3; j++)
		{
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(triangleVertices[j].x, triangleVertices[j].y, triangleVertices[j].z);			
		}
	}
	
	glEnd();
}

- (void)drawWire
{
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	[self drawFill];
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
}

- (void)draw:(BOOL)selected
{	
	if (selected)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		[self drawFill];
		glDisable(GL_POLYGON_OFFSET_FILL);
		[self drawWire];
	}
	else
	{
		[self drawFill];
	}
}

- (void)makeCube
{
	vertices->clear();
	triangles->clear();
	selectedIndices->clear();
	
	// back vertices
	vertices->push_back(Vector3D(-1, -1, -1)); // 0
	vertices->push_back(Vector3D( 1, -1, -1)); // 1
	vertices->push_back(Vector3D( 1,  1, -1)); // 2
	vertices->push_back(Vector3D(-1,  1, -1)); // 3
	
	// front vertices
	vertices->push_back(Vector3D(-1, -1,  1)); // 4
	vertices->push_back(Vector3D( 1, -1,  1)); // 5
	vertices->push_back(Vector3D( 1,  1,  1)); // 6
	vertices->push_back(Vector3D(-1,  1,  1)); // 7
	
	// back triangles
	triangles->push_back(MakeTriangle(0, 1, 2));
	triangles->push_back(MakeTriangle(0, 2, 3));
	
	// front triangles
	triangles->push_back(MakeTriangle(6, 5, 4));
	triangles->push_back(MakeTriangle(7, 6, 4));
	
	// bottom triangles
	triangles->push_back(MakeTriangle(4, 1, 0));
	triangles->push_back(MakeTriangle(1, 4, 5));
	
	// top triangles
	triangles->push_back(MakeTriangle(6, 3, 2));
	triangles->push_back(MakeTriangle(3, 6, 7));
	
	// left triangles
	triangles->push_back(MakeTriangle(0, 3, 4));
	triangles->push_back(MakeTriangle(7, 4, 3));
	
	// right triangles
	triangles->push_back(MakeTriangle(5, 2, 1));
	triangles->push_back(MakeTriangle(2, 5, 6));
	
	for (int i = 0; i < vertices->size(); i++)
		selectedIndices->push_back(NO);
}

- (void)removeVertex:(NSUInteger)index
{
	for (NSUInteger i = 0; i < triangles->size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (triangles->at(i).vertexIndices[j] >= index)
				triangles->at(i).vertexIndices[j]--;
		}
	}
	vertices->erase(vertices->begin() + index);
}

- (void)removeTriangle:(NSUInteger)index
{
	triangles->erase(triangles->begin() + index);
}

- (void)removeDegeneratedTriangles
{
	NSLog(@"removeDegeneratedTriangles");
	
	for (int i = 0; i < triangles->size(); i++)
	{
		if (IsTriangleDegenerated(triangles->at(i)))
		{
			[self removeTriangle:i];
			i--;
		}
	}	
}

- (void)removeSelectedVertices
{
	NSLog(@"removeSelectedVertices");
	
	NSAssert(vertices->size() == selectedIndices->size(), @"vertices->size() == selectedIndices->size()");
	
	for (int i = 0; i < selectedIndices->size(); i++)
	{
		if (selectedIndices->at(i))
		{
			[self removeVertex:i];
			selectedIndices->erase(selectedIndices->begin() + i);
			i--;
		}
	}
}

- (void)collapseSelectedVertices
{
	NSLog(@"collapseSelectedVertices");
	NSAssert(vertices->size() == selectedIndices->size(), @"vertices->size() == selectedIndices->size()");
	
	int selectedCount = 0;
	Vector3D center = Vector3D();
	
	for (int i = 0; i < selectedIndices->size(); i++)
	{
		if (selectedIndices->at(i))
		{
			selectedCount++;
			center += vertices->at(i);
		}
	}
	
	NSLog(@"selectedCount = %i", selectedCount);
		
	if (selectedCount < 2)
		return;
	
	center /= selectedCount;
	vertices->push_back(center);
	selectedIndices->push_back(NO);
	
	int centerIndex = vertices->size() - 1;
	
	for (int i = 0; i < selectedIndices->size(); i++)
	{
		if (selectedIndices->at(i))
		{
			for (int j = 0; j < triangles->size(); j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (triangles->at(j).vertexIndices[k] == i)
						triangles->at(j).vertexIndices[k] = centerIndex;
				}				
			}
		}
	}
	
	[self removeSelectedVertices];
	[self removeDegeneratedTriangles];
	
	NSAssert(vertices->size() == selectedIndices->size(), @"vertices->size() == selectedIndices->size()");
	
	NSLog(@"finished");
}

- (void)transformWithMatrix:(Matrix4x4)matrix
{
	for (int i = 0; i < vertices->size(); i++)
		vertices->at(i).Transform(matrix);
}

- (void)mergeWithMesh:(Mesh *)mesh
{
	NSLog(@"mergeWithMesh:");
	
	int vertexCount = vertices->size();
	for (int i = 0; i < mesh->vertices->size(); i++)
	{
		vertices->push_back((*mesh->vertices)[i]);
	}
	for (int i = 0; i < mesh->triangles->size(); i++)
	{
		Triangle triangle = (*mesh->triangles)[i];
		triangle.vertexIndices[0] += vertexCount;
		triangle.vertexIndices[1] += vertexCount;
		triangle.vertexIndices[2] += vertexCount;
		triangles->push_back(triangle);
	}
	selectedIndices->clear();
	for (int i = 0; i < vertices->size(); i++)
		selectedIndices->push_back(NO);
}

- (NSUInteger)count
{
	return vertices->size();
}

- (Vector3D)positionAtIndex:(NSUInteger)index
{
	return vertices->at(index);
}

- (Quaternion)rotationAtIndex:(NSUInteger)index
{
	return Quaternion();
}

- (Vector3D)scaleAtIndex:(NSUInteger)index
{
	return Vector3D(1, 1, 1);
}

- (void)setPosition:(Vector3D)position atIndex:(NSUInteger)index
{
	vertices->at(index) = position;
}

- (void)setRotation:(Quaternion)rotation atIndex:(NSUInteger)index {}
- (void)setScale:(Vector3D)scale atIndex:(NSUInteger)index {}

- (void)moveByOffset:(Vector3D)offset atIndex:(NSUInteger)index
{
	vertices->at(index) += offset;
}

- (void)rotateByOffset:(Quaternion)offset atIndex:(NSUInteger)index {}
- (void)scaleByOffset:(Vector3D)offset atIndex:(NSUInteger)index {}

- (BOOL)isSelectedAtIndex:(NSUInteger)index
{
	return selectedIndices->at(index);
}

- (void)setSelected:(BOOL)selected atIndex:(NSUInteger)index 
{
	selectedIndices->at(index) = selected;
}

- (void)drawAtIndex:(NSUInteger)index
{
	Vector3D v = [self vertexAtIndex:index];
	BOOL selected = [self isSelectedAtIndex:index];
	glPointSize(4.0f);
	if (selected)
		glColor3f(1, 0, 0);
	else
		glColor3f(0, 0, 1);
	glDisable(GL_LIGHTING);
	glBegin(GL_POINTS);
	glVertex3f(v.x, v.y, v.z);
	glEnd();
}

- (void)removeAtIndex:(NSUInteger)index
{
	
}

@end
