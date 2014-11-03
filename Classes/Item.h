//
//  Item.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#pragma once

#include <Cocoa/Cocoa.h>
#include "MathDeclaration.h"
#include "MeshHelpers.h"
#include "Mesh2.h"
#include "OpenGLManipulatingController.h"
#include "MemoryStream.h"
#include "MemoryStreaming.h"

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
    
    Item(MemoryReadStream *stream, TextureCollection &textures);
    void encode(MemoryWriteStream *stream, TextureCollection &textures);
    
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
    virtual unsigned int count();
    virtual bool isSelectedAtIndex(unsigned int index);
    virtual void setSelectedAtIndex(unsigned int index, bool selected);
    virtual void expandSelectionFromIndex(unsigned int index, bool invert);
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

