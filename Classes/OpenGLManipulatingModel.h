/*
 *  OpenGLManipulatingModel.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/28/09.
 *  For license see LICENSE.TXT
 *
 */

#import "OpenGLDrawing.h"
#import "MathDeclaration.h"
#import "OpenGLManipulating.h"

class IOpenGLManipulatingModel
{
public:
    virtual ~IOpenGLManipulatingModel() { }
    
    virtual ViewMode viewMode() = 0;
    virtual void setViewMode(ViewMode viewMode) = 0;
    virtual uint count() = 0;
    virtual bool isSelectedAtIndex(uint index) = 0;
    virtual void setSelectedAtIndex(uint index, bool selected) = 0;
    virtual void expandSelectionFromIndex(uint index, bool invert) = 0;
    virtual void duplicateSelected() = 0;
    virtual void removeSelected() = 0;
    virtual void hideSelected() = 0;
    virtual void unhideAll() = 0;
    
    virtual NSColor *selectionColor() = 0; // can be Vector4D instead of NSColor *
    virtual void setSelectionColor(NSColor *color) = 0;
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
    
    virtual Vector3D positionAtIndex(uint index) = 0;
    virtual Quaternion rotationAtIndex(uint index) = 0;
    virtual Vector3D scaleAtIndex(uint index) = 0;
    virtual void setPositionAtIndex(uint index, Vector3D position) = 0;
    virtual void setRotationAtIndex(uint index, Quaternion rotation) = 0;
    virtual void setScaleAtIndex(uint index, Vector3D scale) = 0;
    virtual void moveByOffset(uint index, Vector3D offset) = 0;
    virtual void rotateByOffset(uint index, Quaternion offset) = 0;
    virtual void scaleByOffset(uint index, Vector3D offset) = 0;
    virtual void drawAtIndex(uint index, bool forSelection) = 0;
};
