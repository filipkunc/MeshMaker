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
#import "MeshHelpers.h"
#import "Camera.h"
#import "MemoryStream.h"

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
    Point3D texCoord;
};

struct GLEdgeVertex
{
    Point3D position;
    Point3D color;
};

struct GLEdgeTexCoord
{
    Point3D position;
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
    
    static bool _useSoftSelection;
    static bool _selectThrough;
    
    bool _isUnwrapped;
    
    uint _vboID;
    bool _vboGenerated;

    float _colorComponents[4];
    NSColor *_color;
    FPTexture *_texture;
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
    void splitSelectedTriangles();
    void splitSelectedEdges();
    void detachSelectedVertices();
    void detachSelectedTriangles();
    void flipSelectedTriangles();
    void turnSelectedEdges();
    void uvToPixels(float &u, float &v);
    void initOrUpdateTexture();
    
    template <class T>
    FPList<VEdgeNode<T>, VEdge<T> > &edges();
    
    template <class T>
    FPList<VNode<T>, T> &vertices();    
    
    template <class T>
    VEdgeNode<T> *findOrCreateEdge(VNode<T> *v1, VNode<T> *v2, TriangleNode * triangle);
    
    template <class T>
    VNode<T> *duplicateVertex(VNode<T> *original);    
public:
    Mesh2();
    Mesh2(MemoryReadStream *stream);
    ~Mesh2();
    
    void encode(MemoryWriteStream *stream);
    
    FPTexture *texture();
    NSColor *color() { return _color; }
    void setColor(NSColor *color);
    
    uint vertexCount() { return _vertices.count(); }
    uint triangleCount() { return _triangles.count(); }
    uint vertexEdgeCount() { return _vertexEdges.count(); }
    
    MeshSelectionMode selectionMode() const { return _selectionMode; };
    void setSelectionMode(MeshSelectionMode value);
    
    void resetAlgorithmData();
    
    uint selectedCount() const;
    bool isSelectedAtIndex(uint index) const;
    void setSelectedAtIndex(bool selected, uint index);
    void expandSelectionFromIndex(uint index);
    void getSelectionCenterRotationScale(Vector3D &center, Quaternion &rotation, Vector3D &scale);
    
    static bool useSoftSelection() { return _useSoftSelection; }
    static void setUseSoftSelection(bool value) { _useSoftSelection = value; }
    
    static bool selectThrough() { return _selectThrough; }
    static void setSelectThrough(bool value) { _selectThrough = value; }
    
    static NSString *descriptionOfMeshType(MeshType meshType);
    
    bool isUnwrapped() { return _isUnwrapped; }
    void setUnwrapped(bool value) { _isUnwrapped = value; }
    
    void transformAll(const Matrix4x4 &matrix);
    void transformSelected(const Matrix4x4 &matrix);    
    
    void removeSelected();
    void mergeSelected();
    void splitSelected();
    void detachSelected();
    void duplicateSelectedTriangles();
    void flipSelected();
    void flipAllTriangles();
    void extrudeSelectedTriangles();
    void triangulate();
    void openSubdivision();
    
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
    
    bool useGLProject() { return _selectThrough && _selectionMode == MeshSelectionModeVertices; }
    void glProjectSelect(int x, int y, int width, int height, const Matrix4x4 &transform, OpenGLSelectionMode selectionMode);
    
    void hideSelected();
    void unhideAll();
    
    // texturing
    
    TriangleNode *rayToUV(const Vector3D &origin, const Vector3D &direction, float &u, float &v);
    
    void cleanTexture();    

    // make
    
    void addTriangle(VertexNode *v1, VertexNode *v2, VertexNode *v3);
    void addQuad(VertexNode *v1, VertexNode *v2, VertexNode *v3, VertexNode *v4);
    
    const FPList<VertexNode, Vertex2> &vertices() const { return _vertices; }
	const FPList<TriangleNode, Triangle2> &triangles() const { return _triangles; }
    const FPList<TexCoordNode, TexCoord> &texCoords() const { return _texCoords; }
    
    void makeTexCoords();
    void makeEdges();
    void makePlane();
    void makeCube();
    void makeCylinder(uint steps);
    void makeSphere(uint steps);
    void makeIcosahedron();
    void make(MeshType meshType, uint steps);
    
    void fromVertices(const vector<Vector3D> &vertices);
    void toVertices(vector<Vector3D> &vertices);
    
    void fromIndexRepresentation(const vector<Vector3D> &vertices, const vector<Vector3D> &texCoords, const vector<TriQuad> &triangles);
    void toIndexRepresentation(vector<Vector3D> &vertices, vector<Vector3D> &texCoords, vector<TriQuad> &triangles);
  
    void setSelection(const vector<bool> &selection);
    void getSelection(vector<bool> &selection);
};

template <>
inline FPList<VertexEdgeNode, VertexEdge> &Mesh2::edges() { return this->_vertexEdges; }

template <>
inline FPList<TexCoordEdgeNode, TexCoordEdge> &Mesh2::edges() { return this->_texCoordEdges; }

template <>
inline FPList<VertexNode, Vertex2> &Mesh2::vertices() { return this->_vertices; }

template <>
inline FPList<TexCoordNode, TexCoord> &Mesh2::vertices() { return this->_texCoords; }

template <class T>
inline VEdgeNode<T> *Mesh2::findOrCreateEdge(VNode<T> *v1, VNode<T> *v2, TriangleNode * triangle)
{
    VEdgeNode<T> *sharedEdge = v1->sharedEdge(v2);
    if (sharedEdge)
    {
        sharedEdge->data().setTriangle(1, triangle);
        return sharedEdge;
    }
    
    VNode<T> *vertices[2] = { v1, v2 };
    VEdgeNode<T> *node = edges<T>().add(vertices);
    
    node->data().setTriangle(0, triangle);
    return node;
}

template <class T>
inline VNode<T> *Mesh2::duplicateVertex(VNode<T> *original)
{
    if (original->algorithmData.duplicatePair == NULL)
        original->algorithmData.duplicatePair = vertices<T>().add(original->data().position);
    
    return original->algorithmData.duplicatePair;
}
