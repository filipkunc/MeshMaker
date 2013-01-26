//
//  MyDocument.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#if defined(__APPLE__)

#import "MyDocument.h"

@implementation UndoStatePointer

- (IUndoState *)undoState
{
    return _undoState;
}

- (id)initWithUndoState:(IUndoState *)undoState
{
    self = [super init];
    if (self)
    {
        _undoState = undoState;
    }
    return self;
}

- (void)dealloc
{
    if (_undoState)
        delete _undoState;
}

@end

@implementation MyDocument

@synthesize items, scriptPullDown;

- (id)init
{
    self = [super init];
    if (self) 
	{
        // Add your subclass-specific initialization here.
        // If an error occurs here, send a [self release] message and return nil.
		items = new ItemCollection();
        textures = new TextureCollection();
		itemsController = new OpenGLManipulatingController();
		meshController = new OpenGLManipulatingController();
		
        itemsController->setModel(items);
		manipulated = itemsController;

        itemsController->addSelectionObserver(self);
        meshController->addSelectionObserver(self);
		
		manipulationFinished = YES;
		oldManipulations = nil;
		oldMeshState = nil;
		
		views = [[NSMutableArray alloc] init];
		oneView = nil;
        
        currentManipulator = ManipulatorType::Default;
        
        NSUndoManager *undo = [self undoManager];
        [undo setLevelsOfUndo:100];
    }
    return self;
}
						   
- (void)dealloc
{
    itemsController->removeSelectionObserver(self);
    meshController->removeSelectionObserver(self);
    
    delete items;
    delete textures;
    delete itemsController;
    delete meshController;
}

- (void)awakeFromNib
{
	[editModePopUp selectItemWithTag:0];
    [scriptWindowController setDelegate:self];
    [[scriptPullDown menu] setDelegate:scriptWindowController];
    [scriptWindowController menuNeedsUpdate:[scriptPullDown menu]];
    
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
	
	[viewTop setCameraMode:CameraMode::Top];
	[viewLeft setCameraMode:CameraMode::Left];
	[viewFront setCameraMode:CameraMode::Front];
	[viewPerspective setCameraMode:CameraMode::Perspective];
}

- (enum ManipulatorType)currentManipulator
{
    return currentManipulator;
}

- (void)setCurrentManipulator:(enum ManipulatorType)value;
{
    currentManipulator = value;
    itemsController->setCurrentManipulator(value, manipulated != itemsController);
    meshController->setCurrentManipulator(value, manipulated != meshController);
}

- (NSApplicationPresentationOptions)window:(NSWindow *)window willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
    return proposedOptions;    
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

- (IOpenGLManipulating *)manipulated
{
	return manipulated;
}

- (void)setManipulated:(IOpenGLManipulating *)value
{
	manipulated = value;
	
	for (OpenGLSceneView *view in views)
	{ 
		[view setManipulated:value];
		[view setNeedsDisplay:YES];
	}

	if (manipulated == itemsController)
	{
		[editModePopUp selectItemWithTag:(int)EditMode::Items];
	}
	else if (manipulated == meshController)
	{
		int meshTag = (int)[self currentMesh]->selectionMode() + 1;
		[editModePopUp selectItemWithTag:meshTag];
	}
}

- (void)updateManipulatedSelection
{
    manipulated->updateSelection();
}

- (Mesh2 *)currentMesh
{
	if (manipulated == meshController)
    {
        Item *item = (Item *)meshController->model();
        return item->mesh;
    }
    if (manipulated == itemsController)
    {
        ItemCollection *itemCollection = (ItemCollection *)itemsController->model();
        return itemCollection->currentMesh();
    }
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
	Item *item = new Item(new Mesh2());
	Mesh2 *mesh = item->mesh;
    mesh->make(type, steps);
	
	NSString *name = Mesh2::descriptionOfMeshType(type);
	
	MyDocument *document = [self prepareUndoWithName:[NSString stringWithFormat:@"Add %@", name]];
	[document removeItemWithType:type steps:steps];
	
    items->addItem(item);
    [textureBrowserWindowController setItems:items textures:textures];
    itemsController->changeSelection(false);
    items->setSelectedAtIndex(items->count() - 1, true);
    itemsController->updateSelection();
    [self setManipulated:itemsController];
}

- (void)removeItemWithType:(enum MeshType)type steps:(uint)steps
{
	NSString *name = Mesh2::descriptionOfMeshType(type);
	
	MyDocument *document = [self prepareUndoWithName:[NSString stringWithFormat:@"Remove %@", name]];
	[document addItemWithType:type steps:steps];

	items->removeLastItem();
    meshController->setModel(NULL);
    [textureBrowserWindowController setItems:items textures:textures];
    itemsController->changeSelection(false);
	[self setManipulated:itemsController];
}

- (float)selectionX
{
	return manipulated->selectionX();
}

- (float)selectionY
{
	return manipulated->selectionY();
}

- (float)selectionZ
{
	return manipulated->selectionZ();
}

- (void)setSelectionX:(float)value
{
	[self manipulationStartedInView:nil];
	manipulated->setSelectionX(value);
	[self manipulationEndedInView:nil];
}

- (void)setSelectionY:(float)value
{
	[self manipulationStartedInView:nil];
	manipulated->setSelectionY(value);
	[self manipulationEndedInView:nil];
}

- (void)setSelectionZ:(float)value
{
	[self manipulationStartedInView:nil];
	manipulated->setSelectionZ(value);
	[self manipulationEndedInView:nil];
}

- (BOOL)selectionColorEnabled
{
    return manipulated->selectionColorEnabled();
}

- (NSColor *)selectionColor
{
    Vector4D color = manipulated->selectionColor();
    return [NSColor colorWithCalibratedRed:color.x green:color.y blue:color.z alpha:color.w];
}

