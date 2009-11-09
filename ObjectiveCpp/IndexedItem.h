//
//  IndexedItem.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 11/8/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
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
