//
//  MyDocument.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#import "MyDocument.h"
#import "ItemManipulationState.h"
#import "TmdModel.h"
#import "IndexedItem.h"

#import "btBulletFile.h"
#import "btBulletWorldImporter.h"
#import "btDynamicsWorld.h"
#import "btDiscreteDynamicsWorld.h"
#import "btDispatcher.h"
#import "btDefaultCollisionConfiguration.h"
#import "btDbvtBroadphase.h"
#import "btSequentialImpulseConstraintSolver.h"
using namespace bParse;

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
		oneView = NO;
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
	[propertyReflector release];
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
	
	
	propertyReflector = [[PropertyReflector alloc] initWithTableView:propertyView];
	[propertyReflector setReflectedObject:self];
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
	else
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

- (void)mergeSelectedItems
{
	MyDocument *document = [self prepareUndoWithName:@"Merge"];
	NSMutableArray *oldItems = [items allItems];
	NSLog(@"oldItems count = %i", [oldItems count]);
	
	[items mergeSelectedItems];
	
	NSMutableArray *currentItems = [items allItems];
	NSLog(@"currentItems count = %i", [currentItems count]);
	[document swapAllItemsWithOld:oldItems 
						  current:currentItems
					   actionName:@"Merge"];	
}

// macro magic to simulate block syntax in Objective-C 2.1

#define beginFullMeshActionWithName(name) \
MyDocument *document = [self prepareUndoWithName:name]; \
MeshFullState *oldState = [items currentMeshFull];

#define endFullMeshActionWithName(name) \
MeshFullState *currentState = [items currentMeshFull]; \
[document swapMeshFullStateWithOld:oldState \
						   current:currentState \
						actionName:name];

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
	
	for (OpenGLSceneView *v in views)
	{ 
		if (v != view)
			[v setNeedsDisplay:YES]; 
	}
}

- (void)selectionChangedInView:(OpenGLSceneView *)view
{
	NSLog(@"selectionChangedInView:");
	
	for (OpenGLSceneView *v in views)
	{ 
		if (v != view)
			[v setNeedsDisplay:YES]; 
	}
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
		[self mergeSelectedItems];
	}
	else
	{
		beginFullMeshActionWithName(@"Merge")
		[[self currentMesh] mergeSelected];
		endFullMeshActionWithName(@"Merge")
	}
	
	[manipulated updateSelection];
	for (OpenGLSceneView *view in views) 
	{ 
		[view setNeedsDisplay:YES]; 
	}
}

- (IBAction)splitSelected:(id)sender
{
	NSLog(@"splitSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		beginFullMeshActionWithName(@"Split")
		[[self currentMesh] splitSelected];
		endFullMeshActionWithName(@"Split")
	}
	
	[manipulated updateSelection];
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
}

- (IBAction)flipSelected:(id)sender
{
	NSLog(@"flipSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		beginFullMeshActionWithName(@"Flip")
		[[self currentMesh] flipSelected];
		endFullMeshActionWithName(@"Flip")
	}
	
	[manipulated updateSelection];
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES];
	}
}

- (IBAction)cloneSelected:(id)sender
{	
	NSLog(@"cloneSelected:");
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
		MyDocument *document = [self prepareUndoWithName:@"Clone"];
		[document undoCloneSelected:selection];
		[manipulated cloneSelected];
	}
	else 
	{
		beginFullMeshActionWithName(@"Clone")
		[manipulated cloneSelected];
		endFullMeshActionWithName(@"Clone")
	}
	
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
	
	if (startManipulation)
	{
		[self manipulationStartedInView:nil];
	}
}

- (void)redoCloneSelected:(NSMutableArray *)selection
{
	NSLog(@"redoCloneSelected:");
	
	[self setManipulated:itemsController];
	[items setCurrentSelection:selection];
	[manipulated cloneSelected];
	
	MyDocument *document = [self prepareUndoWithName:@"Clone"];
	[document undoCloneSelected:selection];
	
	[itemsController updateSelection];
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
}

- (void)undoCloneSelected:(NSMutableArray *)selection
{	
	NSLog(@"undoCloneSelected:");
	
	[self setManipulated:itemsController];
	uint clonedCount = [selection count];
	[items removeItemsInRange:NSMakeRange([items count] - clonedCount, clonedCount)];
	[items setCurrentSelection:selection];

	MyDocument *document = [self prepareUndoWithName:@"Clone"];
	[document redoCloneSelected:selection];
		
	[itemsController updateSelection];
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
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
	else 
	{
		beginFullMeshActionWithName(@"Delete")
		[manipulated removeSelected];
		endFullMeshActionWithName(@"Delete")
	}
	
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
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
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
}

