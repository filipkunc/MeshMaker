//
//  Item.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#include "OpenGLDrawing.h"
#include "Item.h"

Item::Item(Mesh2 *aMesh)
{
    scale = Vector3D(1, 1, 1);
    mesh = aMesh;
    selected = false;
    visible = true;
    _viewMode = ViewModeSolidFlat;
}

Item::~Item()
{
    delete mesh;
}

Item::Item(MemoryReadStream *stream)
{
    stream->readBytes(&position, sizeof(Vector3D));
    stream->readBytes(&rotation, sizeof(Quaternion));
    stream->readBytes(&scale, sizeof(Vector3D));
    stream->readBytes(&selected, sizeof(BOOL));
    
    visible = true;
    
    mesh = new Mesh2(stream);
}

void Item::encode(MemoryWriteStream *stream)
{
    stream->writeBytes(&position, sizeof(Vector3D));
    stream->writeBytes(&rotation, sizeof(Quaternion));
    stream->writeBytes(&scale, sizeof(Vector3D));
    stream->writeBytes(&selected, sizeof(BOOL));
	
    mesh->encode(stream);
}

Matrix4x4 Item::transform()
{
    Matrix4x4 m;
    m.TranslateRotateScale(position, rotation, scale);
    return m;
}

void Item::drawForSelection(bool forSelection)
{
    if (visible)
	{
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		Matrix4x4 rotationMatrix = rotation.ToMatrix();
		glMultMatrixf(rotationMatrix);
        if (forSelection)
        {
            mesh->draw(ViewModeSolidFlat, scale, selected, forSelection);
        }
        else
        {
            if (_viewMode == ViewModeMixedWireSolid)
            {
                glDisable(GL_DEPTH_TEST);
                mesh->draw(_viewMode, scale, selected, forSelection);
                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                mesh->draw(_viewMode, scale, selected, forSelection);
            }
        }
		glPopMatrix();
	}
}

void Item::moveByOffset(Vector3D offset)
{
    position += offset;
}

void Item::rotateByOffset(Quaternion offset)
{
    rotation = offset * rotation;
}

void Item::scaleByOffset(Vector3D offset)
{
    scale += offset;
}

Item *Item::duplicate()
{
    Item *newItem = new Item(new Mesh2());
    
    newItem->position = position;
    newItem->rotation = rotation;
    newItem->scale = scale;
    
    newItem->mesh->merge(mesh);
    newItem->mesh->setColor(mesh->color());
    newItem->selected = selected;
	
	return newItem;
}

void Item::setPositionToGeometricCenter()
{
    Vector3D center = Vector3D();
    
    for (VertexNode *node = mesh->vertices().begin(), *end = mesh->vertices().end(); node != end; node = node->next())
        center += node->data().position;
    
	if (mesh->vertices().count() > 0)
		center /= (float)mesh->vertices().count();
    
    Matrix4x4 inverseTranslate;
    inverseTranslate.Translate(-center);
    mesh->transformAll(inverseTranslate);
    
    position = center;
}

ViewMode Item::viewMode()
{
    return _viewMode;
}

void Item::setViewMode(ViewMode viewMode)
{
    _viewMode = viewMode;
}

uint Item::count()
{
    return mesh->selectedCount();
}

bool Item::isSelectedAtIndex(uint index)
{
    return mesh->isSelectedAtIndex(index);
}

void Item::setSelectedAtIndex(uint index, bool selected)
{
    mesh->setSelectedAtIndex(selected, index);
}

void Item::expandSelectionFromIndex(uint index, bool invert)
{
    mesh->expandSelectionFromIndex(index, invert);
}

void Item::duplicateSelected()
{
    mesh->duplicateSelectedTriangles();
}

void Item::removeSelected()
{
    mesh->removeSelected();
}

void Item::hideSelected()
{
    mesh->hideSelected();
}

void Item::unhideAll()
{
    mesh->unhideAll();
}

Vector4D Item::selectionColor()
{
    return mesh->color();
}

void Item::setSelectionColor(Vector4D color)
{
    mesh->setColor(color);
}

void Item::willSelectThrough(bool selectThrough)
{
    Mesh2::setSelectThrough(selectThrough);
}

void Item::didSelect()
{
    mesh->resetTriangleCache();
    mesh->computeSoftSelection();
}

bool Item::needsCullFace()
{
    if (mesh->selectionMode() == MeshSelectionModeTriangles && Mesh2::selectThrough())
        return true;
    return false;
}

void Item::getSelectionCenterRotationScale(Vector3D &center, Quaternion &rotation, Vector3D &scale)
{
    mesh->getSelectionCenterRotationScale(center, rotation, scale);
}

void Item::transformSelectedByMatrix(Matrix4x4 &matrix)
{
    mesh->transformSelected(matrix);
}

void Item::drawAllForSelection(bool forSelection)
{
    if (forSelection)
    {
        mesh->drawAll(ViewModeSolidFlat, forSelection);
    }
    else
    {
        if (_viewMode == ViewModeMixedWireSolid)
        {
            glDisable(GL_DEPTH_TEST);
            mesh->drawAll(_viewMode, forSelection);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            mesh->drawAll(_viewMode, forSelection);
        }
    }
}

bool Item::useGLProject()
{
    return mesh->useGLProject();
}

void Item::glProjectSelect(int x, int y, int width, int height, Matrix4x4 &matrix, OpenGLSelectionMode selectionMode)
{
    mesh->glProjectSelect(x, y, width, height, matrix, selectionMode);
}
