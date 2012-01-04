//
//  Mesh2.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#include "Mesh2.h"

bool Mesh2::_useSoftSelection = false;
bool Mesh2::_selectThrough = false;
bool Mesh2::_isUnwrapped = false;

Mesh2::Mesh2(float colorComponents[4])
{
    _selectionMode = MeshSelectionModeVertices;
    
    for (int i = 0; i < 4; i++)
        _colorComponents[i] = colorComponents[i];
    
    _vboID = 0U;
    _vboGenerated = false;
}

Mesh2::~Mesh2()
{
    resetTriangleCache();
}

void Mesh2::setSelectionMode(MeshSelectionMode value)
{
    resetEdgeCache();
    
    _selectionMode = value;
    _cachedVertexSelection.clear();
    _cachedTriangleSelection.clear();
    _cachedEdgeSelection.clear();
    _cachedTextureCoordinateSelection.clear();
    
    switch (_selectionMode)
    {
        case MeshSelectionModeVertices:
        {
            for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
                _cachedVertexSelection.push_back(node);
            
            for (TextureCoordinateNode *node = _textureCoordinates.begin(), *end = _textureCoordinates.end(); node != end; node = node->next())
                _cachedTextureCoordinateSelection.push_back(node);            
        } break;
        case MeshSelectionModeTriangles:
        {
            for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
                node->data.selected = false;
            
            for (TextureCoordinateNode *node = _textureCoordinates.begin(), *end = _textureCoordinates.end(); node != end; node = node->next())
                node->data.selected = false;
            
            for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
            {
                _cachedTriangleSelection.push_back(node);
                
                Triangle2 &triangle = node->data;
                if (triangle.selected)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        triangle.vertex(i)->data.selected = true;
                        triangle.texCoord(i)->data.selected = true;
                    }
                }
            }
        } break;
        case MeshSelectionModeEdges:
        {
            for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
                node->data.selected = false;
            
            for (TextureCoordinateNode *node = _textureCoordinates.begin(), *end = _textureCoordinates.end(); node != end; node = node->next())
                node->data.selected = false;
            
            for (EdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
            {
                _cachedEdgeSelection.push_back(node);
                
                Edge2 &edge = node->data;
                if (edge.selected)
                {
                    for (int i = 0; i < 2; i++)
                    {
                        edge.vertex(i)->data.selected = true;
                        edge.texCoord(i)->data.selected = true;
                    }
                }
            }
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
            if (_isUnwrapped)
                return _cachedTextureCoordinateSelection.size();
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
            if (_isUnwrapped)
                return _cachedTextureCoordinateSelection.at(index)->data.selected;
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
            if (_isUnwrapped)
                _cachedTextureCoordinateSelection.at(index)->data.selected = selected;
            else
                _cachedVertexSelection.at(index)->data.selected = selected;
            break;
        case MeshSelectionModeTriangles:
        {
            Triangle2 &triangle = _cachedTriangleSelection.at(index)->data;
            triangle.selected = selected;
            for (int i = 0; i < 3; i++)
            {
                triangle.vertex(i)->data.selected = selected;
                triangle.texCoord(i)->data.selected = selected;
            }                
        } break;
        case MeshSelectionModeEdges:
        {
            Edge2 &edge = _cachedEdgeSelection.at(index)->data;
            edge.selected = selected;
            for (int i = 0; i < 2; i++)
            {
                edge.vertex(i)->data.selected = selected;
                edge.texCoord(i)->data.selected = selected;
            }
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
    
    if (_isUnwrapped)
    {
        for (TextureCoordinateNode *node = _textureCoordinates.begin(), *end = _textureCoordinates.end(); node != end; node = node->next())
        {
            if (node->data.selected)
            {
                center.x += node->data.position.x;
                center.y += node->data.position.y;
                selectedCount++;
            }
        }
    }
    else
    {
        for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
        {
            if (node->data.selected)
            {
                center += node->data.position;
                selectedCount++;
            }
        }        
    }
	if (selectedCount > 0)
		center /= (float)selectedCount;
}

void Mesh2::transformAll(const Matrix4x4 &matrix)
{
    resetTriangleCache();
    
    if (_isUnwrapped)
    {
        for (TextureCoordinateNode *node = _textureCoordinates.begin(), *end = _textureCoordinates.end(); node != end; node = node->next())
        {
            Vector3D v = Vector3D(node->data.position.x, node->data.position.y, 0.0f);
            v = matrix.Transform(v);
            node->data.position = Vector2D(v.x, v.y);
        }
    }
    else
    {
        for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
        {
            Vector3D &v = node->data.position;
            v = matrix.Transform(v);
        }
    }
    
    setSelectionMode(_selectionMode);
}

void Mesh2::transformSelected(const Matrix4x4 &matrix)
{
    resetTriangleCache();
    
    if (_isUnwrapped)
    {
        for (TextureCoordinateNode *node = _textureCoordinates.begin(), *end = _textureCoordinates.end(); node != end; node = node->next())
        {
            if (node->data.selected)
            {
                Vector3D v = Vector3D(node->data.position.x, node->data.position.y, 0.0f);
                v = matrix.Transform(v);
                node->data.position = Vector2D(v.x, v.y);
            }
        }
    }
    else
    {
        for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
        {
            if (node->data.selected)
            {
                Vector3D &v = node->data.position;
                v = matrix.Transform(v);
            }
            else if (_useSoftSelection && node->selectionWeight > 0.1f)
            {
                Vector3D &v = node->data.position;
                v = v.Lerp(node->selectionWeight, matrix.Transform(v));            
            }
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
    resetTriangleCache();
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        if (!node->isUsed())
            _vertices.remove(node);
    }
}

void Mesh2::mergeSelectedVertices()
{
    resetTriangleCache();
    
    fastMergeSelectedVertices();
    removeDegeneratedTrianglesAndEdges();
    removeNonUsedVertices();
    
    makeEdges(); // TODO: Boundary edges causes problems?
    
    setSelectionMode(_selectionMode);
}

void Mesh2::removeSelectedVertices()
{
    resetTriangleCache();
    
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
    resetTriangleCache();
    
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
    resetTriangleCache();
    
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

void Mesh2::loopSubdivision()
{
    resetTriangleCache();
    
    VertexNode *vertices[6];
    
    FPList<TriangleNode, Triangle2> subdivided;
    
    int vertexCount = _vertices.count();
    
    for (EdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        Vector3D v1 = node->data.vertex(0)->data.position;
        Vector3D v2 = node->data.vertex(1)->data.position;
        
        Vector3D edgeVertex;
        
        // boundary
        if (node->data.triangle(0) == NULL || node->data.triangle(1) == NULL)
        {
            edgeVertex = (v1 + v2) / 2.0f;
        }
        else
        {        
            Vector3D v3 = node->data.triangle(0)->data.vertexNotInEdge(&node->data)->data.position;
            Vector3D v4 = node->data.triangle(1)->data.vertexNotInEdge(&node->data)->data.position;
            
            edgeVertex = 3.0f * (v1 + v2) / 8.0f + 1.0f * (v3 + v4) / 8.0f;           
        }
        
        node->data.halfVertex = _vertices.add(edgeVertex);
    }
    
    vector<Vector3D> tempVertices;
    
    int index = 0;
    
    for (VertexNode *node = _vertices.begin(); index < vertexCount; node = node->next())
    {
        node->index = index;
        index++;
        
        float beta, n;
        
        n = (float)node->_edges.count();
        beta = 3.0f + 2.0f * cosf(FLOAT_PI * 2.0f / n);
        beta = 5.0f / 8.0f - (beta * beta) / 64.0f;
        
        Vector3D finalPosition = (1.0f - beta) * node->data.position;
        
        float bon = beta / n;
        
        for (SimpleNode<EdgeNode *> *edgeNode = node->_edges.begin(), *endEdgeNode = node->_edges.end(); edgeNode != endEdgeNode; edgeNode = edgeNode->next())
        {
            Edge2 &edge = edgeNode->data->data;
            VertexNode *oppositeVertex = edge.opposite(node);
            
            finalPosition += oppositeVertex->data.position * bon;
        }
        
        tempVertices.push_back(finalPosition);
    }
    
    index = 0;
    
    for (VertexNode *node = _vertices.begin(); index < vertexCount; node = node->next())
    {
        node->data.position = tempVertices[node->index];
        index++;
    }
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        for (int j = 0; j < 3; j++)
        {
            vertices[j] = node->data.vertex(j);
            vertices[j + 3] = node->data.edge(j)->data.halfVertex;
        }
        
        /*    
               2
              /\
             /  \
          *5/____\*4
           /\    /\
          /  \  /  \
         /____\/____\
         0    *3     1
         
        */
        
        subdivided.add((VertexNode *[3]){ vertices[0], vertices[3], vertices[5] });
        subdivided.add((VertexNode *[3]){ vertices[3], vertices[1], vertices[4] });
        subdivided.add((VertexNode *[3]){ vertices[5], vertices[4], vertices[2] });
        subdivided.add((VertexNode *[3]){ vertices[3], vertices[4], vertices[5] });
    }
    
    _triangles.moveFrom(subdivided);
    
    makeEdges();
    
    setSelectionMode(_selectionMode);    
}

void Mesh2::splitSelectedTriangles()
{
    resetTriangleCache();
    
    VertexNode *vertices[6];
    
    FPList<TriangleNode, Triangle2> subdivided;
    
    for (EdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        node->data.halfVertex = NULL;
    }
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (!node->data.selected)
        {
            subdivided.add(node->data);
            continue;
        }
        
        for (int j = 0; j < 3; j++)
        {
            Edge2 &edge = node->data.edge(j)->data;
            
            if (edge.halfVertex == NULL)
            {
                Vector3D v1 = edge.vertex(0)->data.position;
                Vector3D v2 = edge.vertex(1)->data.position;
                
                Vector3D edgeVertex = (v1 + v2) / 2.0f;
                
                edge.halfVertex = _vertices.add(edgeVertex);
            }
            
            vertices[j] = node->data.vertex(j);
            vertices[j + 3] = edge.halfVertex;
        }
        
        /*    
               2
              /\
             /  \
          *5/____\*4
           /\    /\
          /  \  /  \
         /____\/____\
         0    *3     1
         
        */
        
        subdivided.add((VertexNode *[3]){ vertices[0], vertices[3], vertices[5] });
        subdivided.add((VertexNode *[3]){ vertices[3], vertices[1], vertices[4] });
        subdivided.add((VertexNode *[3]){ vertices[5], vertices[4], vertices[2] });
        subdivided.add((VertexNode *[3]){ vertices[3], vertices[4], vertices[5] });
    }
    
    _triangles.moveFrom(subdivided);
    
    makeEdges();
    
    setSelectionMode(_selectionMode);

}

void Mesh2::splitSelectedEdges()
{
    for (EdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        Edge2 &edge = node->data;
        if (!edge.selected)
            continue;
        
        if (edge.halfVertex == NULL)
        {
            Vector3D v1 = edge.vertex(0)->data.position;
            Vector3D v2 = edge.vertex(1)->data.position;
            
            Vector3D edgeVertex = (v1 + v2) / 2.0f;
            
            edge.halfVertex = _vertices.add(edgeVertex);
        }
        
        for (int i = 0; i < 2; i++)
        {
            TriangleNode *triangleNode = edge.triangle(i);
            if (triangleNode == NULL)
                continue;
            
            VertexNode *oppositeVertexNode = triangleNode->data.vertexNotInEdge(&edge);
            
            VertexNode *original0 = edge.vertex(0);
            VertexNode *original1 = edge.vertex(1);
            
            triangleNode->data.sortVertices(original0, original1);
            
            addTriangle(edge.halfVertex, oppositeVertexNode, original0);
            addTriangle(original1, oppositeVertexNode, edge.halfVertex);
            
            _triangles.remove(triangleNode);
        }
    }
    
    makeEdges();
    
    setSelectionMode(_selectionMode);
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

void Mesh2::flipSelectedTriangles()
{
    resetTriangleCache();
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (node->data.selected)
            node->data.flip();
    }
}

void Mesh2::turnSelectedEdges()
{
    resetTriangleCache();
    
    for (EdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data.selected)
            node->data.turn();
    }
    
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::flipSelected()
{
    switch (_selectionMode) 
    {
        case MeshSelectionModeTriangles:
            flipSelectedTriangles();
            break;
        case MeshSelectionModeEdges:
            turnSelectedEdges();
            break;
        default:
            break;
    }
}

void Mesh2::flipAllTriangles()
{
    resetTriangleCache();
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        node->data.flip();
    }
}

void Mesh2::extrudeSelectedTriangles()
{
    resetTriangleCache();
    
    vector<ExtrudePair> extrudePairs;
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (!node->data.selected)
            continue;
        
        for (int i = 0; i < 3; i++)
        {
            Edge2 &edge = node->data.edge(i)->data;
            
            if (edge.isNotShared())
            {
                VertexNode *original0 = edge.vertex(0);
                VertexNode *original1 = edge.vertex(1);
                
                node->data.sortVertices(original0, original1);
                
                VertexNode *extruded0 = findOrCreateVertex(extrudePairs, original0);
                VertexNode *extruded1 = findOrCreateVertex(extrudePairs, original1);
                
                addQuad(original0, original1, extruded1, extruded0);
            }
        }
    }
    
    for (size_t i = 0; i < extrudePairs.size(); i++)
    {
        extrudePairs[i].original->replaceVertexInSelectedTriangles(extrudePairs[i].extruded);
    }
    
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::merge(Mesh2 *mesh)
{
    vector<Vector3D> thisVertices;
    vector<Triangle> thisTriangles;
    
    this->toIndexRepresentation(thisVertices, thisTriangles);
    
    vector<Vector3D> otherVertices;
    vector<Triangle> otherTriangles;
    
    mesh->toIndexRepresentation(otherVertices, otherTriangles);
    
    vector<Vector3D> mergedVertices;
    vector<Triangle> mergedTriangles;
    
    for (uint i = 0; i < thisVertices.size(); i++)
        mergedVertices.push_back(thisVertices[i]);
    
    for (uint i = 0; i < otherVertices.size(); i++)
        mergedVertices.push_back(otherVertices[i]);
    
    for (uint i = 0; i < thisTriangles.size(); i++)
        mergedTriangles.push_back(thisTriangles[i]);
    
    for (uint i = 0; i < otherTriangles.size(); i++)
    {
        Triangle triangle = otherTriangles[i];
        triangle.vertexIndices[0] += thisVertices.size();
        triangle.vertexIndices[1] += thisVertices.size();
        triangle.vertexIndices[2] += thisVertices.size();
        mergedTriangles.push_back(triangle);
    }
    
    this->fromIndexRepresentation(mergedVertices, mergedTriangles);     
}

void Mesh2::computeSoftSelection()
{
    if (!_useSoftSelection)
        return;
    
    Vector3D center;
    Quaternion rotation;
    Vector3D scale;
    
    getSelectionCenterRotationScale(center, rotation, scale);
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        if (!node->data.selected)
        {
            float sqDistance = center.SqDistance(node->data.position);
            
            node->selectionWeight = 1.0f / expf(sqDistance);
            
            if (node->selectionWeight < 0.1f)
                node->selectionWeight = 0.0f;

            if (node->selectionWeight > 1.0f)
                node->selectionWeight = 1.0f;
        }
        else
        {
            node->selectionWeight = 1.0f;
        }
    }
}
