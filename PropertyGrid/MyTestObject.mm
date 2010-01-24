//
//  MyTestObject.m
//  PropertyGrid
//
//  Created by Filip Kunc on 1/23/10.
//  For license see LICENSE.TXT
//

#import "MyTestObject.h"

@implementation MyTestObject

@synthesize a, b, test;

- (id)init
{
	self = [super init];
	if (self)
	{
		a = 1.0f;
		b = 2.0f;
		test.x = 3.0f;
		test.y = 4.0f;
		test.z = 5.0f;
	}
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (id)valueForKey:(NSString *)key
{
	if ([key isEqualTo:@"test"])
	{
		return [NSValue valueWithBytes:&test objCType:@encode(Vector3D)];
	}
	return [super valueForKey:key];
}

- (void)setValue:(id)value forKey:(NSString *)key
{
	if (![key isEqualTo:@"test"])
		[super setValue:value forKey:key];
}

@end
