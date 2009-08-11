//
//  ItemCollection.mm
//  OpenGLWidgets
//
//  Created by Filip Kunc on 6/28/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <GLUT/glut.h>
#import "ItemCollection.h"

@implementation ItemCollection

- (id)init
{
	self = [super init];
	if (self)
	{
		items = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)dealloc
{
	[items release];
	[super dealloc];
}

- (Item *)itemAtIndex:(NSUInteger)index
{
	return (Item *)[items objectAtIndex:index];
}

- (void)addItem:(Item *)item
{
	[items addObject:item];
}

- (NSUInteger)count
{
	return [items count];
}

- (Vector3D)positionAtIndex:(NSUInteger)index
{
	return [[self itemAtIndex:index] position];
}

- (Quaternion)rotationAtIndex:(NSUInteger)index
{
	return [[self itemAtIndex:index] rotation];
}

- (Vector3D)scaleAtIndex:(NSUInteger)index
{
	return [[self itemAtIndex:index] scale];
}

- (void)setPosition:(Vector3D)position atIndex:(NSUInteger)index
{
	[[self itemAtIndex:index] setPosition:position];
}

- (void)setRotation:(Quaternion)rotation atIndex:(NSUInteger)index
{
	[[self itemAtIndex:index] setRotation:rotation];
}

- (void)setScale:(Vector3D)scale atIndex:(NSUInteger)index
{
	[[self itemAtIndex:index] setScale:scale];
}

- (void)moveByOffset:(Vector3D)offset atIndex:(NSUInteger)index
{
	[[self itemAtIndex:index] moveByOffset:offset];
}

- (void)rotateByOffset:(Quaternion)offset atIndex:(NSUInteger)index
{
	[[self itemAtIndex:index] rotateByOffset:offset];
}

- (void)scaleByOffset:(Vector3D)offset atIndex:(NSUInteger)index
{
	[[self itemAtIndex:index] scaleByOffset:offset];
}

- (BOOL)isSelectedAtIndex:(NSUInteger)index
{
	return [[self itemAtIndex:index] selected];
}

- (void)setSelected:(BOOL)selected atIndex:(NSUInteger)index
{
	[[self itemAtIndex:index] setSelected:selected];
}

- (void)drawAtIndex:(NSUInteger)index
{
	[[self itemAtIndex:index] draw];
}

- (void)removeAtIndex:(NSUInteger)index
{
	[items removeObjectAtIndex:index];
}

@end