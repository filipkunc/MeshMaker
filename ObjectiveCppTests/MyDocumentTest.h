//
//  MyDocumentTest.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/2/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <SenTestingKit/SenTestingKit.h>
#import "MyDocument.h"

@interface MyDocumentTest : SenTestCase 
{
	MyDocument *document;
}

- (void)testUndoMakeCube;
- (void)testUndoItemManipulation;

@end
