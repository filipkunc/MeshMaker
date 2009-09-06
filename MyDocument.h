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

enum EditMode
{
	EditModeItems = 0,
	EditModeVertices = 1,
	EditModeTriangles = 2,
	EditModeEdges = 3
};

@interface MyDocument : NSDocument
{
	ItemCollection *items;
	OpenGLManipulatingController *itemsController;
	OpenGLManipulatingController *meshController;
	id<OpenGLManipulating> manipulated;
	IBOutlet OpenGLSceneView *view;
	IBOutlet NSPopUpButton *editModePopUp;
}

@property (readwrite, assign) id<OpenGLManipulating> manipulated;

- (IBAction)addCube:(id)sender;
- (IBAction)addCylinder:(id)sender;
- (void)editMeshWithMode:(enum MeshSelectionMode)mode;
- (void)editItems;
- (IBAction)changeEditMode:(id)sender;
- (IBAction)collapseSelected:(id)sender;
- (IBAction)splitSelected:(id)sender;
- (IBAction)turnSelectedEdges:(id)sender;
- (IBAction)changeManipulator:(id)sender;

@end
