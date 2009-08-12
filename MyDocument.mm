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

- (IBAction)addMesh:(id)sender
{
	[items addItem:[[Item alloc] initWithPosition:Vector3D() rotation:Quaternion() scale:Vector3D(1, 1, 1)]];
	[view setNeedsDisplay:YES];
}

- (IBAction)editVertices:(id)sender
{
	if (manipulated == itemsController)
	{
		NSInteger index = [itemsController lastSelectedIndex];
		if (index > -1)
		{
			Item *item = [items itemAtIndex:index];
			[meshController setModel:[item mesh]];
			[meshController setPosition:[item position] 
							   rotation:[item rotation] 
								  scale:[item scale]];
			[self setManipulated:meshController];
		}
	}
	else
	{
		[itemsController setModel:items];
		[itemsController setPosition:Vector3D()
							rotation:Quaternion()
							   scale:Vector3D(1, 1, 1)];
		[self setManipulated:itemsController];
	}
}

- (IBAction)collapseVertices:(id)sender
{
	if (manipulated == itemsController)
	{
		Mesh *mesh = nil;
		Matrix4x4 firstMatrix, itemMatrix;
		for (int i = 0; i < [items count]; i++)
		{
			if ([items isSelectedAtIndex:i])
			{
				Item *item = [items itemAtIndex:i];
				if (mesh == nil)
				{
					mesh = [[items itemAtIndex:i] mesh];
					firstMatrix.TranslateRotateScale([item position],
													 [item rotation],
													 [item scale]);
					firstMatrix = firstMatrix.Inverse();
				}
				else
				{
					itemMatrix.TranslateRotateScale([item position],
													 [item rotation],
													 [item scale]);
					Matrix4x4 finalMatrix = firstMatrix * itemMatrix;
					[[item mesh] transformWithMatrix:finalMatrix];
					[mesh mergeWithMesh:[item mesh]];
					[items removeAtIndex:i];
					i--;
				}
			}
		}
	}
	else
	{
		Mesh *mesh = (Mesh *)[meshController model];
		[mesh collapseSelectedVertices];
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