- (void)setSelectionColor:(NSColor *)selectionColor
{
    Vector4D color = Vector4D(selectionColor.redComponent, selectionColor.greenComponent, selectionColor.blueComponent, selectionColor.alphaComponent);
    [self meshOnlyActionWithName:@"Change Color" block:^ { manipulated->setSelectionColor(color); }];
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
    if (manipulationFinished)
    {
        [self willChangeValueForKey:keyPath];
        [self didChangeValueForKey:keyPath];
    }
}

- (void)swapManipulationsWithOld:(UndoStatePointer *)old
                         current:(UndoStatePointer *)current
{
    items->setCurrentManipulations(old.undoState);
	
	MyDocument *document = [self prepareUndoWithName:@"Manipulations"];	
	[document swapManipulationsWithOld:current current:old];
	
	itemsController->updateSelection();
	[self setManipulated:itemsController];
}

- (void)swapAllItemsWithOld:(UndoStatePointer *)old
					current:(UndoStatePointer *)current
				 actionName:(NSString *)actionName
{
    items->setAllItems(old.undoState);

	MyDocument *document = [self prepareUndoWithName:actionName];
	[document swapAllItemsWithOld:current
						  current:old
					   actionName:actionName];
	
	itemsController->updateSelection();
	[self setManipulated:itemsController];
}

- (void)swapMeshStateWithOld:(UndoStatePointer *)old
					 current:(UndoStatePointer *)current
				  actionName:(NSString *)actionName
{
    if (old.undoState == NULL)
        return;
    
    items->setCurrentMeshState(old.undoState);
    
    MeshState *meshState = dynamic_cast<UndoState<MeshState> *>(old.undoState)->state();
	Item *item = items->itemAtIndex(meshState->index());
	
    meshController->setModel(item);
    meshController->setPositionRotationScale(item->position, item->rotation, item->scale);
    
	MyDocument *document = [self prepareUndoWithName:actionName];
	[document swapMeshStateWithOld:current
						   current:old
						actionName:actionName];
	
	itemsController->updateSelection();
	meshController->updateSelection();
    
	[self setManipulated:meshController];
}

- (void)allItemsActionWithName:(NSString *)actionName block:(void (^)())action
{
	MyDocument *document = [self prepareUndoWithName:actionName];
	UndoStatePointer *oldItems = [[UndoStatePointer alloc] initWithUndoState:items->allItems()];

	action();
    
    [textureBrowserWindowController setItems:items textures:textures];
	
	UndoStatePointer *currentItems = [[UndoStatePointer alloc] initWithUndoState:items->allItems()];
	[document swapAllItemsWithOld:oldItems 
						  current:currentItems
					   actionName:actionName];	
}

- (void)meshActionWithName:(NSString *)actionName block:(void (^)())action
{
	MyDocument *document = [self prepareUndoWithName:actionName];
	UndoStatePointer *oldState = [[UndoStatePointer alloc] initWithUndoState:items->currentMeshState()];
	
	action();
	
	UndoStatePointer *currentState = [[UndoStatePointer alloc] initWithUndoState:items->currentMeshState()];
	[document swapMeshStateWithOld:oldState 
						   current:currentState
						actionName:actionName];
}

- (void)manipulationStartedInView:(OpenGLSceneView *)view
{
	manipulationFinished = NO;
	
	if (manipulated == itemsController)
	{
		oldManipulations = [[UndoStatePointer alloc] initWithUndoState:items->currentManipulations()];
	}
	else if (manipulated == meshController)
	{
		oldMeshState = [[UndoStatePointer alloc] initWithUndoState:items->currentMeshState()];
	}
}

- (void)manipulationEndedInView:(OpenGLSceneView *)view
{
	manipulationFinished = YES;
	
	if (manipulated == itemsController)
	{
		MyDocument *document = [self prepareUndoWithName:@"Manipulations"];
		[document swapManipulationsWithOld:oldManipulations
                                   current:[[UndoStatePointer alloc] initWithUndoState:items->currentManipulations()]];
		oldManipulations = nil;
        
        itemsController->willChangeSelection();
        itemsController->didChangeSelection();
	}
	else if (manipulated == meshController)
	{
		MyDocument *document = [self prepareUndoWithName:@"Mesh Manipulation"];
		[document swapMeshStateWithOld:oldMeshState
                               current:[[UndoStatePointer alloc] initWithUndoState:items->currentMeshState()]
                            actionName:@"Mesh Manipulation"];
		oldMeshState = nil;
        
        meshController->willChangeSelection();
        meshController->didChangeSelection();
	}
	
	[self setNeedsDisplayExceptView:view];
}

- (NSColor *)brushColor
{
    return [texturePaintToolWindowController brushColor];
}

- (float)brushSize
{
    return [texturePaintToolWindowController brushSize];
}

- (void)selectionChangedInView:(OpenGLSceneView *)view
{
	[self setNeedsDisplayExceptView:view];
}

- (IBAction)addPlane:(id)sender
{
    [self addItemWithType:MeshType::Plane steps:0];
}

- (IBAction)addCube:(id)sender
{
	[self addItemWithType:MeshType::Cube steps:0];
}

- (IBAction)addCylinder:(id)sender
{
	itemWithSteps = MeshType::Cylinder;
	[addItemWithStepsSheetController beginSheetWithProtocol:self];
}

- (IBAction)addSphere:(id)sender
{
	itemWithSteps = MeshType::Sphere;
	[addItemWithStepsSheetController beginSheetWithProtocol:self];
}

- (void)addIcosahedron:(id)sender
{
    [self addItemWithType:MeshType::Icosahedron steps:0];
}

- (void)addItemWithSteps:(uint)steps
{
	[self addItemWithType:itemWithSteps steps:steps];
}

- (void)editMeshWithMode:(enum MeshSelectionMode)mode
{
	NSInteger index = itemsController->lastSelectedIndex();
	if (index > -1)
	{
		Item *item = items->itemAtIndex(index);
        item->mesh->setSelectionMode(mode);
        
		meshController->setModel(item);
		meshController->setPositionRotationScale(item->position, item->rotation, item->scale);
        
		[self setManipulated:meshController];
	}
}

