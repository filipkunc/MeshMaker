//
//  ManipulatorWidget.h
//  OpenGLWidgets
//
//  Created by Filip Kunc on 6/22/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OpenGLDrawing.h"

enum Widget
{
	WidgetLine,
	WidgetArrow,
	WidgetPlane,
	WidgetCircle,
	WidgetCube,
};

@interface ManipulatorWidget : NSObject 
{
	enum Axis axis;
	enum Widget widget;
}

@property (readonly) enum Axis axis;
@property (readonly) enum Widget widget;

- (id)initWithAxis:(enum Axis)anAxis widget:(enum Widget)aWidget;
- (void)drawCoreWithSize:(float)size;
- (void)drawWithSize:(float)size isSelected:(BOOL)isSelected isGray:(BOOL)isGray;

@end
