//
//  MyDocument.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#import "MyDocument.h"
#import "ItemManipulationState.h"
#import "MeshManipulationState.h"
#import "TmdModel.h"
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

		[itemsController addSelectionObserver:self];
		[meshController addSelectionObserver:self];
		
		manipulationFinished = YES;
		oldManipulations = nil;
    }
    return self;
}

- (void)dealloc
{
	[itemsController removeSelectionObserver:self];
	[meshController removeSelectionObserver:self];
	[meshController release];
	[itemsController release];
	[items release];
	[oldManipulations release];
	[super dealloc];
}

- (void)awakeFromNib
{
	[editModePopUp selectItemWithTag:0];
	[viewModePopUp selectItemWithTag:0];
	[view setManipulated:manipulated];
	[view setDisplayed:itemsController];
	[view setDelegate:self];
}

- (id<OpenGLManipulating>)manipulated
{
	return manipulated;
}

- (void)setManipulated:(id<OpenGLManipulating>)value
{
	manipulated = value;
	[manipulated setCurrentManipulator:[view currentManipulator]];
	[view setManipulated:value];
	[view setNeedsDisplay:YES];
}

- (MyDocument *)prepareUndoWithName:(NSString *)actionName
{
	NSUndoManager *undo = [self undoManager];
	MyDocument *document = [undo prepareWithInvocationTarget:self];
	if (![undo isUndoing])
		[undo setActionName:actionName];
	return document;
}

- (void)addItem:(Item *)item withName:(NSString *)name
{
	NSLog(@"item vertexCount = %i", [[item mesh] vertexCount]);
	NSLog(@"item triangleCount = %i", [[item mesh] triangleCount]);
	NSLog(@"adding %@", name);
	
	MyDocument *document = [self prepareUndoWithName:[NSString stringWithFormat:@"Add %@", name]];
	[document removeItem:item withName:name];
		
	[items addItem:item];
	[itemsController changeSelection:NO];
	[items setSelected:YES atIndex:[items count] - 1];
	[itemsController updateSelection];
	[view setNeedsDisplay:YES];
}

- (void)removeItem:(Item *)item withName:(NSString *)name
{
	NSLog(@"removing %@", name);
	
	MyDocument *document = [self prepareUndoWithName:[NSString stringWithFormat:@"Remove %@", name]];
	[document addItem:item withName:name];
		
	[items removeItem:item];
	[itemsController changeSelection:NO];
	[view setNeedsDisplay:YES];
}

- (float)selectionX
{
	return [manipulated selectionX];
}

- (float)selectionY
{
	return [manipulated selectionY];
}

- (float)selectionZ
{
	return [manipulated selectionZ];
}

- (void)setSelectionX:(float)value
{
	[self manipulationStarted];
	[manipulated setSelectionX:value];
	[self manipulationEnded];
}

- (void)setSelectionY:(float)value
{
	[self manipulationStarted];
	[manipulated setSelectionY:value];
	[self manipulationEnded];
}

- (void)setSelectionZ:(float)value
{
	[self manipulationStarted];
	[manipulated setSelectionZ:value];
	[self manipulationEnded];
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
	[view setNeedsDisplay:YES];
}

- (void)manipulationStarted
{
	NSLog(@"manipulationStarted");
	manipulationFinished = NO;
	
	if (manipulated == itemsController)
	{
		oldManipulations = [items currentManipulations];
	}
	else if (manipulated == meshController)
	{
		NSLog(@"started on mesh");
	}
}

- (void)manipulationEnded
{
	NSLog(@"manipulationEnded");	
	manipulationFinished = YES;
	
	if (manipulated == itemsController)
	{
		MyDocument *document = [self prepareUndoWithName:@"Manipulations"];
		[document swapManipulationsWithOld:oldManipulations current:[items currentManipulations]];
		oldManipulations = nil;
	}
	else if (manipulated == meshController)
	{
		NSLog(@"ended on mesh");
	}
}

- (IBAction)addCube:(id)sender
{
	Item *cube = [[Item alloc] init];
	[[cube mesh] makeCube];
	[self addItem:cube withName:@"Cube"];
}

- (IBAction)addCylinder:(id)sender
{
	itemWithSteps = ItemWithStepsCylinder;
	[addItemWithStepsSheetController beginSheetWithProtocol:self];
}

- (IBAction)addSphere:(id)sender
{
	itemWithSteps = ItemWithStepsSphere;
	[addItemWithStepsSheetController beginSheetWithProtocol:self];
}

