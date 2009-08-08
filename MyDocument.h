//
//  MyDocument.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  Copyright __MyCompanyName__ 2009 . All rights reserved.
//


#import <Cocoa/Cocoa.h>
#import "ItemCollection.h"
#import "OpenGLSceneView.h"

@interface MyDocument : NSDocument
{
	ItemCollection *items;
	OpenGLManipulatingController *itemsController;
	OpenGLManipulatingController *meshController;
	IBOutlet OpenGLSceneView *view;
}

- (id<OpenGLManipulating>)manipulated;
- (IBAction)addMesh:(id)sender;
- (IBAction)editVertices:(id)sender;
- (IBAction)changeManipulator:(id)sender;

@end
