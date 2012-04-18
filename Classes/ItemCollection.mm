//
//  ItemCollection.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#import "OpenGLDrawing.h"
#import "ItemCollection.h"
#import "ItemManipulationState.h"

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

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len;
{
    return [items countByEnumeratingWithState:state objects:buffer count:len];
}

#pragma mark CppFileStreaming implementation

- (id)initWithReadStream:(MemoryReadStream *)stream
{
	self = [super init];
	if (self)
	{
		items = [[NSMutableArray alloc] init];
		uint itemsCount;
        [stream readBytes:&itemsCount length:sizeof(uint)];
		for (uint i = 0; i < itemsCount; i++)
		{
			Item *item = [[Item alloc] initWithReadStream:stream];
			[items addObject:item];
		}
	}
	return self;
}

- (void)encodeWithWriteStream:(MemoryWriteStream *)stream
{
	uint itemsCount = [items count];
    [stream writeBytes:&itemsCount length:sizeof(uint)];
	for (uint i = 0; i < itemsCount; i++)
	{		
		Item *item = [self itemAtIndex:i];
		[item encodeWithWriteStream:stream];
	}
}


- (Item *)itemAtIndex:(uint)index
{
	return (Item *)[items objectAtIndex:index];
}

- (void)addItem:(Item *)item
{
	[items addObject:item];
}

- (void)removeItem:(Item *)item
{
	[items removeObject:item];
}

- (void)removeLastItem
{
	[items removeLastObject];
}

- (void)removeItemAtIndex:(uint)index
{
	[items removeObjectAtIndex:index];
}

- (void)removeItemsInRange:(NSRange)range
{
	[items removeObjectsInRange:range];
}

- (void)insertItem:(Item *)item atIndex:(uint)index
{
	[items insertObject:item atIndex:index];
}

- (uint)count
{
	return [items count];
}

- (Vector3D)positionAtIndex:(uint)index
{
	return [[self itemAtIndex:index] position];
}

- (Quaternion)rotationAtIndex:(uint)index
{
	return [[self itemAtIndex:index] rotation];
}

- (Vector3D)scaleAtIndex:(uint)index
{
	return [[self itemAtIndex:index] scale];
}

- (void)setPosition:(Vector3D)position atIndex:(uint)index
{
	[[self itemAtIndex:index] setPosition:position];
}

- (void)setRotation:(Quaternion)rotation atIndex:(uint)index
{
	[[self itemAtIndex:index] setRotation:rotation];
}

- (void)setScale:(Vector3D)scale atIndex:(uint)index
{
	[[self itemAtIndex:index] setScale:scale];
}

- (void)moveByOffset:(Vector3D)offset atIndex:(uint)index
{
	[[self itemAtIndex:index] moveByOffset:offset];
}

- (void)rotateByOffset:(Quaternion)offset atIndex:(uint)index
{
	[[self itemAtIndex:index] rotateByOffset:offset];
}

- (void)scaleByOffset:(Vector3D)offset atIndex:(uint)index
{
	[[self itemAtIndex:index] scaleByOffset:offset];
}

- (BOOL)isSelectedAtIndex:(uint)index
{
	return [[self itemAtIndex:index] selected];
}

- (void)setSelected:(BOOL)selected atIndex:(uint)index
{
	[[self itemAtIndex:index] setSelected:selected];
}

- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection withMode:(enum ViewMode)mode
{
	[[self itemAtIndex:index] drawWithMode:mode forSelection:forSelection];
}

- (void)duplicateSelected
{
	int count = [self count];
	for (int i = 0; i < count; i++)
	{
		if ([self isSelectedAtIndex:i])
		{
			Item *oldItem = [self itemAtIndex:i];
			Item *newItem = [oldItem duplicate];
			[oldItem setSelected:NO];
			[items addObject:newItem];
		}
	}
}

- (void)removeSelected
{
	for (int i = 0; i < (int)[self count]; i++)
	{
		if ([self isSelectedAtIndex:i])
		{
			[items removeObjectAtIndex:i];
			i--;
		}
	}
}

- (void)hideSelected
{
	for (Item *item in items)
	{
		if ([item selected])
		{
			[item setSelected:NO];
			[item setVisible:NO];
		}
	}
}

- (void)unhideAll
{
	for	(Item *item in items)
	{
		[item setVisible:YES];
	}
}

- (void)mergeSelectedItems
{
	Vector3D center = Vector3D();
	uint selectedCount = 0;
	
	for (uint i = 0; i < [items count]; i++)
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
	
	for (int i = 0; i < (int)[items count]; i++)
	{
		if ([self isSelectedAtIndex:i])
		{
			Item *item = [self itemAtIndex:i];
			Vector3D scale = [item scale];
			
			itemMatrix.TranslateRotateScale([item position],
											[item rotation],
											scale);
			
			Matrix4x4 finalMatrix = firstMatrix * itemMatrix;
			Mesh *itemMesh = [item mesh];
			
			[itemMesh transformWithMatrix:&finalMatrix];
			
			// mirror detection, some component of scale is negative
			if (scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f)
				[itemMesh flipAllTriangles];
				
			[mesh mergeWithMesh:itemMesh];
			[items removeObjectAtIndex:i];
			i--;
		}
	}
	
	[newItem setSelected:YES];
	[self addItem:newItem];
}

