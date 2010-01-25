//
//  MyTestObject.m
//  PropertyGrid
//
//  Created by Filip Kunc on 1/23/10.
//  For license see LICENSE.TXT
//

#import "MyTestObject.h"

@implementation MyTestObject

@synthesize a, b;

- (id)init
{
	self = [super init];
	if (self)
	{
		a = 1.0f;
		b = 2.0f;
	}
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

@end
