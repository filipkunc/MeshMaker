//
//  MyDocument.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#import "MyDocument.h"
#import "ItemManipulationState.h"
#import "IndexedItem.h"

@implementation MyDocument

- (id)init
{
    self = [super init];
    if (self) 
	{
        // Add your subclass-specific initialization here.
        // If an error occurs here, send a [self release] message and return nil.
		items = [[ItemCollection alloc] init];
		itemsController = [[OpenGLManipulatingController alloc] init];
		meshController = [[OpenGLManipulatingController alloc] init];
		
		[itemsController setModel:items];
		manipulated = itemsController;

		[itemsController addTransformationObserver:self];
		[meshController addTransformationObserver:self];
		
		manipulationFinished = YES;
		oldManipulations = nil;
		oldMeshManipulation = nil;
		
		views = [[NSMutableArray alloc] init];
		oneView = nil;
    }
    return self;
}
						   
- (void)dealloc
{
	[itemsController removeTransformationObserver:self];
	[meshController removeTransformationObserver:self];
	[meshController release];
	[itemsController release];
	[items release];
	[oldManipulations release];
	[oldMeshManipulation release];
	[views release];
	[super dealloc];
}

- (void)awakeFromNib
{
	// maximizing window on start
	[[viewPerspective window] performZoom:self];
	
	[editModePopUp selectItemWithTag:0];
	[viewModePopUp selectItemWithTag:0];
	
	[views addObject:viewTop];
	[views addObject:viewLeft];
	[views addObject:viewFront];
	[views addObject:viewPerspective];
	
	for (OpenGLSceneView *view in views)
	{ 
		[view setManipulated:manipulated]; 
		[view setDisplayed:itemsController];
		[view setDelegate:self];
	};
	
	[viewTop setCameraMode:CameraModeTop];
	[viewLeft setCameraMode:CameraModeLeft];
	[viewFront setCameraMode:CameraModeFront];
	[viewPerspective setCameraMode:CameraModePerspective];
}

- (void)setNeedsDisplayExceptView:(OpenGLSceneView *)view
{
	for (OpenGLSceneView *v in views)
	{ 
		if (v != view)
			[v setNeedsDisplay:YES]; 
	}
}

- (void)setNeedsDisplayOnAllViews
{
	for (OpenGLSceneView *v in views)
	{
		[v setNeedsDisplay:YES];
	}
}

- (id<OpenGLManipulating>)manipulated
{
	return manipulated;
}

- (void)setManipulated:(id<OpenGLManipulating>)value
{
	manipulated = value;
	
	for (OpenGLSceneView *view in views)
	{ 
		[view setManipulated:value];
		[view setNeedsDisplay:YES];
	};

	if (manipulated == itemsController)
	{
		[editModePopUp selectItemWithTag:EditModeItems];
	}
	else if (manipulated == meshController)
	{
		int meshTag = [[self currentMesh] selectionMode] + 1;
		[editModePopUp selectItemWithTag:meshTag];
	}
}

- (Mesh *)currentMesh
{
	if (manipulated == meshController)
		return (Mesh *)[meshController model];
	return nil;
}

- (MyDocument *)prepareUndoWithName:(NSString *)actionName
{
	NSUndoManager *undo = [self undoManager];
	MyDocument *document = [undo prepareWithInvocationTarget:self];
	if (![undo isUndoing])
		[undo setActionName:actionName];
	return document;
}

- (void)addItemWithType:(enum MeshType)type steps:(uint)steps;
{
	Item *item = [[Item alloc] init];
	Mesh *mesh = [item mesh];
	[mesh makeMeshWithType:type steps:steps];
	
	NSString *name = [Mesh descriptionOfMeshType:type];
	NSLog(@"adding %@", name);
	NSLog(@"vertexCount = %i", [mesh vertexCount]);
	NSLog(@"triangleCount = %i", [mesh triangleCount]);
	
	MyDocument *document = [self prepareUndoWithName:[NSString stringWithFormat:@"Add %@", name]];
	[document removeItemWithType:type steps:steps];
	
	[items addItem:item];
	[item release];
	
	[itemsController changeSelection:NO];
	[items setSelected:YES atIndex:[items count] - 1];
	[itemsController updateSelection];
	[self setManipulated:itemsController];
}

