//
//  MyDocument.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "ItemCollection.h"
#import "OpenGLSceneView.h"
#import "AddItemWithStepsSheetController.h"
#import "FPTexturePaintToolWindowController.h"

@interface MyDocument : NSDocument <AddItemWithStepsProtocol, OpenGLSceneViewDelegate, OpenGLTransforming, NSTableViewDataSource, NSTableViewDelegate, NSWindowDelegate>
{
@public // public for unit tests
	BOOL manipulationFinished;
	ItemCollection *items;
	OpenGLManipulatingController *itemsController;
	OpenGLManipulatingController *meshController;
	id<OpenGLManipulating> manipulated;
	IBOutlet OpenGLSceneView *viewLeft;
	IBOutlet OpenGLSceneView *viewTop;
	IBOutlet OpenGLSceneView *viewFront;
	IBOutlet OpenGLSceneView *viewPerspective;
	IBOutlet NSPopUpButton *editModePopUp;
	IBOutlet NSPopUpButton *viewModePopUp;
	IBOutlet AddItemWithStepsSheetController *addItemWithStepsSheetController;
	enum MeshType itemWithSteps;
	
	NSMutableArray *views;
	NSMutableArray *oldManipulations;
	MeshState *oldMeshState;
	
	IBOutlet NSSplitView *topSplit;
	IBOutlet NSSplitView *bottomSplit;
	IBOutlet NSSplitView *mainSplit;
	OpenGLSceneView *oneView;
    IBOutlet FPTexturePaintToolWindowController *texturePaintToolWindowController;
    
    enum ManipulatorType currentManipulator;
}

@property (readwrite, assign) id<OpenGLManipulating> manipulated;

+ (BOOL)softSelection;
+ (void)setSoftSelection:(BOOL)value;
- (void)setNeedsDisplayExceptView:(OpenGLSceneView *)view;
- (void)setNeedsDisplayOnAllViews;
- (Mesh *)currentMesh;
- (MyDocument *)prepareUndoWithName:(NSString *)actionName;
- (void)swapManipulationsWithOld:(NSMutableArray *)old current:(NSMutableArray *)current;

- (void)swapAllItemsWithOld:(NSMutableArray *)old
					current:(NSMutableArray *)current
				 actionName:(NSString *)actionName;

- (void)swapMeshStateWithOld:(MeshState *)old 
					 current:(MeshState *)current 
				  actionName:(NSString *)actionName;

- (void)allItemsActionWithName:(NSString *)actionName block:(void (^)())action;
- (void)meshActionWithName:(NSString *)actionName block:(void (^)())action;
- (void)addItemWithType:(enum MeshType)type steps:(uint)steps;
- (void)removeItemWithType:(enum MeshType)type steps:(uint)steps;
- (IBAction)addCube:(id)sender;
- (IBAction)addCylinder:(id)sender;
- (IBAction)addSphere:(id)sender;
- (void)editMeshWithMode:(enum MeshSelectionMode)mode;
- (void)editItems;
- (IBAction)changeEditMode:(id)sender;
- (IBAction)changeManipulator:(id)sender;
- (IBAction)changeViewMode:(id)sender;
- (IBAction)mergeSelected:(id)sender;
- (IBAction)unionSelected:(id)sender;
- (IBAction)subtractSelected:(id)sender;
- (IBAction)intersectSelected:(id)sender;
- (IBAction)splitSelected:(id)sender;
- (IBAction)duplicateSelected:(id)sender;
- (void)redoDuplicateSelected:(NSMutableArray *)selection;
- (void)undoDuplicateSelected:(NSMutableArray *)selection;
- (IBAction)flipSelected:(id)sender;
- (IBAction)deleteSelected:(id)sender;
- (void)redoDeleteSelected:(NSMutableArray *)selectedItems;
- (void)undoDeleteSelected:(NSMutableArray *)selectedItems;
- (IBAction)selectAll:(id)sender;
- (IBAction)invertSelection:(id)sender;
- (IBAction)hideSelected:(id)sender;
- (IBAction)unhideAll:(id)sender;
- (IBAction)detachSelected:(id)sender;
- (BOOL)readFromModel3D:(NSData *)data;
- (NSData *)dataOfModel3D;
- (void)toggleOneViewFourView:(id)sender;
- (IBAction)subdivision:(id)sender;
- (IBAction)softSelection:(id)sender;
- (IBAction)cleanTexture:(id)sender;
- (IBAction)resetTexCoords:(id)sender;
- (IBAction)viewTexturePaintTool:(id)sender;

@end
