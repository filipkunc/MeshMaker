//
//  MyDocumentTest.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/2/09.
//  For license see LICENSE.TXT
//

#import "MyDocumentTest.h"

@implementation MyDocumentTest

- (void)testUndoMakeCube
{
	document = [[MyDocument alloc] init];
	
	STAssertNotNil(document, @"document can't be nil");
	
	// check the initial state
	STAssertEquals([document->items count], 0U, @"items must be empty");	
	
	// adding cube adds one item on undo stack
	[document addCube:self];
	
	STAssertEquals([document->items count], 1U, @"items must contain one item");	
	
	// undo this action
	[[document undoManager] undo];
	
	// state must be same as before
	STAssertEquals([document->items count], 0U, @"items must be empty");
	
	[document release];
}

- (void)testUndoItemManipulation
{
	document = [[MyDocument alloc] init];
	
	STAssertNotNil(document, @"document can't be nil");
	STAssertEquals([document->items count], 0U, @"items must be empty");	
	[document addCube:self];
	STAssertEquals([document->items count], 1U, @"items must contain one item");
	
	[document manipulationStarted];
	[document->itemsController moveSelectedByOffset:Vector3D(0, 5, 0)]; 
	[document manipulationEnded];

	Item *item = [document->items itemAtIndex:0];
	STAssertEquals([item position], Vector3D(0, 5, 0), @"item position after move must be x = 0, y = 5, z = 0");
	
	[[document undoManager] undo];
	STAssertEquals([item position], Vector3D(0, 0, 0), @"item position after undo must be x = 0, y = 0, z = 0");	
	
	[[document undoManager] redo];
	STAssertEquals([item position], Vector3D(0, 5, 0), @"item position after redo must be x = 0, y = 5, z = 0");
}

@end