- (void)removeItemWithType:(enum MeshType)type steps:(uint)steps
{
	NSString *name = [Mesh descriptionOfMeshType:type];
	NSLog(@"removing %@", name);
	
	MyDocument *document = [self prepareUndoWithName:[NSString stringWithFormat:@"Remove %@", name]];
	[document addItemWithType:type steps:steps];
		
	[items removeLastItem];
	[itemsController changeSelection:NO];
	[self setManipulated:itemsController];
}

- (float)positionX
{
	return [manipulated positionX];
}

- (float)positionY
{
	return [manipulated positionY];
}

- (float)positionZ
{
	return [manipulated positionZ];
}

- (float)rotationX
{
	return [manipulated rotationX];
}

- (float)rotationY
{
	return [manipulated rotationY];
}

- (float)rotationZ
{
	return [manipulated rotationZ];
}

- (float)scaleX
{
	return [manipulated scaleX];
}

- (float)scaleY
{
	return [manipulated scaleY];
}

- (float)scaleZ
{
	return [manipulated scaleZ];
}

- (void)setPositionX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setPositionX:value];
	[self manipulationEndedInView:nil];
}

- (void)setPositionY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setPositionY:value];
	[self manipulationEndedInView:nil];
}

- (void)setPositionZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setPositionZ:value];
	[self manipulationEndedInView:nil];
}

- (void)setRotationX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setRotationX:value];
	[self manipulationEndedInView:nil];
}

- (void)setRotationY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setRotationY:value];
	[self manipulationEndedInView:nil];
}

- (void)setRotationZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setRotationZ:value];
	[self manipulationEndedInView:nil];
}

- (void)setScaleX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setScaleX:value];
	[self manipulationEndedInView:nil];
}

- (void)setScaleY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setScaleY:value];
	[self manipulationEndedInView:nil];
}

- (void)setScaleZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setScaleZ:value];
	[self manipulationEndedInView:nil];
}

- (void)setNilValueForKey:(NSString *)key
{
	[self setValue:[NSNumber numberWithFloat:0.0f] forKey:key];
}

- (void)observeValueForKeyPath:(NSString *)keyPath 
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	// Is it right way to do it? I really don't know, but it works.
	[self willChangeValueForKey:keyPath];
	[self didChangeValueForKey:keyPath];
}

- (void)swapManipulationsWithOld:(NSMutableArray *)old current:(NSMutableArray *)current
{
	NSLog(@"swapManipulationsWithOld:current:");
	NSAssert([old count] == [current count], @"old count == current count");
	[items setCurrentManipulations:old];
	
	MyDocument *document = [self prepareUndoWithName:@"Manipulations"];	
	[document swapManipulationsWithOld:current current:old];
	
	[itemsController updateSelection];
	[self setManipulated:itemsController];
}

- (void)swapMeshManipulationWithOld:(MeshManipulationState *)old current:(MeshManipulationState *)current
{
	NSLog(@"swapMeshManipulationWithOld:current:");
	
	[items setCurrentMeshManipulation:old];
	Item *item = [items itemAtIndex:[old itemIndex]];
	[meshController setModel:[item mesh]];
	
	MyDocument *document = [self prepareUndoWithName:@"Mesh Manipulation"];
	[document swapMeshManipulationWithOld:current current:old];
	
	[itemsController updateSelection];
	[meshController updateSelection];
	[self setManipulated:meshController];
}

- (void)swapAllItemsWithOld:(NSMutableArray *)old
					current:(NSMutableArray *)current
				 actionName:(NSString *)actionName
{
	NSLog(@"swapAllItemsWithOld:current:actionName:");
	
	NSLog(@"items count before set = %i", [items count]);
	[items setAllItems:old];
	NSLog(@"items count after set = %i", [items count]);
	
	MyDocument *document = [self prepareUndoWithName:actionName];
	[document swapAllItemsWithOld:current
						  current:old
					   actionName:actionName];
	
	[itemsController updateSelection];
	[self setManipulated:itemsController];
}