- (void)undoDeleteSelected:(NSMutableArray *)selectedItems
{
	NSLog(@"undoDeleteSelected:");
	
	[self setManipulated:itemsController];
	[items setCurrentItems:selectedItems];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document redoDeleteSelected:selectedItems];
	
	[itemsController updateSelection];
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
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
			beginFullMeshActionWithName(@"Merge Vertex Pairs")
			[[self currentMesh] mergeVertexPairs];
			endFullMeshActionWithName(@"Merge Vertex Pairs")
		}
	}
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
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
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
}

- (IBAction)invertSelection:(id)sender
{
	[[self manipulated] invertSelection];
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
}

- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"MyDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    [super windowControllerDidLoadNib:aController];
    // Add any code here that needs to be executed once the windowController has loaded the document's window.
}

#pragma mark Archivation

- (BOOL)readFromBullet:(NSString *)fileName
{
	// from DemoApplication 
	btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	btDbvtBroadphase *broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
	btDynamicsWorld *dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));	
	btBulletWorldImporter *worldImporter = new btBulletWorldImporter(dynamicsWorld);
	
	// loading file from ASCII fileName
	if (worldImporter->loadFile([fileName cStringUsingEncoding:NSASCIIStringEncoding]))
	{
		return YES;
	}
	return NO;
}

- (void)readFromTmd:(NSString *)fileName
{
	TmdModel *model = new TmdModel();
	model->Load([fileName cStringUsingEncoding:NSASCIIStringEncoding]);
	
	ItemCollection *newItems = [[ItemCollection alloc] init];
	for (int i = 0; i < model->desc.no_meshes; i++)
	{
		Item *item = [[Item alloc] init];
		Mesh *itemMesh = [item mesh];
		
		mesh &tmdMesh = model->meshes[i];
		mesh_desc &tmdMeshDesc = model->m_descs[i];
		
		itemMesh->vertices->clear();
		for (int i = 0; i < tmdMeshDesc.no_vertices; i++)
		{
			itemMesh->vertices->push_back(tmdMesh.vertices[i]);
		}
		
		itemMesh->triangles->clear();
		for (int i = 0; i < tmdMeshDesc.no_faces; i++)
		{
			face &f = tmdMesh.faces[i];
			Triangle tri = MakeTriangle(f.vertID[2], f.vertID[1], f.vertID[0]);
			itemMesh->triangles->push_back(tri);
		}
		
		[itemMesh setSelectionMode:[itemMesh selectionMode]];
		
		[newItems addItem:item];
		[item release];
	}
	
	delete model;
	
	[items release];
	items = newItems;
	[itemsController setModel:items];
	[itemsController updateSelection];
	[self setManipulated:itemsController];
}

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
	else if ([typeName isEqual:@"TMD"])
	{
		[self readFromTmd:fileName];
		return YES;
	}
	else if ([typeName isEqual:@"bullet"])
	{
		return [self readFromBullet:fileName];
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

// fix for issue four-views works independetly on Mac version
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

- (void)toggleOneViewFourView:(id)sender
{
	NSLog(@"toggleOneViewFourView");
	
	if (oneView)
	{
		[viewPerspective setCameraMode:CameraModePerspective];
		NSRect frame = [viewPerspective frame];
		[[[mainSplit subviews] objectAtIndex:0] setFrame:frame];
		oneView = NO;
		return;
	}
	
	NSWindow *window = [viewPerspective window];
	NSPoint point = [window convertScreenToBase:[NSEvent mouseLocation]];
	
	NSView *hittedView = [[[window contentView] superview] hitTest:point];
		
	for (OpenGLSceneView *view in views)
	{
		if (view == hittedView)
		{
			oneView = YES;
			
			[[[topSplit subviews] objectAtIndex:0] setFrame:NSZeroRect];
			[[[bottomSplit subviews] objectAtIndex:0] setFrame:NSZeroRect];
			[[[mainSplit subviews] objectAtIndex:0] setFrame:NSZeroRect];
			
			[viewPerspective setCameraMode:[view cameraMode]];
			
			return;
		}
	}
	
	NSLog(@"No view is under mouse");
}

@end
