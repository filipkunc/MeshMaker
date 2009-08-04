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

@property (readwrite, assign) enum ManipulatorType currentManipulator;
@property (readwrite, assign) Vector3D selectionCenter;
@property (readwrite, assign) Quaternion selectionRotation;
@property (readwrite, assign) Vector3D selectionScale;
@property (readwrite, assign) BOOL toggleWhenSelecting;
@property (readwrite, assign) float selectionX, selectionY, selectionZ;
@property (readonly) NSUInteger selectedCount;

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
