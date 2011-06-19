//
//  Mesh2.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#include "Mesh2.h"

Mesh2::Mesh2(float colorComponents[4])
{
    _vertices = new FPList<VertexNode, Vertex2>();
    _triangles = new FPList<TriangleNode, Triangle2>();
    
    _cachedVertexSelection = new vector<VertexNode *>();
    _cachedTriangleSelection = new vector<TriangleNode *>();
    
    _cachedVertices = NULL;
    _cachedNormals = NULL;
    _cachedColors = NULL;
    
    _selectionMode = MeshSelectionModeVertices;
    
    for (int i = 0; i < 4; i++)
        _colorComponents[i] = colorComponents[i];
}

Mesh2::~Mesh2()
{
    delete _vertices;
    delete _triangles;
    
    delete _cachedVertexSelection;
    delete _cachedTriangleSelection;
    
    resetCache();
}

void Mesh2::addQuad(VertexNode *v1, VertexNode *v2, VertexNode *v3, VertexNode *v4)
{
    Triangle2 triangle1(v1, v2, v3);
    Triangle2 triangle2(v1, v3, v4);
  	_triangles->add(triangle1);
    _triangles->add(triangle2);
}

void Mesh2::setSelectionMode(MeshSelectionMode value)
{
    _selectionMode = value;
    _cachedVertexSelection->clear();
    _cachedTriangleSelection->clear();
    
    switch (_selectionMode)
    {
        case MeshSelectionModeVertices:
        {
            for (VertexNode *node = _vertices->begin(), *end = _vertices->end(); node != end; node = node->next())
                _cachedVertexSelection->push_back(node);
        } break;
        case MeshSelectionModeTriangles:
        {
            for (TriangleNode *node = _triangles->begin(), *end = _triangles->end(); node != end; node = node->next())
                _cachedTriangleSelection->push_back(node);
        } break;            
        default:
            break;
    }
}

uint Mesh2::selectedCount() const
{
    switch (_selectionMode)
    {
        case MeshSelectionModeVertices:
            return _cachedVertexSelection->size();
        case MeshSelectionModeTriangles:
            return _cachedTriangleSelection->size();
        default:
            return 0;
    }
}

bool Mesh2::isSelectedAtIndex(uint index) const
{
    switch (_selectionMode)
    {
        case MeshSelectionModeVertices:
            return _cachedVertexSelection->at(index)->data.selected;
        case MeshSelectionModeTriangles:
            return _cachedTriangleSelection->at(index)->data.selected;
        default:
            return false;
    }
}

void Mesh2::setSelectedAtIndex(bool selected, uint index)
{
    switch (_selectionMode)
    {
        case MeshSelectionModeVertices:
            _cachedVertexSelection->at(index)->data.selected = selected;
            break;
        case MeshSelectionModeTriangles:
        {
            Triangle2 &triangle = _cachedTriangleSelection->at(index)->data;
            triangle.selected = selected;
            for (uint i = 0; i < 3; i++)
                triangle[i]->data.selected = selected;            
        } break;
        default:
            break;
    }
}

void Mesh2::getSelectionCenterRotationScale(Vector3D &center, Quaternion &rotation, Vector3D &scale)
{
    center = Vector3D();
	rotation = Quaternion();
	scale = Vector3D(1, 1, 1);
    
	uint selectedCount = 0;
    
    for (VertexNode *node = _vertices->begin(), *end = _vertices->end(); node != end; node = node->next())
    {
        if (node->data.selected)
        {
            center += node->data.position;
            selectedCount++;
        }
    }
	if (selectedCount > 0)
		center /= (float)selectedCount;
}

void Mesh2::transformAll(const Matrix4x4 &matrix)
{
    resetCache();
     
     for (VertexNode *node = _vertices->begin(), *end = _vertices->end(); node != end; node = node->next())
     {
         Vector3D &v = node->data.position;
         v = matrix.Transform(v);
     }
     
    setSelectionMode(_selectionMode);
}

void Mesh2::transformSelected(const Matrix4x4 &matrix)
{
    resetCache();
    
    for (VertexNode *node = _vertices->begin(), *end = _vertices->end(); node != end; node = node->next())
    {
        if (node->data.selected)
        {
            Vector3D &v = node->data.position;
            v = matrix.Transform(v);
        }
    }
}

void Mesh2::fastMergeSelectedVertices()
{
    Vector3D center = Vector3D();
    
    SimpleList<VertexNode *> *selectedNodes = new SimpleList<VertexNode *>();
    
    for (VertexNode *node = _vertices->begin(), *end = _vertices->end(); node != end; node = node->next())
    {
        if (node->data.selected)
        {
            selectedNodes->add(node);
            center += node->data.position;
        }
    }
    
    if (selectedNodes->count() < 2)
    {
        delete selectedNodes;
        return;
    }
    
    center /= (float)selectedNodes->count();
    
    VertexNode *centerNode = _vertices->add(center);
    
    for (SimpleNode<VertexNode *> *node = selectedNodes->begin(), *end = selectedNodes->end(); node != end; node = node->next())
    {
        node->data->ReplaceVertex(centerNode);        
    }
    
    delete selectedNodes;
}

void Mesh2::removeDegeneratedTriangles()
{
    resetCache();
    
    for (TriangleNode *node = _triangles->begin(), *end = _triangles->end(); node != end; node = node->next())
    {
        if (node->data.IsDegenerated())
            _triangles->remove(node);
    }
}

void Mesh2::removeNonUsedVertices()
{
    resetCache();
    
    for (VertexNode *node = _vertices->begin(), *end = _vertices->end(); node != end; node = node->next())
    {
        if (!node->IsUsed())
            _vertices->remove(node);
    }
}

void Mesh2::mergeSelectedVertices()
{
    resetCache();
    
    fastMergeSelectedVertices();
    removeDegeneratedTriangles();
    removeNonUsedVertices();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::mergeSelected()
{
    switch (_selectionMode)
	{
		case MeshSelectionModeVertices:
			mergeSelectedVertices();
			break;
		default:
			break;
	}
}
