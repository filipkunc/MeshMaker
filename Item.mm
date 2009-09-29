//
//  Item.mm
//  OpenGLWidgets
//
//  Created by Filip Kunc on 6/28/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <GLUT/glut.h>
#import "Item.h"

@implementation Item

- (Vector3D)position
{
	return *position;
}

- (void)setPosition:(Vector3D)aPosition
{
	*position = aPosition;
}

- (Quaternion)rotation
{
	return *rotation;
}

- (void)setRotation:(Quaternion)aRotation
{
	*rotation = aRotation;
}

- (Vector3D)scale
{
	return *scale;
}

- (void)setScale:(Vector3D)aScale
{
	*scale = aScale;
}

@synthesize selected, mesh;

- (id)init
{
	self = [super init];
	if (self)
	{
		position = new Vector3D();
		rotation = new Quaternion();
		scale = new Vector3D(1, 1, 1);
		mesh = [[Mesh alloc] init];
		selected = NO;
	}
	return self;
}

- (id)initWithPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale
{
	self = [self init];
	if (self)
	{
		*position = aPosition;
		*rotation = aRotation;
		*scale = aScale;
	}
	return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
	self = [super init];
	if (self)
	{
		position = new Vector3D();
		position->x = [aDecoder decodeFloatForKey:@"positionX"];
		position->y = [aDecoder decodeFloatForKey:@"positionY"];
		position->z = [aDecoder decodeFloatForKey:@"positionZ"];
		
		rotation = new Quaternion();
		rotation->x = [aDecoder decodeFloatForKey:@"rotationX"];
		rotation->y = [aDecoder decodeFloatForKey:@"rotationY"];
		rotation->z = [aDecoder decodeFloatForKey:@"rotationZ"];
		rotation->w = [aDecoder decodeFloatForKey:@"rotationW"];
		
		scale = new Vector3D();
		scale->x = [aDecoder decodeFloatForKey:@"scaleX"];
		scale->y = [aDecoder decodeFloatForKey:@"scaleY"];
		scale->z = [aDecoder decodeFloatForKey:@"scaleZ"];
		
		selected = [aDecoder decodeBoolForKey:@"selected"];
		
		mesh = [[aDecoder decodeObjectForKey:@"mesh"] retain];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{	
	[aCoder encodeFloat:position->x forKey:@"positionX"];
	[aCoder encodeFloat:position->y forKey:@"positionY"];
	[aCoder encodeFloat:position->z forKey:@"positionZ"];
	
	[aCoder encodeFloat:rotation->x forKey:@"rotationX"];
	[aCoder encodeFloat:rotation->y forKey:@"rotationY"];
	[aCoder encodeFloat:rotation->z forKey:@"rotationZ"];
	[aCoder encodeFloat:rotation->w forKey:@"rotationW"];
	
	[aCoder encodeFloat:scale->x forKey:@"scaleX"];
	[aCoder encodeFloat:scale->y forKey:@"scaleY"];
	[aCoder encodeFloat:scale->z forKey:@"scaleZ"];
	
	[aCoder encodeBool:selected forKey:@"selected"];

	[aCoder encodeObject:mesh forKey:@"mesh"];
}

- (void)dealloc
{
	delete position;
	delete rotation;
	delete scale;
	[mesh release];
	[super dealloc];
}

- (void)draw
{
	glPushMatrix();
	glTranslatef(position->x, position->y, position->z);
	Matrix4x4 rotationMatrix;
	rotation->ToMatrix(rotationMatrix);
	glMultMatrixf(rotationMatrix);
	glScalef(scale->x, scale->y, scale->z);
	[mesh drawWithScale:*scale selected:selected];
	glPopMatrix();
}

- (void)moveByOffset:(Vector3D)offset
{
	*position += offset;
}

- (void)rotateByOffset:(Quaternion)offset
{
	*rotation = offset * *rotation;
}

- (void)scaleByOffset:(Vector3D)offset
{
	*scale += offset;
}

- (Item *)clone
{
	Item *newItem = [[Item alloc] init];

	[newItem setPosition:[self position]];
	[newItem setRotation:[self rotation]];
	[newItem setScale:[self scale]];
	
	[[newItem mesh] mergeWithMesh:[self mesh]];
	
	[newItem setSelected:YES];
	[self setSelected:NO];
	
	return newItem;
}

@end
