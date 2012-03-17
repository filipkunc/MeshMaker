//
//  FPTexturePaintToolWindowController.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FPTexturePaintToolWindowController.h"

@interface FPTexturePaintToolWindowController ()
{
    IBOutlet NSColorWell *brushColorWell;
    IBOutlet NSSlider *brushSizeSlider;
}

@end

@implementation FPTexturePaintToolWindowController

- (NSColor *)brushColor
{
    return [brushColorWell color];
}

- (float)brushSize
{
    return [brushSizeSlider floatValue];
}

- (id)init
{
    self = [super initWithWindowNibName:@"FPTexturePaintToolWindowController"];
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end
