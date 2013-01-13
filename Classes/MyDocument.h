//
//  MyDocument.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#pragma once

#if defined(__APPLE__)

#import <Cocoa/Cocoa.h>
#import "ItemCollection.h"
#import "OpenGLSceneView.h"
#import "AddItemWithStepsSheetController.h"
#import "FPTexturePaintToolWindowController.h"
#import "FPTextureBrowserWindowController.h"
#import "ScriptWindowController.h"

@interface UndoStatePointer : NSObject
{
    IUndoState *_undoState;
}

- (id)initWithUndoState:(IUndoState *)undoState;

@property (readonly, assign) IUndoState *undoState;

@end

@interface MyDocument : NSDocument <AddItemWithStepsProtocol, OpenGLSceneViewDelegate, ScriptDelegate, NSWindowDelegate>
{
@public // public for unit tests
	BOOL manipulationFinished;
	ItemCollection *items;
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
    IBOutlet ScriptWindowController *scriptWindowController;
    
    IBOutlet NSPopUpButton *scriptPullDown;
}

@property (readwrite, assign) IOpenGLManipulating *manipulated;
@property (readonly) NSPopUpButton *scriptPullDown;

+ (BOOL)softSelection;
+ (void)setSoftSelection:(BOOL)value;
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
- (void)addItemWithType:(enum MeshType)type steps:(uint)steps;
- (void)removeItemWithType:(enum MeshType)type steps:(uint)steps;
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
- (IBAction)softSelection:(id)sender;
- (IBAction)cleanTexture:(id)sender;
- (IBAction)resetTexCoords:(id)sender;
- (IBAction)triangulate:(id)sender;
- (IBAction)viewTexturePaintTool:(id)sender;
- (IBAction)viewTextureBrowser:(id)sender;
- (IBAction)viewScriptEditor:(id)sender;

@end

@interface MyDocument (Archiving)

@end

#elif defined(WIN32)

#include "ItemCollection.h"
#include "OpenGLSceneView.h"

namespace MeshMakerCppCLI
{
	public interface class IDocumentDelegate
	{
		property ToolStripComboBox ^editModePopup { ToolStripComboBox ^ get(); }
		void updateSelectionValues();
	};

	public ref class MyDocument : public IOpenGLSceneViewDelegate, public IOpenGLManipulatingControllerKVC
	{
	private:
		IDocumentDelegate ^documentDelegate;

		ItemCollection *items;
		OpenGLManipulatingController *itemsController;
		OpenGLManipulatingController *meshController;
		IOpenGLManipulating *manipulated;

		bool manipulationFinished;

		array<OpenGLSceneView ^> ^views;
		OpenGLSceneView ^viewLeft;
		OpenGLSceneView ^viewTop;
		OpenGLSceneView ^viewFront;
		OpenGLSceneView ^viewPerspective;

		ManipulatorType currentManipulator;
	private:
		void setManipulated(IOpenGLManipulating *value);
		Mesh2 *currentMesh();
		void editItems();
		void editMesh(MeshSelectionMode mode);
	public:
		MyDocument(IDocumentDelegate ^documentForm);
		~MyDocument();


		property array<OpenGLSceneView^ > ^Views 
		{
			array<OpenGLSceneView^ > ^get() { return views; }
		}

		property ManipulatorType CurrentManipulator
		{
			ManipulatorType get() 
			{
				return currentManipulator; 
			}
			void set(ManipulatorType value)
			{
				currentManipulator = value;
				itemsController->setCurrentManipulator(value, manipulated != itemsController);
				meshController->setCurrentManipulator(value, manipulated != meshController);

				for each (OpenGLSceneView ^view in views)
				{
					view->coreView()->setCurrentManipulator(value);
				}
			}
		}

		property float selectionX
		{
			float get()
			{
				return manipulated->selectionX(); 
			}
			void set(float value)
			{
				this->manipulationStartedInView(nullptr);
				manipulated->setSelectionX(value);
				this->manipulationEndedInView(nullptr);
			}
		}

		property float selectionY
		{
			float get()
			{
				return manipulated->selectionY(); 
			}
			void set(float value)
			{
				this->manipulationStartedInView(nullptr);
				manipulated->setSelectionY(value);
				this->manipulationEndedInView(nullptr);
			}
		}

		property float selectionZ
		{
			float get()
			{
				return manipulated->selectionZ(); 
			}
			void set(float value)
			{
				this->manipulationStartedInView(nullptr);
				manipulated->setSelectionZ(value);
				this->manipulationEndedInView(nullptr);
			}
		}

		void setViews(OpenGLSceneView ^left, OpenGLSceneView ^top, OpenGLSceneView ^front, OpenGLSceneView ^perspective);
		void addItem(MeshType meshType, uint steps);
		void changeEditMode();
		void setNeedsDisplayExceptView(OpenGLSceneView ^except);
		void setNeedsDisplayOnAllViews();

		virtual void manipulationStartedInView(OpenGLSceneView ^view);
		virtual void manipulationEndedInView(OpenGLSceneView ^view);
		virtual void selectionChangedInView(OpenGLSceneView ^view);

		virtual void willChangeSelection();
		virtual void didChangeSelection();

		void undo();
		void redo();
		void cut();
		void copy();
		void paste();
		void duplicateSelected();
		void deleteSelected();
		void selectAll();
		void invertSelection();
		void hideSelected();
		void unhideAll();
		void mergeSelected();
		void splitSelected();
		void flipSelected();
		void subdivision();
		void detachSelected();
		void extrudeSelected();
		void triangulateSelected();
	};
}

#endif