- (void)editItems
{
    Mesh2 *currentMesh = [self currentMesh];
    if (currentMesh)
        currentMesh->setSelectionMode(MeshSelectionMode::Vertices);
    
	itemsController->setModel(items);
    itemsController->setPositionRotationScale(Vector3D(), Quaternion(), Vector3D(1, 1, 1));
    
	[self setManipulated:itemsController];
}

- (IBAction)changeEditMode:(id)sender
{
	EditMode mode = (EditMode)[[editModePopUp selectedItem] tag];
    Mesh2 *currentMesh = [self currentMesh];
    
    if (!currentMesh)
        [editModePopUp selectItemWithTag:(int)EditMode::Items];
    
	switch (mode)
	{
		case EditMode::Items:
			[self editItems];
			break;
		case EditMode::Vertices:
			[self editMeshWithMode:MeshSelectionMode::Vertices];
			break;
		case EditMode::Triangles:
			[self editMeshWithMode:MeshSelectionMode::Triangles];
			break;
		case EditMode::Edges:
			[self editMeshWithMode:MeshSelectionMode::Edges];
			break;
	}
}

- (enum ViewMode)viewMode
{
    return manipulated->viewMode();
}

- (void)setViewMode:(enum ViewMode)viewMode
{
    manipulated->setViewMode(viewMode);
    [self setNeedsDisplayOnAllViews];
}

- (IBAction)mergeSelected:(id)sender
{
	if (manipulated->selectedCount() <= 0)
		return;
	
	if (manipulated == itemsController)
	{
		[self allItemsActionWithName:@"Merge" block:^ { items->mergeSelectedItems(); }];
	}
	else if (manipulated == meshController)
	{
		[self meshActionWithName:@"Merge" block:^ { [self currentMesh]->mergeSelected(); }]; 
	}
	
	manipulated->updateSelection();
	[self setNeedsDisplayOnAllViews];
}

- (void)meshOnlyActionWithName:(NSString *)actionName block:(void (^)())action
{    
    if ([self currentMesh] == nil)
        return;
	
	BOOL startManipulation = NO;
	if (!manipulationFinished)
	{
		startManipulation = YES;
		[self manipulationEndedInView:nil];
	}
	
    [self meshActionWithName:actionName block:action];
	
    manipulated->updateSelection();
	[self setNeedsDisplayOnAllViews];
	
	if (startManipulation)
	{
		[self manipulationStartedInView:nil];
	}
}

- (IBAction)splitSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Split" block:^ { [self currentMesh]->splitSelected(); }];
}

- (IBAction)flipSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Flip" block:^ { [self currentMesh]->flipSelected(); }];
}

- (IBAction)duplicateSelected:(id)sender
{	
	if (manipulated->selectedCount() <= 0)
		return;
	
	BOOL startManipulation = NO;
	if (!manipulationFinished)
	{
		startManipulation = YES;
		[self manipulationEndedInView:nil];
	}
	
	if (manipulated == itemsController)
	{
		UndoStatePointer *selection = [[UndoStatePointer alloc] initWithUndoState:items->currentSelection()];
		MyDocument *document = [self prepareUndoWithName:@"Duplicate"];
		[document undoDuplicateSelected:selection];
		manipulated->duplicateSelected();
	}
	else if (manipulated == meshController)
	{
        [self meshActionWithName:@"Duplicate" block:^ { manipulated->duplicateSelected(); }];
	}
	
    manipulated->updateSelection();
	[self setNeedsDisplayOnAllViews];
	
	if (startManipulation)
	{
		[self manipulationStartedInView:nil];
	}
}

- (void)redoDuplicateSelected:(UndoStatePointer *)selection
{
	[self setManipulated:itemsController];
	items->setCurrentSelection(selection.undoState);
	manipulated->duplicateSelected();
	
	MyDocument *document = [self prepareUndoWithName:@"Duplicate"];
	[document undoDuplicateSelected:selection];
	
	manipulated->updateSelection();
	[self setNeedsDisplayOnAllViews];
}

- (void)undoDuplicateSelected:(UndoStatePointer *)selection
{	
	[self setManipulated:itemsController];
    vector<uint> *selectedIndices = dynamic_cast<UndoState<vector<uint>> *>(selection.undoState)->state();
	uint duplicatedCount = selectedIndices->size();
    items->removeItemsInRange(items->count() - duplicatedCount, duplicatedCount);
    items->setCurrentSelection(selection.undoState);

	MyDocument *document = [self prepareUndoWithName:@"Duplicate"];
	[document redoDuplicateSelected:selection];
		
	manipulated->updateSelection();
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)deleteSelected:(id)sender
{
	if (manipulated->selectedCount() <= 0)
		return;
	
	if (manipulated == itemsController)
	{
		UndoStatePointer *currentItems = [[UndoStatePointer alloc] initWithUndoState:items->currentItems()];
		MyDocument *document = [self prepareUndoWithName:@"Delete"];
		[document undoDeleteSelected:currentItems];
		manipulated->removeSelected();
        [textureBrowserWindowController setItems:items textures:textures];
	}
	else if (manipulated == meshController)
	{
		[self meshActionWithName:@"Delete" block:^ { manipulated->removeSelected(); }];
	}
	
	[self setNeedsDisplayOnAllViews];
}

- (void)redoDeleteSelected:(UndoStatePointer *)selectedItems
{
	[self setManipulated:itemsController];
    items->setSelectionFromRemovedItems(selectedItems.undoState);
	manipulated->removeSelected();
    [textureBrowserWindowController setItems:items textures:textures];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document undoDeleteSelected:selectedItems];

	itemsController->updateSelection();
	[self setNeedsDisplayOnAllViews];
}

