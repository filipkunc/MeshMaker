//
//  ItemCollection.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
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
- (void)removeItemAtIndex:(uint)index;
- (void)insertItem:(Item *)item atIndex:(uint)index;
- (void)mergeSelectedItems;

@end
