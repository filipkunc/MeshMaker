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
    _cachedVertices = NULL;
    _cachedNormals = NULL;
    _cachedColors = NULL;
    
    _selectionMode = MeshSelectionModeVertices;
    
    for (int i = 0; i < 4; i++)
        _colorComponents[i] = colorComponents[i];
}

Mesh2::~Mesh2()
{
    resetCache();
}

void Mesh2::setSelectionMode(MeshSelectionMode value)
{
    _selectionMode = value;
    _cachedVertexSelection.clear();
    _cachedTriangleSelection.clear();
    _cachedEdgeSelection.clear();
    
    switch (_selectionMode)
    {
        case MeshSelectionModeVertices:
        {
            for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
                _cachedVertexSelection.push_back(node);
        } break;
        case MeshSelectionModeTriangles:
        {
            for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
                _cachedTriangleSelection.push_back(node);
        } break;
        case MeshSelectionModeEdges:
        {
            for (EdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
                _cachedEdgeSelection.push_back(node);
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
            return _cachedVertexSelection.size();
        case MeshSelectionModeTriangles:
            return _cachedTriangleSelection.size();
        case MeshSelectionModeEdges:
            return _cachedEdgeSelection.size();
        default:
            return 0;
    }
}

bool Mesh2::isSelectedAtIndex(uint index) const
{
    switch (_selectionMode)
    {
        case MeshSelectionModeVertices:
            return _cachedVertexSelection.at(index)->data.selected;
        case MeshSelectionModeTriangles:
            return _cachedTriangleSelection.at(index)->data.selected;
        case MeshSelectionModeEdges:
            return _cachedEdgeSelection.at(index)->data.selected;
        default:
            return false;
    }
}

void Mesh2::setSelectedAtIndex(bool selected, uint index)
{
    switch (_selectionMode)
    {
        case MeshSelectionModeVertices:
            _cachedVertexSelection.at(index)->data.selected = selected;
            break;
        case MeshSelectionModeTriangles:
        {
            Triangle2 &triangle = _cachedTriangleSelection.at(index)->data;
            triangle.selected = selected;
            for (int i = 0; i < 3; i++)
                triangle.vertex(i)->data.selected = selected;            
        } break;
        case MeshSelectionModeEdges:
        {
            Edge2 &edge = _cachedEdgeSelection.at(index)->data;
            edge.selected = selected;
            for (int i = 0; i < 2; i++)
                edge.vertex(i)->data.selected = selected;
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
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
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
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        Vector3D &v = node->data.position;
        v = matrix.Transform(v);
    }
    
    setSelectionMode(_selectionMode);
}

void Mesh2::transformSelected(const Matrix4x4 &matrix)
{
    resetCache();
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
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
    
    SimpleList<VertexNode *> selectedNodes;
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        if (node->data.selected)
        {
            selectedNodes.add(node);
            center += node->data.position;
        }
    }
    
    if (selectedNodes.count() < 2)
        return;
    
    center /= (float)selectedNodes.count();
    
    VertexNode *centerNode = _vertices.add(center);
    
    for (SimpleNode<VertexNode *> *node = selectedNodes.begin(), *end = selectedNodes.end(); node != end; node = node->next())
    {
        node->data->replaceVertex(centerNode);        
    }    
}

void Mesh2::removeDegeneratedTrianglesAndEdges()
{
    removeDegeneratedTriangles();
    removeDegeneratedEdges();    
}

void Mesh2::removeDegeneratedTriangles()
{
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (node->data.isDegenerated())
            _triangles.remove(node);
    }
}

void Mesh2::removeDegeneratedEdges()
{
    for (EdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data.isDegenerated())
            _edges.remove(node);
    }
}

void Mesh2::removeNonUsedVertices()
{
    resetCache();
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        if (!node->isUsed())
            _vertices.remove(node);
    }
}

void Mesh2::mergeSelectedVertices()
{
    resetCache();
    
    fastMergeSelectedVertices();
    removeDegeneratedTrianglesAndEdges();
    removeNonUsedVertices();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::removeSelectedVertices()
{
    resetCache();
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        if (node->data.selected)
            _vertices.remove(node);
    }
    
    removeDegeneratedTrianglesAndEdges();
    removeNonUsedVertices();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::removeSelectedTriangles()
{
    resetCache();
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (node->data.selected)
            _triangles.remove(node);
    }
    
    removeDegeneratedEdges();
    removeNonUsedVertices();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::removeSelectedEdges()
{
    resetCache();
    
    for (EdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data.selected)
            _edges.remove(node);
    }
    
    removeDegeneratedTrianglesAndEdges();
    removeNonUsedVertices();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::removeSelected()
{
    switch (_selectionMode)
    {
        case MeshSelectionModeTriangles:
            removeSelectedTriangles();
            break;
        case MeshSelectionModeVertices:
            removeSelectedVertices();
            break;
        case MeshSelectionModeEdges:
            removeSelectedEdges();
            break;
        default:
            break;
    }    
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

void Mesh2::splitSelectedTriangles()
{
    
}

void Mesh2::splitSelectedEdges()
{
    
}

void Mesh2::splitSelected()
{
    switch (_selectionMode)
    {
        case MeshSelectionModeTriangles:
            splitSelectedTriangles();
            break;
        case MeshSelectionModeEdges:
            splitSelectedEdges();
            break;
        default:
            break;
    }
}

void Mesh2::flipSelected()
{
    
}

void Mesh2::extrudeSelectedTriangles()
{
    resetCache();
    
    // This method finds all nonShared edges and copies all 
    // vertexIndices in selectedTriangles.
    // Then it makes quads between new and old edges.
    
    for (EdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        Edge2 &edge = node->data;
        
        if (edge.isNotShared())
        {
            VertexNode *old0 = edge.vertex(0);
            VertexNode *old1 = edge.vertex(1);
            
            VertexNode *ex0 = _vertices.add(old0->data.position);
            //VertexNode *ex1 = _vertices->add(old1->data.position);
            
            TriangleNode *selectedTriangle;
            
            for (int i = 0; i < 2; i++)
            {
                selectedTriangle = edge.triangle(i);
                if (selectedTriangle != NULL && selectedTriangle->data.selected)
                    break;
            }
            
            if (selectedTriangle)
            {
                old0->removeTriangle(selectedTriangle);
                selectedTriangle->replaceVertex(old0, ex0);
            }
            
            addTriangle(old0, ex0, old1);
        }
    }
    
    /*vector<uint> *vertexIndices = new vector<uint>();
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