- (void)undoDeleteSelected:(UndoStatePointer *)selectedItems
{
	[self setManipulated:itemsController];
    items->setCurrentItems(selectedItems.undoState);
    [textureBrowserWindowController setItems:items textures:textures];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document redoDeleteSelected:selectedItems];
	
	itemsController->updateSelection();
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)subdivision:(id)sender
{
    [self meshOnlyActionWithName:@"Subdivision" block:^ { [self currentMesh]->openSubdivision(); }];
}

+ (BOOL)softSelection
{
    return Mesh2::useSoftSelection();
}

+ (void)setSoftSelection:(BOOL)value
{
    Mesh2::setUseSoftSelection(value);
}

- (IBAction)softSelection:(id)sender
{
    NSMenuItem *menuItem = (NSMenuItem *)sender;
    [MyDocument setSoftSelection:![MyDocument softSelection]];
    [menuItem setState:[MyDocument softSelection]];
}

- (IBAction)changeManipulator:(id)sender
{
	self.currentManipulator = (ManipulatorType)[sender tag];
	for (OpenGLSceneView *view in views)
	{ 
		[view setCurrentManipulator:currentManipulator]; 
	}
}

- (IBAction)selectAll:(id)sender
{
    manipulated->changeSelection(true);
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)invertSelection:(id)sender
{
    manipulated->invertSelection();
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)hideSelected:(id)sender
{
    manipulated->hideSelected();
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)unhideAll:(id)sender
{
    manipulated->unhideAll();
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)detachSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Detach" block:^ { [self currentMesh]->detachSelected(); }];
}

- (IBAction)extrudeSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Extrude" block:^ { [self currentMesh]->extrudeSelectedTriangles(); }];
}

- (IBAction)cleanTexture:(id)sender
{
//    Mesh2 *mesh = [self currentMesh];
//    if (mesh)
//    {
//        mesh->cleanTexture();
//        [self setNeedsDisplayOnAllViews];
//    }
}

- (IBAction)resetTexCoords:(id)sender
{
    [self meshOnlyActionWithName:@"Reset Texture Coordinates" block:^ 
    { 
        Mesh2 *mesh = [self currentMesh];
        mesh->resetTriangleCache();
        mesh->makeTexCoords();
        mesh->makeEdges();
    }];
}

- (IBAction)triangulate:(id)sender
{
    if (manipulated == meshController)
    {
        [self meshOnlyActionWithName:@"Triangulate" block:^ { [self currentMesh]->triangulateSelectedQuads(); }];
    }
    else if (manipulated == itemsController)
    {
        [self allItemsActionWithName:@"Triangulate" block:^
        {
            for (uint i = 0; i < items->count(); i++)
                items->itemAtIndex(i)->mesh->triangulate();
        }];
    }
}

- (IBAction)viewTexturePaintTool:(id)sender
{
    [texturePaintToolWindowController showWindow:nil];
}

- (IBAction)viewTextureBrowser:(id)sender
{
    [textureBrowserWindowController setItems:items textures:textures];
    [textureBrowserWindowController showWindow:nil];
}

- (void)viewScriptEditor:(id)sender
{
    [scriptWindowController showWindow:nil];
}

- (BOOL)texturePaintEnabled
{
    if (texturePaintToolWindowController.isWindowLoaded)
    {
        if (texturePaintToolWindowController.window.isVisible)
            return YES;
    }
    return NO;
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

+ (BOOL)autosavesInPlace
{
    return YES;
}

- (void)windowWillEnterVersionBrowser:(NSNotification *)notification
{
    NSWindow *window = (NSWindow *)[notification object];
    [window.toolbar setVisible:NO];
}

- (void)windowDidExitVersionBrowser:(NSNotification *)notification
{
    NSWindow *window = (NSWindow *)[notification object];
    [window.toolbar setVisible:YES];
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
			[bottomSubview0 setFrame:[topSubview0 frame]];
			[bottomSubview1 setFrame:[topSubview1 frame]];
		}
		else
		{
			[topSubview0 setFrame:[bottomSubview0 frame]];
			[topSubview1 setFrame:[bottomSubview1 frame]];
		}
	}
}

- (void)collapseSplitView:(NSSplitView *)splitView
{
	[[[splitView subviews] objectAtIndex:0] setFrame:NSZeroRect];
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
			
			[self collapseSplitView:mainSplit];
            [self collapseSplitView:topSplit];
			[self collapseSplitView:bottomSplit];			
			
			if (oneView != viewPerspective)
			{
				[self swapCamerasBetweenFirst:oneView second:viewPerspective];
			}
			return;
		}
	}	
}

@end

#elif defined(WIN32)

#include "MyDocument.h"

namespace MeshMakerCppCLI
{
	MyDocument::MyDocument(IDocumentDelegate ^documentForm)
	{
		documentDelegate = documentForm;

		items = new ItemCollection();
		itemsController = new OpenGLManipulatingController(this);
		meshController = new OpenGLManipulatingController(this);
		
        itemsController->setModel(items);
		manipulated = itemsController;
		
		currentManipulator = ManipulatorType::Default;

		manipulationFinished = true;

		undoManager = gcnew UndoManager();
	}

	MyDocument::~MyDocument()
	{
		
	}

	Mesh2 *MyDocument::currentMesh()
	{
		if (manipulated == meshController)
		{
			Item *item = (Item *)meshController->model();
			return item->mesh;
		}
		if (manipulated == itemsController)
		{
			ItemCollection *itemCollection = (ItemCollection *)itemsController->model();
			return itemCollection->currentMesh();
		}
		return NULL;
	}

	void MyDocument::editMesh(MeshSelectionMode mode)
	{
		NSInteger index = itemsController->lastSelectedIndex();
		if (index > -1)
		{
			Item *item = items->itemAtIndex(index);
			item->mesh->setSelectionMode(mode);
	        
			meshController->setModel(item);
			meshController->setPositionRotationScale(item->position, item->rotation, item->scale);
	        
			this->setManipulated(meshController);			
		}
	}

