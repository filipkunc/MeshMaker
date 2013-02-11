//
//  FPSelectionWindowController.m
//  MeshMaker
//
//  Created by Filip Kunc on 2/10/13.
//
//

#import "FPSelectionWindowController.h"

@interface FPSelectionWindowController ()

@end

@implementation FPSelectionWindowController

- (BOOL)selectThrough
{
    return self.delegate.selectThrough;
}

- (void)setSelectThrough:(BOOL)selectThrough
{
    self.delegate.selectThrough = selectThrough;
}

- (BOOL)useSoftSelection
{
    return self.delegate.useSoftSelection;
}

- (void)setUseSoftSelection:(BOOL)useSoftSelection
{
    self.delegate.useSoftSelection = useSoftSelection;
}

- (id)init
{
    self = [super initWithWindowNibName:@"FPSelectionWindowController"];
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end
