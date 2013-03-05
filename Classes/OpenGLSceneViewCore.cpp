//
//  OpenGLSceneViewCore.mm
//  MeshMaker
//
//  Created by Filip Kunc on 1/6/13.
//
//

#include "OpenGLSceneViewCore.h"

const float perspectiveAngle = 45.0f;
const float minDistance = 1.0f;
const float maxDistance = 500.0f;
const float planeSize = 4000.0f;

#if defined(WIN32) || defined(__linux__)

NSPoint NSMakePoint(float x, float y)
{
	NSPoint point;
	point.x = x;
	point.y = y;
	return point;
}

NSSize NSMakeSize(float width, float height)
{
	NSSize size;
	size.width = width;
	size.height = height;
	return size;
}

NSRect NSMakeRect(float x, float y, float width, float height)
{
	NSRect rect;
	rect.origin = NSMakePoint(x, y);
	rect.size = NSMakeSize(width, height);
	return rect;
}

bool NSPointInRect(NSPoint point, NSRect rect)
{
	if (point.x < rect.origin.x)
		return false;
	if (point.x > rect.origin.x + rect.size.width)
		return false;
	if (point.y < rect.origin.y)
		return false;
	if (point.y > rect.origin.y + rect.size.height)
		return false;

	return true;
}

#endif

bool OpenGLSceneViewCore::_alwaysSelectThrough = false;

OpenGLSceneViewCore::OpenGLSceneViewCore(IOpenGLSceneViewCoreDelegate *delegate)
{
    _displayed = NULL;
    _manipulated = NULL;
    
    _selectionOffset = Vector3D();
    _isManipulating = false;
    _isSelecting = false;
    _highlightCameraMode = false;
    
    _camera = new Camera();
    _camera->SetRadians(Vector2D(-45.0f * DEG_TO_RAD, 45.0f * DEG_TO_RAD));
    _camera->SetZoom(20.0f);
    
    _perspectiveRadians = Vector2D(_camera->GetRadians());
    
    _lastPoint = NSMakePoint(0, 0);
    
    _defaultManipulator = new Manipulator(ManipulatorType::Default);
    _translationManipulator = new Manipulator(ManipulatorType::Translation);
    _rotationManipulator = new Manipulator(ManipulatorType::Rotation);
    _scaleManipulator = new Manipulator(ManipulatorType::Scale);
    
    _currentManipulator = _defaultManipulator;
    
    _cameraMode = CameraMode::Perspective;
    
    _delegate = delegate;
}

OpenGLSceneViewCore::~OpenGLSceneViewCore()
{
    delete _camera;
    delete _defaultManipulator;
    delete _translationManipulator;
    delete _rotationManipulator;
    delete _scaleManipulator;
}

IOpenGLManipulating *OpenGLSceneViewCore::displayed()
{
    return _displayed;
}

void OpenGLSceneViewCore::setDisplayed(IOpenGLManipulating *displayed)
{
    _displayed = displayed;
}

IOpenGLManipulating *OpenGLSceneViewCore::manipulated()
{
    return _manipulated;
}

void OpenGLSceneViewCore::setManipulated(IOpenGLManipulating *manipulated)
{
    _manipulated = manipulated;
}

Camera *OpenGLSceneViewCore::camera()
{
    return _camera;
}