- (void)swapMeshFullStateWithOld:(MeshFullState *)old 
						 current:(MeshFullState *)current 
					  actionName:(NSString *)actionName
{
	NSLog(@"swapMeshFullStateWithOld:current:actionName:");
	
	[items setCurrentMeshFull:old];
	Item *item = [items itemAtIndex:[old itemIndex]];
	[meshController setModel:[item mesh]];
	
	MyDocument *document = [self prepareUndoWithName:actionName];
	[document swapMeshFullStateWithOld:current
							   current:old
							actionName:actionName];
	
	[itemsController updateSelection];
	[meshController updateSelection];
	[self setManipulated:meshController];
}

- (void)allItemsActionWithName:(NSString *)actionName block:(void (^)())action
{
	MyDocument *document = [self prepareUndoWithName:actionName];
	NSMutableArray *oldItems = [items allItems];
	NSLog(@"oldItems count = %i", (int)[oldItems count]);
	
	action();
	
	NSMutableArray *currentItems = [items allItems];
	NSLog(@"currentItems count = %i", (int)[currentItems count]);
	[document swapAllItemsWithOld:oldItems 
						  current:currentItems
					   actionName:actionName];	
}

- (void)fullMeshActionWithName:(NSString *)actionName block:(void (^)())action
{
	MyDocument *document = [self prepareUndoWithName:actionName];
	MeshFullState *oldState = [items currentMeshFull];
	
	action();
	
	MeshFullState *currentState = [items currentMeshFull];
	[document swapMeshFullStateWithOld:oldState 
							   current:currentState
							actionName:actionName];
}

- (void)manipulationStartedInView:(OpenGLSceneView *)view
{
	NSLog(@"manipulationStartedInView:");
	manipulationFinished = NO;
	
	if (manipulated == itemsController)
	{
		oldManipulations = [[items currentManipulations] retain];
	}
	else if (manipulated == meshController)
	{
		oldMeshManipulation = [[items currentMeshManipulation] retain];
	}
}

- (void)manipulationEndedInView:(OpenGLSceneView *)view
{
	NSLog(@"manipulationEndedInView:");	
	manipulationFinished = YES;
	
	if (manipulated == itemsController)
	{
		MyDocument *document = [self prepareUndoWithName:@"Manipulations"];
		[document swapManipulationsWithOld:oldManipulations current:[items currentManipulations]];
		[oldManipulations release];
		oldManipulations = nil;
	}
	else if (manipulated == meshController)
	{
		MyDocument *document = [self prepareUndoWithName:@"Mesh Manipulation"];
		[document swapMeshManipulationWithOld:oldMeshManipulation current:[items currentMeshManipulation]];
		[oldMeshManipulation release];
		oldMeshManipulation = nil;
	}
	
	[self setNeedsDisplayExceptView:view];
}

- (void)selectionChangedInView:(OpenGLSceneView *)view
{
	NSLog(@"selectionChangedInView:");
	[self setNeedsDisplayExceptView:view];
}

- (IBAction)addCube:(id)sender
{
	[self addItemWithType:MeshTypeCube steps:0];
}

- (IBAction)addCylinder:(id)sender
{
	itemWithSteps = MeshTypeCylinder;
	[addItemWithStepsSheetController beginSheetWithProtocol:self];
}

- (IBAction)addSphere:(id)sender
{
	itemWithSteps = MeshTypeSphere;
	[addItemWithStepsSheetController beginSheetWithProtocol:self];
}

- (void)addItemWithSteps:(uint)steps
{
	[self addItemWithType:itemWithSteps steps:steps];
}

