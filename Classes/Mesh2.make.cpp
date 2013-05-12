//
//  Mesh2.make.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  For license see LICENSE.TXT
//

#include "Mesh2.h"
#include <algorithm>

VertexNode *Mesh2::addVertex(const Vector3D &position)
{
    return _vertices.add(position);
}

TriangleNode *Mesh2::connectVerticesNearPosition(const Vector3D &position, const Vector3D &eyeVector)
{
    vector<VertexNode *> vertices;
    Vector3D center = Vector3D();
    
    for (uint i = 0; i < 4; i++)
    {
        VertexNode *nearestVertex = findNearestVertex(position, vertices);
        vertices.push_back(nearestVertex);
        center += nearestVertex->data().position;
    }
    
    center /= 4.0f;
    
    Vector3D v0_center = vertices[0]->data().position - center;
    uint oppositeIndex_v0 = UINT_MAX;
    float smallestAngle = 0.0f;
    
    for (uint i = 1; i < 4; i++)
    {
        Vector3D v0_vi = vertices[0]->data().position - vertices[i]->data().position;
        float angle = fabsf(v0_vi.GetAngle(v0_center));
        if (oppositeIndex_v0 == UINT_MAX || smallestAngle > angle)
        {
            smallestAngle = angle;
            oppositeIndex_v0 = i;
        }
    }
    
    TriangleNode *quad = NULL;
    
    switch (oppositeIndex_v0)
    {
        case 1:
            quad = addQuad(vertices[0], vertices[3], vertices[1], vertices[2]);
            break;
        case 3:
            quad = addQuad(vertices[0], vertices[1], vertices[3], vertices[2]);
            break;
        case 2:
            quad = addQuad(vertices[0], vertices[1], vertices[2], vertices[3]);
            break;
    }
    
    if (quad)
    {
        quad->data().computeNormalsIfNeeded();
        if (quad->data().vertexNormal.Dot(eyeVector) > 0.0f)
            quad->data().flip();
    }
    
    makeEdges();
    
    setSelectionMode(_selectionMode);
    
    return quad;
}

VertexNode *Mesh2::findNearestVertex(const Vector3D &position, const vector<VertexNode *> &skipVertices) const
{
    VertexNode *nearest = NULL;
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        if (find(skipVertices.begin(), skipVertices.end(), node) != skipVertices.end())
        {
            continue;
        }
        
        if (nearest == NULL ||
            node->data().position.Distance(position) < nearest->data().position.Distance(position))
        {
            nearest = node;
        }
    }
    
    return nearest;
}

TriangleNode *Mesh2::addTriangle(VertexNode *v0, VertexNode *v1, VertexNode *v2)
{
    TexCoordNode *t0 = _texCoords.add(v0->data().position);
    TexCoordNode *t1 = _texCoords.add(v1->data().position);
    TexCoordNode *t2 = _texCoords.add(v2->data().position);
    
    VertexNode *vertices[3] = { v0, v1, v2 };
    TexCoordNode *texCoords[3] = { t0, t1, t2 };
    
    return _triangles.add(Triangle2(vertices, texCoords));
}

TriangleNode *Mesh2::addQuad(VertexNode *v0, VertexNode *v1, VertexNode *v2, VertexNode *v3)
{
    TexCoordNode *t0 = _texCoords.add(v0->data().position);
    TexCoordNode *t1 = _texCoords.add(v1->data().position);
    TexCoordNode *t2 = _texCoords.add(v2->data().position);
    TexCoordNode *t3 = _texCoords.add(v3->data().position);
    
    VertexNode *vertices[4] = { v0, v1, v2, v3 };
    TexCoordNode *texCoords[4] = { t0, t1, t2, t3 };
    
  	return _triangles.add(Triangle2(vertices, texCoords, true));
}

void Mesh2::removeTriQuad(TriangleNode *&triQuad)
{
    _triangles.remove(triQuad);
}

void Mesh2::makeTexCoords()
{
    _texCoords.removeAll();
    
    for (VertexNode *vertex = _vertices.begin(), *end = _vertices.end(); vertex != end; vertex = vertex->next())
    {
        TexCoordNode *texCoord = _texCoords.add(vertex->data().position);
        
        for (VertexTriangleNode *triangle = vertex->_triangles.begin(),
             *triangleEnd = vertex->_triangles.end(); 
             triangle != triangleEnd; 
             triangle = triangle->next())
        {
            triangle->data()->data().setTexCoordByVertex(texCoord, vertex);
            texCoord->addTriangle(triangle->data());            
        }
    }
}