void OpenGLSceneViewCore::drawGrid(int size, int step)
{
    float dark = 0.1f;
	float light = 0.4f;
	
	glPushMatrix();
	
	if (_cameraMode == CameraMode::Front || _cameraMode == CameraMode::Back)
		glRotatef(90.0f, 1, 0, 0);
	else if (_cameraMode == CameraMode::Left || _cameraMode == CameraMode::Right)
		glRotatef(90.0f, 0, 0, 1);
	
	glBegin(GL_LINES);
	for (int x = -size; x <= size; x += step)
    {
		if (x == 0)
			glColor3f(dark, dark, dark);
		else
			glColor3f(light, light, light);
		
        glVertex3i(x, 0, -size);
        glVertex3i(x, 0, size);
	}
    for (int z = -size; z <= size; z += step)
    {
		if (z == 0)
			glColor3f(dark, dark, dark);
		else
			glColor3f(light, light, light);
		
		glVertex3i(-size, 0, z);
        glVertex3i(size, 0, z);
    }
	glEnd();
	
	glPopMatrix();
    
//    glPushMatrix();
//    glMultMatrixf(_camera->GetRotationQuaternion().Conjugate().ToMatrix());
//    
//    int index = (int)PlaneAxis::Z - (int)PlaneAxis::X;
//    
//    drawSelectionPlane(index);
//    
//    glPopMatrix();
}

NSRect OpenGLSceneViewCore::orthoManipulatorRect()
{
    return NSMakeRect(3.0f, 3.0f, 30.0f, 30.0f);
}

NSRect OpenGLSceneViewCore::currentRect()
{
    float minX = Min(_lastPoint.x, _currentPoint.x);
	float maxX = Max(_lastPoint.x, _currentPoint.x);
	float minY = Min(_lastPoint.y, _currentPoint.y);
	float maxY = Max(_lastPoint.y, _currentPoint.y);
	
	return NSMakeRect(minX, minY, maxX - minX, maxY - minY);
}

void OpenGLSceneViewCore::beginOrtho()
{
    NSRect bounds = _delegate->bounds();
	glDepthMask(GL_FALSE);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, bounds.size.width, 0, bounds.size.height, -maxDistance, maxDistance);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 1, 1, 1);
}

void OpenGLSceneViewCore::endOrtho()
{
    glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glDepthMask(GL_TRUE);
}

void OpenGLSceneViewCore::applyProjection()
{
    NSRect bounds = _delegate->bounds();
    float w_h = bounds.size.width / bounds.size.height;
    
	if (_cameraMode != CameraMode::Perspective)
	{
		float x = _camera->GetZoom() * w_h;
		float y = _camera->GetZoom();
		
		x /= 2.0f;
		y /= 2.0f;
		
		glOrtho(-x, x, -y, y, -maxDistance, maxDistance);
	}
	else
	{
		gluPerspective(perspectiveAngle, w_h, minDistance, maxDistance);
	}
}

const uint kMaxSelectedIndicesCount = 2000 * 2000;  // max width * max height resolution

uint selectedIndices[kMaxSelectedIndicesCount];

bool *OpenGLSceneViewCore::select(int x, int y, int width, int height, IOpenGLSelecting *selecting)
{
    IOpenGLSelectingOptional *optional = dynamic_cast<IOpenGLSelectingOptional *>(selecting);
    uint count = selecting->selectableCount();
    
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    setupViewportAndCamera();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    if (optional != NULL)
    {
        optional->drawAllForSelection();
    }
    else
    {
        for (uint i = 0; i < count; i++)
        {
            uint colorIndex = i + 1;
            ColorIndex(colorIndex);
            selecting->drawForSelectionAtIndex(i);
        }
    }
    
	glFinish();
    
    uint selectedIndicesCount = (uint)width * (uint)height;
    
    if (selectedIndicesCount >= kMaxSelectedIndicesCount)
        return NULL;
    
    if (selectedIndicesCount > 0)
    {
        ReadSelectedIndices(x, y, width, height, selectedIndices);
        
        bool *selected = new bool[count];
        for (uint i = 0; i < count; i++)
            selected[i] = false;
        
        for (uint i = 0; i < selectedIndicesCount; i++)
        {
            uint selectedIndex = selectedIndices[i];
            if (selectedIndex > 0)
            {
                if (selectedIndex - 1 < count)
                    selected[selectedIndex - 1] = true;
//                else
//                    NSLog(@"selectedIndex: %x", selectedIndex);
            }
        }
        
        return selected;
    }

    return NULL;
}

