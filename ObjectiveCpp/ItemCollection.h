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
#import "OpenGLManipulatingController.h"

@interface ItemCollection : NSObject <OpenGLManipulatingModelItem, NSCoding>
{
	NSMutableArray *items;
}

- (Item *)itemAtIndex:(uint)index;
- (void)addItem:(Item *)item;
- (void)removeItem:(Item *)item;
- (void)mergeSelectedItems;

@end
