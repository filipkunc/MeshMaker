/*
 *  OpenGLSelecting.h
 *  OpenGLWidgets
 *
 *  Created by Filip Kunc on 6/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#import "Enums.h"

@protocol OpenGLSelecting

- (uint)selectableCount;
- (void)drawForSelectionAtIndex:(uint)index;
- (void)selectObjectAtIndex:(uint)index withMode:(enum OpenGLSelectionMode)selectionMode;

@optional
- (void)willSelect;
- (void)didSelect;

@end