void OpenGLSceneViewCore::select(NSPoint point, IOpenGLSelecting *selecting, OpenGLSelectionMode selectionMode)
{
    if (selecting == NULL || selecting->selectableCount() <= 0)
		return;

	uint count = selecting->selectableCount();
    
    IOpenGLSelectingOptional *optional = dynamic_cast<IOpenGLSelectingOptional *>(selecting);
    
    if (optional != NULL)
        optional->willSelectThrough(false);
    
    _delegate->makeCurrentContext();
    
	bool *selected = select(point.x - 5, point.y - 5, 10, 10, selecting);
    if (selected != NULL)
    {
        for (uint i = 0; i < count; i++)
        {
            if (selected[i])
            {
                selecting->selectObjectAtIndex(i, selectionMode);
                break;
            }
        }
        
        delete [] selected;
    }
    
    if (optional != NULL)
        optional->didSelect();
}

void OpenGLSceneViewCore::select(NSRect rect, IOpenGLSelecting *selecting, OpenGLSelectionMode selectionMode, bool selectThrough)
{
    if (selecting == NULL || selecting->selectableCount() <= 0)
		return;
    
    uint count = selecting->selectableCount();
    
    IOpenGLSelectingOptional *optional = dynamic_cast<IOpenGLSelectingOptional *>(selecting);
    
    if (optional != NULL)
        optional->willSelectThrough(selectThrough);
    
    _delegate->makeCurrentContext();
    
    if (optional != NULL)
    {
        if (optional->useGLProject())
        {
            optional->glProjectSelect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, selectionMode);
            optional->didSelect();
            return;
        }
    }
    
    bool *selected = NULL;
    
    if (optional != NULL && optional->needsCullFace())
    {
        glCullFace(GL_CCW);
        glEnable(GL_CULL_FACE);
        
        bool *selected1 = select(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, selecting);
        
        glDisable(GL_CULL_FACE);
        
        bool *selected2 = select(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, selecting);
        
        if (selected1 != NULL)
        {
            selected = selected1;
        }
        if (selected2 != NULL)
        {
            if (selected1 != NULL)
            {
                for (uint i = 0; i < count; i++)
                {
					selected[i] = selected[i] || selected2[i];
                }
                delete [] selected2;
            }
            else
            {
                selected = selected2;
            }
        }
    }
    else
    {
        selected = select(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, selecting);
    }
    
    if (selected != NULL)
    {
        for (uint i = 0; i < count; i++)
        {
            if (selected[i])
                selecting->selectObjectAtIndex(i, selectionMode);
        }
        
        delete [] selected;
    }
    
    if (optional != NULL)
        optional->didSelect();
}

Vector3D OpenGLSceneViewCore::positionInSpaceByPoint(NSPoint point)
{
    int viewport[4];
    double modelview[16];
    double projection[16];
    float winX, winY, winZ;
    double posX = 0.0, posY = 0.0, posZ = 0.0;
    
    _delegate->makeCurrentContext();
	
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
	
    winX = point.x;
    winY = point.y;
    glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	
	return Vector3D((float)posX, (float)posY, (float)posZ);
}

void OpenGLSceneViewCore::drawSelectionPlane(int index)
{
    Vector3D position = _manipulated->selectionCenter();
	
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	DrawSelectionPlane((PlaneAxis)((int)PlaneAxis::X + index));
	glPopMatrix();
}

Vector3D OpenGLSceneViewCore::positionFromAxisPoint(Axis axis, NSPoint point)
{
	DrawPlane(_camera->GetAxisX(), _camera->GetAxisY(), planeSize);
	
	Vector3D position = positionInSpaceByPoint(point);
	Vector3D result = _manipulated->selectionCenter();
	result[(int)axis] = position[(int)axis];
	return result;
}

