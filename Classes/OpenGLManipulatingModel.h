/*
 *  OpenGLManipulatingModel.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/28/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include "OpenGLDrawing.h"
#include "MathDeclaration.h"
#include "OpenGLManipulating.h"

class IOpenGLManipulatingModel
{
public:
    virtual ~IOpenGLManipulatingModel() { }
    
    virtual ViewMode viewMode() = 0;
    virtual void setViewMode(ViewMode viewMode) = 0;
    virtual unsigned int count() = 0;
    virtual bool isSelectedAtIndex(unsigned int index) = 0;
    virtual void setSelectedAtIndex(unsigned int index, bool selected) = 0;
    virtual void expandSelectionFromIndex(unsigned int index, bool invert) = 0;
    virtual void duplicateSelected() = 0;
    virtual void removeSelected() = 0;
    virtual void hideSelected() = 0;
    virtual void unhideAll() = 0;
    
    virtual Vector4D selectionColor() = 0;
    virtual void setSelectionColor(Vector4D color) = 0;
    virtual void willSelectThrough(bool selectThrough) = 0;
    virtual void didSelect() = 0;
    virtual bool needsCullFace() = 0;
};

class IOpenGLManipulatingModelMesh : public IOpenGLManipulatingModel
{
public:
    virtual ~IOpenGLManipulatingModelMesh() { }
    
    virtual void getSelectionCenterRotationScale(Vector3D &center, Quaternion &rotation, Vector3D &scale) = 0;
    virtual void transformSelectedByMatrix(Matrix4x4 &matrix) = 0;
    virtual void drawAllForSelection(bool forSelection) = 0;
    virtual bool useGLProject() = 0;
    virtual void glProjectSelect(int x, int y, int width, int height, Matrix4x4 &matrix, OpenGLSelectionMode selectionMode) = 0;
};

class IOpenGLManipulatingModelItem : public IOpenGLManipulatingModel
{
public:
    virtual ~IOpenGLManipulatingModelItem() { }
    
    virtual Vector3D positionAtIndex(unsigned int index) = 0;
    virtual Quaternion rotationAtIndex(unsigned int index) = 0;
    virtual Vector3D scaleAtIndex(unsigned int index) = 0;
    virtual void setPositionAtIndex(unsigned int index, Vector3D position) = 0;
    virtual void setRotationAtIndex(unsigned int index, Quaternion rotation) = 0;
    virtual void setScaleAtIndex(unsigned int index, Vector3D scale) = 0;
    virtual void moveByOffset(unsigned int index, Vector3D offset) = 0;
    virtual void rotateByOffset(unsigned int index, Quaternion offset) = 0;
    virtual void scaleByOffset(unsigned int index, Vector3D offset) = 0;
    virtual void drawAtIndex(unsigned int index, bool forSelection) = 0;
};
