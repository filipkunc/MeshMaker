/*
 *  Mesh.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include "../PureCpp/MathCore/MathDeclaration.h"
#include "../PureCpp/Enums.h"
#include "../PureCpp/MeshHelpers.h"
using namespace std;

using namespace System;
using namespace System::Drawing;
using namespace System::Diagnostics;

#include "OpenGLManipulatingModel.h"

namespace ManagedCpp
{
	public ref class Mesh : OpenGLManipulatingModelMesh
	{
	private:
		vector<Vector3D> *vertices;
		vector<Triangle> *triangles;
		vector<Edge> *edges;
		vector<CocoaBool> *selected;
		vector<CocoaBool> *markedVertices;
		Color color;
		enum MeshSelectionMode selectionMode;
	public:
		Mesh();
		~Mesh();

		property MeshSelectionMode SelectionMode { MeshSelectionMode get(); void set(MeshSelectionMode value); }
		virtual property uint Count { uint get(); }
		property uint VertexCount { uint get(); }
		property uint TriangleCount { uint get(); }
		property uint EdgeCount { uint get(); }
		
		Vector3D GetVertex(uint index);
		Triangle GetTriangle(uint index);
		Edge GetEdge(uint index);
		CocoaBool IsVertexUsed(uint index);
		CocoaBool IsVertexMarked(uint index);
		void SetVertexMarked(CocoaBool isMarked, uint index);
		void SetEdgeMarked(CocoaBool isMarked, uint index);
		void SetTriangleMarked(CocoaBool isMarked, uint index);
		void AddVertex(Vector3D vertex);
		void AddTriangle(Triangle triangle);
		void AddTriangle(uint index1, uint index2, uint index3);
		void AddQuad(uint index1, uint index2, uint index3, uint index4);
		void AddEdge(uint index1, uint index2);
		void RemoveVertex(uint index);
		void RemoveTriangle(uint index);
		void RemoveEdge(uint index);
		void DrawFast();
		void DrawFill(Vector3D scale);
		void DrawWire(Vector3D scale, CocoaBool isSelected);
		void Draw(Vector3D scale, CocoaBool isSelected);
		virtual void Draw(uint index, CocoaBool forSelection, ViewMode mode);
		void MakeCube();
		void MakeCylinder(uint steps);
		void MakeSphere(uint steps);
		void MakeEdges();
		void MakeMarkedVertices();
		void RemoveNonUsedVertices();
		void RemoveDegeneratedTriangles();
		void RemoveSelectedVertices();
		void FastMergeVertex(uint firstIndex, uint secondIndex);
		void FastMergeSelectedVertices();
		void MergeSelectedVertices();
		void Transform(Matrix4x4 matrix);
		void Merge(Mesh ^mesh);
		void GetTriangleVertices(Vector3D *triangleVertices, Triangle triangle);
		void SplitTriangle(uint index);
		void SplitEdge(uint index);
		void SplitSelectedEdges();
		void SplitSelectedTriangles();
		void TurnEdge(uint index);
		void TurnSelectedEdges();
		void MergeVertexPairs();
		void MergeSelected();
		void SplitSelected();
		void FlipSelectedTriangles();
		void FlipAllTriangles();
		void FlipTriangle(uint index);
		virtual void WillSelect();
		virtual void DidSelect();
		virtual void GetSelectionCenter(Vector3D *center, Quaternion *rotation, Vector3D *scale);
		virtual void MoveSelectedBy(Vector3D offset);
		virtual void RotateSelectedBy(Quaternion offset);
		virtual void ScaleSelectedBy(Vector3D offset);
		virtual CocoaBool IsSelected(uint index);
		virtual void SetSelected(CocoaBool isSelected, uint index);
		virtual void CloneSelected();
		virtual void RemoveSelected();
	};
}