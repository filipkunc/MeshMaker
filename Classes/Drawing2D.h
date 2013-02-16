//
//  Drawing2D.h
//  MeshMaker
//
//  Created by Filip Kunc on 2/15/13.
//
//

#pragma once

#if defined(WIN32) || defined(__linux__)

struct NSPoint
{
    float x;
    float y;
};

struct NSSize
{
    float width;
    float height;
};

struct NSRect
{
    NSPoint origin;
    NSSize size;
};

NSPoint NSMakePoint(float x, float y);
NSSize NSMakeSize(float width, float height);
NSRect NSMakeRect(float x, float y, float width, float height);
bool NSPointInRect(NSPoint point, NSRect rect);

#endif