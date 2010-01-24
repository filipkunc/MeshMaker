//
//  CachedProperty.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/20/10.
//  For license see LICENSE.TXT
//

#import "CachedProperty.h"

@implementation CachedProperty

@synthesize name, attributes;

- (id)initWithName:(NSString *)aName attributes:(NSString *)anAttributes
{
	self = [super init];
	if (self)
	{
		name = aName;
		[name retain];
		attributes = anAttributes;
		[attributes retain];
	}
	return self;
}

- (void)dealloc
{
	[name release];
	[attributes release];
	[super dealloc];
}

- (unichar)type
{
	return [attributes characterAtIndex:1];
}

// this is highly experimental
- (NSCell *)cell
{
	switch ([self type])
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

- (NSComparisonResult)compare:(id)otherObject 
{
    return [self.name compare:[otherObject name]];
}

@end
