//
//  MyDocument.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#pragma once

#import <Cocoa/Cocoa.h>
#import "ItemCollection.h"
#import "OpenGLSceneView.h"
#import "AddItemWithStepsSheetController.h"
#import "FPTexturePaintToolWindowController.h"
#import "FPTextureBrowserWindowController.h"
#import "FPSelectionWindowController.h"
#import "VertexWindowController.h"

@interface UndoStatePointer : NSObject
{
    IUndoState *_undoState;
}

- (id)initWithUndoState:(IUndoState *)undoState;

@property (readonly, assign) IUndoState *undoState;

@end

@interface MyDocument : NSDocument <
    AddItemWithStepsProtocol,
    OpenGLSceneViewDelegate,
    FPTextureBrowserDelegate,
    FPSelectionWindowDelegate,
    NSWindowDelegate
>
{
@public // public for unit tests
	BOOL manipulationFinished;
	ItemCollection *items;
    TextureCollection *textures;
	OpenGLManipulatingController *itemsController;
	OpenGLManipulatingController *meshController;
    IOpenGLManipulating *manipulated;
	IBOutlet OpenGLSceneView *viewLeft;
	IBOutlet OpenGLSceneView *viewTop;
	IBOutlet OpenGLSceneView *viewFront;
	IBOutlet OpenGLSceneView *viewPerspective;
	IBOutlet NSPopUpButton *editModePopUp;
	IBOutlet AddItemWithStepsSheetController *addItemWithStepsSheetController;
	enum MeshType itemWithSteps;
	
	NSMutableArray *views;
	UndoStatePointer *oldManipulations;
	UndoStatePointer *oldMeshState;
    
    enum ManipulatorType currentManipulator;
	
	IBOutlet NSSplitView *topSplit;
	IBOutlet NSSplitView *bottomSplit;
	IBOutlet NSSplitView *mainSplit;
	OpenGLSceneView *oneView;
    IBOutlet FPTexturePaintToolWindowController *texturePaintToolWindowController;
    IBOutlet FPTextureBrowserWindowController *textureBrowserWindowController;
    IBOutlet FPSelectionWindowController *selectionWindowController;
    IBOutlet VertexWindowController *vertexWindowController;
    
    IBOutlet NSPopUpButton *scriptPullDown;
}

@property (readwrite, assign) IOpenGLManipulating *manipulated;
@property (readonly) NSPopUpButton *scriptPullDown;
@property (readwrite, assign) BOOL syncCameraZoomAndPosition;

- (void)setNeedsDisplayExceptView:(OpenGLSceneView *)view;
- (void)setNeedsDisplayOnAllViews;
- (Mesh2 *)currentMesh;
- (MyDocument *)prepareUndoWithName:(NSString *)actionName;
- (void)swapManipulationsWithOld:(UndoStatePointer *)old
                         current:(UndoStatePointer *)current;

- (void)swapAllItemsWithOld:(UndoStatePointer *)old
					current:(UndoStatePointer *)current
				 actionName:(NSString *)actionName;

- (void)swapMeshStateWithOld:(UndoStatePointer *)old
					 current:(UndoStatePointer *)current
				  actionName:(NSString *)actionName;

- (void)allItemsActionWithName:(NSString *)actionName block:(void (^)())action;
- (void)meshActionWithName:(NSString *)actionName block:(void (^)())action;
- (void)addItemWithType:(enum MeshType)type steps:(unsigned int)steps;
- (void)removeItemWithType:(enum MeshType)type steps:(unsigned int)steps;
- (IBAction)addCube:(id)sender;
- (IBAction)addCylinder:(id)sender;
- (IBAction)addSphere:(id)sender;
- (IBAction)addIcosahedron:(id)sender;
- (void)editMeshWithMode:(enum MeshSelectionMode)mode;
- (void)editItems;
- (void)meshOnlyActionWithName:(NSString *)actionName block:(void (^)())action;
- (IBAction)changeEditMode:(id)sender;
- (IBAction)changeManipulator:(id)sender;
- (IBAction)mergeSelected:(id)sender;
- (IBAction)splitSelected:(id)sender;
- (IBAction)duplicateSelected:(id)sender;
- (void)redoDuplicateSelected:(UndoStatePointer *)selection;
- (void)undoDuplicateSelected:(UndoStatePointer *)selection;
- (IBAction)flipSelected:(id)sender;
- (IBAction)deleteSelected:(id)sender;
- (void)redoDeleteSelected:(UndoStatePointer *)selectedItems;
- (void)undoDeleteSelected:(UndoStatePointer *)selectedItems;
- (IBAction)selectAll:(id)sender;
- (IBAction)invertSelection:(id)sender;
- (IBAction)hideSelected:(id)sender;
- (IBAction)unhideAll:(id)sender;
- (IBAction)extrudeSelected:(id)sender;
- (IBAction)detachSelected:(id)sender;
- (IBAction)subdivision:(id)sender;
- (IBAction)cleanTexture:(id)sender;
- (IBAction)resetTexCoords:(id)sender;
- (IBAction)triangulate:(id)sender;
- (IBAction)viewTexturePaintTool:(id)sender;
- (IBAction)viewTextureBrowser:(id)sender;
- (IBAction)viewSelectionTool:(id)sender;
- (IBAction)viewVertexTool:(id)sender;
- (IBAction)importPointCloud:(id)sender;

@end

@interface MyDocument (Archiving)

+ (ItemCollection *)readItemsFromWavefrontObject:(NSData *)data;

@end
