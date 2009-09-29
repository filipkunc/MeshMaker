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

- (id)initWithCoder:(NSCoder *)aDecoder
{
	self = [super init];
	if (self)
	{
		items = [[aDecoder decodeObjectForKey:@"items"] retain];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:items forKey:@"items"];
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

- (void)drawAtIndex:(NSUInteger)index forSelection:(BOOL)forSelection
{
	[[self itemAtIndex:index] draw];
}

- (void)cloneAtIndex:(NSUInteger)index
{
	Item *newItem = [[self itemAtIndex:index] clone];
	[items addObject:newItem];
}

- (void)removeAtIndex:(NSUInteger)index
{
	[items removeObjectAtIndex:index];
}

- (void)mergeSelectedItems
{
	Vector3D center = Vector3D();
	int selectedCount = 0;
	
	for (int i = 0; i < [items count]; i++)
	{
		if ([self isSelectedAtIndex:i])
		{
			selectedCount++;
			center += [[self itemAtIndex:i] position];
		}
	}
	
	if (selectedCount < 2)
		return;
	
	center /= selectedCount;
	
	Item *newItem = [[Item alloc] initWithPosition:center rotation:Quaternion() scale:Vector3D(1, 1, 1)];
	Mesh *mesh = [newItem mesh];
	
	Matrix4x4 firstMatrix, itemMatrix;
	
	firstMatrix.TranslateRotateScale([newItem position],
									 [newItem rotation],
									 [newItem scale]);
	
	firstMatrix = firstMatrix.Inverse();
	
	for (int i = 0; i < [items count]; i++)
	{
		if ([self isSelectedAtIndex:i])
		{
			Item *item = [self itemAtIndex:i];
			
			itemMatrix.TranslateRotateScale([item position],
											[item rotation],
											[item scale]);
			
			Matrix4x4 finalMatrix = firstMatrix * itemMatrix;
			
			[[item mesh] transformWithMatrix:finalMatrix];
			[mesh mergeWithMesh:[item mesh]];
			[self removeAtIndex:i];
			i--;
		}
	}
	
	[newItem setSelected:YES];
	[self addItem:newItem];
}

@end