- (void)csgOperationOnSelectedItems:(CsgOperation)operation
{
    Item *first = nil;
    Item *second = nil;
    
    for (Item *item in items)
	{
        if (item.selected)
		{
            if (!first)
                first = item;
            else if (!second)
            {
                second = item;
                
                first.mesh->mesh->transformAll(first.transform);
                second.mesh->mesh->transformAll(second.transform);
                first.mesh->mesh->csg(second.mesh->mesh, operation);
                
                [first setPosition:Vector3D(0, 0, 0)];
                [first setRotation:Quaternion(0, 0, 0, 1)];
                [first setScale:Vector3D(1, 1, 1)];
                
                [items removeObject:second];
                
                return;
            }
		}
	}
}

- (NSMutableArray *)currentManipulations
{
	NSMutableArray *manipulations = [[NSMutableArray alloc] init];
	
	for (uint i = 0; i < [self count]; i++)
	{
		Item *item = [self itemAtIndex:i];
		if ([item selected])
		{
			ItemManipulationState *itemState = [[ItemManipulationState alloc] initWithItem:item index:i];
			[manipulations addObject:itemState];
		}
	}
	
	return manipulations;
}

- (void)setCurrentManipulations:(NSMutableArray *)manipulations
{
	[self deselectAll];
	
	for (ItemManipulationState *manipulation in manipulations)
	{
		Item *item = [self itemAtIndex:[manipulation itemIndex]];
		[manipulation applyManipulationToItem:item];
	}
}

- (MeshState *)currentMeshState
{
	for (uint i = 0; i < [self count]; i++)
	{
		Item *item = [self itemAtIndex:i];
		if ([item selected])
		{
			MeshState *meshState = [[MeshState alloc] initWithMesh:[item mesh]
																 itemIndex:i];
			return meshState;
		}
	}
	return nil;
}

- (void)setCurrentMeshState:(MeshState *)meshState
{
	[self deselectAll];
	
	Item *item = [self itemAtIndex:[meshState itemIndex]];
	[item setSelected:YES];
	[meshState applyToMesh:[item mesh]];
}

- (NSMutableArray *)currentSelection
{
	NSMutableArray *selection = [[NSMutableArray alloc] init];
	
	for (uint i = 0; i < [self count]; i++)
	{
		Item *item = [self itemAtIndex:i];
		if ([item selected])
		{
			NSNumber *number = [[NSNumber alloc] initWithUnsignedInt:i];
			[selection addObject:number];
		}
	}
	
	return selection;	
}

- (void)setCurrentSelection:(NSMutableArray *)selection
{
	[self deselectAll];
	
	for (NSNumber *number in selection)
	{
		[self setSelected:YES atIndex:[number unsignedIntValue]];
	}
}

- (NSMutableArray *)currentItems
{
	NSMutableArray *anItems = [[NSMutableArray alloc] init];
	
	for (uint i = 0; i < [self count]; i++)
	{
		Item *item = [self itemAtIndex:i];
		if ([item selected])
		{
			IndexedItem *indexedItem = [[IndexedItem alloc] initWithIndex:i item:item];
			[anItems addObject:indexedItem];
		}
	}
	
	return anItems;
}

- (void)setCurrentItems:(NSMutableArray *)anItems
{
	[self deselectAll];
	
	for (IndexedItem *indexedItem in anItems)
	{
		[self insertItem:[indexedItem item] atIndex:[indexedItem index]];
	}
}

- (NSMutableArray *)allItems
{
	NSMutableArray *anItems = [[NSMutableArray alloc] init];
	
	for (uint i = 0; i < [self count]; i++)
	{
		Item *duplicate = [[self itemAtIndex:i] duplicate];
		[anItems addObject:duplicate];
	}
	
	return anItems;
}

- (void)setAllItems:(NSMutableArray *)anItems
{
	if (items == anItems)
		return;
	
	items = anItems;
}

- (void)setSelectionFromIndexedItems:(NSMutableArray *)anItems
{
	[self deselectAll];
	
	for (IndexedItem *indexedItem in anItems)
	{
		[self setSelected:YES atIndex:[indexedItem index]];
	}
}

- (void)deselectAll
{
	for (uint i = 0; i < [self count]; i++)
		[self setSelected:NO atIndex:i];
}

- (void)getVertexCount:(uint *)vertexCount triangleCount:(uint *)triangleCount
{
	*vertexCount = 0;
	*triangleCount = 0;
	for (Item *item in items)
	{
		Mesh *mesh = [item mesh];
		*vertexCount += [mesh vertexCount];
		*triangleCount += [mesh triangleCount];
	}
}

- (NSString *)nameAtIndex:(uint)index
{
	return [NSString stringWithFormat:@"Item %i", index];
}

- (Mesh *)currentMesh
{
    for (Item *item in items)
	{
		if ([item selected])
			return [item mesh];
	}
    return nil;
}

- (Item *)firstSelectedItem
{
    for (Item *item in items)
	{
		if ([item selected])
			return item;
	}
    return nil;
}

- (NSColor *)selectionColor
{
    Item *first = [self firstSelectedItem];
    if (first)
        return first.mesh.selectionColor;
    return nil;
}

- (void)setSelectionColor:(NSColor *)selectionColor
{
    for (Item *item in items)
	{
		if (item.selected)
            item.mesh.selectionColor = selectionColor;
	}
}

@end