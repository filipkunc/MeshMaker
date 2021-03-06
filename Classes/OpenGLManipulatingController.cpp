//
//  OpenGLManipulatingController.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 8/4/09.
//  For license see LICENSE.TXT
//

#include "OpenGLManipulatingController.h"

@implementation OpenGLManipulatingControllerKVC

- (id)initWithController:(OpenGLManipulatingController *)controller
{
    self = [super init];
    if (self)
    {
        _controller = controller;
    }
    return self;
}

- (void)addObserver:(id)observer forKeyPath:(NSString *)keyPath
{
	[self addObserver:observer
		   forKeyPath:keyPath
			  options:NSKeyValueObservingOptionNew
			  context:NULL];
}

- (float)selectionX { return _controller->selectionX(); }
- (float)selectionY { return _controller->selectionY(); }
- (float)selectionZ { return _controller->selectionZ(); }
- (void)setSelectionX:(float)selectionX { _controller->setSelectionX(selectionX); }
- (void)setSelectionY:(float)selectionY { _controller->setSelectionY(selectionY); }
- (void)setSelectionZ:(float)selectionZ { _controller->setSelectionZ(selectionZ); }
- (BOOL)selectionColorEnabled { return _controller->selectionColorEnabled(); }

- (NSColor *)selectionColor
{
    Vector4D color = _controller->selectionColor();
    return [NSColor colorWithCalibratedRed:color.x green:color.y blue:color.z alpha:color.w];
}

- (void)setSelectionColor:(NSColor *)selectionColor
{
    Vector4D color = Vector4D(selectionColor.redComponent, selectionColor.greenComponent, selectionColor.blueComponent, selectionColor.alphaComponent);
    _controller->setSelectionColor(color);
}

- (enum ViewMode)viewMode { return _controller->viewMode(); }
- (void)setViewMode:(enum ViewMode)viewMode { _controller->setViewMode(viewMode); }

@end

OpenGLManipulatingController::OpenGLManipulatingController()
{
    _model = NULL;
    _selectionScale = Vector3D(1, 1, 1);
    _selectedCount = 0;
    _lastSelectedIndex = -1;
    _modelScale = Vector3D(1, 1, 1);
    _currentManipulator = ManipulatorType::Default;
    _kvc = [[OpenGLManipulatingControllerKVC alloc] initWithController:this];
}

OpenGLManipulatingController::~OpenGLManipulatingController()
{
    
}

unsigned int OpenGLManipulatingController::selectableCount()
{
    return _model->count();
}

void OpenGLManipulatingController::drawForSelectionAtIndex(unsigned int index)
{
    glPushMatrix();
	glMultMatrixf(_modelTransform);
    _modelItem->drawAtIndex(index, true);
	glPopMatrix();
}

void OpenGLManipulatingController::selectObjectAtIndex(unsigned int index, OpenGLSelectionMode selectionMode)
{
    switch (selectionMode)
	{
		case OpenGLSelectionMode::Add:
            _model->setSelectedAtIndex(index, true);
			break;
		case OpenGLSelectionMode::Subtract:
            _model->setSelectedAtIndex(index, false);
			break;
		case OpenGLSelectionMode::Invert:
            _model->setSelectedAtIndex(index, !_model->isSelectedAtIndex(index));
			break;
        case OpenGLSelectionMode::Expand:
            _model->expandSelectionFromIndex(index, false);
            break;
        case OpenGLSelectionMode::InvertExpand:
            _model->expandSelectionFromIndex(index, true);
            break;
		default:
			break;
	}
}

void OpenGLManipulatingController::willSelectThrough(bool selectThrough)
{
    _model->willSelectThrough(selectThrough);
}

void OpenGLManipulatingController::didSelect()
{
    _model->didSelect();
    updateSelection();
}

bool OpenGLManipulatingController::isObjectSelectedAtIndex(unsigned int index)
{
    return _model->isSelectedAtIndex(index);
}