Vector3D OpenGLSceneViewCore::positionFromRotatedAxisPoint(Axis axis, NSPoint point, Quaternion rotation)
{
    DrawPlane(_camera->GetAxisX(), _camera->GetAxisY(), planeSize);
	
	Vector3D position = positionInSpaceByPoint(point);
	Vector3D result = _manipulated->selectionCenter();
	position = rotation.Conjugate().ToMatrix().Transform(position);
	result[(int)axis] = position[(int)axis];
	return result;
}

Vector3D OpenGLSceneViewCore::positionFromPlaneAxis(PlaneAxis plane, NSPoint point)
{
    int index = (int)plane - (int)PlaneAxis::X;
    drawSelectionPlane(index);
    Vector3D position = positionInSpaceByPoint(point);
	Vector3D result = position;
	result[index] = _manipulated->selectionCenter()[index];
	return result;
}

Vector3D OpenGLSceneViewCore::addVertexPositionFromPoint(NSPoint point)
{
    _delegate->makeCurrentContext();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadMatrixf(_camera->GetViewMatrix());
    
    Vector3D position = _manipulated->selectionCenter();
	
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
    glMultMatrixf(_camera->GetRotationQuaternion().Conjugate().ToMatrix());
	DrawSelectionPlane(PlaneAxis::Z);
	glPopMatrix();
    
    position = positionInSpaceByPoint(point);
    
    return position;
}

Vector3D OpenGLSceneViewCore::translationFromPoint(NSPoint point)
{
	_delegate->makeCurrentContext();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadMatrixf(_camera->GetViewMatrix());
	
	Vector3D position = _manipulated->selectionCenter();
	uint selectedIndex = _currentManipulator->selectedIndex;
	
	if (selectedIndex <= (uint)Axis::Z)
        return positionFromAxisPoint((Axis)selectedIndex, point);
	if (selectedIndex >= (uint)PlaneAxis::X && selectedIndex <= (uint)PlaneAxis::Z)
        return positionFromPlaneAxis((PlaneAxis)selectedIndex, point);
	
	return position;
}

Vector3D OpenGLSceneViewCore::scaleFromPoint(NSPoint point, Vector3D &lastPosition)
{
	_delegate->makeCurrentContext();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadMatrixf(_camera->GetViewMatrix());
	
	Vector3D position = _manipulated->selectionCenter();
	uint selectedIndex = _currentManipulator->selectedIndex;
	
	Vector3D scale = Vector3D();
	
	if (selectedIndex < UINT_MAX)
	{
		ManipulatorWidget &selectedWidget = _currentManipulator->widgetAtIndex(selectedIndex);
		Axis selectedAxis = selectedWidget.axis;
		if (selectedAxis >= Axis::X && selectedAxis <= Axis::Z)
		{
			position = positionFromRotatedAxisPoint(selectedAxis, point, _manipulated->selectionRotation());
			scale = position - lastPosition;
		}
		else if (selectedAxis == Axis::Center)
		{
			position = positionFromPlaneAxis(PlaneAxis::Y, point);
			scale = position - lastPosition;
            scale.y = scale.x;
            scale.z = scale.x;
		}
		
		lastPosition = position;
		scale *= 2.0f;
	}
    
	return scale;
}

Quaternion OpenGLSceneViewCore::rotationFromPoint(NSPoint point, Vector3D &lastPosition)
{
	_delegate->makeCurrentContext();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadMatrixf(_camera->GetViewMatrix());
	
	Quaternion quaternion;
	Vector3D position;
	float angle;
	
	uint selectedIndex = _currentManipulator->selectedIndex;
	
	position = this->positionFromPlaneAxis((PlaneAxis)(selectedIndex + 3), point);
	position -= _manipulated->selectionCenter();
	
	switch ((Axis)selectedIndex)
    {
		case Axis::X:
			angle = atan2f(position.y, position.z) - atan2f(lastPosition.y, lastPosition.z);
			quaternion.FromAngleAxis(-angle, Vector3D(1, 0, 0));
			break;
		case Axis::Y:
			angle = atan2f(position.x, position.z) - atan2f(lastPosition.x, lastPosition.z);
			quaternion.FromAngleAxis(angle, Vector3D(0, 1, 0));
			break;
		case Axis::Z:
			angle = atan2f(position.x, position.y) - atan2f(lastPosition.x, lastPosition.y);
			quaternion.FromAngleAxis(-angle, Vector3D(0, 0, 1));
			break;
        default:
            break;
	}
	
	lastPosition = position;
	return quaternion;
}

