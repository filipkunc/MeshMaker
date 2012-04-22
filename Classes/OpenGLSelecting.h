/*
 *  OpenGLSelecting.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 6/21/09.
 *  For license see LICENSE.TXT
 *
 */
#pragma once

#import "Enums.h"

@protocol OpenGLSelecting <NSObject>

- (uint)selectableCount;
- (void)drawForSelectionAtIndex:(uint)index;
- (void)selectObjectAtIndex:(uint)index withMode:(enum OpenGLSelectionMode)selectionMode;

@optional
- (void)willSelectThrough:(BOOL)selectThrough;
- (void)didSelect;
- (BOOL)isObjectSelectedAtIndex:(uint)index;
- (void)drawAllForSelection;
- (BOOL)needsCullFace;
- (BOOL)useGLProject;
- (void)glProjectSelectWithX:(int)x  
                           y:(int)y 
                       width:(int)width
                      height:(int)height
               selectionMode:(enum OpenGLSelectionMode)selectionMode;

@end