- (void)editMeshWithMode:(enum MeshSelectionMode)mode
{
	NSInteger index = [itemsController lastSelectedIndex];
	if (index > -1)
	{
		Item *item = [items itemAtIndex:index];
		[[item mesh] setSelectionMode:mode];
		[meshController setModel:[item mesh]];
		[meshController setPosition:[item position] 
						   rotation:[item rotation] 
							  scale:[item scale]];
		[self setManipulated:meshController];
	}
}

- (void)editItems
{
	[[self currentMesh] setSelectionMode:MeshSelectionModeVertices];
	[itemsController setModel:items];
	[itemsController setPosition:Vector3D()
						rotation:Quaternion()
						   scale:Vector3D(1, 1, 1)];
	[self setManipulated:itemsController];
}

- (IBAction)changeEditMode:(id)sender
{
	EditMode mode = (EditMode)[[editModePopUp selectedItem] tag];
	switch (mode)
	{
		case EditModeItems:
			[self editItems];
			break;
		case EditModeVertices:
			[self editMeshWithMode:MeshSelectionModeVertices];
			break;
		case EditModeTriangles:
			[self editMeshWithMode:MeshSelectionModeTriangles];
			break;
		case EditModeEdges:
			[self editMeshWithMode:MeshSelectionModeEdges];
			break;
		default:
			break;
	}
}

- (IBAction)changeViewMode:(id)sender
{
	ViewMode mode = (ViewMode)[[viewModePopUp selectedItem] tag];
	for (OpenGLSceneView *view in views)
	{ 
		[view setViewMode:mode]; 
	}
}

