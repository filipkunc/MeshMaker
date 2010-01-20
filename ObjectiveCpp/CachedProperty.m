//
//  CachedProperty.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/20/10.
//  For license see LICENSE.TXT
//

#import "CachedProperty.h"

@implementation CachedProperty

@synthesize name, type;

- (id)initWithName:(NSString *)aName type:(unichar)aType
{
	self = [super init];
	if (self)
	{
		name = aName;
		[name retain];
		type = aType;
	}
	return self;
}

- (void)dealloc
{
	[name release];
	[super dealloc];
}

// this is highly experimental
- (NSCell *)cell
{
	switch (type)
	{
		case 'c': // BOOL is in runtime same as char
		{
			NSButtonCell *buttonCell = [[[NSButtonCell alloc] init] autorelease];
			[buttonCell setButtonType:NSSwitchButton];
			[buttonCell setTitle:nil];
			return buttonCell;
		}
		default:
			return nil;
	}
}

@end