void OpenGLSceneViewCore::reshapeViewport()
{
    NSRect bounds = _delegate->bounds();
	glViewport(0, 0, bounds.size.width, bounds.size.height);
}

void OpenGLSceneViewCore::setupViewportAndCamera()
{
	reshapeViewport();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    applyProjection();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(_camera->GetViewMatrix());
}

ManipulatorType OpenGLSceneViewCore::currentManipulator()
{
    if (_currentManipulator == _defaultManipulator)
		return ManipulatorType::Default;
	if (_currentManipulator == _translationManipulator)
		return ManipulatorType::Translation;
	if (_currentManipulator == _rotationManipulator)
		return ManipulatorType::Rotation;
	if (_currentManipulator == _scaleManipulator)
		return ManipulatorType::Scale;
	return ManipulatorType::Default;
}

void OpenGLSceneViewCore::setCurrentManipulator(ManipulatorType manipulator)
{
    switch (manipulator)
	{
		case ManipulatorType::Default:
			_currentManipulator = _defaultManipulator;
			break;
		case ManipulatorType::Translation:
			_currentManipulator = _translationManipulator;
			break;
		case ManipulatorType::Rotation:
			_currentManipulator = _rotationManipulator;
			break;
		case ManipulatorType::Scale:
			_currentManipulator = _scaleManipulator;
			break;
		default:
			break;
	}
    _delegate->setNeedsDisplay();
}

CameraMode OpenGLSceneViewCore::cameraMode()
{
    return _cameraMode;
}

void OpenGLSceneViewCore::setCameraMode(CameraMode value)
{
    if (_cameraMode == CameraMode::Perspective)
	{
		_perspectiveRadians = _camera->GetRadians();
	}
	_cameraMode = value;
	switch (_cameraMode)
	{
		case CameraMode::Perspective:
			_camera->SetRadians(_perspectiveRadians);
			break;
		case CameraMode::Top:
			_camera->SetRadians(Vector2D(-90.0f * DEG_TO_RAD, 0));
			break;
		case CameraMode::Bottom:
			_camera->SetRadians(Vector2D(90.0f * DEG_TO_RAD, 0));
			break;
		case CameraMode::Left:
			_camera->SetRadians(Vector2D(0, -90.0f * DEG_TO_RAD));
			break;
		case CameraMode::Right:
			_camera->SetRadians(Vector2D(0, 90.0f * DEG_TO_RAD));
			break;
		case CameraMode::Front:
			_camera->SetRadians(Vector2D());
			break;
		case CameraMode::Back:
			_camera->SetRadians(Vector2D(0, 180.0f * DEG_TO_RAD));
			break;
		default:
			break;
	}
    _delegate->setNeedsDisplay();
}

void OpenGLSceneViewCore::drawManipulatedAndDisplatedForSelection(bool forSelection)
{
    if (_displayed != _manipulated)
    {
        if (_displayed != NULL)
            _displayed->drawForSelection(forSelection);
    }
    if (_manipulated != NULL)
        _manipulated->drawForSelection(forSelection);
}

void OpenGLSceneViewCore::drawOrthoDefaultManipulator()
{
    beginOrtho();
	glPushMatrix();
	glTranslatef(18.0f, 18.0f, 0.0f);
	glMultMatrixf(_camera->GetRotationQuaternion().ToMatrix());
    _defaultManipulator->position = Vector3D();
    _defaultManipulator->size = 15.0f;
    _defaultManipulator->draw(_camera->GetAxisZ(), _defaultManipulator->position, _highlightCameraMode);
	glPopMatrix();
    endOrtho();
}