	void MyDocument::editItems()
	{
		Mesh2 *currentMesh = this->currentMesh();
		if (currentMesh)
			currentMesh->setSelectionMode(MeshSelectionMode::Vertices);
	    
		itemsController->setModel(items);
		itemsController->setPositionRotationScale(Vector3D(), Quaternion(), Vector3D(1, 1, 1));
	    
		this->setManipulated(itemsController);		
	}

	void MyDocument::changeEditMode()
	{
		EditMode mode = (EditMode)documentDelegate->editModePopup->SelectedItem;
		Mesh2 *currentMesh = this->currentMesh();
	    
		if (!currentMesh)
			documentDelegate->editModePopup->SelectedItem = EditMode::Items;			
	    
		switch (mode)
		{
			case EditMode::Items:
				editItems();
				break;
			case EditMode::Vertices:
				editMesh(MeshSelectionMode::Vertices);
				break;
			case EditMode::Triangles:
				editMesh(MeshSelectionMode::Triangles);
				break;
			case EditMode::Edges:
				editMesh(MeshSelectionMode::Edges);
				break;
		}
	}

	void MyDocument::setManipulated(IOpenGLManipulating *value)
	{
		manipulated = value;
	
		for each (OpenGLSceneView ^view in views)
		{
			view->coreView()->setManipulated(manipulated);
			view->Refresh();			
		}

		if (manipulated == itemsController)
		{
			documentDelegate->editModePopup->SelectedItem = EditMode::Items;
		}
		else if (manipulated == meshController)
		{
			documentDelegate->editModePopup->SelectedItem = (EditMode)((int)currentMesh()->selectionMode() + 1);
		}
	}

    void MyDocument::setViews(OpenGLSceneView ^left, OpenGLSceneView ^top, OpenGLSceneView ^front, OpenGLSceneView ^perspective)
	{
		viewLeft = left;
		viewTop = top;
		viewFront = front;
		viewPerspective = perspective;

		views = gcnew array<OpenGLSceneView ^>(4) { viewLeft, viewTop, viewFront, viewPerspective };

		viewLeft->CurrentCameraMode = CameraMode::Left;
		viewTop->CurrentCameraMode = CameraMode::Top;
		viewFront->CurrentCameraMode = CameraMode::Front;
		viewPerspective->CurrentCameraMode = CameraMode::Perspective;

		for each (OpenGLSceneView ^view in views)
		{
			view->coreView()->setManipulated(manipulated);
			view->coreView()->setDisplayed(itemsController);
			view->Delegate = this;			
		}
	}

	void MyDocument::addItem(MeshType meshType, uint steps)
	{
		Item *item = new Item(new Mesh2());
		Mesh2 *mesh = item->mesh;
		mesh->make(meshType, steps);

		String ^actionName = String::Format(L"Add {0}", Mesh2::descriptionOfMeshType(meshType));

		undoManager->PrepareUndo(actionName, gcnew Invocation(
			gcnew AddRemoveItem(this, &MyDocument::removeItem),
			meshType, steps));
		
		items->addItem(item);
		itemsController->changeSelection(false);
		items->setSelectedAtIndex(items->count() - 1, true);
		itemsController->updateSelection();

		this->setManipulated(itemsController);
	}

	void MyDocument::removeItem(MeshType meshType, uint steps)
	{
		String ^actionName = String::Format(L"Remove {0}", Mesh2::descriptionOfMeshType(meshType));
	
		undoManager->PrepareUndo(actionName, gcnew Invocation(
			gcnew AddRemoveItem(this, &MyDocument::addItem),
			meshType, steps));

		items->removeLastItem();
		meshController->setModel(NULL);
		itemsController->changeSelection(false);

		this->setManipulated(itemsController);		
	}

	void MyDocument::setNeedsDisplayExceptView(OpenGLSceneView ^except)
	{
		for each (OpenGLSceneView ^view in views)
		{
			if (view != except)
				view->Refresh();
		}
	}

	void MyDocument::setNeedsDisplayOnAllViews()
	{
		for each (OpenGLSceneView ^view in views)
		{
			view->Refresh();
		}
	}

	void MyDocument::manipulationStartedInView(OpenGLSceneView ^view)
	{
		manipulationFinished = false;
	
		if (manipulated == itemsController)
		{
			oldManipulations = gcnew UndoStatePointer(items->currentManipulations());
		}
		else if (manipulated == meshController)
		{
			oldMeshState = gcnew UndoStatePointer(items->currentMeshState());
		}
	}

	void MyDocument::manipulationEndedInView(OpenGLSceneView ^view)
	{
		manipulationFinished = true;
	
		if (manipulated == itemsController)
		{
			undoManager->PrepareUndo(L"Manipulations", gcnew Invocation(
				gcnew SwapOldCurrent(this, &MyDocument::swapManipulationsAction),
				oldManipulations, gcnew UndoStatePointer(items->currentManipulations())));

			oldManipulations = nullptr;
	        
			itemsController->willChangeSelection();
			itemsController->didChangeSelection();
		}
		else if (manipulated == meshController)
		{
			undoManager->PrepareUndo(L"Mesh Manipulation", gcnew Invocation(
				gcnew SwapOldCurrentNamed(this, &MyDocument::swapMeshStateAction),
				oldMeshState, gcnew UndoStatePointer(items->currentMeshState()), L"Mesh Manipulation"));

			oldMeshState = nullptr;
	        
			meshController->willChangeSelection();
			meshController->didChangeSelection();
		}
		
		this->setNeedsDisplayExceptView(view);
	}

	void MyDocument::selectionChangedInView(OpenGLSceneView ^view)
	{
		this->setNeedsDisplayExceptView(view);
		documentDelegate->updateSelectionValues();
	}

	void MyDocument::willChangeSelection()
	{

	}

