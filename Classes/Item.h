//
//  Item.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "MeshHelpers.h"
#import "Mesh2.h"
#import "OpenGLManipulatingController.h"
#import "MemoryStream.h"
#import "MemoryStreaming.h"

class Item : public IOpenGLManipulatingModelMesh
{
public:
    Vector3D position;
    Quaternion rotation;
    Vector3D scale;
    Mesh2 *mesh;
    bool selected;
    bool visible;
    ViewMode _viewMode;
    
    Item(Mesh2 *aMesh);
    virtual ~Item();
    
    Item(MemoryReadStream *stream);
    void encode(MemoryWriteStream *stream);
    
    Matrix4x4 transform();
    
    void drawForSelection(bool forSelection);
    void moveByOffset(Vector3D offset);
    void rotateByOffset(Quaternion offset);
    void scaleByOffset(Vector3D offset);
    Item *duplicate();
    void setPositionToGeometricCenter();
    
    // IOpenGLManipulatingModel
    
    virtual ViewMode viewMode();
    virtual void setViewMode(ViewMode viewMode);
    virtual uint count();
    virtual bool isSelectedAtIndex(uint index);
    virtual void setSelectedAtIndex(uint index, bool selected);
    virtual void expandSelectionFromIndex(uint index, bool invert);
    virtual void duplicateSelected();
    virtual void removeSelected();
    virtual void hideSelected();
    virtual void unhideAll();
    
    virtual Vector4D selectionColor();
    virtual void setSelectionColor(Vector4D color);
    virtual void willSelectThrough(bool selectThrough);
    virtual void didSelect();
    virtual bool needsCullFace();
    
    // IOpenGLManipulatingModelMesh
    
    virtual void getSelectionCenterRotationScale(Vector3D &center, Quaternion &rotation, Vector3D &scale);
    virtual void transformSelectedByMatrix(Matrix4x4 &matrix);
    virtual void drawAllForSelection(bool forSelection);
    virtual bool useGLProject();
    virtual void glProjectSelect(int x, int y, int width, int height, Matrix4x4 &matrix, OpenGLSelectionMode selectionMode);
};

