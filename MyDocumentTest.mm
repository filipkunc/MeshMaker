//
//  MyDocumentTest.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/2/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MyDocumentTest.h"

@implementation MyDocumentTest

- (void)testUndo
{
	document = [[MyDocument alloc] init];
	
	STAssertNotNil(document, @"document can't be nil");
	
	// check the initial state
	STAssertEquals([document->items count], 0UL, @"items must be empty");	
	
	// adding cube adds one item on undo stack
	[document addCube:self];
	
	STAssertEquals([document->items count], 1UL, @"items must contain one item");	
	
	// undo this action
	[[document undoManager] undo];
	
	// state must be same as before
	STAssertEquals([document->items count], 0UL, @"items must be empty");
}

@end
