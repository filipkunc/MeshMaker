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
		case 'f': // float, 2 fraction digits max
		{
			NSNumberFormatter *formatter = [[[NSNumberFormatter alloc] init] autorelease];
			[formatter setNumberStyle:NSNumberFormatterBehavior10_4];
			[formatter setAllowsFloats:YES];
			[formatter setMinimumFractionDigits:0];
			[formatter setMaximumFractionDigits:2];
			NSTextFieldCell *textFieldCell = [[[NSTextFieldCell alloc] init] autorelease];
			[textFieldCell setFormatter:formatter];
			[textFieldCell setEditable:YES];
			return textFieldCell;
		}
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
