//
//  Mesh.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/29/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Mesh.h"

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

- (void)setSelectionMode:(enum MeshSelectionMode)value
{
	selectionMode = value;
	selectedIndices->clear();
	switch (selectionMode) 
	{
		case MeshSelectionModeVertices:
		{
			for (int i = 0; i < vertices->size(); i++)
			{
				selectedIndices->push_back(NO);
			}
		}	break;
		case MeshSelectionModeTriangles:
		{
			for (int i = 0; i < triangles->size(); i++)
			{
				selectedIndices->push_back(NO);
			}
		}	break;
		default:
			break;
	}
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

- (void)drawFillWithScale:(Vector3D)scale
{	
	float normalDiffuse[] = { 0.5, 0.7, 1.0, 1 };
	float selectedDiffuse[] = { 1, 0, 0, 1 };
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, normalDiffuse);
	
	Vector3D triangleVertices[3];
	
	float *lastDiffuse = normalDiffuse; 
	
	glBegin(GL_TRIANGLES);
	
	for (NSUInteger i = 0; i < triangles->size(); i++)
	{
		if (selectionMode == MeshSelectionModeTriangles) 
		{
			if (selectedIndices->at(i))
			{
				if (lastDiffuse == normalDiffuse)
				{
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, selectedDiffuse);
					lastDiffuse = selectedDiffuse;
				}
			}
			else if (lastDiffuse == selectedDiffuse)
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, normalDiffuse);
				lastDiffuse = normalDiffuse;
			}
		}
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
		n.x *= scale.x;
		n.y *= scale.y;
		n.z *= scale.z;		
		for (NSUInteger j = 0; j < 3; j++)
		{
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(triangleVertices[j].x, triangleVertices[j].y, triangleVertices[j].z);			
		}
	}
	
	glEnd();
}

- (void)drawWireWithScale:(Vector3D)scale
{
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	[self drawFillWithScale:scale];
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
}

- (void)drawWithScale:(Vector3D)scale selected:(BOOL)selected
{	
	if (selected)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		[self drawFillWithScale:scale];
		glDisable(GL_POLYGON_OFFSET_FILL);
		[self drawWireWithScale:scale];
	}
	else
	{
		[self drawFillWithScale:scale];
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

- (void)makeCylinder
{
	vertices->push_back(Vector3D(0, -1, 0)); // 0
 	vertices->push_back(Vector3D(0,  1, 0)); // 1
	
	vertices->push_back(Vector3D(cosf(0.0f), -1, sinf(0.0f))); // 2
	vertices->push_back(Vector3D(cosf(0.0f),  1, sinf(0.0f))); // 3
		
	int max = 6;
	float step = (FLOAT_PI * 2.0f) / max;
	float angle = step;
	for (int i = 1; i < max; i++)
	{
		vertices->push_back(Vector3D(cosf(angle), -1, sinf(angle))); // 4
		vertices->push_back(Vector3D(cosf(angle),  1, sinf(angle))); // 5
		
		Triangle triangle1, triangle2;
		triangle1.vertexIndices[0] = vertices->size() - 3;
		triangle1.vertexIndices[1] = vertices->size() - 2;
		triangle1.vertexIndices[2] = vertices->size() - 1;
	
		triangle2.vertexIndices[0] = vertices->size() - 2;
		triangle2.vertexIndices[1] = vertices->size() - 3;
		triangle2.vertexIndices[2] = vertices->size() - 4;
		
		triangles->push_back(triangle1);
		triangles->push_back(triangle2);
		
		Triangle triangle3, triangle4;
		triangle3.vertexIndices[0] = vertices->size() - 4;
		triangle3.vertexIndices[1] = 0;
		triangle3.vertexIndices[2] = vertices->size() - 2;
		
		triangle4.vertexIndices[0] = vertices->size() - 3;
		triangle4.vertexIndices[1] = vertices->size() - 1;
		triangle4.vertexIndices[2] = 1;
		
		triangles->push_back(triangle3);
		triangles->push_back(triangle4);
		
		angle += step;
	}
	
	Triangle triangle1, triangle2;
	triangle1.vertexIndices[0] = 2;
	triangle1.vertexIndices[1] = 3;
	triangle1.vertexIndices[2] = vertices->size() - 1;
	
	triangle2.vertexIndices[0] = vertices->size() - 1;
	triangle2.vertexIndices[1] = vertices->size() - 2;
	triangle2.vertexIndices[2] = 2;
	
	triangles->push_back(triangle1);
	triangles->push_back(triangle2);
	
	Triangle triangle3, triangle4;
	triangle3.vertexIndices[0] = 0;
	triangle3.vertexIndices[1] = 2;
	triangle3.vertexIndices[2] = vertices->size() - 2;
	
	triangle4.vertexIndices[0] = 3;
	triangle4.vertexIndices[1] = 1;
	triangle4.vertexIndices[2] = vertices->size() - 1;
	
	triangles->push_back(triangle3);
	triangles->push_back(triangle4);
	
	for (int i = 0; i < vertices->size(); i++)
		selectedIndices->push_back(NO);
}

- (void)removeVertexAtIndex:(NSUInteger)index
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
	if (selectionMode == MeshSelectionModeVertices)
		selectedIndices->erase(selectedIndices->begin() + index);
}

