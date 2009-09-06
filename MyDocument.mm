//
//  MyDocument.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  Copyright __MyCompanyName__ 2009 . All rights reserved.
//

#import "MyDocument.h"

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
    }
    return self;
}

- (void)dealloc
{
	[meshController release];
	[itemsController release];
	[items release];
	[super dealloc];
}

- (void)awakeFromNib
{
	[editModePopUp selectItemWithTag:0];
	[view setManipulated:manipulated];
	[view setDisplayed:itemsController];
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

- (IBAction)addCube:(id)sender
{
	Item *cube = [[Item alloc] initWithPosition:Vector3D() rotation:Quaternion() scale:Vector3D(1, 1, 1)];
	[[cube mesh] makeCube];
	NSLog(@"cube vertexCount = %i",[[cube mesh] vertexCount]);
	NSLog(@"cube triangleCount = %i",[[cube mesh] triangleCount]);
	[items addItem:cube];
	[view setNeedsDisplay:YES];
}

- (IBAction)addCylinder:(id)sender
{
	Item *cylinder = [[Item alloc] initWithPosition:Vector3D() rotation:Quaternion() scale:Vector3D(1, 1, 1)];
	[[cylinder mesh] makeCylinderWithSteps:10];
	NSLog(@"cylinder vertexCount = %i",[[cylinder mesh] vertexCount]);
	NSLog(@"cylinder triangleCount = %i",[[cylinder mesh] triangleCount]);
	[items addItem:cylinder];
	[view setNeedsDisplay:YES];
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

- (IBAction)collapseSelected:(id)sender
{
	if (manipulated == itemsController)
	{
		[items collapseSelectedItems];
		[itemsController updateSelection];
	}
	else
	{
		Mesh *mesh = (Mesh *)[meshController model];
		if ([mesh selectionMode] == MeshSelectionModeVertices)
			[mesh collapseSelectedVertices];
	}
	[view setNeedsDisplay:YES];
}

- (IBAction)splitSelected:(id)sender
{
	if (manipulated == meshController)
	{
		Mesh *mesh = (Mesh *)[meshController model];
		if ([mesh selectionMode] == MeshSelectionModeEdges)
			[mesh splitSelectedEdges];
	}
	[view setNeedsDisplay:YES];
}

- (IBAction)turnSelectedEdges:(id)sender
{
	if (manipulated == meshController)
	{
		Mesh *mesh = (Mesh *)[meshController model];
		if ([mesh selectionMode] == MeshSelectionModeEdges)
			[mesh turnSelectedEdges];
	}
	[view setNeedsDisplay:YES];
}

- (IBAction)changeManipulator:(id)sender
{
	ManipulatorType newManipulator = (ManipulatorType)[sender tag];
	[[self manipulated] setCurrentManipulator:newManipulator];
	[view setCurrentManipulator:newManipulator];
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
    // Insert code here to write your document to data of the specified type. If the given outError != NULL, ensure that you set *outError when returning nil.

    // You can also choose to override -fileWrapperOfType:error:, -writeToURL:ofType:error:, or -writeToURL:ofType:forSaveOperation:originalContentsURL:error: instead.

    // For applications targeted for Panther or earlier systems, you should use the deprecated API -dataRepresentationOfType:. In this case you can also choose to override -fileWrapperRepresentationOfType: or -writeToFile:ofType: instead.

    if ( outError != NULL ) {
		*outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:unimpErr userInfo:NULL];
	}
	return nil;
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
    // Insert code here to read your document from the given data of the specified type.  If the given outError != NULL, ensure that you set *outError when returning NO.

    // You can also choose to override -readFromFileWrapper:ofType:error: or -readFromURL:ofType:error: instead. 
    
    // For applications targeted for Panther or earlier systems, you should use the deprecated API -loadDataRepresentation:ofType. In this case you can also choose to override -readFromFile:ofType: or -loadFileWrapperRepresentation:ofType: instead.
    
    if ( outError != NULL ) {
		*outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:unimpErr userInfo:NULL];
	}
    return YES;
}

@end