void OpenGLManipulatingController::drawAllForSelection()
{
    if (_modelMesh != NULL)
    {
        glPushMatrix();
        glMultMatrixf(_modelTransform);
        _modelMesh->drawAllForSelection(true);
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glMultMatrixf(_modelTransform);
        for (unsigned int i = 0; i < _modelItem->count(); i++)
        {
            unsigned int colorIndex = i + 1;
            ColorIndex(colorIndex);
            _modelItem->drawAtIndex(i, true);
        }
        glPopMatrix();
    }
}

bool OpenGLManipulatingController::needsCullFace()
{
    return _model->needsCullFace();
}

bool OpenGLManipulatingController::useGLProject()
{
    if (_modelMesh != NULL)
        return _modelMesh->useGLProject();
    return false;
}

void OpenGLManipulatingController::glProjectSelect(int x, int y, int width, int height, OpenGLSelectionMode selectionMode)
{
    if (_modelMesh != NULL)
        _modelMesh->glProjectSelect(x, y, width, height, _modelTransform, selectionMode);
}

float OpenGLManipulatingController::selectionX()
{
    return transformValueAtIndex(0, _currentManipulator);
}

void OpenGLManipulatingController::setSelectionX(float x)
{
    setTransformValueAtIndex(0, _currentManipulator, x);
}

float OpenGLManipulatingController::selectionY()
{
    return transformValueAtIndex(1, _currentManipulator);
}

void OpenGLManipulatingController::setSelectionY(float y)
{
    setTransformValueAtIndex(1, _currentManipulator, y);
}

float OpenGLManipulatingController::selectionZ()
{
    return transformValueAtIndex(2, _currentManipulator);
}

void OpenGLManipulatingController::setSelectionZ(float z)
{
    setTransformValueAtIndex(2, _currentManipulator, z);
}

ManipulatorType OpenGLManipulatingController::currentManipulator()
{
    return _currentManipulator;
}

void OpenGLManipulatingController::setCurrentManipulator(ManipulatorType manipulator, bool ignoreKVC)
{
    if (ignoreKVC)
    {
        _currentManipulator = manipulator;
    }
    else
    {
        willChangeSelection();
        _currentManipulator = manipulator;
        didChangeSelection();
    }
}

bool OpenGLManipulatingController::selectionColorEnabled()
{
    if (_model != NULL)
        return true;
    return false;
}

Vector4D OpenGLManipulatingController::selectionColor()
{
    // TODO: often crashes here, find why...
    if (selectionColorEnabled())
        return _model->selectionColor();
    return Vector4D();
}

void OpenGLManipulatingController::setSelectionColor(Vector4D color)
{
    if (selectionColorEnabled())
        _model->setSelectionColor(color);
}

Vector3D OpenGLManipulatingController::selectionCenter()
{
    return _selectionCenter;
}

void OpenGLManipulatingController::setSelectionCenter(Vector3D center)
{
    willChangeSelection();
    _selectionCenter = center;
    didChangeSelection();
}

Quaternion OpenGLManipulatingController::selectionRotation()
{
    return _selectionRotation;
}

void OpenGLManipulatingController::setSelectionRotation(Quaternion rotation)
{
    willChangeSelection();
    _selectionRotation = rotation;
    _selectionEuler = _selectionRotation.ToEulerAngles();
    didChangeSelection();
}

Vector3D OpenGLManipulatingController::selectionScale()
{
    return _selectionScale;
}

void OpenGLManipulatingController::setSelectionScale(Vector3D scale)
{
    willChangeSelection();
    _selectionScale = scale;
    didChangeSelection();
}

unsigned int OpenGLManipulatingController::selectedCount()
{
    return _selectedCount;
}

ViewMode OpenGLManipulatingController::viewMode()
{
    if (_model != NULL)
        return _model->viewMode();
    return ViewMode::SolidFlat;
}

void OpenGLManipulatingController::setViewMode(ViewMode mode)
{
    if (_model != NULL)
        _model->setViewMode(mode);
}