	void MyDocument::didChangeSelection()
	{
		documentDelegate->updateSelectionValues();
	}
	
	void MyDocument::swapManipulationsAction(UndoStatePointer ^old, UndoStatePointer ^current)
	{
		items->setCurrentManipulations(old->_undoState);

		undoManager->PrepareUndo(L"Manipulations", gcnew Invocation(
				gcnew SwapOldCurrent(this, &MyDocument::swapManipulationsAction),
				current, old));

		itemsController->updateSelection();
		this->setManipulated(itemsController);
	}

	void MyDocument::swapAllItemsAction(UndoStatePointer ^old, UndoStatePointer ^current, String ^actionName)
	{
		items->setAllItems(old->_undoState);

		undoManager->PrepareUndo(actionName, gcnew Invocation(
				gcnew SwapOldCurrentNamed(this, &MyDocument::swapAllItemsAction),
				current, old, actionName));

		itemsController->updateSelection();
		this->setManipulated(itemsController);
	}

	void MyDocument::swapMeshStateAction(UndoStatePointer ^old, UndoStatePointer ^current, String ^actionName)
	{
		if (old->_undoState == NULL)
			return;
	    
		items->setCurrentMeshState(old->_undoState);
	    
		MeshState *meshState = dynamic_cast<UndoState<MeshState> *>(old->_undoState)->state();
		Item *item = items->itemAtIndex(meshState->index());
		
		meshController->setModel(item);
		meshController->setPositionRotationScale(item->position, item->rotation, item->scale);
	    
		undoManager->PrepareUndo(actionName, gcnew Invocation(
				gcnew SwapOldCurrentNamed(this, &MyDocument::swapMeshStateAction),
				current, old, actionName));
		
		itemsController->updateSelection();
		meshController->updateSelection();

		this->setManipulated(meshController);
	}

	void MyDocument::allItemsAction(String ^actionName, Action ^action)
	{
		UndoStatePointer ^oldItems = gcnew UndoStatePointer(items->allItems());

		action();
    
		UndoStatePointer ^currentItems =  gcnew UndoStatePointer(items->allItems());

		undoManager->PrepareUndo(actionName, gcnew Invocation(
			gcnew SwapOldCurrentNamed(this, &MyDocument::swapAllItemsAction),
			oldItems, currentItems, actionName));
	}
		
	void MyDocument::meshAction(String ^actionName, Action ^action)
	{
		UndoStatePointer ^oldState = gcnew UndoStatePointer(items->currentMeshState());
		
		action();
		
		UndoStatePointer ^currentState = gcnew UndoStatePointer(items->currentMeshState());

		undoManager->PrepareUndo(actionName, gcnew Invocation(
			gcnew SwapOldCurrentNamed(this, &MyDocument::swapMeshStateAction),
			oldState, currentState, actionName));
	}
		
	void MyDocument::meshOnlyAction(String ^actionName, Action ^action)
	{
		if (this->currentMesh() == nullptr)
			return;			 
		
		bool startManipulation = false;
		if (!manipulationFinished)
		{
			startManipulation = true;
			this->manipulationEndedInView(nullptr);			
		}

		this->meshAction(actionName, action);
		
		manipulated->updateSelection();
		this->setNeedsDisplayOnAllViews();
		
		if (startManipulation)
		{
			this->manipulationStartedInView(nullptr);
		}
	}

	void MyDocument::undo()
	{
		if (undoManager->CanUndo)
			undoManager->Undo();
	}

	void MyDocument::redo()
	{
		if (undoManager->CanRedo)
			undoManager->Redo();
	}

	void MyDocument::duplicateSelectedCore()
	{
		manipulated->duplicateSelected();
	}
	
	void MyDocument::duplicateSelected()
	{
		if (manipulated->selectedCount() <= 0)
			return;
		
		bool startManipulation = false;
		if (!manipulationFinished)
		{
			startManipulation = true;
			this->manipulationEndedInView(nullptr);			
		}
		
		if (manipulated == itemsController)
		{
			this->allItemsAction(L"Duplicate", gcnew Action(this, &MyDocument::duplicateSelectedCore));
		}
		else if (manipulated == meshController)
		{
			this->meshOnlyAction(L"Duplicate", gcnew Action(this, &MyDocument::duplicateSelectedCore));			
		}
		
		manipulated->updateSelection();
		this->setNeedsDisplayOnAllViews();
		
		if (startManipulation)
		{
			this->manipulationStartedInView(nullptr);
		}
	}

	void MyDocument::removeSelectedCore()
	{
		manipulated->removeSelected();
	}

	void MyDocument::deleteSelected()
	{
		if (manipulated->selectedCount() <= 0)
			return;
		
		if (manipulated == itemsController)
		{
			this->allItemsAction(L"Delete", gcnew Action(this, &MyDocument::removeSelectedCore));
		}
		else if (manipulated == meshController)
		{
			this->meshAction(L"Delete", gcnew Action(this, &MyDocument::removeSelectedCore));
		}
		
		this->setNeedsDisplayOnAllViews();
	}

	void MyDocument::selectAll()
	{
		manipulated->changeSelection(true);
		this->setNeedsDisplayOnAllViews();
	}

	void MyDocument::invertSelection()
	{
		manipulated->invertSelection();
		this->setNeedsDisplayOnAllViews();
	}

	void MyDocument::hideSelected()
	{
		manipulated->hideSelected();
		this->setNeedsDisplayOnAllViews();
	}

	void MyDocument::unhideAll()
	{
		manipulated->unhideAll();
		this->setNeedsDisplayOnAllViews();
	}

	void MyDocument::mergeSelectedCore()
	{
		if (manipulated == itemsController)
		{
			items->mergeSelectedItems();
		}
		else if (manipulated == meshController)
		{
			currentMesh()->mergeSelected();
		}
	}

