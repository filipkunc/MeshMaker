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

@interface ItemCollection : NSObject <OpenGLManipulatingModel>
{
	NSMutableArray *items;
}

- (Item *)itemAtIndex:(NSUInteger)index;
- (void)addItem:(Item *)item;
- (void)mergeSelectedItems;

@end
