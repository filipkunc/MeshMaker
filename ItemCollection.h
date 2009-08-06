//
//  ItemCollection.h
//  OpenGLWidgets
//
//  Created by Filip Kunc on 6/28/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Item.h"
#import "OpenGLSelecting.h"
#import "OpenGLManipulating.h"

@interface ItemCollection : NSObject <OpenGLManipulating>
{
	NSMutableArray *items;
	Vector3D *selectionCenter;
	Quaternion *selectionRotation;
	Vector3D *selectionEuler;
	Vector3D *selectionScale;
	BOOL toggleWhenSelecting;
	NSUInteger selectedCount;
	Item *lastSelected;
	enum ManipulatorType currentManipulator;
}

- (float)selectionValueAtIndex:(NSUInteger)index;
- (void)setSelectionValue:(float)value atIndex:(NSUInteger)index;
- (void)addItem:(Item *)item;
- (Item *)itemAtIndex:(NSUInteger)index;
- (NSUInteger)count;
- (void)removeItemAtIndex:(NSUInteger)index;
- (void)updateSelection;
- (void)willChangeSelection;
- (void)didChangeSelection;

@end
