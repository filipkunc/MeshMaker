//
//  Manipulator.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#pragma once

#include <Cocoa/Cocoa.h>
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
    unsigned int selectedIndex;
    
    Manipulator() : size(10.0f), selectedIndex(UINT_MAX) { }
    Manipulator(ManipulatorType type);
    void addWidget(const ManipulatorWidget &widget);
    void addWidgetWithAxis(Widget widget, Axis axis);
    void draw(Vector3D axisZ, Vector3D center, bool highlightAll=false);
    ManipulatorWidget &widgetAtIndex(unsigned int index);
    
    // IOpenGLSelecting
    virtual unsigned int selectableCount();
    virtual void drawForSelectionAtIndex(unsigned int index);
    virtual void selectObjectAtIndex(unsigned int index, OpenGLSelectionMode selectionMode);
};
