//
//  Manipulator.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#pragma once

#if defined(__APPLE__)
#include <Cocoa/Cocoa.h>
#elif defined(WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <limits.h>
#endif
#include "MathDeclaration.h"
#include "Camera.h"
#include "OpenGLSelecting.h"
#include "ManipulatorWidget.h"
#include <vector>
using namespace std;

class Manipulator : public IOpenGLSelecting
{
private:
    vector<ManipulatorWidget> widgets;
public:
    Vector3D position;
    Quaternion rotation;
    float size;
    uint selectedIndex;
    
    Manipulator() : size(10.0f), selectedIndex(UINT_MAX) { }
    Manipulator(ManipulatorType type);
    void addWidget(const ManipulatorWidget &widget);
    void addWidgetWithAxis(Widget widget, Axis axis);
    void draw(Vector3D axisZ, Vector3D center, bool highlightAll=false);
    ManipulatorWidget &widgetAtIndex(uint index);
    
    // IOpenGLSelecting
    virtual uint selectableCount();
    virtual void drawForSelectionAtIndex(uint index);
    virtual void selectObjectAtIndex(uint index, OpenGLSelectionMode selectionMode);
};