void OpenGLSceneViewCore::drawCurrentManipulator()
{
    if (_manipulated != NULL && _manipulated->selectedCount() > 0)
	{
        _currentManipulator->position = _manipulated->selectionCenter();
        
		if (_cameraMode == CameraMode::Perspective)
        {
            Vector3D manipulatorPosition = _manipulated->selectionCenter();
            Vector3D cameraPosition = -_camera->GetCenter() + (_camera->GetAxisZ() * _camera->GetZoom());
            
            float distance = cameraPosition.Distance(manipulatorPosition);
            
            _currentManipulator->size = distance * 0.15f;
        }
		else
        {
            _currentManipulator->size = _camera->GetZoom() * 0.17f;
        }
        
        _scaleManipulator->rotation = _manipulated->selectionRotation();
        if (_currentManipulator != _defaultManipulator)
            _currentManipulator->draw(_camera->GetAxisZ(), _manipulated->selectionCenter());
	}
}

void OpenGLSceneViewCore::drawSelectionRect()
{
    if (_isSelecting)
	{
        beginOrtho();
		glDisable(GL_TEXTURE_2D);
		float color[4] = { 0.2f, 0.4f, 1.0f, 0.0f };
		color[3] = 0.2f;
		glColor4fv(color);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glRectf(_lastPoint.x, _lastPoint.y, _currentPoint.x, _currentPoint.y);
		color[3] = 0.9f;
		glColor4fv(color);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glRectf(_lastPoint.x, _lastPoint.y, _currentPoint.x, _currentPoint.y);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        endOrtho();
	}
}

