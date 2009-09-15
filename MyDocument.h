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
#import "AddCylinderSheetController.h"

enum EditMode
{
	EditModeItems = 0,
	EditModeVertices = 1,
	EditModeTriangles = 2,
	EditModeEdges = 3
};

@interface MyDocument : NSDocument <AddCylinderProtocol>
{
	ItemCollection *items;
	OpenGLManipulatingController *itemsController;
	OpenGLManipulatingController *meshController;
	id<OpenGLManipulating> manipulated;
	IBOutlet OpenGLSceneView *view;
	IBOutlet NSPopUpButton *editModePopUp;
	IBOutlet AddCylinderSheetController *addCylinderSheetController;
}

@property (readwrite, assign) id<OpenGLManipulating> manipulated;

- (void)addItem:(Item *)item;
- (IBAction)addCube:(id)sender;
- (IBAction)addCylinder:(id)sender;
- (void)addCylinderWithSteps:(NSUInteger)steps;
- (IBAction)addSphere:(id)sender;
- (void)editMeshWithMode:(enum MeshSelectionMode)mode;
- (void)editItems;
- (IBAction)changeEditMode:(id)sender;
- (IBAction)mergeSelected:(id)sender;
- (IBAction)splitSelected:(id)sender;
- (IBAction)turnSelectedEdges:(id)sender;
- (IBAction)mergeVertexPairs:(id)sender;
- (IBAction)changeManipulator:(id)sender;
- (IBAction)cloneSelected:(id)sender;
- (IBAction)deleteSelected:(id)sender;
- (IBAction)selectAll:(id)sender;
- (IBAction)invertSelection:(id)sender;

@end
