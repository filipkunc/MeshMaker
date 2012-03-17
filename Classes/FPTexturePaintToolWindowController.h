//
//  FPTexturePaintToolWindowController.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface FPTexturePaintToolWindowController : NSWindowController

@property (readonly) NSColor *brushColor;
@property (readonly) float brushSize;

@end