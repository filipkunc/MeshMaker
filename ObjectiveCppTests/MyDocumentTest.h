//
//  MyDocumentTest.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/2/09.
//  For license see LICENSE.TXT
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