void OpenGLManipulatingController::moveSelectedByOffset(Vector3D offset)
{
    Vector3D transformedOffset;
	Matrix4x4 m, r, s;
	Quaternion inverseRotation = _modelRotation.Conjugate();
	
	Vector3D inverseScale = _modelScale;
	for (int i = 0; i < 3; i++)
		inverseScale[i] = 1.0f / inverseScale[i];
	
	r = inverseRotation.ToMatrix();
	s.Scale(inverseScale);
	m = s * r;
	transformedOffset = m.Transform(offset);
	
	if (_modelMesh != NULL)
	{
        Matrix4x4 translate;
        translate.Translate(transformedOffset);
        _modelMesh->transformSelectedByMatrix(translate);
	}
	else if (_modelItem != NULL)
	{
		for (unsigned int i = 0; i < _model->count(); i++)
		{
            if (_model->isSelectedAtIndex(i))
                _modelItem->moveByOffset(i, transformedOffset);
		}
	}
    
    setSelectionCenter(_selectionCenter + offset);
}

void OpenGLManipulatingController::rotateSelectedByOffset(Quaternion offset)
{
    if (_modelMesh != NULL)
	{
		Vector3D rotationCenter = _selectionCenter;
		rotationCenter = _modelTransform.Inverse().Transform(rotationCenter);
        
        Matrix4x4 t1, t2, r;
        t1.Translate(-rotationCenter);
        t2.Translate(rotationCenter);
        r = offset.ToMatrix();
        
        Matrix4x4 m = t2 * r * t1;
        _modelMesh->transformSelectedByMatrix(m);
        
        setSelectionRotation(offset * _selectionRotation);
	}
	else if (_modelItem != NULL)
	{
		if (selectedCount() > 1)
		{
			Vector3D rotationCenter = _selectionCenter;
			rotationCenter = _modelTransform.Inverse().Transform(rotationCenter);
            Matrix4x4 offsetMatrix = offset.ToMatrix();
			for (unsigned int i = 0; i < _model->count(); i++)
			{
				if (_model->isSelectedAtIndex(i))
				{
					Vector3D itemPosition = _modelItem->positionAtIndex(i);
					itemPosition -= rotationCenter;
					itemPosition = offsetMatrix.Transform(itemPosition);
					itemPosition += rotationCenter;
                    _modelItem->setPositionAtIndex(i, itemPosition);
                    _modelItem->rotateByOffset(i, offset);
				}
			}
            setSelectionRotation(offset * _selectionRotation);
		}
		else if (_lastSelectedIndex > -1)
		{
            _modelItem->rotateByOffset(_lastSelectedIndex, offset);
            setSelectionRotation(_modelItem->rotationAtIndex(_lastSelectedIndex));            
		}		
	}
}

void OpenGLManipulatingController::scaleSelectedByOffset(Vector3D offset)
{
    if (_modelMesh != NULL)
	{
        Vector3D rotationCenter = _selectionCenter;
		rotationCenter = _modelTransform.Inverse().Transform(rotationCenter);
        
        Matrix4x4 t1, t2, s;
        t1.Translate(-rotationCenter);
        t2.Translate(rotationCenter);
        s.Scale(offset + Vector3D(1, 1, 1));
        
        Matrix4x4 m = t2 * s * t1;
        _modelMesh->transformSelectedByMatrix(m);
	}
	else if (_modelItem != NULL)
	{
		if (selectedCount() > 1)
		{
			Vector3D rotationCenter = _selectionCenter;
			rotationCenter = _modelTransform.Inverse().Transform(rotationCenter);
			for (unsigned int i = 0; i < _model->count(); i++)
			{
                if (_model->isSelectedAtIndex(i))
				{
					Vector3D itemPosition = _modelItem->positionAtIndex(i);
					itemPosition -= rotationCenter;
					itemPosition.x *= 1.0f + offset.x;
					itemPosition.y *= 1.0f + offset.y;
					itemPosition.z *= 1.0f + offset.z;
					itemPosition += rotationCenter;
                    _modelItem->setPositionAtIndex(i, itemPosition);
                    _modelItem->scaleByOffset(i, offset);
				}
			}
		}
		else if (_lastSelectedIndex > -1)
		{
            _modelItem->scaleByOffset(_lastSelectedIndex, offset);
		}
	}
    setSelectionScale(_selectionScale + offset);
}