void OpenGLSceneViewCore::draw()
{
#if defined(__APPLE__) || defined(SHADERS)
    ShaderProgram::resetProgram();
#endif
	float clearColor = 0.6f;
	glClearColor(clearColor, clearColor, clearColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

	setupViewportAndCamera();
    drawGrid(10, 2);
    drawManipulatedAndDisplatedForSelection(false);
    
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
    
    drawCurrentManipulator();
    
    drawOrthoDefaultManipulator();
    drawSelectionRect();
	
    glEnable(GL_DEPTH_TEST);
}

void OpenGLSceneViewCore::mouseDown(NSPoint point, bool alt)
{
    _lastPoint = point;
    _isPainting = false;
    
    if (alt)
	{
		_isManipulating = _isSelecting = false;
		return;
	}
    
	if (_highlightCameraMode)
	{
		switch (_cameraMode)
		{
			case CameraMode::Top:
				setCameraMode(CameraMode::Bottom);
				break;
			case CameraMode::Bottom:
				setCameraMode(CameraMode::Top);
				break;
			case CameraMode::Left:
				setCameraMode(CameraMode::Right);
				break;
			case CameraMode::Right:
				setCameraMode(CameraMode::Left);
				break;
			case CameraMode::Front:
				setCameraMode(CameraMode::Back);
				break;
			case CameraMode::Back:
                setCameraMode(CameraMode::Front);
				break;
			default:
				break;
		}
	}
    else if (_delegate->texturePaintEnabled())
    {
        _isPainting = true;
        //[self paintOnTextureWithFirstPoint:lastPoint secondPoint:lastPoint];
        return;
    }
    else if (_delegate->addVertexEnabled())
    {
        Vector3D position = addVertexPositionFromPoint(point);
        _delegate->addVertex(position);
    }
	else if (_manipulated != NULL && _manipulated->selectedCount() > 0 && _currentManipulator->selectedIndex < UINT_MAX)
	{
		if (_currentManipulator == _translationManipulator)
		{
			_selectionOffset = translationFromPoint(_lastPoint);
			_selectionOffset -= _manipulated->selectionCenter();
			_isManipulating = true;
		}
		else if (_currentManipulator == _rotationManipulator)
		{
            rotationFromPoint(_lastPoint, _selectionOffset);
			_isManipulating = true;
		}
		else if (_currentManipulator == _scaleManipulator)
		{
            scaleFromPoint(_lastPoint, _selectionOffset);
			_isManipulating = true;
		}

        if (_isManipulating)
            _delegate->manipulationStarted();
	}
	else
	{
		_isSelecting = true;
	}
}

void OpenGLSceneViewCore::mouseMoved(NSPoint point)
{
    _highlightCameraMode = false;
	_currentPoint = point;
	if (_manipulated != NULL && _manipulated->selectedCount() > 0)
	{
		if (!_isManipulating)
		{
            _currentManipulator->selectedIndex = UINT_MAX;
            _currentManipulator->position = _manipulated->selectionCenter();
            if (_currentManipulator != _defaultManipulator)
            {
                select(_currentPoint, _currentManipulator, OpenGLSelectionMode::Add);
                _delegate->setNeedsDisplay();
            }
		}
	}
	
	if (_currentManipulator->selectedIndex == UINT_MAX)
	{
		if (NSPointInRect(_currentPoint, orthoManipulatorRect()))
			_highlightCameraMode = true;
        _delegate->setNeedsDisplay();
	}
}

void OpenGLSceneViewCore::mouseExited()
{
    _highlightCameraMode = false;
    _delegate->setNeedsDisplay();
}

void OpenGLSceneViewCore::mouseUp(NSPoint point, bool alt, bool cmd, bool ctrl, bool shift, int clickCount)
{
    _isPainting = false;
    
	_currentPoint = point;
	
	if (_isManipulating)
	{
        _delegate->manipulationEnded();
        _isManipulating = false;
	}

	if (_isSelecting)
	{
		_isSelecting = false;

		if (_manipulated != NULL)
		{
			OpenGLSelectionMode selectionMode = OpenGLSelectionMode::Add;
	        
			if (cmd)
				selectionMode = OpenGLSelectionMode::Invert;
			else if (shift)
				selectionMode = OpenGLSelectionMode::Add;
			else
				_manipulated->changeSelection(false);
			
			NSRect rect = currentRect();
			if (clickCount <= 1 && rect.size.width > 5.0f && rect.size.height > 5.0f)
			{
				select(rect, _manipulated, selectionMode, ctrl || _alwaysSelectThrough);
			}
			else
			{
				if (clickCount == 2)
				{
					if (selectionMode == OpenGLSelectionMode::Invert)
						selectionMode = OpenGLSelectionMode::InvertExpand;
					else
						selectionMode = OpenGLSelectionMode::Expand;
				}
	            
				select(_currentPoint, _manipulated, selectionMode);
			}
			
			_delegate->selectionChanged();
		}

        _delegate->setNeedsDisplay();
	}
}

void OpenGLSceneViewCore::mouseDragged(NSPoint point, bool alt, bool cmd)
{
    _currentPoint = point;
	float deltaX = _currentPoint.x - _lastPoint.x;
	float deltaY = _currentPoint.y - _lastPoint.y;
    
	if (alt && cmd)
	{
        NSRect bounds = _delegate->bounds();
		float w = bounds.size.width;
		float h = bounds.size.height;
		float sensitivity = (w + h) / 2.0f;
		sensitivity = 1.0f / sensitivity;
        sensitivity *= _camera->GetZoom() * 1.12f;
		_camera->LeftRight(-deltaX * sensitivity);
		_camera->UpDown(deltaY * sensitivity);
		
		_lastPoint = _currentPoint;
        _delegate->setNeedsDisplay();
	}
	else if (alt)
	{
		if (_cameraMode == CameraMode::Perspective)
		{
			_lastPoint = _currentPoint;
			const float sensitivity = 0.005f;
			_camera->RotateLeftRight(deltaX * sensitivity);
			_camera->RotateUpDown(-deltaY * sensitivity);
            _delegate->setNeedsDisplay();
		}
	}
	else if (_delegate->texturePaintEnabled())
    {
        //[self paintOnTextureWithFirstPoint:lastPoint secondPoint:currentPoint];
        _lastPoint = _currentPoint;
    }
    else if (_isManipulating)
	{
		_lastPoint = _currentPoint;
		if (_currentManipulator == _translationManipulator)
		{
			Vector3D move = translationFromPoint(_currentPoint);
			move -= _selectionOffset;
			move -= _manipulated->selectionCenter();
            _manipulated->moveSelectedByOffset(move);
            _delegate->setNeedsDisplay();
		}
		else if (_currentManipulator == _rotationManipulator)
		{
			Quaternion rotation = rotationFromPoint(_currentPoint, _selectionOffset);
            _manipulated->rotateSelectedByOffset(rotation);
            _delegate->setNeedsDisplay();
		}
		else if (_currentManipulator == _scaleManipulator)
		{
			Vector3D scale = scaleFromPoint(_currentPoint, _selectionOffset);
            _manipulated->scaleSelectedByOffset(scale);
			_delegate->setNeedsDisplay();
		}
	}
	else if (_isSelecting)
	{
        _delegate->setNeedsDisplay();
	}
}

void OpenGLSceneViewCore::otherMouseDown(NSPoint point)
{
    _lastPoint = point;
}

void OpenGLSceneViewCore::otherMouseDragged(NSPoint point, bool alt)
{
    _currentPoint = point;
	float deltaX = _currentPoint.x - _lastPoint.x;
	float deltaY = _currentPoint.y - _lastPoint.y;
	
	if (alt)
	{
		NSRect bounds = _delegate->bounds();
		float w = bounds.size.width;
		float h = bounds.size.height;
		float sensitivity = (w + h) / 2.0f;
		sensitivity = 1.0f / sensitivity;
		_camera->LeftRight(-deltaX * _camera->GetZoom() * sensitivity);
		_camera->UpDown(deltaY * _camera->GetZoom() * sensitivity);
		
		_lastPoint = _currentPoint;
        _delegate->setNeedsDisplay();
	}
}

void OpenGLSceneViewCore::rightMouseDown(NSPoint point)
{
    _lastPoint = point;
}

void OpenGLSceneViewCore::rightMouseDragged(NSPoint point, bool alt)
{
    _currentPoint = point;
	float deltaY = _currentPoint.y - _lastPoint.y;
	
	if (alt)
	{
		float sensitivity = _camera->GetZoom() * 0.02f;
		
		_camera->Zoom(-deltaY * sensitivity);
		
		_lastPoint = _currentPoint;
        _delegate->setNeedsDisplay();
	}
}

void OpenGLSceneViewCore::scrollWheel(float deltaX, float deltaY, bool alt, bool cmd)
{
 	if (alt && cmd)
	{
		NSRect bounds = _delegate->bounds();
		float w = bounds.size.width;
		float h = bounds.size.height;
		float sensitivity = (w + h) / 6.0f;
		sensitivity = 1.0f / sensitivity;
		_camera->LeftRight(-deltaX * _camera->GetZoom() * sensitivity);
		_camera->UpDown(-deltaY * _camera->GetZoom() * sensitivity);
        _delegate->setNeedsDisplay();
	}
	else if (alt)
	{
		if (_cameraMode == CameraMode::Perspective)
		{
			const float sensitivity = 0.02f;
			_camera->RotateLeftRight(-deltaX * sensitivity);
			_camera->RotateUpDown(-deltaY * sensitivity);
            _delegate->setNeedsDisplay();
		}
	}
	else
	{
		float sensitivity = _camera->GetZoom() * 0.02f;
		_camera->Zoom(deltaY * sensitivity);
        _delegate->setNeedsDisplay();
	}
}
