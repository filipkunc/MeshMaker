//
//  Manipulator.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "Camera.h"
#import "OpenGLSelecting.h"
#import "ManipulatorWidget.h"

@interface Manipulator : NSObject <OpenGLSelecting>
{
	NSMutableArray *widgets;
	Vector3D *position;
	Quaternion *rotation;
	float size;
	int selectedIndex;
}

@property (readwrite, assign) Vector3D position;
@property (readwrite, assign) Quaternion rotation;
@property (readwrite, assign) float size;
@property (readwrite, assign) int selectedIndex;

- (id)initWithManipulatorType:(enum ManipulatorType)type;
- (void)addWidget:(ManipulatorWidget *)widget;
- (void)addWidgetWithAxis:(enum Axis)anAxis widget:(enum Widget)aWidget;
- (void)drawWithAxisZ:(Vector3D)axisZ center:(Vector3D)center;
- (void)drawWithAxisZ:(Vector3D)axisZ center:(Vector3D)center highlightAll:(BOOL)higlightAll;
- (ManipulatorWidget *)widgetAtIndex:(int)index;

@end
