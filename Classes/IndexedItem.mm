//
//  IndexedItem.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 11/8/09.
//  For license see LICENSE.TXT
//

#import "IndexedItem.h"

@implementation IndexedItem

@synthesize index, item;

- (id)initWithIndex:(uint)anIndex item:(Item *)anItem
{
	self = [super init];
	if (self)
	{
		index = anIndex;
		item = anItem;
		[item retain];
	}
	return self;
}

- (void)dealloc
{
	[item release];
	[super dealloc];
}

@end
