//
//  Mesh2.make.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#include "Mesh2.h"

void Mesh2::addTriangle(VertexNode *v1, VertexNode *v2, VertexNode *v3)
{
    VertexNode *vertices[3] = { v1, v2, v3 };
    _triangles.add(vertices);
}

void Mesh2::addQuad(VertexNode *v1, VertexNode *v2, VertexNode *v3, VertexNode *v4)
{
    TextureCoordinateNode *t1 = _textureCoordinates.add(Vector2D(0, 0));
    TextureCoordinateNode *t2 = _textureCoordinates.add(Vector2D(0, 1));
    TextureCoordinateNode *t3 = _textureCoordinates.add(Vector2D(1, 1));
    TextureCoordinateNode *t4 = _textureCoordinates.add(Vector2D(1, 0));
    
    VertexNode *vertices1[3] = { v1, v2, v3 };
    VertexNode *vertices2[3] = { v1, v3, v4 };
    
    TextureCoordinateNode *textureCoordinates1[3] = { t1, t2, t3 };
    TextureCoordinateNode *textureCoordinates2[3] = { t1, t3, t4 };
    
  	_triangles.add(Triangle2(vertices1, textureCoordinates1));
    _triangles.add(Triangle2(vertices2, textureCoordinates2));
}

EdgeNode *Mesh2::findOrCreateEdge(VertexNode *v1, VertexNode *v2, TriangleNode *triangle)
{
    EdgeNode *sharedEdge = v1->sharedEdge(v2);
    if (sharedEdge)
    {
        sharedEdge->data.setTriangle(1, triangle);
        return sharedEdge;
    }
    
    VertexNode *vertices[2] = { v1, v2 };
    EdgeNode *node = _edges.add(vertices);
    node->data.setTriangle(0, triangle);
    return node;
}

VertexNode *Mesh2::findOrCreateVertex(vector<ExtrudePair> &extrudePairs, VertexNode *original)
{
    for (int i = 0; i < (int)extrudePairs.size(); i++)
    {
        if (extrudePairs[i].original == original)
            return extrudePairs[i].extruded;
    }
    
    VertexNode *extruded = _vertices.add(original->data.position);
    ExtrudePair extrudePair = { original, extruded };
    extrudePairs.push_back(extrudePair);
    
    return extruded;
}

void Mesh2::makeEdges()
{
    _edges.removeAll();
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        node->removeEdges();
    }
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle2 &triangle = node->data;
        
        triangle.removeEdges();
        
        VertexNode *v0 = triangle.vertex(0);
        VertexNode *v1 = triangle.vertex(1);
        VertexNode *v2 = triangle.vertex(2);
        
        EdgeNode *e0 = findOrCreateEdge(v0, v1, node);
        EdgeNode *e1 = findOrCreateEdge(v1, v2, node);
        EdgeNode *e2 = findOrCreateEdge(v2, v0, node);
        
        triangle.setEdge(0, e0);
        triangle.setEdge(1, e1);
        triangle.setEdge(2, e2);        
    }
}

void Mesh2::makePlane()
{
    _vertices.removeAll();
    _triangles.removeAll();
    
    VertexNode *v0 = _vertices.add(Vector3D(-1, -1, 0));
	VertexNode *v1 = _vertices.add(Vector3D( 1, -1, 0));
    VertexNode *v2 = _vertices.add(Vector3D( 1,  1, 0));
	VertexNode *v3 = _vertices.add(Vector3D(-1,  1, 0));
    
    addQuad(v0, v1, v2, v3);
    
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::makeCube()
{
    _vertices.removeAll();
	_triangles.removeAll();
    _textureCoordinates.removeAll();
    
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
        VertexNode *last2 = _vertices.add(Vector3D(cosf(angle), -1, sinf(angle))); // 4
        VertexNode *last1 = _vertices.add(Vector3D(cosf(angle),  1, sinf(angle))); // 5
        
        VertexNode *last3 = last2->previous();
        VertexNode *last4 = last3->previous();
        
        addTriangle(last3, last2, last1);
        addTriangle(last2, last3, last4);
        
        addTriangle(last4, node0, last2);
        addTriangle(last3, last1, node1);
        
        angle += step;
    }
    
    VertexNode *last1 = _vertices.last();
    VertexNode *last2 = last1->previous();
    
    addTriangle(node2, node3, last1);
    addTriangle(last1, last2, node2);
    
    addTriangle(node0, node2, last2);
    addTriangle(node3, node1, last1);
    
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::makeSphere(uint steps)
{
    _vertices.removeAll();
    _triangles.removeAll();
    
    uint max = steps;
    
    vector<VertexNode *> tempVertices;
    vector<Triangle> tempTriangles;
    
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
        _triangles.add(triangleVertices);
    }    
    
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::fromIndexRepresentation(const vector<Vector3D> &vertices, const vector<Triangle> &triangles)
{
    resetTriangleCache();
    _vertices.removeAll();
    _triangles.removeAll();
    
    vector<VertexNode *> tempVertices;
    
    for (uint i = 0; i < vertices.size(); i++)
    {
        tempVertices.push_back(_vertices.add(vertices[i]));
    }
    
    VertexNode *triangleVertices[3];
    
    for (uint i = 0; i < triangles.size(); i++)
    {
        Triangle indexTriangle = triangles[i];
        for (uint j = 0; j < 3; j++)
        {
            VertexNode *node = tempVertices.at(indexTriangle.vertexIndices[j]);
            triangleVertices[j] = node;
        }
        _triangles.add(triangleVertices);
    }    
    
    makeEdges();
    
    setSelectionMode(_selectionMode);
}

void Mesh2::toIndexRepresentation(vector<Vector3D> &vertices, vector<Triangle> &triangles)
{
    int index = 0;
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        node->index = index;
        index++;
        
        vertices.push_back(node->data.position);
    }
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle indexTriangle;
        for (int j = 0; j < 3; j++)
            indexTriangle.vertexIndices[j] = node->data.vertex(j)->index;
        
        triangles.push_back(indexTriangle);
    }
}

void Mesh2::setSelection(const vector<bool> &selection)
{
    for (uint i = 0; i < selection.size(); i++)
        setSelectedAtIndex(selection[i], i);
}

void Mesh2::getSelection(vector<bool> &selection)
{
    selection.clear();
    for (uint i = 0; i < selectedCount(); i++)
        selection.push_back(isSelectedAtIndex(i));
}

