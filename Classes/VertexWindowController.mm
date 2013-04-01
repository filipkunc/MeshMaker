//
//  VertexWindowController.mm
//  MeshMaker
//
//  Created by Filip Kunc on 3/10/13.
//
//

#import "VertexWindowController.h"

@interface VertexWindowController ()
{
    IBOutlet NSMatrix *radioButtons;
}

@end

@implementation VertexWindowController

- (id)init
{
    self = [super initWithWindowNibName:@"VertexWindowController"];
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
}

- (VertexWindowMode)vertexMode
{
    NSButtonCell *cell = [radioButtons selectedCell];
    return (VertexWindowMode)cell.tag;
}

@end
