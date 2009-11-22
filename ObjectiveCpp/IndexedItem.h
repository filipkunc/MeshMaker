//
//  IndexedItem.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 11/8/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "Item.h"

@interface IndexedItem : NSObject 
{
	uint index;
	Item *item;
}

@property (readonly, assign) uint index;
@property (readonly, retain) Item *item;

- (id)initWithIndex:(uint)anIndex item:(Item *)anItem;

@end
