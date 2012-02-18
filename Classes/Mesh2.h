//
//  Mesh2.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  For license see LICENSE.TXT
//

#pragma once

#import "OpenGLDrawing.h"
#import "ShaderProgram.h"
#import "FPTexture.h"
#include "MeshHelpers.h"
#include "Camera.h"

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

struct Point2D 
{
    float coords[2];
};

struct GLTriangleVertex
{
    Point3D position;
    Point3D flatNormal;
    Point3D smoothNormal;
    Point3D color;
    Point2D texCoord;
};

struct GLEdgeVertex
{
    Point3D position;
    Point3D color;
};

struct GLEdgeTexCoord
{
    Point2D position;
    Point3D color;
};

struct FillMode
{
    bool colored;
    bool textured;
};

class Mesh2
{
private:
    FPList<VertexNode, Vertex2> _vertices;
	FPList<TriangleNode, Triangle2> _triangles;
    FPList<TexCoordNode, TexCoord> _texCoords;
    
    FPList<VertexEdgeNode, VertexEdge> _vertexEdges;
    FPList<TexCoordEdgeNode, TexCoordEdge> _texCoordEdges;
    
    MeshSelectionMode _selectionMode;
	
    vector<VertexNode *> _cachedVertexSelection;
    vector<TriangleNode *> _cachedTriangleSelection;
    vector<TexCoordNode *> _cachedTexCoordSelection;
    vector<VertexEdgeNode *> _cachedVertexEdgeSelection;
    vector<TexCoordEdgeNode *> _cachedTexCoordEdgeSelection;
    
	FPArrayCache<GLTriangleVertex> _cachedTriangleVertices;
    FPArrayCache<GLEdgeVertex> _cachedEdgeVertices;
    FPArrayCache<GLEdgeTexCoord> _cachedEdgeTexCoords;
    
    float _colorComponents[4];
    
    static bool _useSoftSelection;
    static bool _selectThrough;
    static bool _isUnwrapped;
    
    uint _vboID;
    bool _vboGenerated;
    
private:
    void fastMergeSelectedVertices();
    void fastMergeSelectedTexCoords();
    void removeDegeneratedTriangles();
    void removeNonUsedVertices();
    void removeNonUsedTexCoords();
    void mergeSelectedVertices();
    void removeSelectedVertices();
    void removeSelectedTriangles();
    void removeSelectedEdges();
    void splitSelectedVertices();
    void splitSelectedTriangles();
    void splitSelectedEdges();
    void flipSelectedTriangles();
    void turnSelectedEdges();
    void halfEdges();
    void repositionVertices(int vertexCount);
    void makeSubdividedTriangles();
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
    
    static bool isUnwrapped() { return _isUnwrapped; }
    static void setUnwrapped(bool value) { _isUnwrapped = value; }
    
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
    
    void drawFill(FillMode fillMode, ViewMode viewMode);
    void draw(ViewMode viewMode, const Vector3D &scale, bool selected, bool forSelection);

    void drawAtIndex(uint index, bool forSelection, ViewMode viewMode);
    void drawAll(ViewMode viewMode, bool forSelection);
    
    void drawAllVertices(ViewMode viewMode, bool forSelection);
    void drawAllTriangles(ViewMode viewMode, bool forSelection);
    void drawAllEdges(ViewMode viewMode, bool forSelection);
    
    void paintOnTexture(const Vector3D &origin, const Vector3D &direction, FPTexture *texture);
    
    // make
    
    void addTriangle(VertexNode *v1, VertexNode *v2, VertexNode *v3);
    void addQuad(VertexNode *v1, VertexNode *v2, VertexNode *v3, VertexNode *v4);
    VertexEdgeNode *findOrCreateVertexEdge(VertexNode *v1, VertexNode *v2, TriangleNode *triangle);
    TexCoordEdgeNode *findOrCreateTexCoordEdge(TexCoordNode *t1, TexCoordNode *t2, TriangleNode *triangle);
    VertexNode *findOrCreateVertex(vector<ExtrudePair> &extrudePairs, VertexNode *original);
    void makeEdges();
    void makePlane();
    void makeCube();
    void makeCylinder(uint steps);
    void makeSphere(uint steps);
    
    void fromIndexRepresentation(const vector<Vector3D> &vertices, const vector<Vector2D> &texCoords, const vector<Triangle> &triangles);
    void toIndexRepresentation(vector<Vector3D> &vertices, vector<Vector2D> &texCoords, vector<Triangle> &triangles);
  
    void setSelection(const vector<bool> &selection);
    void getSelection(vector<bool> &selection);
};