void OpenGLManipulatingController::updateSelection()
{
    willChangeSelection();
	
	if (_modelMesh != NULL)
	{
        _modelMesh->getSelectionCenterRotationScale(_selectionCenter, _selectionRotation, _selectionScale);
		_selectedCount = 0;
		for (unsigned int i = 0; i < _model->count(); i++)
		{
            if (_model->isSelectedAtIndex(i))
				_selectedCount++;
		}
	}
	else if (_modelItem != NULL)
	{
		_selectionCenter = Vector3D();
		_selectionRotation = Quaternion();
		_selectionScale = Vector3D(1, 1, 1);
		_selectedCount = 0;
		_lastSelectedIndex = -1;
		for (unsigned int i = 0; i < _model->count(); i++)
		{
            if (_model->isSelectedAtIndex(i))
			{
				_selectedCount++;
				_selectionCenter += _modelItem->positionAtIndex(i);
				_lastSelectedIndex = i;
			}
		}
		if (_selectedCount > 0)
		{
			_selectionCenter /= (float)_selectedCount;
			if (_selectedCount == 1 && _lastSelectedIndex > -1)
			{
				_selectionRotation = _modelItem->rotationAtIndex(_lastSelectedIndex);
				_selectionScale = _modelItem->scaleAtIndex(_lastSelectedIndex);
			}
		}
		else
		{
			_selectionCenter = Vector3D();
		}
	}
	
    _selectionEuler = _selectionRotation.ToEulerAngles();
    _selectionCenter = _modelTransform.Transform(_selectionCenter);
    
    didChangeSelection();
}

void OpenGLManipulatingController::drawForSelection(bool forSelection)
{
    if (_modelMesh != NULL)
    {
        glPushMatrix();
        glMultMatrixf(_modelTransform);
        _modelMesh->drawAllForSelection(forSelection);
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glMultMatrixf(_modelTransform);
        for (unsigned int i = 0; i < _modelItem->count(); i++)
        {
            _modelItem->drawAtIndex(i, forSelection);
        }
        glPopMatrix();
    }
}

void OpenGLManipulatingController::changeSelection(bool isSelected)
{
    willSelectThrough(false);
	for (unsigned int i = 0; i < _model->count(); i++)
        _model->setSelectedAtIndex(i, isSelected);
    didSelect();
    updateSelection();
}

void OpenGLManipulatingController::invertSelection()
{
    willSelectThrough(false);
    for (unsigned int i = 0; i < _model->count(); i++)
        _model->setSelectedAtIndex(i, !_model->isSelectedAtIndex(i));
    didSelect();
    updateSelection();
}

void OpenGLManipulatingController::duplicateSelected()
{
    _model->duplicateSelected();
    updateSelection();
}

void OpenGLManipulatingController::removeSelected()
{
    _model->removeSelected();
    updateSelection();
}

void OpenGLManipulatingController::hideSelected()
{
    _model->hideSelected();
    updateSelection();
}

void OpenGLManipulatingController::unhideAll()
{
    _model->unhideAll();
}

IOpenGLManipulatingModel *OpenGLManipulatingController::model()
{
    return _model;
}

void OpenGLManipulatingController::setModel(IOpenGLManipulatingModel *aModel)
{
    _model = aModel;
    _modelMesh = dynamic_cast<IOpenGLManipulatingModelMesh *>(aModel);
    _modelItem = dynamic_cast<IOpenGLManipulatingModelItem *>(aModel);
}

Matrix4x4 &OpenGLManipulatingController::modelTransform()
{
    return _modelTransform;
}

void OpenGLManipulatingController::setModelTransform(Matrix4x4 &transform)
{
    _modelTransform = transform;
}

int OpenGLManipulatingController::lastSelectedIndex()
{
    return _lastSelectedIndex;
}

