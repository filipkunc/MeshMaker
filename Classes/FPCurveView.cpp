//
//  FPCurveView.m
//  MeshMaker
//
//  Created by Filip Kunc on 2/14/13.
//
//

#include "FPCurveView.h"

CurveView::CurveView()
{
    _lastPoint = NSZeroPoint;
    _lastBounds = NSZeroRect;
    _pointSize = 8.0f;
    _lastSelectedIndex = UINT_MAX;
}

void CurveView::draw(NSRect bounds)
{
    _lastBounds = bounds;
    
    NSBezierPath *pathLines = [NSBezierPath bezierPath];
    
    vector<float> &weights = Mesh2::selectionWeights();
    
    float scaleX = bounds.size.width / (float)weights.size();
    float scaleY = bounds.size.height;
    
    vector<NSRect> rects;
    
    for (unsigned int i = 0; i < weights.size(); i++)
    {
        float x = bounds.origin.x + i * scaleX;
        float y = NSMaxY(bounds) - weights[i] * scaleY;
        
        NSRect pointRect = NSMakeRect(x - _pointSize / 2.0f, y - _pointSize / 2.0f, _pointSize, _pointSize);
        rects.push_back(pointRect);
        
        if (i == 0)
            [pathLines moveToPoint:NSMakePoint(x, y)];
        else
            [pathLines lineToPoint:NSMakePoint(x, y)];
    }
    
    [[NSColor grayColor] setStroke];
    [pathLines stroke];
    
    for (unsigned int i = 0; i < rects.size(); i++)
    {
        NSRect pointRect = rects[i];
        
        if (i == _lastSelectedIndex)
        {
            [[NSColor blueColor] setFill];
            [[NSBezierPath bezierPathWithOvalInRect:pointRect] fill];
        }
        else
        {
            [[NSColor darkGrayColor] setFill];
            [[NSBezierPath bezierPathWithOvalInRect:pointRect] fill];
        }
    }
}

void CurveView::mouseMoved(NSPoint point)
{
    _lastPoint = point;
    _lastSelectedIndex = UINT_MAX;
    
    NSRect bounds = _lastBounds;
    vector<float> &weights = Mesh2::selectionWeights();
    
    float scaleX = bounds.size.width / (float)weights.size();
    float scaleY = bounds.size.height;
    
    for (unsigned int i = 0; i < weights.size(); i++)
    {
        float x = bounds.origin.x + i * scaleX;
        float y = NSMaxY(bounds) - weights[i] * scaleY;
        
        NSRect pointRect = NSMakeRect(x - _pointSize / 2.0f, y - _pointSize / 2.0f, _pointSize, _pointSize);
        
        if (_lastPoint.x >= pointRect.origin.x &&
            _lastPoint.y >= pointRect.origin.y &&
            _lastPoint.x <= pointRect.origin.x + pointRect.size.width &&
            _lastPoint.y <= pointRect.origin.y + pointRect.size.height)
        {
            _lastSelectedIndex = i;
            break;
        }
    }
}

void CurveView::mouseDragged(NSPoint point)
{
    if (_lastSelectedIndex < UINT_MAX)
    {
        NSRect bounds = _lastBounds;
        vector<float> &weights = Mesh2::selectionWeights();
        
        float scaleY = bounds.size.height;
        
        float y = NSMaxY(bounds) - weights[_lastSelectedIndex] * scaleY;
        
        y += point.y - _lastPoint.y;
        weights[_lastSelectedIndex] = (NSMaxY(bounds) - y) / scaleY;
        
        if (weights[_lastSelectedIndex] < 0.0f)
            weights[_lastSelectedIndex] = 0.0f;
        
        if (weights[_lastSelectedIndex] > 1.0f)
            weights[_lastSelectedIndex] = 1.0f;
    }
    _lastPoint = point;
}

void CurveView::mouseDown(NSPoint point)
{
    mouseMoved(point);
}

@implementation FPCurveView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        // Initialization code here.
        _view = new CurveView();
    }
    
    return self;
}

- (void)awakeFromNib
{
    NSWindow *window = [self window];
    [window setAcceptsMouseMovedEvents:YES];
}

- (BOOL)isFlipped
{
    return YES;
}

- (void)drawRect:(NSRect)dirtyRect
{
    float inflate = 10.0f;
    
    NSRect bounds = [self bounds];
    
    [[NSColor whiteColor] setFill];
    NSRectFill(bounds);
    
    bounds.origin.x += inflate;
    bounds.origin.y += inflate;
    bounds.size.width -= inflate * 2.0f;
    bounds.size.height -= inflate * 2.0f;
    
    _view->draw(bounds);
}

- (NSPoint)locationFromNSEvent:(NSEvent *)e
{
	return [self convertPoint:[e locationInWindow] fromView:nil];
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    _view->mouseMoved([self locationFromNSEvent:theEvent]);
    [self setNeedsDisplay:YES];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    _view->mouseDragged([self locationFromNSEvent:theEvent]);
    [self setNeedsDisplay:YES];
    [self.delegate weightsChanged];
}

- (void)mouseDown:(NSEvent *)theEvent
{
    _view->mouseDown([self locationFromNSEvent:theEvent]);
    [self setNeedsDisplay:YES];
}

@end
