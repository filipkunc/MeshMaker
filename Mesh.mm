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
	triangle.edgeIndices[0] = index1;
	triangle.edgeIndices[1] = index2;
	triangle.edgeIndices[2] = index3;
	return triangle;
}

@implementation Mesh

@synthesize selectionMode;

- (id)init
{
	self = [super init];
	if (self)
	{
		vertices = new vector<Vector3D>();
		edges = new vector<Edge>();
		triangles = new vector<Triangle>();
		selectedIndices = new vector<BOOL>();
	}
	return self;
}

- (void)dealloc
{
	delete vertices;
	delete edges;
	delete triangles;
	delete selectedIndices;
	[super dealloc];
}

- (Vector3D)vertexAtIndex:(NSUInteger)anIndex
{
	return (*vertices)[anIndex];
}

- (Edge)edgeAtIndex:(NSUInteger)anIndex
{
	return (*edges)[anIndex];
}

- (Triangle)triangleAtIndex:(NSUInteger)anIndex
{
	return (*triangles)[anIndex];
}

- (void)addVertex:(Vector3D)aVertex
{
	vertices->push_back(aVertex);
}

- (void)addEdge:(Edge)anEdge
{
	edges->push_back(anEdge);
}

- (void)addTriangle:(Triangle)aTriangle
{
	triangles->push_back(aTriangle);
}

- (void)drawFill
{
	Vector3D triangleVertices[3];
	NSUInteger indices[3];
	
	glBegin(GL_TRIANGLES);
	
	for (NSUInteger i = 0; i < triangles->size(); i++)
	{
		Triangle currentTriangle = [self triangleAtIndex:i];
		for (NSUInteger j = 0; j < 3; j++)
		{
			Edge currentEdge = [self edgeAtIndex:currentTriangle.edgeIndices[j]];
			indices[j] = currentEdge.vertexIndices[0];
			for (NSUInteger k = 0; k < j; k++)
			{
				if (indices[j] == indices[k])
				{
					indices[j] = currentEdge.vertexIndices[1];
					break;
				}
			}
			Vector3D currentVertex = [self vertexAtIndex:indices[j]];
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
	edges->clear();
	triangles->clear();
	selectedIndices->clear();
	
	// back vertices
	vertices->push_back(Vector3D(-1, 0, -1)); // 0
	vertices->push_back(Vector3D( 1, 0, -1)); // 1
	vertices->push_back(Vector3D( 0, 1, -1)); // 2
	
//	vertices->push_back(Vector3D(-1, 0, 1)); // 3
//	vertices->push_back(Vector3D( 1, 0, 1)); // 4
//	vertices->push_back(Vector3D( 0, 1, 1)); // 5
	
	// back triangles
	edges->push_back(MakeEdge(0, 1)); // 0
	edges->push_back(MakeEdge(1, 2)); // 1
	edges->push_back(MakeEdge(0, 2)); // 2
	
//	edges->push_back(MakeEdge(3, 4)); // 3
//	edges->push_back(MakeEdge(4, 5)); // 4
//	edges->push_back(MakeEdge(3, 5)); // 5
	
	triangles->push_back(MakeTriangle(0, 1, 2));
//	triangles->push_back(MakeTriangle(3, 4, 5));
	
	for (int i = 0; i < vertices->size(); i++)
		selectedIndices->push_back(NO);
}

- (void)removeVertex:(NSUInteger)index
{
	for (NSUInteger i = 0; i < edges->size(); i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if ((*edges)[i].vertexIndices[j] >= index)
				(*edges)[i].vertexIndices[j]--;
		}
	}
	vertices->erase(vertices->begin() + index);
}

- (void)removeEdge:(NSUInteger)index
{
	for (NSUInteger i = 0; i < triangles->size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if ((*triangles)[i].edgeIndices[j] >= index)
				(*triangles)[i].edgeIndices[j]--;
		}
	}
	edges->erase(edges->begin() + index);
}

- (void)collapseSelected
{
	int selectedCount = 0;
	Vector3D center = Vector3D();
	
	for (int i = 0; i < selectedIndices->size(); i++)
	{
		if ((*selectedIndices)[i])
		{
			selectedCount++;
			center += [self vertexAtIndex:i];
			
			for (int j = 0; j < edges->size(); j++)
			{
				for (int k = 0; k < 2; k++)
				{
					if ((*edges)[j].vertexIndices[k] == i)
						(*edges)[j].vertexIndices[k] = vertices->size();
				}
				if ((*edges)[j].vertexIndices[0] == (*edges)[j].vertexIndices[1])
				{
					[self removeEdge:j];
					j--;
				}
			}
		}
	}
	
	center /= selectedCount;
	vertices->push_back(center);
	
	for (int i = 0; i < selectedIndices->size(); i++)
	{
		if ((*selectedIndices)[i])
		{
			[self removeVertex:i];
			selectedIndices->erase(selectedIndices->begin() + i);
			i--;
		}
	}
}

- (void)transformWithMatrix:(Matrix4x4)matrix
{
	for (int i = 0; i < vertices->size(); i++)
	{
		Vector3D transformed = (*vertices)[i];
		transformed.Transform(matrix);
		(*vertices)[i] = transformed;
	}
}

- (void)mergeWithMesh:(Mesh *)mesh
{
	int vertexCount = vertices->size();
	int edgeCount = edges->size();
	for (int i = 0; i < mesh->vertices->size(); i++)
	{
		vertices->push_back((*mesh->vertices)[i]);
	}
	for (int i = 0; i < mesh->edges->size(); i++)
	{
		Edge edge = (*mesh->edges)[i];
		edge.vertexIndices[0] += vertexCount;
		edge.vertexIndices[1] += vertexCount;
		edges->push_back(edge);
	}
	for (int i = 0; i < mesh->triangles->size(); i++)
	{
		Triangle triangle = (*mesh->triangles)[i];
		triangle.edgeIndices[0] += edgeCount;
		triangle.edgeIndices[1] += edgeCount;
		triangle.edgeIndices[2] += edgeCount;
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
	return (*vertices)[index];
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
	(*vertices)[index] = position;
}

- (void)setRotation:(Quaternion)rotation atIndex:(NSUInteger)index {}
- (void)setScale:(Vector3D)scale atIndex:(NSUInteger)index {}

- (void)moveByOffset:(Vector3D)offset atIndex:(NSUInteger)index
{
	(*vertices)[index] += offset;
}

- (void)rotateByOffset:(Quaternion)offset atIndex:(NSUInteger)index {}
- (void)scaleByOffset:(Vector3D)offset atIndex:(NSUInteger)index {}

- (BOOL)isSelectedAtIndex:(NSUInteger)index
{
	return (*selectedIndices)[index];
}

- (void)setSelected:(BOOL)selected atIndex:(NSUInteger)index 
{
	(*selectedIndices)[index] = selected;
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