void Mesh2::makeEdges(TriangleNode *node)
{
    Triangle2 &triangle = node->data();
    triangle.removeEdges();
    
    for (uint i = 0; i < triangle.count(); i++)
    {
        uint j = i + 1;
        if (j == triangle.count())
            j = 0;
        
        VertexNode *vi = triangle.vertex(i);
        VertexNode *vj = triangle.vertex(j);
        
        TexCoordNode *ti = triangle.texCoord(i);
        TexCoordNode *tj = triangle.texCoord(j);
        
        VertexEdgeNode *vij = findOrCreateEdge(vi, vj, node);
        TexCoordEdgeNode *tij = findOrCreateEdge(ti, tj, node);
        
        triangle.setVertexEdge(i, vij);
        triangle.setTexCoordEdge(i, tij);
    }
}

void Mesh2::makeEdges()
{
    _vertexEdges.removeAll();
    _texCoordEdges.removeAll();
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        node->removeEdges();
    }
    
    for (TexCoordNode *node = _texCoords.begin(), *end = _texCoords.end(); node != end; node = node->next())
    {
        node->removeEdges();
    }
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        makeEdges(node);
    }
}

void Mesh2::makePlane()
{
    _vertices.removeAll();
    _triangles.removeAll();
    
    VertexNode *v0 = _vertices.add(Vector3D(-1, -1, 0));
	VertexNode *v1 = _vertices.add(Vector3D(-1,  1, 0));
    VertexNode *v2 = _vertices.add(Vector3D( 1,  1, 0));
	VertexNode *v3 = _vertices.add(Vector3D( 1, -1, 0));
    
    addQuad(v0, v1, v2, v3);
    
    makeTexCoords();
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::makeCube()
{
    _vertices.removeAll();
	_triangles.removeAll();
    _texCoords.removeAll();
    
	// back vertices
	VertexNode *v0 = _vertices.add(Vector3D(-1, -1, -1));
	VertexNode *v1 = _vertices.add(Vector3D( 1, -1, -1));
    VertexNode *v2 = _vertices.add(Vector3D( 1,  1, -1));
	VertexNode *v3 = _vertices.add(Vector3D(-1,  1, -1));
	
	// front vertices
	VertexNode *v4 = _vertices.add(Vector3D(-1, -1,  1));
	VertexNode *v5 = _vertices.add(Vector3D( 1, -1,  1));
	VertexNode *v6 = _vertices.add(Vector3D( 1,  1,  1));
	VertexNode *v7 = _vertices.add(Vector3D(-1,  1,  1));
    
	// back triangles
    addQuad(v0, v1, v2, v3);
	
	// front triangles
    addQuad(v7, v6, v5, v4);
	
	// bottom triangles
    addQuad(v1, v0, v4, v5);
	
	// top triangles
    addQuad(v3, v2, v6, v7);
	
	// left triangles
    addQuad(v7, v4, v0, v3);
	
	// right triangles
    addQuad(v2, v1, v5, v6);
    
    makeTexCoords();
    makeEdges();
	
    setSelectionMode(_selectionMode);
}

void Mesh2::makeCylinder(uint steps)
{
    _vertices.removeAll();
    _triangles.removeAll();
    
    VertexNode *node0 = _vertices.add(Vector3D(0, -1, 0)); // 0
    VertexNode *node1 = _vertices.add(Vector3D(0,  1, 0)); // 1
    
    VertexNode *node2 = _vertices.add(Vector3D(cosf(0.0f), -1, sinf(0.0f))); // 2
    VertexNode *node3 = _vertices.add(Vector3D(cosf(0.0f),  1, sinf(0.0f))); // 3
    
    uint max = steps;
    float step = (FLOAT_PI * 2.0f) / max;
    float angle = step;
    for (uint i = 1; i < max; i++)
    {
        VertexNode *last1 = _vertices.add(Vector3D(cosf(angle), -1, sinf(angle))); // 4
        VertexNode *last0 = _vertices.add(Vector3D(cosf(angle),  1, sinf(angle))); // 5
        
        VertexNode *last2 = last1->previous();
        VertexNode *last3 = last2->previous();
                
        addQuad(last2, last3, last1, last0);
        
        addTriangle(last3, node0, last1);
        addTriangle(last2, last0, node1);
        
        angle += step;
    }
    
    VertexNode *last0 = _vertices.last();
    VertexNode *last1 = last0->previous();
        
    addQuad(last1, node2, node3, last0);
    
    addTriangle(node0, node2, last1);
    addTriangle(node3, node1, last0);
    
    makeTexCoords();
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::makeSphere(uint steps)
{
    _vertices.removeAll();
    _triangles.removeAll();
    
    uint max = steps;
    
    vector<VertexNode *> tempVertices;
    vector<TriQuad> tempTriangles;
    
    tempVertices.push_back(_vertices.add(Vector3D(0, 1, 0)));
    tempVertices.push_back(_vertices.add(Vector3D(0, -1, 0)));
    
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
            
            tempVertices.push_back(_vertices.add(Vector3D(x0, y0, z0)));
            
            if (i > 0 && j < max - 1)
            {
                uint index = (i - 1) * (max - 1);
                
                AddQuad(tempTriangles, 
                        1 + max + j + index, 
                        2 + j + index,
                        1 + j + index,
                        max + j + index);                
            }
        }
        
        uint index = i * (max - 1);
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
        uint index = (max - 1) * (max - 1);
        AddQuad(tempTriangles,
                1 + j + index,
                1 + j,
                2 + j,
                2 + j + index);
    }
    
    VertexNode *triQuadVertices[4];
    
    for (uint i = 0; i < tempTriangles.size(); i++)
    {
        TriQuad indexTriangle = tempTriangles[i];
        if (indexTriangle.isQuad)
        {
            for (uint j = 0; j < 4; j++)
            {
                VertexNode *node = tempVertices.at(indexTriangle.vertexIndices[j]);
                triQuadVertices[j] = node;
            }
            addQuad(triQuadVertices[0], triQuadVertices[1], triQuadVertices[2], triQuadVertices[3]);
        }
        else
        {
            for (uint j = 0; j < 3; j++)
            {
                VertexNode *node = tempVertices.at(indexTriangle.vertexIndices[j]);
                triQuadVertices[j] = node;
            }
            addTriangle(triQuadVertices[0], triQuadVertices[1], triQuadVertices[2]);
        }
    }
    
    makeTexCoords();
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::makeIcosahedron()
{
    const float X = 0.525731112119133606f;
    const float Z = 0.850650808352039932f;
    
    static float vdata[12][3] =
    {    
        {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
        {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
        {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
    };
    
    static uint tindices[20][3] =
    { 
        {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
        {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
        {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
        {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} 
    };
    
    vector<VertexNode *> vertexNodes;
    
    for (uint i = 0; i < 12; i++)
    {
        vertexNodes.push_back(_vertices.add(Vector3D(vdata[i])));
    }
    
    for (uint i = 0; i < 20; i++)
    {
        VertexNode *triangleVertices[3];
        for (uint j = 0; j < 3; j++)
            triangleVertices[j] = vertexNodes[tindices[i][j]];
        _triangles.add(Triangle2(triangleVertices));
    }
    
    makeTexCoords();
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::fromVertices(const vector<Vector3D> &vertices)
{
    resetTriangleCache();
    _vertices.removeAll();
    _texCoords.removeAll();
    _triangles.removeAll();
    
    vector<VertexNode *> tempVertices;
    vector<VertexNode *> uniqueVertices;
    
    uint verticesSize = vertices.size();
    
    for (uint i = 0; i < verticesSize; i++)
    {
        bool found = false;
        
        for (uint j = 0; j < uniqueVertices.size(); j++)
        {
            if (uniqueVertices[j]->data().position.SqDistance(vertices[i]) < FLOAT_EPS)
            {
                VertexNode *last = tempVertices.size() > 0 ? tempVertices[tempVertices.size() - 1] : NULL;
                VertexNode *lastPrevious = tempVertices.size() > 1 ? tempVertices[tempVertices.size() - 2] : NULL;
                
                if (last == uniqueVertices[j] || lastPrevious == uniqueVertices[j])
                    break;
                
                tempVertices.push_back(uniqueVertices[j]);
                found = true;
                break;
            }            
        }
        
        if (!found)
        {
            VertexNode *newVertex = _vertices.add(vertices[i]);
            uniqueVertices.push_back(newVertex);
            tempVertices.push_back(newVertex);
        }        
    }
    
    VertexNode *triangleVertices[3];
    
    for (uint i = 0; i < verticesSize; i += 3)
    {
        for (uint j = 0; j < 3; j++)
        {
            triangleVertices[j] = tempVertices.at(i + j);
        }        
        _triangles.add(Triangle2(triangleVertices));
    }    
    
    makeTexCoords();
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::toVertices(vector<Vector3D> &vertices)
{
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        for (uint j = 0; j < 3; j++)
        {
            Vector3D vertex = node->data().vertex(j)->data().position;
            vertices.push_back(vertex);
        }        
    }
}

void Mesh2::fromIndexRepresentation(const vector<Vector3D> &vertices, const vector<Vector3D> &texCoords, const vector<TriQuad> &triangles)
{
    resetTriangleCache();
    _vertices.removeAll();
    _texCoords.removeAll();
    _triangles.removeAll();
    
    vector<VertexNode *> tempVertices;
    vector<TexCoordNode *> tempTexCoords;
    
    for (uint i = 0; i < vertices.size(); i++)
    {
        tempVertices.push_back(_vertices.add(vertices[i]));
    }
    
    for (uint i = 0; i < texCoords.size(); i++)
    {
        tempTexCoords.push_back(_texCoords.add(texCoords[i]));
    }
    
    VertexNode *triangleVertices[4];
    TexCoordNode *triangleTexCoords[4];
    
    for (uint i = 0; i < triangles.size(); i++)
    {
        const TriQuad &indexTriangle = triangles[i];
        if (indexTriangle.isQuad)
        {
            for (uint j = 0; j < 4; j++)
            {
                triangleVertices[j] = tempVertices.at(indexTriangle.vertexIndices[j]);
                triangleTexCoords[j] = tempTexCoords.at(indexTriangle.texCoordIndices[j]);
            }
            _triangles.add(Triangle2(triangleVertices, triangleTexCoords, true));
        }
        else
        {
            for (uint j = 0; j < 3; j++)
            {
                triangleVertices[j] = tempVertices.at(indexTriangle.vertexIndices[j]);
                triangleTexCoords[j] = tempTexCoords.at(indexTriangle.texCoordIndices[j]);
            }        
            _triangles.add(Triangle2(triangleVertices, triangleTexCoords, false));
        }
    }
    
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::toIndexRepresentation(vector<Vector3D> &vertices, vector<Vector3D> &texCoords, vector<TriQuad> &triangles) const
{
    uint index = 0;
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        node->algorithmData.index = index;
        index++;
        
        vertices.push_back(node->data().position);
    }
    
    index = 0;
    
    for (TexCoordNode *node = _texCoords.begin(), *end = _texCoords.end(); node != end; node = node->next())
    {
        node->algorithmData.index = index;
        index++;
        
        texCoords.push_back(node->data().position);
    }
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        TriQuad indexTriangle;
        indexTriangle.isQuad = node->data().isQuad();
        for (uint j = 0; j < node->data().count(); j++)
        {
            indexTriangle.vertexIndices[j] = node->data().vertex(j)->algorithmData.index;
            indexTriangle.texCoordIndices[j] = node->data().texCoord(j)->algorithmData.index;
        }        
        triangles.push_back(indexTriangle);
    }
}

void Mesh2::setSelection(const vector<bool> &selection)
{
    for (uint i = 0; i < selection.size(); i++)
    {
        if (!selection[i])
            setSelectedAtIndex(selection[i], i);
    }
    
    for (uint i = 0; i < selection.size(); i++)
    {
        if (selection[i])
            setSelectedAtIndex(selection[i], i);
    }
}

void Mesh2::getSelection(vector<bool> &selection) const
{
    selection.clear();
    for (uint i = 0; i < selectedCount(); i++)
        selection.push_back(isSelectedAtIndex(i));
}


void Mesh2::make(MeshType meshType, uint steps)
{
	switch (meshType) 
	{
        case MeshType::Plane:
            this->makePlane();
            break;
		case MeshType::Cube:
            this->makeCube();
			break;
		case MeshType::Cylinder:
            this->makeCylinder(steps);
			break;
		case MeshType::Sphere:
            this->makeSphere(steps);
			break;
        case MeshType::Icosahedron:
            this->makeIcosahedron();
            break;
		default:
			break;
	}
}

void Mesh2::fillMeshFromSelectedTriangles(Mesh2 &mesh)
{
    mesh._vertices.removeAll();
    mesh._texCoords.removeAll();
    mesh._triangles.removeAll();

    resetAlgorithmData();
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle2 &triangle = node->data();
        if (triangle.selected)
        {
            VertexNode *vertices[4];
            TexCoordNode *texCoords[4];
            for (uint i = 0; i < triangle.count(); i++)
            {
                VertexNode *vertex = triangle.vertex(i);
                TexCoordNode *texCoord = triangle.texCoord(i);
                
                if (vertex->algorithmData.duplicatePair == NULL)
                    vertex->algorithmData.duplicatePair = vertices[i] = mesh._vertices.add(vertex->data());
                else
                    vertices[i] = vertex->algorithmData.duplicatePair;
                
                if (texCoord->algorithmData.duplicatePair == NULL)
                    texCoord->algorithmData.duplicatePair = texCoords[i] = mesh._texCoords.add(texCoord->data());
                else
                    texCoords[i] = texCoord->algorithmData.duplicatePair;
            }
            mesh._triangles.add(Triangle2(vertices, texCoords, triangle.isQuad()));
        }
    }
    
    mesh.makeEdges();
    mesh.setSelectionMode(mesh._selectionMode);
}
