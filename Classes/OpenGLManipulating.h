/*
 *  OpenGLManipulating.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 8/3/09.
 *  For license see LICENSE.TXT
 *
 */
#import "OpenGLSelecting.h"

class IOpenGLTransforming
{
public:
    virtual ~IOpenGLTransforming() { }
    
    virtual float selectionX() = 0;
    virtual void setSelectionX(float x) = 0;
    virtual float selectionY() = 0;
    virtual void setSelectionY(float y) = 0;
    virtual float selectionZ() = 0;
    virtual void setSelectionZ(float z) = 0;
    
    virtual ManipulatorType currentManipulator() = 0;
    virtual void setCurrentManipulator(ManipulatorType manipulator, bool ignoreKVC) = 0;
    
    virtual bool selectionColorEnabled() = 0;
    virtual Vector4D selectionColor() = 0;
    virtual void setSelectionColor(Vector4D color) = 0;
};

class IOpenGLManipulating : public IOpenGLSelectingOptional, public IOpenGLTransforming
{
public:
    virtual ~IOpenGLManipulating() { }
    
    virtual Vector3D selectionCenter() = 0;
    virtual void setSelectionCenter(Vector3D center) = 0;
    virtual Quaternion selectionRotation() = 0;
    virtual void setSelectionRotation(Quaternion rotation) = 0;
    virtual Vector3D selectionScale() = 0;
    virtual void setSelectionScale(Vector3D scale) = 0;
    virtual uint selectedCount() = 0;
    virtual ViewMode viewMode() = 0;
    virtual void setViewMode(ViewMode mode) = 0;
    
    virtual void moveSelectedByOffset(Vector3D offset) = 0;
    virtual void rotateSelectedByOffset(Quaternion offset) = 0;
    virtual void scaleSelectedByOffset(Vector3D offset) = 0;
    virtual void updateSelection() = 0;
    virtual void drawForSelection(bool forSelection) = 0;
    virtual void changeSelection(bool isSelected) = 0;
    virtual void invertSelection() = 0;
    virtual void duplicateSelected() = 0;
    virtual void removeSelected() = 0;
    virtual void hideSelected() = 0;
    virtual void unhideAll() = 0;
};
