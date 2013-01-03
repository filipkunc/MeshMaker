//
//  OpenGLSelecting.mm
//  MeshMaker
//
//  Created by Filip Kunc on 1/3/13.
//
//

#import "OpenGLSelecting.h"

@implementation OpenGLSelectingWrapper

- (uint)selectableCount
{
    return _cppSelecting->selectableCount();
}

- (void)drawForSelectionAtIndex:(uint)index
{
    _cppSelecting->drawForSelectionAtIndex(index);
}

- (void)selectObjectAtIndex:(uint)index withMode:(enum OpenGLSelectionMode)selectionMode
{
    _cppSelecting->selectObjectAtIndex(index, selectionMode);
}

@end
