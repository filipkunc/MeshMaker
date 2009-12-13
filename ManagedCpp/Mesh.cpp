/*
 *  Mesh.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#include "Mesh.h"

namespace ManagedCpp
{
	Mesh::Mesh()
	{
		vertices = new vector<Vector3D>();
		triangles = new vector<Triangle>();
		edges = new vector<Edge>();
		selected = new vector<CocoaBool>();
		markedVertices = new vector<CocoaBool>();
		selectionMode = MeshSelectionMode::MeshSelectionModeVertices;

		int red = rand() % 50 + 150;
		int green = rand() % 50 + 150;
		int blue = rand() % 50 + 150;
		color = Color::FromArgb(red, green, blue); 
	}
	
	Mesh::~Mesh()
	{
		delete vertices;
		delete triangles;
		delete edges;
		delete selected;
		delete markedVertices;
	}

	MeshSelectionMode Mesh::SelectionMode::get()
	{
	   return selectionMode;
	}

	void Mesh::SelectionMode::set(MeshSelectionMode value)
	{
		selectionMode = value;
		selected->clear();
		switch (selectionMode) 
		{
			case MeshSelectionMode::MeshSelectionModeVertices:
			{
				for (uint i = 0; i < vertices->size(); i++)
				{
					selected->push_back(NO);
				}
			} break;
			case MeshSelectionMode::MeshSelectionModeTriangles:
			{
				for (uint i = 0; i < triangles->size(); i++)
				{
					selected->push_back(NO);
				}
			} break;
			case MeshSelectionMode::MeshSelectionModeEdges:
			{
				this->MakeEdges();
				for (uint i = 0; i < edges->size(); i++)
				{
					selected->push_back(NO);
				}
			} break;
		}		
	}

	uint Mesh::VertexCount::get()
	{
		return vertices->size();
	}

	uint Mesh::TriangleCount::get()
	{
		return triangles->size();
	}

	uint Mesh::EdgeCount::get()
	{
		return edges->size();
	}
	
	Vector3D Mesh::GetVertex(uint index)
	{
		return vertices->at(index);
	}

	Triangle Mesh::GetTriangle(uint index)
	{
		return triangles->at(index);
	}

	Edge Mesh::GetEdge(uint index)
	{
		return edges->at(index);
	}
	
	CocoaBool Mesh::IsVertexMarked(uint index)
	{
		return markedVertices->at(index);
	}
	
	void Mesh::SetVertexMarked(CocoaBool isMarked, uint index)
	{
		markedVertices->at(index) = isMarked;
	}
	
	void Mesh::AddVertex(Vector3D vertex)
	{
		vertices->push_back(vertex);
		if (selectionMode == MeshSelectionMode::MeshSelectionModeVertices)
			selected->push_back(NO);
	}
	
	void Mesh::AddTriangle(Triangle triangle)
	{
		triangles->push_back(triangle);
		if (selectionMode == MeshSelectionMode::MeshSelectionModeTriangles)
			selected->push_back(NO);
	}
	
	void Mesh::AddTriangle(uint index1, uint index2, uint index3)
	{
		Triangle triangle;
		triangle.vertexIndices[0] = index1;
		triangle.vertexIndices[1] = index2;
		triangle.vertexIndices[2] = index3;
		this->AddTriangle(triangle);
	}
	
	void Mesh::AddQuad(uint index1, uint index2, uint index3, uint index4)
	{
		Triangle triangle1, triangle2;
		triangle1.vertexIndices[0] = index1;
		triangle1.vertexIndices[1] = index2;
		triangle1.vertexIndices[2] = index3;
		
		triangle2.vertexIndices[0] = index1;
		triangle2.vertexIndices[1] = index3;
		triangle2.vertexIndices[2] = index4;
		
		this->AddTriangle(triangle1);
		this->AddTriangle(triangle2);
	}
	
	void Mesh::AddEdge(uint index1, uint index2)
	{
		Edge edge;
		edge.vertexIndices[0] = index1;
		edge.vertexIndices[1] = index2;
		edges->push_back(edge);
		
		if (selectionMode == MeshSelectionMode::MeshSelectionModeEdges)
			selected->push_back(NO);
	}

	void Mesh::DrawFast()
	{
		// experimental fast drawing
		
		// Flat lighting is missing due to complexity of generating normals.
		// Normal must be different for each triangle and distributed on all
		// three vertices of this triangle.
		// Problem is that vertices is shared, but normals not.
		// Another possibility is using shared normals and doing smooth shading.
		// Each normal will be then averaged on each vertex.
		
		glEnableClientState(GL_VERTEX_ARRAY);
		
		uint *trianglePtr = (uint *)(&(*triangles)[0]);
		float *vertexPtr = (float *)(&(*vertices)[0]);
		
		glVertexPointer(3, GL_FLOAT, 0, vertexPtr);
		
		glDrawElements(GL_TRIANGLES, triangles->size() * 3, GL_UNSIGNED_INT, trianglePtr);
		
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void Mesh::DrawFill(Vector3D scale)
	{
		float frontDiffuse[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
		float components[4] = { color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };
		float backDiffuse[4];
		float selectedDiffuse[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		
		for (uint i = 0; i < 4; i++)
			backDiffuse[i] = components[i];
		
		glMaterialfv(GL_BACK, GL_DIFFUSE, backDiffuse);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, frontDiffuse);
		
		Vector3D triangleVertices[3];
		
		float *lastDiffuse = frontDiffuse; 
		
		CocoaBool flip = scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f;
		
		glBegin(GL_TRIANGLES);
		
		for (uint i = 0; i < triangles->size(); i++)
		{
			if (selectionMode == MeshSelectionMode::MeshSelectionModeTriangles) 
			{
				if (selected->at(i))
				{
					if (lastDiffuse == frontDiffuse)
					{
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, selectedDiffuse);
						lastDiffuse = selectedDiffuse;
					}
				}
				else if (lastDiffuse == selectedDiffuse)
				{
					glMaterialfv(GL_BACK, GL_DIFFUSE, backDiffuse);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, frontDiffuse);
					lastDiffuse = frontDiffuse;
				}
			}
			Triangle currentTriangle = this->GetTriangle(i);
			if (flip)
				currentTriangle = ::FlipTriangle(currentTriangle);
				
			this->GetTriangleVertices(triangleVertices, currentTriangle);
			for (uint j = 0; j < 3; j++)
			{
				for (uint k = 0; k < 3; k++)
				{
					triangleVertices[j][k] *= scale[k];
				}
			}
			Vector3D n = NormalFromTriangleVertices(triangleVertices);
			n.Normalize();
			for (uint j = 0; j < 3; j++)
			{
				glNormal3f(n.x, n.y, n.z);
				glVertex3f(triangleVertices[j].x, triangleVertices[j].y, triangleVertices[j].z);			
			}
		}
		
		glEnd();
	}
	
	void Mesh::DrawWire(Vector3D scale, CocoaBool isSelected)
	{
		glDisable(GL_LIGHTING);
		if (isSelected)
			glColor3f(1, 1, 1);
		else
			glColor3f(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f); 
		if (selectionMode != MeshSelectionMode::MeshSelectionModeEdges)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			this->DrawFill(scale);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		glEnable(GL_LIGHTING);
	}
	
	void Mesh::Draw(Vector3D scale, CocoaBool isSelected)
	{
		if (isSelected)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);
			this->DrawFill(scale);
			glDisable(GL_POLYGON_OFFSET_FILL);
			this->DrawWire(scale, isSelected);
		}
		else
		{
			this->DrawFill(scale);
		}
	}

	void Mesh::MakeMesh(MeshType type, uint steps)
	{
		switch (type)
		{
		case MeshType::MeshTypeCube:
			MakeCube();
			break;
		case MeshType::MeshTypeCylinder:
			MakeCylinder(steps);
			break;
		case MeshType::MeshTypeSphere:
			MakeSphere(steps);
			break;
		default:
			Trace::WriteLine(String::Format("Unknown mesh type:{0}", type));
			break;
		}
	}
	
	void Mesh::MakeCube()
	{
		Trace::WriteLine("MakeCube");
		
		vertices->clear();
		triangles->clear();
		selected->clear();
		
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
		this->AddQuad(0, 1, 2, 3);
		
		// front triangles
		this->AddQuad(7, 6, 5, 4);
		
		// bottom triangles
		this->AddQuad(1, 0, 4, 5);
		
		// top triangles
		this->AddQuad(3, 2, 6, 7);
		
		// left triangles
		this->AddQuad(7, 4, 0, 3);
		
		// right triangles
		this->AddQuad(2, 1, 5, 6);

		SelectionMode = SelectionMode;
	}
	
	void Mesh::MakeCylinder(uint steps)
	{
		Trace::WriteLine(String::Format("MakeCylinder:{0}", steps));
		
		vertices->clear();
		triangles->clear();
		selected->clear();
		
		vertices->push_back(Vector3D(0, -1, 0)); // 0
		vertices->push_back(Vector3D(0,  1, 0)); // 1
		
		vertices->push_back(Vector3D(cosf(0.0f), -1, sinf(0.0f))); // 2
		vertices->push_back(Vector3D(cosf(0.0f),  1, sinf(0.0f))); // 3
		
		uint max = steps;
		float step = (FLOAT_PI * 2.0f) / max;
		float angle = step;
		for (uint i = 1; i < max; i++)
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
		
		SelectionMode = SelectionMode;
	}
	
	void Mesh::MakeSphere(uint steps)
	{
		Trace::WriteLine(String::Format("MakeSphere:{0}", steps));
		
		vertices->clear();
		triangles->clear();
		selected->clear();
		
		uint max = steps;
		
		vertices->push_back(Vector3D(0, 1, 0));
		vertices->push_back(Vector3D(0, -1, 0));
		
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
				
				vertices->push_back(Vector3D(x0, y0, z0));
				
				if (i > 0 && j < max - 1)
				{
					int index = (i - 1) * (max - 1);
					this->AddQuad(1 + max + j + index,
								  2 + j + index,
								  1 + j + index,
								  max + j + index);
				}
			}
			
			int index = i * (max - 1);
			if (i < max - 1)
			{
				this->AddTriangle(0,
								  2 + index + max - 1,
								  2 + index);

				this->AddTriangle(1,
								  index + max,
								  index + 2 * max - 1);
			}
			else 
			{
				this->AddTriangle(0,
								  2,
								  2 + index);
				
				this->AddTriangle(1,
								  index + max,
								  max);
			}
		}
		
		for (uint j = 1; j < max - 1; j++)
		{
			int index = (max - 1) * (max - 1);
			this->AddQuad(1 + j + index,
						  1 + j,
						  2 + j,
						  2 + j + index);
		}
		
		SelectionMode = SelectionMode;
	}

	void Mesh::MakeEdges()
	{
		edges->clear();
		for (uint i = 0; i < triangles->size(); i++)
		{
			Triangle triangle = triangles->at(i);
			Edge triangleEdges[3];
			CocoaBool addTriangleEdges[3];
			
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
		
		//NSLog(@"edgeCount:%i", [self edgeCount]);
	}
	
	void Mesh::MakeMarkedVertices()
	{
		Trace::WriteLine("MakeMarkedVertices");
	
		markedVertices->resize(vertices->size());
		for (uint i = 0; i < markedVertices->size(); i++)
		{
			markedVertices->at(i) = NO;
		}
		
		switch (selectionMode)
		{
			case MeshSelectionMode::MeshSelectionModeVertices:
			{
				for (uint i = 0; i < vertices->size(); i++)
				{
					if (selected->at(i))
						markedVertices->at(i) = YES;
				}
			} break;
			case MeshSelectionMode::MeshSelectionModeTriangles:
			{
				for (uint i = 0; i < triangles->size(); i++)
				{
					if (selected->at(i))
					{
						this->SetTriangleMarked(YES, i);
					}
				}
			} break;
			case MeshSelectionMode::MeshSelectionModeEdges:
			{
				for (uint i = 0; i < edges->size(); i++)
				{
					if (selected->at(i))
					{
						this->SetEdgeMarked(YES, i);
					}
				}
			} break;
			default:
				break;
		}
	}
	
	void Mesh::RemoveVertex(uint index)
	{
		for (uint i = 0; i < triangles->size(); i++)
		{
			for (uint j = 0; j < 3; j++)
			{
				if (triangles->at(i).vertexIndices[j] >= index)
					triangles->at(i).vertexIndices[j]--;
			}
		}
		vertices->erase(vertices->begin() + index);
		if (selectionMode == MeshSelectionMode::MeshSelectionModeVertices)
			selected->erase(selected->begin() + index);
	}
	
	void Mesh::RemoveTriangle(uint index)
	{
		triangles->erase(triangles->begin() + index);
		if (selectionMode == MeshSelectionMode::MeshSelectionModeTriangles)
			selected->erase(selected->begin() + index);
	}

	void Mesh::RemoveEdge(uint index)
	{
		edges->erase(edges->begin() + index);
		if (selectionMode == MeshSelectionMode::MeshSelectionModeEdges)
			selected->erase(selected->begin() + index);
	}
	
	void Mesh::RemoveDegeneratedTriangles()
	{
		Trace::WriteLine("RemoveDegeneratedTriangles");
		
		for (int i = 0; i < (int)triangles->size(); i++)
		{
			if (IsTriangleDegenerated(triangles->at(i)))
			{
				this->RemoveTriangle(i);
				i--;
			}
		}	
	}
	
	
	CocoaBool Mesh::IsVertexUsed(uint index)
	{
		for (uint i = 0; i < triangles->size(); i++)
		{
			Triangle triangle = triangles->at(i);
			for (uint j = 0; j < 3; j++)
			{
				if (triangle.vertexIndices[j] == index)
					return YES;
			}
		}
		return NO;
	}
	
	void Mesh::RemoveNonUsedVertices()
	{
		Trace::WriteLine("RemoveNonUsedVertices");
		
		for (int i = 0; i < (int)vertices->size(); i++)
		{
			if (!this->IsVertexUsed(i))
			{
				this->RemoveVertex(i);
				i--;
			}
		}
	}
	
	void Mesh::RemoveSelectedVertices()
	{
		Trace::WriteLine("RemoveSelectedVertices");
		
		Trace::Assert(vertices->size() == selected->size(), "vertices->size() == selected->size()");
		
		for (int i = 0; i < (int)selected->size(); i++)
		{
			if (selected->at(i))
			{
				this->RemoveVertex(i);
				i--;
			}
		}
	}
	
	void Mesh::FastMergeVertex(uint firstIndex, uint secondIndex)
	{
		Trace::WriteLine(String::Format("FastMergeVertex {0}, {1}", firstIndex, secondIndex));
		Trace::Assert(vertices->size() == selected->size(), "vertices->size() == selected->size()");
		
		Vector3D first = GetVertex(firstIndex);
		Vector3D second = GetVertex(secondIndex);
		Vector3D center = first + second;
		center /= 2;
		
		vertices->push_back(center);
		selected->push_back(NO);
		
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
			this->RemoveVertex(firstIndex);
			this->RemoveVertex(secondIndex);
		}
		else
		{
			this->RemoveVertex(secondIndex);
			this->RemoveVertex(firstIndex);
		}
		
		Trace::Assert(vertices->size() == selected->size(), "vertices->size() == selected->size()");
	}
	
	void Mesh::FastMergeSelectedVertices()
	{
		Trace::WriteLine("FastMergeSelectedVertices");
		Trace::Assert(vertices->size() == selected->size(), "vertices->size() == selected->size()");
		
		uint selectedCount = 0;
		Vector3D center = Vector3D();
		
		for (uint i = 0; i < selected->size(); i++)
		{
			if (selected->at(i))
			{
				selectedCount++;
				center += vertices->at(i);
			}
		}
		
		Trace::WriteLine(String::Format("selectedCount = {0}", selectedCount));
		
		if (selectedCount < 2)
			return;
		
		center /= (float)selectedCount;
		vertices->push_back(center);
		selected->push_back(NO);
		
		uint centerIndex = vertices->size() - 1;
		
		for (uint i = 0; i < selected->size(); i++)
		{
			if (selected->at(i))
			{
				for (uint j = 0; j < triangles->size(); j++)
				{
					for (uint k = 0; k < 3; k++)
					{
						if (triangles->at(j).vertexIndices[k] == i)
							triangles->at(j).vertexIndices[k] = centerIndex;
					}				
				}
			}
		}
		
		this->RemoveSelectedVertices();
	}
	
	void Mesh::MergeSelectedVertices()
	{
		Trace::WriteLine("MergeSelectedVertices");
		
		this->FastMergeSelectedVertices();
		
		this->RemoveDegeneratedTriangles();
		this->RemoveNonUsedVertices();
		
		Trace::Assert(vertices->size() == selected->size(), "vertices->size() == selected->size()");
	}

	void Mesh::MergeVertexPairs()
	{
		Trace::WriteLine("MergeVertexPairs");
		
		for (int i = 0; i < (int)selected->size(); i++)
		{
			if (selected->at(i))
			{
				Vector3D firstVertex = GetVertex(i);
				float smallestDistance = 10.0f; // maximum distance between vertices in pair
				int secondIndex = -1;
				for (int j = i + 1; j < (int)selected->size(); j++)
				{
					if (selected->at(j))
					{
						Vector3D secondVertex = GetVertex(j);
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
					this->FastMergeVertex(i, secondIndex);
					i--;
				}
			}
		}
		
		this->RemoveDegeneratedTriangles();
		this->RemoveNonUsedVertices();
		
		Trace::WriteLine(vertices->size() == selected->size(), "vertices->size() == selected->size()");
	}

	void Mesh::Transform(Matrix4x4 matrix)
	{
		for (uint i = 0; i < vertices->size(); i++)
			vertices->at(i).Transform(matrix);
	}

	void Mesh::Merge(Mesh ^mesh)
	{
		Trace::WriteLine("Merge");
	
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
			selected->push_back(NO);
	}
	
	void Mesh::GetTriangleVertices(Vector3D *triangleVertices, Triangle triangle)
	{
		for (uint i = 0; i < 3; i++)
		{
			triangleVertices[i] = GetVertex(triangle.vertexIndices[i]);
		}
	}
	
	void Mesh::SplitTriangle(uint index)
	{
		Trace::WriteLine(String::Format("SplitTriangle:{0}", index));
		
		Triangle triangle = GetTriangle(index);
		Vector3D triangleVertices[3];
		this->GetTriangleVertices(triangleVertices, triangle);
		
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
			
			this->GetTriangleVertices(triangleVertices, newTriangle);
			
			Vector3D newTriangleNormal = NormalFromTriangleVertices(triangleVertices);
			
			if (triangleNormal.Dot(newTriangleNormal) < 0)
				newTriangle = ::FlipTriangle(newTriangle);
			
			this->AddTriangle(newTriangle);
		}
		
		this->RemoveTriangle(index);
	}
	
	void Mesh::SplitEdge(uint index)
	{
		Trace::WriteLine(String::Format("SplitEdgeAtIndex:{0}", index));
		
		Edge edge = this->GetEdge(index);
		this->RemoveEdge(index);
		Vector3D firstVertex = this->GetVertex(edge.vertexIndices[0]);
		Vector3D secondVertex = this->GetVertex(edge.vertexIndices[1]);
		Vector3D centerVertex = firstVertex + secondVertex;
		centerVertex /= 2.0f;
		vertices->push_back(centerVertex);
		uint centerIndex = vertices->size() - 1;
		
		Vector3D triangleVertices[3];
		
		for (int i = 0; i < (int)triangles->size(); i++)
		{
			Triangle triangle = this->GetTriangle(i);
			if (IsEdgeInTriangle(triangle, edge))
			{
				uint oppositeIndex = NonEdgeIndexInTriangle(triangle, edge);
				
				this->RemoveTriangle(i);
				i--;
				
				this->AddEdge(centerIndex, oppositeIndex);
				
				this->GetTriangleVertices(triangleVertices, triangle);
				Vector3D splittedTriangleNormal = NormalFromTriangleVertices(triangleVertices);
				
				Triangle firstTriangle = MakeTriangle(edge.vertexIndices[0], oppositeIndex, centerIndex);
				this->GetTriangleVertices(triangleVertices, firstTriangle);
				Vector3D firstTriangleNormal = NormalFromTriangleVertices(triangleVertices);
				
				Triangle secondTriangle = MakeTriangle(edge.vertexIndices[1], oppositeIndex, centerIndex);
				this->GetTriangleVertices(triangleVertices, secondTriangle);
				Vector3D secondTriangleNormal = NormalFromTriangleVertices(triangleVertices);
				
				if (firstTriangleNormal.Dot(splittedTriangleNormal) < 0.0f)
					firstTriangle = ::FlipTriangle(firstTriangle);
					
				if (secondTriangleNormal.Dot(splittedTriangleNormal) < 0.0f)
					secondTriangle = ::FlipTriangle(secondTriangle);
				
				this->AddTriangle(firstTriangle);
				this->AddTriangle(secondTriangle);
			}
		}
		
		this->AddEdge(centerIndex, edge.vertexIndices[1]);
		this->AddEdge(centerIndex, edge.vertexIndices[0]);
	}

	void Mesh::SplitSelectedEdges()
	{
		Trace::WriteLine("SplitSelectedEdges");
		
		for (int i = 0; i < (int)selected->size(); i++)
		{
			if (selected->at(i))
			{
				this->SplitEdge(i);
				i--;
			}
		}
	}

	void Mesh::SplitSelectedTriangles()
	{
		Trace::WriteLine("SplitSelectedTriangles");
		
		for (int i = 0; i < (int)selected->size(); i++)
		{
			if (selected->at(i))
			{
				this->SplitTriangle(i);
				i--;
			}
		}
	}

	void Mesh::TurnEdge(uint index)
	{
		Trace::WriteLine(String::Format("TurnEdgeAtIndex:{0}", index));
		
		Edge edge = GetEdge(index);
		uint counter = 0;
		uint oldTriangleIndices[2];
		Triangle oldTriangles[2];
		
		for (uint i = 0; i < triangles->size(); i++)
		{
			Triangle triangle = GetTriangle(i);
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
					
					this->GetTriangleVertices(triangleVertices, oldTriangles[0]);
					Vector3D oldTriangleNormal1 = NormalFromTriangleVertices(triangleVertices);
					
					this->GetTriangleVertices(triangleVertices, oldTriangles[1]);
					Vector3D oldTriangleNormal2 = NormalFromTriangleVertices(triangleVertices);
					
					for (int j = 0; j < 2; j++)
					{
						Triangle newTriangle = MakeTriangle(edge.vertexIndices[j], 
															turned.vertexIndices[0], 
															turned.vertexIndices[1]);
						
						this->GetTriangleVertices(triangleVertices, newTriangle);
						Vector3D newTriangleNormal = NormalFromTriangleVertices(triangleVertices);
						
						// two dot products, it is working, but not in all cases
						if (newTriangleNormal.Dot(oldTriangleNormal1) < 0.0f ||
							newTriangleNormal.Dot(oldTriangleNormal2) < 0.0f)
						{
							newTriangle = ::FlipTriangle(newTriangle);
							Trace::WriteLine("opposite in turnEdgeAtIndex");
						}
						
						triangles->at(oldTriangleIndices[j]) = newTriangle;
					}
					
					edges->at(index) = turned;
					
					return;
				}
			}
		}
	}

	void Mesh::TurnSelectedEdges()
	{
		Trace::WriteLine("TurnSelectedEdges");
	
		for (uint i = 0; i < selected->size(); i++)
		{
			if (selected->at(i))
			{
				this->TurnEdge(i);
				
				// uncomment this line to deselect after edge turn
				//selected->at(i) = NO;
			}
		}
	}
	
	void Mesh::MergeSelected()
	{
		Trace::WriteLine("MergeSelected");
		
		switch (selectionMode)
		{
			case MeshSelectionMode::MeshSelectionModeVertices:
				this->MergeSelectedVertices();
				break;
			default:
				break;
		}
	}

	void Mesh::SplitSelected()
	{
		Trace::WriteLine("SplitSelected");
	
		switch (selectionMode)
		{
			case MeshSelectionMode::MeshSelectionModeEdges:
				this->SplitSelectedEdges();
				break;
			case MeshSelectionMode::MeshSelectionModeTriangles:
				this->SplitSelectedTriangles();
				break;
			default:
				break;
		}
	}

	#pragma region OpenGLManipulatingModel implementation

	uint Mesh::Count::get()
	{
		return selected->size();
	}

	void Mesh::WillSelect()
	{

	}

	void Mesh::DidSelect()
	{
		this->MakeMarkedVertices();
	}

	void Mesh::GetSelectionCenter(Vector3D *center, Quaternion *rotation, Vector3D *scale)
	{
		if (markedVertices->size() != vertices->size())
		{
			this->MakeMarkedVertices();
		}
		
		*center = Vector3D();
		*rotation = Quaternion();
		*scale = Vector3D(1, 1, 1);
		
		uint markedCount = 0;
		for (uint i = 0; i < markedVertices->size(); i++)
		{
			if (markedVertices->at(i))
			{
				*center += vertices->at(i);
				markedCount++;
			}
		}
		if (markedCount > 0)
			*center /= (float)markedCount;
			
		Trace::WriteLine(String::Format("markedCount = {0}", markedCount));
	}

	void Mesh::MoveSelectedBy(Vector3D offset)
	{
		if (markedVertices->size() != vertices->size())
		{
			this->MakeMarkedVertices();
		}
		
		for (uint i = 0; i < markedVertices->size(); i++)
		{
			if (markedVertices->at(i))
				vertices->at(i) += offset;
		}
	}

	void Mesh::RotateSelectedBy(Quaternion offset)
	{
		if (markedVertices->size() != vertices->size())
		{
			this->MakeMarkedVertices();
		}
		
		for (uint i = 0; i < markedVertices->size(); i++)
		{
			if (markedVertices->at(i))
				vertices->at(i).Transform(offset);
		}
	}

	void Mesh::ScaleSelectedBy(Vector3D offset)
	{
		if (markedVertices->size() != vertices->size())
		{
			this->MakeMarkedVertices();
		}
		
		for (uint i = 0; i < markedVertices->size(); i++)
		{
			if (markedVertices->at(i))
			{
				Vector3D &v = vertices->at(i);
				v.x *= offset.x;
				v.y *= offset.y;
				v.z *= offset.z;
			}
		}	
	}

	CocoaBool Mesh::IsSelected(uint index)
	{
		return selected->at(index);
	}

	void Mesh::SetEdgeMarked(CocoaBool isMarked, uint index)
	{
		Edge edge = GetEdge(index);
		markedVertices->at(edge.vertexIndices[0]) = isMarked;
		markedVertices->at(edge.vertexIndices[1]) = isMarked;
	}

	void Mesh::SetTriangleMarked(CocoaBool isMarked, uint index)
	{
		Triangle triangle = GetTriangle(index);
		markedVertices->at(triangle.vertexIndices[0]) = isMarked;
		markedVertices->at(triangle.vertexIndices[1]) = isMarked;
		markedVertices->at(triangle.vertexIndices[2]) = isMarked;
	}

	void Mesh::SetSelected(CocoaBool isSelected, uint index)
	{
		selected->at(index) = isSelected;
	}

	void Mesh::Draw(uint index, CocoaBool forSelection, ViewMode mode)
	{
		switch (selectionMode) 
		{
			case MeshSelectionMode::MeshSelectionModeVertices:
			{
				Vector3D v = GetVertex(index);
				if (!forSelection)
				{
					CocoaBool isSelected = selected->at(index);
					glPointSize(5.0f);
					if (isSelected)
						glColor3f(1, 0, 0);
					else
						glColor3f(0, 0, 1);
					glDisable(GL_LIGHTING);
				}
				glBegin(GL_POINTS);
				glVertex3f(v.x, v.y, v.z);
				glEnd();
			} break;
			case MeshSelectionMode::MeshSelectionModeTriangles:
			{
				if (forSelection)
				{
					Triangle currentTriangle = GetTriangle(index);
					glBegin(GL_TRIANGLES);
					for (uint i = 0; i < 3; i++)
					{
						Vector3D v = GetVertex(currentTriangle.vertexIndices[i]);
						glVertex3f(v.x, v.y, v.z);
					}
					glEnd();
				}
			} break;
			case MeshSelectionMode::MeshSelectionModeEdges:
			{
				Edge currentEdge = GetEdge(index);
				if (!forSelection)
				{
					CocoaBool isSelected = selected->at(index);
					if (isSelected)
						glColor3f(1, 0, 0);
						else
							glColor3f(1, 1, 1);
							glDisable(GL_LIGHTING);
							}
				glBegin(GL_LINES);
				for (uint i = 0; i < 2; i++)
				{
					Vector3D v = GetVertex(currentEdge.vertexIndices[i]);
					glVertex3f(v.x, v.y, v.z);
				}
				glEnd();
			} break;
		}
	}

	void Mesh::ExtrudeSelectedTriangles()
	{
		// This method finds all nonShared edges and copies all 
		// vertexIndices in selectedTriangles.
		// Then it makes quads between new and old edges.
		
		vector<uint> *vertexIndices = new vector<uint>();
		vector<Edge> *nonSharedEdges = new vector<Edge>();
		
		uint triCount = this->TriangleCount;
		uint vertCount = this->VertexCount;
		
		for (uint i = 0; i < triCount; i++)
		{
			if (selected->at(i))
			{
				this->SetTriangleMarked(NO, i);
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

					CocoaBool foundEdge = NO;
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
			this->AddQuad(
				edge.vertexIndices[0],
				vertexIndices->at(edge.vertexIndices[0] - vertCount),
				vertexIndices->at(edge.vertexIndices[1] - vertCount),
				edge.vertexIndices[1]);
		}
			
		delete vertexIndices;
		delete nonSharedEdges;
		
		this->RemoveNonUsedVertices(); // slow but sometimes neccessary
	}

	void Mesh::FlipSelected()
	{
		if (selectionMode == MeshSelectionMode::MeshSelectionModeTriangles)
		{
			this->FlipSelectedTriangles();
		}
		else if (selectionMode == MeshSelectionMode::MeshSelectionModeEdges)
		{
			this->TurnSelectedEdges();
		}
	}

	void Mesh::CloneSelected()
	{
		if (selectionMode == MeshSelectionMode::MeshSelectionModeTriangles)
		{
			this->ExtrudeSelectedTriangles();
		}
	}

	void Mesh::RemoveSelected()
	{
		if (selectionMode == MeshSelectionMode::MeshSelectionModeTriangles)
		{
			for (int i = 0; i < (int)triangles->size(); i++)
			{
				if (selected->at(i))
				{
					this->RemoveTriangle(i);
					i--;
				}
			}
			this->RemoveNonUsedVertices(); // still slow, but called once per selection
		}
	}

	void Mesh::FlipSelectedTriangles()
	{
		if (selectionMode == MeshSelectionMode::MeshSelectionModeTriangles)
		{	
			for (uint i = 0; i < triangles->size(); i++)
			{
				if (selected->at(i))
					this->FlipTriangle(i);
			}
		}
	}

	void Mesh::FlipAllTriangles()
	{
		for (uint i = 0; i < triangles->size(); i++)
		{
			this->FlipTriangle(i);
		}
	}

	void Mesh::FlipTriangle(uint index)
	{
		Triangle &triangle = triangles->at(index);
		triangle = ::FlipTriangle(triangle);
	}

	#pragma endregion
}