- (void)removeTriangleAtIndex:(NSUInteger)index
{
	triangles->erase(triangles->begin() + index);
	if (selectionMode == MeshSelectionModeTriangles)
		selectedIndices->erase(selectedIndices->begin() + index);
}

- (void)removeDegeneratedTriangles
{
	NSLog(@"removeDegeneratedTriangles");
	
	for (int i = 0; i < triangles->size(); i++)
	{
		if (IsTriangleDegenerated(triangles->at(i)))
		{
			[self removeTriangleAtIndex:i];
			i--;
		}
	}	
}

- (BOOL)isVertexUsedAtIndex:(NSUInteger)index
{
	for (int i = 0; i < triangles->size(); i++)
	{
		Triangle triangle = triangles->at(i);
		for (int j = 0; j < 3; j++)
		{
			if (triangle.vertexIndices[j] == index)
				return YES;
		}
	}
	return NO;
}

- (void)removeNonUsedVertices
{
	NSLog(@"removeNonUsedVertices");
	
	for (int i = 0; i < vertices->size(); i++)
	{
		if (![self isVertexUsedAtIndex:i])
		{
			[self removeVertexAtIndex:i];
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
			[self removeVertexAtIndex:i];
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
	[self removeNonUsedVertices];
	
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
	if (selectionMode == MeshSelectionModeVertices)
		return vertices->size();
	return triangles->size();
}

- (Vector3D)positionAtIndex:(NSUInteger)index
{
	if (selectionMode == MeshSelectionModeVertices)
		return vertices->at(index);
	return Vector3D();
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
	if (selectionMode == MeshSelectionModeVertices)
		vertices->at(index) = position;
}

- (void)setRotation:(Quaternion)rotation atIndex:(NSUInteger)index {}
- (void)setScale:(Vector3D)scale atIndex:(NSUInteger)index {}

- (void)moveByOffset:(Vector3D)offset atIndex:(NSUInteger)index
{
	if (selectionMode == MeshSelectionModeVertices)
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

- (void)drawAtIndex:(NSUInteger)index forSelection:(BOOL)forSelection
{
	if (selectionMode == MeshSelectionModeVertices)
	{
		Vector3D v = [self vertexAtIndex:index];
		if (!forSelection)
		{
			BOOL selected = [self isSelectedAtIndex:index];
			glPointSize(5.0f);
			if (selected)
				glColor3f(1, 0, 0);
			else
				glColor3f(0, 0, 1);
			glDisable(GL_LIGHTING);
		}
		glBegin(GL_POINTS);
		glVertex3f(v.x, v.y, v.z);
		glEnd();
	}
	else if (forSelection)
	{
		Triangle currentTriangle = [self triangleAtIndex:index];
		glBegin(GL_TRIANGLES);
		for (NSUInteger i = 0; i < 3; i++)
		{
			Vector3D v = [self vertexAtIndex:currentTriangle.vertexIndices[i]];
			glVertex3f(v.x, v.y, v.z);
		}
		glEnd();
	}
}

- (void)cloneAtIndex:(NSUInteger)index {}

- (void)removeAtIndex:(NSUInteger)index
{
	if (selectionMode == MeshSelectionModeTriangles)
	{
		[self removeTriangleAtIndex:index];
		[self removeNonUsedVertices];
	}
}

@end