void OpenGLManipulatingController::addSelectionObserver(id observer)
{
    [_kvc addObserver:observer forKeyPath:@"selectionX"];
	[_kvc addObserver:observer forKeyPath:@"selectionY"];
	[_kvc addObserver:observer forKeyPath:@"selectionZ"];
    [_kvc addObserver:observer forKeyPath:@"selectionColor"];
    [_kvc addObserver:observer forKeyPath:@"selectionColorEnabled"];
    [_kvc addObserver:observer forKeyPath:@"viewMode"];
}

void OpenGLManipulatingController::removeSelectionObserver(id observer)
{
    [_kvc removeObserver:observer forKeyPath:@"selectionX"];
	[_kvc removeObserver:observer forKeyPath:@"selectionY"];
	[_kvc removeObserver:observer forKeyPath:@"selectionZ"];
    [_kvc removeObserver:observer forKeyPath:@"selectionColor"];
    [_kvc removeObserver:observer forKeyPath:@"selectionColorEnabled"];
    [_kvc removeObserver:observer forKeyPath:@"viewMode"];
}

float OpenGLManipulatingController::transformValueAtIndex(unsigned int index, ManipulatorType manipulator)
{
    switch (manipulator)
	{
		case ManipulatorType::Translation:
			return _selectionCenter[index];
		case ManipulatorType::Rotation:
			return _selectionEuler[index] * RAD_TO_DEG;
		case ManipulatorType::Scale:
			return _selectionScale[index];
		case ManipulatorType::Default:
		default:
			return 0.0f;
	}
}

void OpenGLManipulatingController::setTransformValueAtIndex(unsigned int index, ManipulatorType manipulator, float value)
{
    switch (manipulator)
	{
		case ManipulatorType::Translation:
		{
			Vector3D offset = Vector3D();
			offset[index] = value - _selectionCenter[index];
            moveSelectedByOffset(offset);
		}break;
		case ManipulatorType::Rotation:
		{
			_selectionEuler[index] = value * DEG_TO_RAD;
			if (_selectedCount == 1)
			{
				_selectionRotation.FromEulerAngles(_selectionEuler);
				if (_lastSelectedIndex > -1 && _modelItem != NULL)
                    _modelItem->setRotationAtIndex(_lastSelectedIndex, _selectionRotation);
			}
			else
			{
				Quaternion offset = Quaternion();
				offset.FromEulerAngles(_selectionEuler);
				offset = offset * _selectionRotation.Conjugate();
                rotateSelectedByOffset(offset);
			}
		}break;
		case ManipulatorType::Scale:
		{
			if (_selectedCount == 1)
			{
				_selectionScale[index] = value;
				if (_lastSelectedIndex > -1 && _modelItem != NULL)
                    _modelItem->setScaleAtIndex(_lastSelectedIndex, _selectionScale);
			}
			else
			{
				Vector3D offset = Vector3D();
				offset[index] = value - _selectionScale[index];
                scaleSelectedByOffset(offset);
			}
		}break;
		default:
			break;
	}
}

void OpenGLManipulatingController::willChangeSelection()
{
    [_kvc willChangeValueForKey:@"selectionX"];
	[_kvc willChangeValueForKey:@"selectionY"];
	[_kvc willChangeValueForKey:@"selectionZ"];
    [_kvc willChangeValueForKey:@"selectionColor"];
    [_kvc willChangeValueForKey:@"selectionColorEnabled"];
    [_kvc willChangeValueForKey:@"viewMode"];
}

void OpenGLManipulatingController::didChangeSelection()
{
    [_kvc didChangeValueForKey:@"selectionX"];
	[_kvc didChangeValueForKey:@"selectionY"];
	[_kvc didChangeValueForKey:@"selectionZ"];
    [_kvc didChangeValueForKey:@"selectionColor"];
    [_kvc didChangeValueForKey:@"selectionColorEnabled"];
    [_kvc didChangeValueForKey:@"viewMode"];
}

void OpenGLManipulatingController::setPositionRotationScale(Vector3D aPosition, Quaternion aRotation, Vector3D aScale)
{
    _modelPosition = aPosition;
	_modelRotation = aRotation;
	_modelScale = aScale;
	_modelTransform.TranslateRotateScale(aPosition, aRotation, aScale);
}