- (IBAction)mergeSelected:(id)sender
{
	NSLog(@"mergeSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == itemsController)
	{
		[self allItemsActionWithName:@"Merge" block:^ { [items mergeSelectedItems]; }];
	}
	else if (manipulated == meshController)
	{
		[self fullMeshActionWithName:@"Merge" block:^ { [[self currentMesh] mergeSelected]; }]; 
	}
	
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)splitSelected:(id)sender
{
	NSLog(@"splitSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		[self fullMeshActionWithName:@"Split" block:^ { [[self currentMesh] splitSelected]; }];
	}
	
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)flipSelected:(id)sender
{
	NSLog(@"flipSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		[self fullMeshActionWithName:@"Flip" block:^ { [[self currentMesh] flipSelected]; }];
	}
	
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)duplicateSelected:(id)sender
{	
	NSLog(@"duplicateSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	BOOL startManipulation = NO;
	if (!manipulationFinished)
	{
		startManipulation = YES;
		[self manipulationEndedInView:nil];
	}
	
	if (manipulated == itemsController)
	{
		NSMutableArray *selection = [items currentSelection];
		MyDocument *document = [self prepareUndoWithName:@"Duplicate"];
		[document undoDuplicateSelected:selection];
		[manipulated duplicateSelected];
	}
	else if (manipulated == meshController)
	{
		[self fullMeshActionWithName:@"Duplicate" block:^ { [manipulated duplicateSelected]; }];
	}
	
	[self setNeedsDisplayOnAllViews];
	
	if (startManipulation)
	{
		[self manipulationStartedInView:nil];
	}
}

- (void)redoDuplicateSelected:(NSMutableArray *)selection
{
	NSLog(@"redoDuplicateSelected:");
	
	[self setManipulated:itemsController];
	[items setCurrentSelection:selection];
	[manipulated duplicateSelected];
	
	MyDocument *document = [self prepareUndoWithName:@"Duplicate"];
	[document undoDuplicateSelected:selection];
	
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (void)undoDuplicateSelected:(NSMutableArray *)selection
{	
	NSLog(@"undoDuplicateSelected:");
	
	[self setManipulated:itemsController];
	uint duplicatedCount = [selection count];
	[items removeItemsInRange:NSMakeRange([items count] - duplicatedCount, duplicatedCount)];
	[items setCurrentSelection:selection];

	MyDocument *document = [self prepareUndoWithName:@"Duplicate"];
	[document redoDuplicateSelected:selection];
		
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)deleteSelected:(id)sender
{
	NSLog(@"deleteSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == itemsController)
	{
		NSMutableArray *currentItems = [items currentItems];
		MyDocument *document = [self prepareUndoWithName:@"Delete"];
		[document undoDeleteSelected:currentItems];
		[manipulated removeSelected];
	}
	else if (manipulated == meshController)
	{
		[self fullMeshActionWithName:@"Delete" block:^ { [manipulated removeSelected]; }];
	}
	
	[self setNeedsDisplayOnAllViews];
}

- (void)redoDeleteSelected:(NSMutableArray *)selectedItems
{
	NSLog(@"redoDeleteSelected:");

	[self setManipulated:itemsController];
	[items setSelectionFromIndexedItems:selectedItems];
	[manipulated removeSelected];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document undoDeleteSelected:selectedItems];
	
	[itemsController updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (void)undoDeleteSelected:(NSMutableArray *)selectedItems
{
	NSLog(@"undoDeleteSelected:");
	
	[self setManipulated:itemsController];
	[items setCurrentItems:selectedItems];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document redoDeleteSelected:selectedItems];
	
	[itemsController updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)mergeVertexPairs:(id)sender
{
	NSLog(@"mergeVertexPairs:");
	
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		if ([[self currentMesh] selectionMode] == MeshSelectionModeVertices)
		{
			[self fullMeshActionWithName:@"Merge Vertex Pairs" block:^ { [[self currentMesh] mergeVertexPairs]; }];
		}
	}
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)changeManipulator:(id)sender
{
	ManipulatorType newManipulator = (ManipulatorType)[sender tag];
	for (OpenGLSceneView *view in views)
	{ 
		[view setCurrentManipulator:newManipulator]; 
	}
}

- (IBAction)selectAll:(id)sender
{
	[[self manipulated] changeSelection:YES];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)invertSelection:(id)sender
{
	[[self manipulated] invertSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)hideSelected:(id)sender
{
	[[self manipulated] hideSelected];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)unhideAll:(id)sender
{
	[[self manipulated] unhideAll];
	[self setNeedsDisplayOnAllViews];
}

- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"MyDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
    [super windowControllerDidLoadNib:aController];
    // Add any code here that needs to be executed once the windowController has loaded the document's window.
}

#pragma mark Archivation

- (BOOL)readFromModel3D:(NSString *)fileName
{
	// ugly ASCII encoding, should be Unicode
	const char *cFileName = [fileName cStringUsingEncoding:NSASCIIStringEncoding];
	ifstream fin;
	fin.open(cFileName, ios::in | ios::binary);
	if (fin.is_open())
	{
		ItemCollection *newItems = [[ItemCollection alloc] initWithFileStream:&fin];
		[newItems retain];
		[items release];
		items = newItems;
		[itemsController setModel:items];
		[itemsController updateSelection];
		[self setManipulated:itemsController];
		fin.close();
		return YES;
	}
	fin.close();
	return NO;
}

- (void)writeToModel3D:(NSString *)fileName
{
	// ugly ASCII encoding, should be Unicode
	const char *cFileName = [fileName cStringUsingEncoding:NSASCIIStringEncoding];
	ofstream fout;
	fout.open(cFileName, ios::out | ios::binary);
	[items encodeWithFileStream:&fout];
	fout.close();
}

// these read/write methods are deprecated, I need to use newer methods

- (BOOL)readFromFile:(NSString *)fileName ofType:(NSString *)typeName
{
	NSLog(@"readFromFile:%@ typeName:%@", fileName, typeName);
	if ([typeName isEqual:@"model3D"])
	{
		//return [self readFromData:[NSData dataWithContentsOfFile:fileName] ofType:typeName error:NULL];
		return [self readFromModel3D:fileName];
	}
	return NO;
}

- (BOOL)writeToFile:(NSString *)fileName ofType:(NSString *)typeName
{
	NSLog(@"writeToFile:%@ typeName:%@", fileName, typeName);
	if ([typeName isEqual:@"model3D"])
	{
		[self writeToModel3D:fileName];
		return YES;
	}
	return NO;
}

#pragma mark Splitter sync

- (CGFloat)splitView:(NSSplitView *)splitView 
constrainSplitPosition:(CGFloat)proposedPosition 
		 ofSubviewAt:(NSInteger)dividerIndex
{
	if (oneView)
		return 0.0f;
	
	return proposedPosition;
}

// fix for issue four-views works independently on Mac version
- (void)splitViewDidResizeSubviews:(NSNotification *)notification
{
	if (oneView)
		return;
	
	NSSplitView *splitView = (NSSplitView *)[notification object];
	if (splitView == mainSplit)
		return;
	
	NSView *topSubview0 = (NSView *)[[topSplit subviews] objectAtIndex:0];
	NSView *topSubview1 = (NSView *)[[topSplit subviews] objectAtIndex:1];
	
	NSView *bottomSubview0 = (NSView *)[[bottomSplit subviews] objectAtIndex:0];
	NSView *bottomSubview1 = (NSView *)[[bottomSplit subviews] objectAtIndex:1];
	
	// we are interested only in width change
	if (fabsf([bottomSubview0 frame].size.width - [topSubview0 frame].size.width) >= 1.0f)
	{
		if (splitView == topSplit)
		{
			NSLog(@"topSplit");
			[bottomSubview0 setFrame:[topSubview0 frame]];
			[bottomSubview1 setFrame:[topSubview1 frame]];
		}
		else
		{
			NSLog(@"bottomSplit");
			[topSubview0 setFrame:[bottomSubview0 frame]];
			[topSubview1 setFrame:[bottomSubview1 frame]];
		}
	}
}

- (void)collapseSplitView:(NSSplitView *)splitView
{
	[[[splitView subviews] objectAtIndex:0] setFrame:NSZeroRect];
	if ([splitView isVertical])
	{
		NSRect frame = [[[splitView subviews] objectAtIndex:1] frame];
		if (frame.size.width < 1.0f)
		{
			frame.size.width = 2.0f;
			[[[splitView subviews] objectAtIndex:1] setFrame:frame];
		}
	}
	else 
	{
		NSRect frame = [[[splitView subviews] objectAtIndex:1] frame];
		if (frame.size.height < 1.0f)
		{
			frame.size.height = 2.0f;
			[[[splitView subviews] objectAtIndex:1] setFrame:frame];
		}
	}
}

- (void)swapCamerasBetweenFirst:(OpenGLSceneView *)first second:(OpenGLSceneView *)second
{
	Camera firstCamera = [first camera];
	Camera secondCamera = [second camera];
	CameraMode firstMode = [first cameraMode];
	CameraMode secondMode = [second cameraMode];
	[second setCameraMode:firstMode];
	[first setCameraMode:secondMode];
	[second setCamera:firstCamera];
	[first setCamera:secondCamera];
}

- (void)toggleOneViewFourView:(id)sender
{
	NSLog(@"toggleOneViewFourView");
	
	if (oneView)
	{
		if (oneView != viewPerspective)
		{
			[self swapCamerasBetweenFirst:oneView second:viewPerspective];
		}
		NSRect frame = [viewPerspective frame];
		[[[mainSplit subviews] objectAtIndex:0] setFrame:frame];
		oneView = nil;
		return;
	}
	
	NSWindow *window = [viewPerspective window];
	NSPoint point = [window convertScreenToBase:[NSEvent mouseLocation]];
	
	NSView *hittedView = [[[window contentView] superview] hitTest:point];
	
	for (OpenGLSceneView *view in views)
	{
		if (view == hittedView)
		{
			oneView = view;
			
			[self collapseSplitView:topSplit];
			[self collapseSplitView:bottomSplit];
			[self collapseSplitView:mainSplit];
			
			if (oneView != viewPerspective)
			{
				[self swapCamerasBetweenFirst:oneView second:viewPerspective];
			}
			return;
		}
	}
	
	NSLog(@"No view is under mouse");
}

@end
