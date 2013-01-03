//
//  ManipulatorWidget.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/22/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "OpenGLDrawing.h"

class ManipulatorWidget
{
public:
    Widget widget;
    Axis axis;
    
    ManipulatorWidget(Widget aWidget, Axis anAxis) : widget(aWidget), axis(anAxis) { }
    void drawCore(float size);
    void draw(float size, bool isSelected, bool isGray, bool forSelection);
};