	void MyDocument::mergeSelected()
	{
		if (manipulated->selectedCount() <= 0)
			return;
		
		if (manipulated == itemsController)
		{
			this->allItemsAction(L"Merge", gcnew Action(this, &MyDocument::mergeSelectedCore));			
		}
		else if (manipulated == meshController)
		{
			this->meshAction(L"Merge", gcnew Action(this, &MyDocument::mergeSelectedCore));
		}
		
		manipulated->updateSelection();
		this->setNeedsDisplayOnAllViews();
	}

	void MyDocument::splitSelectedCore()
	{
		currentMesh()->splitSelected();
	}

	void MyDocument::splitSelected()
	{
		this->meshOnlyAction(L"Split", gcnew Action(this, &MyDocument::splitSelectedCore));		
	}

	void MyDocument::flipSelectedCore()
	{
		currentMesh()->flipSelected();
	}

	void MyDocument::flipSelected()
	{
		this->meshOnlyAction(L"Flip", gcnew Action(this, &MyDocument::flipSelectedCore));		
	}

	void MyDocument::subdivisionCore()
	{
		currentMesh()->openSubdivision();
	}

	void MyDocument::subdivision()
	{
		this->meshOnlyAction(L"Subdivision", gcnew Action(this, &MyDocument::subdivisionCore));
	}

	void MyDocument::detachSelectedCore()
	{
		currentMesh()->detachSelected();
	}

	void MyDocument::detachSelected()
	{
		this->meshOnlyAction(L"Detach", gcnew Action(this, &MyDocument::detachSelectedCore));		
	}

	void MyDocument::extrudeSelectedCore()
	{
		currentMesh()->extrudeSelectedTriangles();
	}

	void MyDocument::extrudeSelected()
	{
		this->meshOnlyAction(L"Extrude", gcnew Action(this, &MyDocument::extrudeSelectedCore));	
	}

	void MyDocument::triangulateSelectedCore()
	{
		if (manipulated == meshController)
		{
			currentMesh()->triangulateSelectedQuads();
		}
		else if (manipulated == itemsController)
		{
			for (uint i = 0; i < items->count(); i++)
				items->itemAtIndex(i)->mesh->triangulate();
		}
	}

	void MyDocument::triangulateSelected()
	{
		if (manipulated == meshController)
		{
			this->meshOnlyAction(L"Triangulate", gcnew Action(this, &MyDocument::triangulateSelectedCore));
		}
		else if (manipulated == itemsController)
		{
			this->allItemsAction(L"Triangulate", gcnew Action(this, &MyDocument::triangulateSelectedCore));
		}
	}
}

#elif defined(__linux__)

#include "MyDocument.h"

MyDocument::MyDocument()
{
    items = new ItemCollection();
    itemsController = new OpenGLManipulatingController();
    meshController = new OpenGLManipulatingController();

    itemsController->setModel(items);
    manipulated = itemsController;

    _currentManipulator = ManipulatorType::Default;

    manipulationFinished = true;
}

MyDocument::~MyDocument()
{
    delete items;
    delete itemsController;
    delete meshController;
}

void MyDocument::setManipulated(IOpenGLManipulating *value)
{
    manipulated = value;

    for (uint i = 0; i < views.size(); i++)
    {
        OpenGLSceneView *view = views[i];
        view->coreView()->setManipulated(manipulated);
        view->setNeedsDisplay();
    }

    if (manipulated == itemsController)
    {
        //documentDelegate->editModePopup->SelectedItem = EditMode::Items;
    }
    else if (manipulated == meshController)
    {
        //documentDelegate->editModePopup->SelectedItem = (EditMode)((int)currentMesh()->selectionMode() + 1);
    }
}

Mesh2 *MyDocument::currentMesh()
{
    if (manipulated == meshController)
    {
        Item *item = (Item *)meshController->model();
        return item->mesh;
    }
    if (manipulated == itemsController)
    {
        ItemCollection *itemCollection = (ItemCollection *)itemsController->model();
        return itemCollection->currentMesh();
    }
    return NULL;
}

void MyDocument::editItems()
{
    Mesh2 *currentMesh = this->currentMesh();
    if (currentMesh)
        currentMesh->setSelectionMode(MeshSelectionMode::Vertices);

    itemsController->setModel(items);
    itemsController->setPositionRotationScale(Vector3D(), Quaternion(), Vector3D(1, 1, 1));

    this->setManipulated(itemsController);
}

void MyDocument::editMesh(MeshSelectionMode mode)
{
    NSInteger index = itemsController->lastSelectedIndex();
    if (index > -1)
    {
        Item *item = items->itemAtIndex(index);
        item->mesh->setSelectionMode(mode);

        meshController->setModel(item);
        meshController->setPositionRotationScale(item->position, item->rotation, item->scale);

        this->setManipulated(meshController);
    }
}

ManipulatorType MyDocument::currentManipulator()
{
    return _currentManipulator;
}

void MyDocument::setCurrentManipulator(ManipulatorType value)
{
    _currentManipulator = value;
    itemsController->setCurrentManipulator(value, manipulated != itemsController);
    meshController->setCurrentManipulator(value, manipulated != meshController);

    for (uint i = 0; i < views.size(); i++)
    {
        OpenGLSceneView *view = views[i];
        view->coreView()->setCurrentManipulator(value);
    }
}

void MyDocument::allItemsAction(string actionName, function<void ()> action)
{
    action();
}

void MyDocument::meshAction(string actionName, function<void ()> action)
{
    action();
}

void MyDocument::meshOnlyAction(string actionName, function<void ()> action)
{
    action();
}

void MyDocument::manipulationStartedInView(OpenGLSceneView *view)
{

}

void MyDocument::manipulationEndedInView(OpenGLSceneView *view)
{

}

void MyDocument::selectionChangedInView(OpenGLSceneView *view)
{

}

void MyDocument::willChangeSelection()
{

}

void MyDocument::didChangeSelection()
{

}

