//
//  Mesh2.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#pragma once

#include "MeshHelpers.h"

class Mesh2
{
private:
    FPList<VertexNode, Vertex2> *_vertices;
	FPList<TriangleNode, Triangle2> *_triangles;
    FPList<EdgeNode, Edge2> *_edges;
    
    MeshSelectionMode _selectionMode;
	
    vector<VertexNode *> *_cachedVertexSelection;
    vector<TriangleNode *> *_cachedTriangleSelection;
    vector<EdgeNode *> *_cachedEdgeSelection;
    
	Vector3D *_cachedVertices;
	Vector3D *_cachedNormals;
	Vector3D *_cachedColors;
    
    float _colorComponents[4];
private:
    void fastMergeSelectedVertices();
    void removeDegeneratedTriangles();
    void removeNonUsedVertices();
    void mergeSelectedVertices();
    void removeSelectedVertices();
    void removeSelectedTriangles();
    void splitSelectedTriangles();
    void splitSelectedEdges();
    void flipSelectedTriangles();
    void turnSelectedEdges();
public:
    Mesh2(float colorComponents[4]);
    ~Mesh2();
    
    int vertexCount() { return _vertices->count(); }
    int triangleCount() { return _triangles->count(); }    
    
    MeshSelectionMode selectionMode() const { return _selectionMode; };
    void setSelectionMode(MeshSelectionMode value);
    
    uint selectedCount() const;
    bool isSelectedAtIndex(uint index) const;
    void setSelectedAtIndex(bool selected, uint index);
    void getSelectionCenterRotationScale(Vector3D &center, Quaternion &rotation, Vector3D &scale);
    
    void transformAll(const Matrix4x4 &matrix);
    void transformSelected(const Matrix4x4 &matrix);    
    
    void removeSelected();
    void mergeSelected();
    void splitSelected();
    void flipSelected();
    void extrudeSelectedTriangles();

    // drawing
    
    void resetCache();
    void fillCache();
    void fillColorCache(bool darker);
    
    void drawFill(bool darker, bool forSelection);
    void drawWire();
    void draw(ViewMode mode, const Vector3D &scale, bool selected, bool forSelection);
    void drawAtIndex(uint index, bool forSelection, ViewMode mode);
    
    // make
    
    void addQuad(VertexNode *v1, VertexNode *v2, VertexNode *v3, VertexNode *v4);
    EdgeNode *findOrCreateEdge(VertexNode *v1, VertexNode *v2, TriangleNode *triangle);
    void makeEdges();
    void makeCube();    
};
