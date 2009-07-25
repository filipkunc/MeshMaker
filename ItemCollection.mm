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

@synthesize toggleWhenSelecting, currentManipulator, selectedCount;

- (id)init
{
	self = [super init];
	if (self)
	{
		items = [[NSMutableArray alloc] init];
		selectionCenter = new Vector3D();
		selectionRotation = new Quaternion();
		selectionEuler = new Vector3D();
		selectionScale = new Vector3D(1, 1, 1);
		toggleWhenSelecting = NO;
		selectedCount = 0;
		lastSelected = nil;
		currentManipulator = ManipulatorTypeDefault;
	}
	return self;
}

- (void)dealloc
{
	[items release];
	delete selectionCenter;
	delete selectionRotation;
	delete selectionEuler;
	delete selectionScale;
	[super dealloc];
}

- (float)selectionX
{
	return [self selectionValueAtIndex:0];
}

- (float)selectionY
{
	return [self selectionValueAtIndex:1];
}

- (float)selectionZ
{
	return [self selectionValueAtIndex:2];
}

- (void)setSelectionX:(float)value
{
	[self setSelectionValue:value atIndex:0];
}

- (void)setSelectionY:(float)value
{
	[self setSelectionValue:value atIndex:1];
}

- (void)setSelectionZ:(float)value
{
	[self setSelectionValue:value atIndex:2];
}

- (float)selectionValueAtIndex:(NSUInteger)index
{
	switch (currentManipulator)
	{
		case ManipulatorTypeTranslation:
			return (*selectionCenter)[index];
		case ManipulatorTypeRotation:
			return (*selectionEuler)[index] * RAD_TO_DEG;
		case ManipulatorTypeScale:
			return (*selectionScale)[index];
		case ManipulatorTypeDefault:
		default:
			return 0.0f;
	}
}

- (void)setSelectionValue:(float)value atIndex:(NSUInteger)index
{
	switch (currentManipulator)
	{
		case ManipulatorTypeTranslation:
		{
			Vector3D offset = Vector3D();
			offset[index] = value - (*selectionCenter)[index];
			[self moveSelectedByOffset:offset];
		}break;
		case ManipulatorTypeRotation:
		{
			(*selectionEuler)[index] = value * DEG_TO_RAD;
			if (selectedCount == 1)
			{
				selectionRotation->FromEulerAngles(*selectionEuler);
				[lastSelected setRotation:*selectionRotation];
			}
			else
			{
				Quaternion offset = Quaternion();
				offset.FromEulerAngles(*selectionEuler);
				offset = offset * selectionRotation->Conjugate();
				[self rotateSelectedByOffset:offset];
			}
		}break;
		case ManipulatorTypeScale:
		{
			if (selectedCount == 1)
			{
				(*selectionScale)[index] = value;
				[lastSelected setScale:*selectionScale];
			}
			else
			{
				Vector3D offset = Vector3D();
				offset[index] = value - (*selectionScale)[index];
				[self scaleSelectedByOffset:offset];
			}
		}break;
		default:
			break;
	}
}

- (void)setCurrentManipulator:(enum ManipulatorType)value
{
	[self willChangeSelection];
	currentManipulator = value;
	[self didChangeSelection];
}

- (void)updateSelection
{
	[self willChangeSelection];
	selectedCount = 0;
	*selectionCenter = Vector3D();
	lastSelected = nil;
	for (int i = 0; i < [items count]; i++)
	{
		Item *item = [self itemAtIndex:i];
		if ([item selected])
		{
			selectedCount++;
			*selectionCenter += [item position];
			lastSelected = item;
		}
	}
	*selectionRotation = Quaternion();
	*selectionScale = Vector3D(1, 1, 1);
	if (selectedCount > 0)
	{
		*selectionCenter /= (float)selectedCount;
		if (selectedCount == 1 && lastSelected != nil)
		{
			*selectionRotation = [lastSelected rotation];
			*selectionScale = [lastSelected scale];
		}
	}
	else
	{
		*selectionCenter = Vector3D();
	}
	selectionRotation->ToEulerAngles(*selectionEuler);
	[self didChangeSelection];
}

- (void)willChangeSelection
{
	[self willChangeValueForKey:@"selectionX"];
	[self willChangeValueForKey:@"selectionY"];
	[self willChangeValueForKey:@"selectionZ"];	
}

- (void)didChangeSelection
{
	[self didChangeValueForKey:@"selectionX"];
	[self didChangeValueForKey:@"selectionY"];
	[self didChangeValueForKey:@"selectionZ"];	
}

- (Vector3D)selectionCenter
{
	return *selectionCenter;
}