void MyDocument::setViews(OpenGLSceneView *left,
                          OpenGLSceneView *top,
                          OpenGLSceneView *front,
                          OpenGLSceneView *perspective)
{
    views.clear();
    views.push_back(viewLeft = left);
    views.push_back(viewTop = top);
    views.push_back(viewFront = front);
    views.push_back(viewPerspective = perspective);

    viewLeft->coreView()->setCameraMode(CameraMode::Left);
    viewTop->coreView()->setCameraMode(CameraMode::Top);
    viewFront->coreView()->setCameraMode(CameraMode::Front);
    viewPerspective->coreView()->setCameraMode(CameraMode::Perspective);

    for (uint i = 0; i < views.size(); i++)
    {
        OpenGLSceneView *view = views[i];

        view->coreView()->setManipulated(manipulated);
        view->coreView()->setDisplayed(itemsController);

#warning TODO: view delegate
    }
}

void MyDocument::setNeedsDisplayExceptView(OpenGLSceneView *except)
{
    for (uint i = 0; i < views.size(); i++)
    {
        if (views[i] != except)
            views[i]->setNeedsDisplay();
    }
}

void MyDocument::setNeedsDisplayOnAllViews()
{
    for (uint i = 0; i < views.size(); i++)
    {
        views[i]->setNeedsDisplay();
    }
}

void MyDocument::undo()
{
#warning TODO: undo
}

void MyDocument::redo()
{
#warning TODO: redo
}

void MyDocument::addItem(MeshType meshType, uint steps)
{
    Item *item = new Item(new Mesh2());
    Mesh2 *mesh = item->mesh;
    mesh->make(meshType, steps);

//    String ^actionName = String::Format(L"Add {0}", Mesh2::descriptionOfMeshType(meshType));

//    undoManager->PrepareUndo(actionName, gcnew Invocation(
//        gcnew AddRemoveItem(this, &MyDocument::removeItem),
//        meshType, steps));

    items->addItem(item);
    itemsController->changeSelection(false);
    items->setSelectedAtIndex(items->count() - 1, true);
    itemsController->updateSelection();

    this->setManipulated(itemsController);
}

void MyDocument::removeItem(MeshType meshType, uint steps)
{
//    String ^actionName = String::Format(L"Remove {0}", Mesh2::descriptionOfMeshType(meshType));

//    undoManager->PrepareUndo(actionName, gcnew Invocation(
//        gcnew AddRemoveItem(this, &MyDocument::addItem),
//        meshType, steps));

    items->removeLastItem();
    meshController->setModel(NULL);
    itemsController->changeSelection(false);

    this->setManipulated(itemsController);
}

void MyDocument::duplicateSelected()
{
    if (manipulated->selectedCount() <= 0)
        return;

    bool startManipulation = false;
    if (!manipulationFinished)
    {
        startManipulation = true;
        this->manipulationEndedInView(nullptr);
    }

    if (manipulated == itemsController)
    {
        this->allItemsAction("Duplicate", [this] { this->manipulated->duplicateSelected(); });
    }
    else if (manipulated == meshController)
    {
        this->meshOnlyAction("Duplicate", [this] { this->manipulated->duplicateSelected(); });
    }

    manipulated->updateSelection();
    this->setNeedsDisplayOnAllViews();

    if (startManipulation)
    {
        this->manipulationStartedInView(nullptr);
    }
}

void MyDocument::deleteSelected()
{
    if (manipulated->selectedCount() <= 0)
        return;

    if (manipulated == itemsController)
    {
        this->allItemsAction("Delete", [this] { this->manipulated->removeSelected(); });
    }
    else if (manipulated == meshController)
    {
        this->meshAction("Delete", [this] { this->manipulated->removeSelected(); });
    }

    this->setNeedsDisplayOnAllViews();
}

void MyDocument::selectAll()
{
    manipulated->changeSelection(true);
    this->setNeedsDisplayOnAllViews();
}

void MyDocument::invertSelection()
{
    manipulated->invertSelection();
    this->setNeedsDisplayOnAllViews();
}

void MyDocument::hideSelected()
{
    manipulated->hideSelected();
    this->setNeedsDisplayOnAllViews();
}

void MyDocument::unhideAll()
{
    manipulated->unhideAll();
    this->setNeedsDisplayOnAllViews();
}

void MyDocument::mergeSelected()
{
    if (manipulated->selectedCount() <= 0)
        return;

    if (manipulated == itemsController)
    {
        this->allItemsAction("Merge", [this] { this->items->mergeSelectedItems(); });
    }
    else if (manipulated == meshController)
    {
        this->meshAction("Merge", [this] { this->currentMesh()->mergeSelected(); });
    }

    manipulated->updateSelection();
    this->setNeedsDisplayOnAllViews();
}

void MyDocument::splitSelected()
{
    this->meshOnlyAction("Split", [this] { this->currentMesh()->splitSelected(); });
}

void MyDocument::flipSelected()
{
    this->meshOnlyAction("Flip", [this] { this->currentMesh()->flipSelected(); });
}

void MyDocument::subdivision()
{
    this->meshOnlyAction("Subdivision", [this] { this->currentMesh()->openSubdivision(); });
}

void MyDocument::detachSelected()
{
    this->meshOnlyAction("Detach", [this] { this->currentMesh()->detachSelected(); });
}

void MyDocument::extrudeSelected()
{
    this->meshOnlyAction("Extrude", [this] { this->currentMesh()->extrudeSelectedTriangles(); });
}

void MyDocument::triangulateSelected()
{
    if (manipulated == meshController)
    {
        this->meshOnlyAction("Triangulate", [this] { this->currentMesh()->triangulateSelectedQuads(); });
    }
    else if (manipulated == itemsController)
    {
        this->allItemsAction("Triangulate", [this]
        {
            for (uint i = 0; i < this->items->count(); i++)
                this->items->itemAtIndex(i)->mesh->triangulate();
        });
    }
}

#endif
