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
public:
    FPList<VertexNode, Vertex2> *vertices;
	FPList<TriangleNode, Triangle2> *triangles;
    
    MeshSelectionMode selectionMode;
	
    vector<VertexNode *> *cachedVertexSelection;
    vector<TriangleNode *> *cachedTriangleSelection;
    
	Vector3D *cachedVertices;
	Vector3D *cachedNormals;
	Vector3D *cachedColors;
public:
    Mesh2();
    ~Mesh2();
    
    void addQuad(VertexNode *v1, VertexNode *v2, VertexNode *v3, VertexNode *v4);
    
    MeshSelectionMode getSelectionMode() const { return selectionMode; };
    void setSelectionMode(MeshSelectionMode value);
    
    void getSelectionCenterRotationScale(Vector3D &center, Quaternion &rotation, Vector3D &scale);
    
    void resetCache();
    void fillCache();
    void fillColorCacheAsDarker(float colorComponents[4], bool darker);
    
    void makeCube();  
};
