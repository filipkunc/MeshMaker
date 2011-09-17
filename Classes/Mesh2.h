//
//  Mesh2.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#pragma once

#include "MeshHelpers.h"

struct ExtrudePair 
{
    VertexNode *original;
    VertexNode *extruded;
};

enum GLVertexAttribID
{
    GLVertexAttribID_Position = 0,
    GLVertexAttribID_Normal,
    GLVertexAttribID_Color
};

struct Point3D
{
    float coords[3];
};

struct GLTriangleVertex
{
    Point3D position;
    Point3D flatNormal;
    Point3D smoothNormal;
    Point3D color;
};

struct GLEdgeVertex
{
    Point3D position;
    Point3D color;
};

class Mesh2
{
private:
    FPList<VertexNode, Vertex2> _vertices;
	FPList<TriangleNode, Triangle2> _triangles;
    FPList<EdgeNode, Edge2> _edges;
    
    MeshSelectionMode _selectionMode;
	
    vector<VertexNode *> _cachedVertexSelection;
    vector<TriangleNode *> _cachedTriangleSelection;
    vector<EdgeNode *> _cachedEdgeSelection;
    
	FPArrayCache<GLTriangleVertex> _cachedTriangleVertices;
    FPArrayCache<GLEdgeVertex> _cachedEdgeVertices;
    
    float _colorComponents[4];
    
    static bool _useSoftSelection;
    static bool _selectThrough;
    
    uint _vboID;
    bool _vboGenerated;
    
private:
    void fastMergeSelectedVertices();
    void removeDegeneratedTrianglesAndEdges();
    void removeDegeneratedTriangles();
    void removeDegeneratedEdges();
    void removeNonUsedVertices();
    void mergeSelectedVertices();
    void removeSelectedVertices();
    void removeSelectedTriangles();
    void removeSelectedEdges();
    void splitSelectedTriangles();
    void splitSelectedEdges();
    void flipSelectedTriangles();
    void turnSelectedEdges();
public:
    Mesh2(float colorComponents[4]);
    ~Mesh2();
    
    int vertexCount() { return _vertices.count(); }
    int triangleCount() { return _triangles.count(); }    
    
    MeshSelectionMode selectionMode() const { return _selectionMode; };
    void setSelectionMode(MeshSelectionMode value);
    
    uint selectedCount() const;
    bool isSelectedAtIndex(uint index) const;
    void setSelectedAtIndex(bool selected, uint index);
    void getSelectionCenterRotationScale(Vector3D &center, Quaternion &rotation, Vector3D &scale);
    
    static bool useSoftSelection() { return _useSoftSelection; }
    static void setUseSoftSelection(bool value) { _useSoftSelection = value; }
    
    static bool selectThrough() { return _selectThrough; }
    static void setSelectThrough(bool value) { _selectThrough = value; }
    
    void transformAll(const Matrix4x4 &matrix);
    void transformSelected(const Matrix4x4 &matrix);    
    
    void removeSelected();
    void mergeSelected();
    void splitSelected();
    void flipSelected();
    void flipAllTriangles();
    void extrudeSelectedTriangles();
    
    void loopSubdivision();
    
    void merge(Mesh2 *mesh);
    
    void computeSoftSelection();

    // drawing
    
    void resetTriangleCache();
    void fillTriangleCache();
    
    void resetEdgeCache();
    void fillEdgeCache();
    
    void drawColoredFill(bool colored, bool useVertexNormals);
    void draw(ViewMode mode, const Vector3D &scale, bool selected, bool forSelection);

    void drawAtIndex(uint index, bool forSelection, ViewMode mode);
    void drawAll(ViewMode mode, bool forSelection);
    
    void drawAllVertices(ViewMode mode, bool forSelection);
    void drawAllTriangles(ViewMode mode, bool forSelection);
    void drawAllEdges(ViewMode mode, bool forSelection);
    
    // make
    
    void addTriangle(VertexNode *v1, VertexNode *v2, VertexNode *v3);
    void addQuad(VertexNode *v1, VertexNode *v2, VertexNode *v3, VertexNode *v4);
    EdgeNode *findOrCreateEdge(VertexNode *v1, VertexNode *v2, TriangleNode *triangle);
    VertexNode *findOrCreateVertex(vector<ExtrudePair> &extrudePairs, VertexNode *original);
    void makeEdges();
    void makePlane();
    void makeCube();
    void makeCylinder(uint steps);
    void makeSphere(uint steps);
    
    void fromIndexRepresentation(const vector<Vector3D> &vertices, const vector<Triangle> &triangles);
    void toIndexRepresentation(vector<Vector3D> &vertices, vector<Triangle> &triangles);
  
    void setSelection(const vector<bool> &selection);
    void getSelection(vector<bool> &selection);
};