- (void)setSelectionCenter:(Vector3D)value
{
	[self willChangeSelection];
	*selectionCenter = value;
	[self didChangeSelection];
}

- (Quaternion)selectionRotation
{
	return *selectionRotation;
}

- (void)setSelectionRotation:(Quaternion)value
{
	[self willChangeSelection];
	*selectionRotation = value;
	selectionRotation->ToEulerAngles(*selectionEuler);
	[self didChangeSelection];
}

- (Vector3D)selectionScale
{
	return *selectionScale;
}

- (void)setSelectionScale:(Vector3D)value
{
	[self willChangeSelection];
	*selectionScale = value;
	[self didChangeSelection];
}

- (void)addItem:(Item *)item
{
	[items addObject:item];
}

- (Item *)itemAtIndex:(NSUInteger)index
{
	return [items objectAtIndex:index];
}

- (NSUInteger)count
{
	return [items count];
}

- (void)removeItemAtIndex:(NSUInteger)index
{
	BOOL needUpdateSelection = [[self itemAtIndex:index] selected];
	[items removeObjectAtIndex:index];
	if (needUpdateSelection)
		[self updateSelection];
}

- (void)removeSelected
{
	for (int i = 0; i < [items count]; i++)
	{
		if ([[self itemAtIndex:i] selected])
		{
			[items removeObjectAtIndex:i];
			i--;
		}
	}
	[self updateSelection];
}

- (void)changeSelection:(BOOL)isSelected
{
	for (int i = 0; i < [items count]; i++)
	{
		[[self itemAtIndex:i] setSelected:isSelected];
	}
	[self updateSelection];
}

- (void)invertSelection
{
	for (int i = 0; i < [items count]; i++)
	{
		Item *item = [self itemAtIndex:i];
		item.selected = !item.selected;
	}
	[self updateSelection];
}

- (void)draw
{
	for (int i = 0; i < [items count]; i++)
	{
		Item *item = [self itemAtIndex:i];
		if ([item selected])
		{
			glColor3f(1, 1, 1);
			glDisable(GL_LIGHTING);
			glPushMatrix();
			glTranslatef([item position].x, [item position].y, [item position].z);
			glutWireCube(4.0);
			glPopMatrix();
			glEnable(GL_LIGHTING);
			[item draw];
		}
		else
		{
			[item draw];
		}
	}
}

- (void)moveSelectedByOffset:(Vector3D)offset
{
	for (int i = 0; i < [items count]; i++)
	{
		Item *item = [self itemAtIndex:i];
		if ([item selected])
			[item moveByOffset:offset];
	}
	
	[self setSelectionCenter:*selectionCenter + offset];
}

- (void)rotateSelectedByOffset:(Quaternion)offset
{
	if ([self selectedCount] > 1)
	{
		for (int i = 0; i < [items count]; i++)
		{
			Item *item = [self itemAtIndex:i];
			if ([item selected])
			{
				Vector3D position = [item position];
				position -= *selectionCenter;
				position.Transform(offset);
				position += *selectionCenter;
				[item setPosition:position];
				[item rotateByOffset:offset];
			}
		}
		[self setSelectionRotation:offset * (*selectionRotation)];
	}
	else
	{
		[lastSelected rotateByOffset:offset];
		[self setSelectionRotation:[lastSelected rotation]];
	}	
}

- (void)scaleSelectedByOffset:(Vector3D)offset
{
	for (int i = 0; i < [items count]; i++)
	{
		Item *item = [self itemAtIndex:i];
		if ([item selected])
			[item scaleByOffset:offset];
	}
	[self setSelectionScale:*selectionScale + offset];
}

- (void)cloneSelected
{
	int count = [items count];
	for (int i = 0; i < count; i++)
	{
		Item *item = [self itemAtIndex:i];
		if ([item selected])
		{
			Item *clone = [[Item alloc] init];
			[clone setPosition:[item position]];
			[clone setRotation:[item rotation]];
			[clone setScale:[item scale]];
			[clone setSelected:YES];
			[item setSelected:NO];
			[items addObject:clone];
		}
	}
	[self updateSelection];
}

#pragma mark OpenGLSelecting

- (NSUInteger)selectableCount
{
	return [items count];
}

- (void)drawForSelectionAtIndex:(NSUInteger)index
{
	[[self itemAtIndex:index] draw];
}

- (void)selectObjectAtIndex:(NSUInteger)index
{
	Item *item = [self itemAtIndex:index];
	if (toggleWhenSelecting)
		[item setSelected:![item selected]];
	else
		[item setSelected:YES];
	[self updateSelection];
}

@end
