//
//  OpenGLManipulatingController.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 8/4/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "OpenGLManipulatingModel.h"

class OpenGLManipulatingController;

@interface OpenGLManipulatingControllerKVC : NSObject
{
    OpenGLManipulatingController *_controller;
}

@property (readwrite, assign) enum ViewMode viewMode;

- (id)initWithController:(OpenGLManipulatingController *)controller;
- (void)addObserver:(id)observer forKeyPath:(NSString *)keyPath;

@end

class OpenGLManipulatingController : public IOpenGLManipulating
{
private:
    IOpenGLManipulatingModel *_model;
	IOpenGLManipulatingModelMesh *_modelMesh;
	IOpenGLManipulatingModelItem *_modelItem;
	
	Vector3D _selectionCenter;
	Quaternion _selectionRotation;
	Vector3D _selectionEuler;
	Vector3D _selectionScale;
	unsigned int _selectedCount;
	int _lastSelectedIndex;
	
	Matrix4x4 _modelTransform;
	Vector3D _modelPosition;
	Quaternion _modelRotation;
	Vector3D _modelScale;
    
    ManipulatorType _currentManipulator;

    OpenGLManipulatingControllerKVC *_kvc;

public:
    OpenGLManipulatingController();
    virtual ~OpenGLManipulatingController();
    
    // IOpenGLSelecting
    
    virtual unsigned int selectableCount();
    virtual void drawForSelectionAtIndex(unsigned int index);
    virtual void selectObjectAtIndex(unsigned int index, OpenGLSelectionMode selectionMode);

    // IOpenGLSelectingOptional
    
    virtual void willSelectThrough(bool selectThrough);
    virtual void didSelect();
    virtual bool isObjectSelectedAtIndex(unsigned int index);
    virtual void drawAllForSelection();
    virtual bool needsCullFace();
    virtual bool useGLProject();
    virtual void glProjectSelect(int x, int y, int width, int height, OpenGLSelectionMode selectionMode);
    
    // IOpenGLTransforming
    
    virtual float selectionX();
    virtual void setSelectionX(float x);
    virtual float selectionY();
    virtual void setSelectionY(float y);
    virtual float selectionZ();
    virtual void setSelectionZ(float z);
    
    virtual ManipulatorType currentManipulator();
    virtual void setCurrentManipulator(ManipulatorType manipulator, bool ignoreKVC);
    
    virtual bool selectionColorEnabled();
    virtual Vector4D selectionColor();
    virtual void setSelectionColor(Vector4D color);
    
    // IOpenGLManipulating
    
    virtual Vector3D selectionCenter();
    virtual void setSelectionCenter(Vector3D center);
    virtual Quaternion selectionRotation();
    virtual void setSelectionRotation(Quaternion rotation);
    virtual Vector3D selectionScale();
    virtual void setSelectionScale(Vector3D scale);
    virtual unsigned int selectedCount();
    virtual ViewMode viewMode();
    virtual void setViewMode(ViewMode mode);
    
    virtual void moveSelectedByOffset(Vector3D offset);
    virtual void rotateSelectedByOffset(Quaternion offset);
    virtual void scaleSelectedByOffset(Vector3D offset);
    virtual void updateSelection();
    virtual void drawForSelection(bool forSelection);
    virtual void changeSelection(bool isSelected);
    virtual void invertSelection();
    virtual void duplicateSelected();
    virtual void removeSelected();
    virtual void hideSelected();
    virtual void unhideAll();
    
    // OpenGLManipulatingController
    
    IOpenGLManipulatingModel *model();
    void setModel(IOpenGLManipulatingModel *aModel);
    Matrix4x4 &modelTransform();
    void setModelTransform(Matrix4x4 &transform);
    int lastSelectedIndex();

    void addSelectionObserver(id observer);
    void removeSelectionObserver(id observer);
    float transformValueAtIndex(unsigned int index, ManipulatorType manipulator);
    void setTransformValueAtIndex(unsigned int index, ManipulatorType manipulator, float value);
    void willChangeSelection();
    void didChangeSelection();
    void setPositionRotationScale(Vector3D aPosition, Quaternion aRotation, Vector3D aScale);
};
