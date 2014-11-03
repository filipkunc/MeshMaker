//
//  FPCurveView.h
//  MeshMaker
//
//  Created by Filip Kunc on 2/14/13.
//
//

#pragma once

#include <Cocoa/Cocoa.h>
#include "Mesh2.h"

#include <vector>
using namespace std;

class CurveView
{
private:
    NSPoint _lastPoint;
    NSRect _lastBounds;
    float _pointSize;
    unsigned int _lastSelectedIndex;
public:
    CurveView();

    void draw(NSRect bounds);
    void mouseMoved(NSPoint point);
    void mouseDragged(NSPoint point);
    void mouseDown(NSPoint point);
};

@protocol FPCurveViewDelegate <NSObject>

- (void)weightsChanged;

@end

@interface FPCurveView : NSView
{
    CurveView *_view;
}

@property (readonly) CurveView *view;
@property (readwrite, weak) id<FPCurveViewDelegate> delegate;

@end
