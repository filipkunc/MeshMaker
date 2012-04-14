//
//  FPTexturePaintToolWindowController.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/17/12.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>

@interface FPTexturePaintToolWindowController : NSWindowController

@property (readonly) NSColor *brushColor;
@property (readonly) float brushSize;

@end