/*
 *  OpenGLSelecting.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 6/21/09.
 *  For license see LICENSE.TXT
 *
 */
#pragma once

#include "Enums.h"

class IOpenGLSelecting
{
public:
    virtual ~IOpenGLSelecting() { }
    
    virtual unsigned int selectableCount() = 0;
    virtual void drawForSelectionAtIndex(unsigned int index) = 0;
    virtual void selectObjectAtIndex(unsigned int index, OpenGLSelectionMode selectionMode) = 0;
};

class IOpenGLSelectingOptional : public IOpenGLSelecting
{
public:
    virtual ~IOpenGLSelectingOptional() { }
    
    virtual void willSelectThrough(bool selectThrough) = 0;
    virtual void didSelect() = 0;
    virtual bool isObjectSelectedAtIndex(unsigned int index) = 0;
    virtual void drawAllForSelection() = 0;
    virtual bool needsCullFace() = 0;
    virtual bool useGLProject() = 0;
    virtual void glProjectSelect(int x, int y, int width, int height, OpenGLSelectionMode selectionMode) = 0;
};
