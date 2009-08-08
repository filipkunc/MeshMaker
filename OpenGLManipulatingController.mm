//
//  OpenGLManipulatingController.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 8/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "OpenGLManipulatingController.h"


@implementation OpenGLManipulatingController

@synthesize currentManipulator, selectedCount, model, lastSelectedIndex;

- (id)init
{
	self = [super init];
	if (self)
	{
		selectionCenter = new Vector3D();
		selectionRotation = new Quaternion();
		selectionEuler = new Vector3D();
		selectionScale = new Vector3D(1, 1, 1);
		toggleWhenSelecting = NO;
		selectedCount = 0;
		lastSelectedIndex = -1;
		currentManipulator = ManipulatorTypeDefault;
	}
	return self;
}

- (void)dealloc
{
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
				if (lastSelectedIndex > -1)
					[model setRotation:*selectionRotation atIndex:lastSelectedIndex];
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
				if (lastSelectedIndex > -1)
					[model setScale:*selectionScale atIndex:lastSelectedIndex];
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
	lastSelectedIndex = -1;
	for (int i = 0; i < [model count]; i++)
	{
		if ([model isSelectedAtIndex:i])
		{
			selectedCount++;
			*selectionCenter += [model positionAtIndex:i];
			lastSelectedIndex = i;
		}
	}
	*selectionRotation = Quaternion();
	*selectionScale = Vector3D(1, 1, 1);
	if (selectedCount > 0)
	{
		*selectionCenter /= (float)selectedCount;
		if (selectedCount == 1 && lastSelectedIndex > -1)
		{
			*selectionRotation = [model rotationAtIndex:lastSelectedIndex];
			*selectionScale = [model scaleAtIndex:lastSelectedIndex];
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

- (void)moveSelectedByOffset:(Vector3D)offset
{
	for (int i = 0; i < [model count]; i++)
	{
		if ([model isSelectedAtIndex:i])
			[model moveByOffset:offset atIndex:i];
	}
	
	[self setSelectionCenter:*selectionCenter + offset];
}

- (void)rotateSelectedByOffset:(Quaternion)offset
{
	if ([self selectedCount] > 1)
	{
		for (int i = 0; i < [model count]; i++)
		{
			if ([model isSelectedAtIndex:i])
			{
				Vector3D position = [model positionAtIndex:i];
				position -= *selectionCenter;
				position.Transform(offset);
				position += *selectionCenter;
				[model setPosition:position atIndex:i];
				[model rotateByOffset:offset atIndex:i];
			}
		}
		[self setSelectionRotation:offset * (*selectionRotation)];
	}
	else if (lastSelectedIndex > -1)
	{
		[model rotateByOffset:offset atIndex:lastSelectedIndex];
		[self setSelectionRotation:[model rotationAtIndex:lastSelectedIndex]];
	}	
}

- (void)scaleSelectedByOffset:(Vector3D)offset
{
	for (int i = 0; i < [model count]; i++)
	{
		if ([model isSelectedAtIndex:i])
			[model scaleByOffset:offset atIndex:i];
	}
	[self setSelectionScale:*selectionScale + offset];
}

- (void)draw
{
	for (int i = 0; i < [model count]; i++)
	{
		[model drawAtIndex:i];
	}
}

- (NSUInteger)selectableCount
{
	return [model count];
}

- (void)drawForSelectionAtIndex:(NSUInteger)index
{
	[model drawAtIndex:index];
}

- (void)selectObjectAtIndex:(NSUInteger)index
{
	if (toggleWhenSelecting)
		[model setSelected:![model isSelectedAtIndex:index] atIndex:index];
	else
		[model setSelected:YES atIndex:index];
	[self updateSelection];
}

- (void)changeSelection:(BOOL)isSelected
{
	for (int i = 0; i < [model count]; i++)
		[model setSelected:isSelected atIndex:i];
	[self updateSelection];
}

- (void)invertSelection
{
	for (int i = 0; i < [model count]; i++)
		[model setSelected:![model isSelectedAtIndex:i] atIndex:i];
	[self updateSelection];
}

@end