- (void)addItemWithSteps:(uint)steps
{
	Item *item = [[Item alloc] init];
	switch (itemWithSteps)
	{
		case ItemWithStepsCylinder:
			[[item mesh] makeCylinderWithSteps:steps];
			[self addItem:item withName:@"Cylinder"];
			break;
		case ItemWithStepsSphere:
			[[item mesh] makeSphereWithSteps:steps];
			[self addItem:item withName:@"Sphere"];
			break;
		default:
			break;
	}
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
	Mesh *mesh = (Mesh *)[meshController model];
	[mesh setSelectionMode:MeshSelectionModeVertices];
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

- (IBAction)changeCameraMode:(id)sender
{
	CameraMode mode = (CameraMode)[[cameraModePopUp selectedItem] tag];
	[view setCameraMode:mode];
}

- (IBAction)changeViewMode:(id)sender
{
	ViewMode mode = (ViewMode)[[viewModePopUp selectedItem] tag];
	[view setViewMode:mode];
}

- (IBAction)mergeSelected:(id)sender
{
	NSLog(@"mergeSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == itemsController)
	{
		[items mergeSelectedItems];
		[itemsController updateSelection];
	}
	else
	{
		Mesh *mesh = (Mesh *)[meshController model];
		[mesh mergeSelected];
	}
	[view setNeedsDisplay:YES];
}

- (IBAction)splitSelected:(id)sender
{
	NSLog(@"splitSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		Mesh *mesh = (Mesh *)[meshController model];
		[mesh splitSelected];
	}
	[view setNeedsDisplay:YES];
}

- (IBAction)flipSelected:(id)sender
{
	NSLog(@"flipSelected:");
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		Mesh *mesh = (Mesh *)[meshController model];
		[mesh flipSelected];
	}
	[view setNeedsDisplay:YES];
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
		[self manipulationEnded];
	}
	
	if (manipulated == itemsController)
	{
		NSMutableArray *selection = [items currentSelection];
		MyDocument *document = [self prepareUndoWithName:@"Clone"];
		[document undoCloneSelected:selection];
	}
			
	[manipulated cloneSelected];
	[view setNeedsDisplay:YES];
	
	if (startManipulation)
	{
		[self manipulationStarted];
	}
}

- (void)redoCloneSelected:(NSMutableArray *)selection
{
	NSLog(@"redoCloneSelected:");
	
	[items setCurrentSelection:selection];
	[manipulated cloneSelected];
	
	MyDocument *document = [self prepareUndoWithName:@"Clone"];
	[document undoCloneSelected:selection];
	
	[itemsController updateSelection];
	[view setNeedsDisplay:YES];
}

- (void)undoCloneSelected:(NSMutableArray *)selection
{	
	NSLog(@"undoCloneSelected:");
		
	uint clonedCount = [selection count];
	[items removeItemsInRange:NSMakeRange([items count] - clonedCount, clonedCount)];
	[items setCurrentSelection:selection];

	MyDocument *document = [self prepareUndoWithName:@"Clone"];
	[document redoCloneSelected:selection];
		
	[itemsController updateSelection];
	[view setNeedsDisplay:YES];
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
	}
	
	[manipulated removeSelected];
	[view setNeedsDisplay:YES];
}

- (void)redoDeleteSelected:(NSMutableArray *)selectedItems
{
	NSLog(@"redoDeleteSelected:");

	[items setSelectionFromIndexedItems:selectedItems];
	[manipulated removeSelected];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document undoDeleteSelected:selectedItems];
	
	[itemsController updateSelection];
	[view setNeedsDisplay:YES];
}

- (void)undoDeleteSelected:(NSMutableArray *)selectedItems
{
	NSLog(@"undoDeleteSelected:");
	
	[items setCurrentItems:selectedItems];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document redoDeleteSelected:selectedItems];
	
	[itemsController updateSelection];
	[view setNeedsDisplay:YES];
}

- (IBAction)mergeVertexPairs:(id)sender
{
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		Mesh *mesh = (Mesh *)[meshController model];
		if ([mesh selectionMode] == MeshSelectionModeVertices)
			[mesh mergeVertexPairs];
	}
	[view setNeedsDisplay:YES];
}

- (IBAction)changeManipulator:(id)sender
{
	ManipulatorType newManipulator = (ManipulatorType)[sender tag];
	[[self manipulated] setCurrentManipulator:newManipulator];
	[view setCurrentManipulator:newManipulator];
}

- (IBAction)selectAll:(id)sender
{
	[[self manipulated] changeSelection:YES];
	[view setNeedsDisplay:YES];
}

- (IBAction)invertSelection:(id)sender
{
	[[self manipulated] invertSelection];
	[view setNeedsDisplay:YES];
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

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
	return [NSKeyedArchiver archivedDataWithRootObject:items];
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
	}
	
	delete model;
	
	[items release];
	items = newItems;
	[itemsController setModel:items];
	[itemsController updateSelection];
	[self setManipulated:itemsController];
}

- (BOOL)readFromFile:(NSString *)fileName ofType:(NSString *)typeName
{
	NSLog(@"readFromFile typeName:%@", typeName);
	if ([typeName isEqual:@"model3D"])
	{
		return [self readFromData:[NSData dataWithContentsOfFile:fileName] ofType:typeName error:NULL];
	}
	else if ([typeName isEqual:@"TMD"])
	{
		[self readFromTmd:fileName];
		return YES;
	}
	return NO;
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
	NSLog(@"readFromData typeName:%@", typeName);
	ItemCollection *newItems = nil;
	@try
	{
		newItems = [NSKeyedUnarchiver unarchiveObjectWithData:data];
	}
	@catch (NSException *e)
	{
		if (outError)
		{
			NSDictionary *d = 
			[NSDictionary dictionaryWithObject:@"The data is corrupted."
										forKey:NSLocalizedFailureReasonErrorKey];
			
			*outError = [NSError errorWithDomain:NSOSStatusErrorDomain 
											code:unimpErr
										userInfo:d];
		}
		return NO;
	}
	[newItems retain];
	[items release];
	items = newItems;
	[itemsController setModel:items];
	[itemsController updateSelection];
	[self setManipulated:itemsController];
	return YES;
}

@end
