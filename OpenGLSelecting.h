/*
 *  OpenGLSelecting.h
 *  OpenGLWidgets
 *
 *  Created by Filip Kunc on 6/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

@protocol OpenGLSelecting

- (NSUInteger)selectableCount;
- (void)drawForSelectionAtIndex:(NSUInteger)index;
- (void)selectObjectAtIndex:(NSUInteger)index;

@end
