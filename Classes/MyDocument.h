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
	ScriptDelegate,
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
    IBOutlet ScriptWindowController *scriptWindowController;
    IBOutlet FPSelectionWindowController *selectionWindowController;
    IBOutlet VertexWindowController *vertexWindowController;
    
    IBOutlet NSPopUpButton *scriptPullDown;
}

@property (readwrite, assign) IOpenGLManipulating *manipulated;
@property (readonly) NSPopUpButton *scriptPullDown;

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
- (IBAction)cleanTexture:(id)sender;
- (IBAction)resetTexCoords:(id)sender;
- (IBAction)triangulate:(id)sender;
- (IBAction)viewTexturePaintTool:(id)sender;
- (IBAction)viewTextureBrowser:(id)sender;
- (IBAction)viewScriptEditor:(id)sender;
- (IBAction)viewSelectionTool:(id)sender;
- (IBAction)viewVertexTool:(id)sender;

@end

@interface MyDocument (Archiving)

@end

#elif defined(WIN32)

#include "ItemCollection.h"
#include "OpenGLSceneView.h"
#include "TextureCollection.h"
#include "JSWrappers.h"
#include "../MeshMakerCppCLI/MarshalHelpers.h"

namespace MeshMakerCppCLI
{
	public interface class IDocumentDelegate
	{
		property ToolStripComboBox ^editModePopup { ToolStripComboBox ^ get(); }
		void updateSelectionValues();
	};
	
	public ref class UndoStatePointer
	{
	public:
		 IUndoState *_undoState;

		 UndoStatePointer(IUndoState *undoState)
		 {
			 _undoState = undoState;
		 }

		 ~UndoStatePointer()
		 {
			 if (_undoState)
				 delete _undoState;
		 }
	};

	delegate void SwapOldCurrent(UndoStatePointer ^old, UndoStatePointer ^current);
	delegate void SwapOldCurrentNamed(UndoStatePointer ^old, UndoStatePointer ^current, String ^actionName);
	delegate void AddRemoveItem(MeshType meshType, uint steps);

	public ref class MyDocument : public IOpenGLSceneViewDelegate, public IOpenGLManipulatingControllerKVC
	{
	private:
		IDocumentDelegate ^documentDelegate;

		ItemCollection *items;
		TextureCollection *textures;
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

		UndoManager ^undoManager;

		UndoStatePointer ^oldManipulations;
		UndoStatePointer ^oldMeshState;

		Action<ItemCollectionWrapper ^> ^scriptAction;
	private:
        void setManipulated(IOpenGLManipulating *value);
		Mesh2 *currentMesh();
		void editItems();
		void editMesh(MeshSelectionMode mode);

		void swapMeshStateAction(UndoStatePointer ^old, UndoStatePointer ^current, String ^actionName);
		void swapAllItemsAction(UndoStatePointer ^old, UndoStatePointer ^current, String ^actionName);
		void swapManipulationsAction(UndoStatePointer ^old, UndoStatePointer ^current);

		void allItemsAction(String ^actionName, Action ^action);
		void meshAction(String ^actionName, Action ^action);
		void meshOnlyAction(String ^actionName, Action ^action);
		void runScriptActionCore();
	public:
		MyDocument(IDocumentDelegate ^documentForm);
		~MyDocument();

		property UndoManager ^DocumentUndoManager
		{
			UndoManager ^get() { return undoManager; }
		}

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

		property ViewMode viewMode
		{
			ViewMode get()
			{
			    return manipulated->viewMode();
			}
			void set(ViewMode value)
			{
				manipulated->setViewMode(value);
				this->setNeedsDisplayOnAllViews();
			}
		}

		property Color color
		{
			Color get()
			{
				Vector4D color = manipulated->selectionColor() * 255.0f;
				return Color::FromArgb(color.w, color.x, color.y, color.z);
			}
			void set(Color value)
			{
				Vector4D color = Vector4D(value.R, value.G, value.B, value.A);
				color /= 255.0f;
				manipulated->setSelectionColor(color);
			}
		}

		void setViews(OpenGLSceneView ^left, OpenGLSceneView ^top, OpenGLSceneView ^front, OpenGLSceneView ^perspective);
		void addItem(MeshType meshType, uint steps);
		void removeItem(MeshType meshType, uint steps);
		void changeEditMode();		
		void setNeedsDisplayExceptView(OpenGLSceneView ^except);
		void setNeedsDisplayOnAllViews();

		virtual void manipulationStartedInView(OpenGLSceneView ^view);
		virtual void manipulationEndedInView(OpenGLSceneView ^view);
		virtual void selectionChangedInView(OpenGLSceneView ^view);

		virtual void willChangeSelection();
		virtual void didChangeSelection();	

	private:
		void duplicateSelectedCore();
		void removeSelectedCore(); 
		void mergeSelectedCore();
		void splitSelectedCore();
		void flipSelectedCore();
		void subdivisionCore();
		void detachSelectedCore();
		void extrudeSelectedCore();
		void triangulateSelectedCore();
	public:
		void undo();
		void redo();
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

		void readModel3D(MemoryStream ^memoryStream);
		void writeModel3D(MemoryStream ^memoryStream);
		void readWavefrontObject(String ^asciiString);
		String ^writeWavefrontObject();

		uint textureCount();
		void addTexture(String ^fileName);
		void removeTextureAtIndex(uint index);
		String ^textureNameAtIndex(uint index);
		Bitmap ^textureImageAtIndex(uint index);
		void setTextureAtIndex(uint index);

		void runScriptAction(Action<ItemCollectionWrapper ^> ^scriptAction);
	};
}

#elif defined(__linux__)

#include "ItemCollection.h"
#include "OpenGLSceneView.h"

class MyDocument
{
private:
    ItemCollection *items;
    OpenGLManipulatingController *itemsController;
    OpenGLManipulatingController *meshController;
    IOpenGLManipulating *manipulated;

    bool manipulationFinished;

    vector<OpenGLSceneView *> views;
    OpenGLSceneView *viewLeft;
    OpenGLSceneView *viewTop;
    OpenGLSceneView *viewFront;
    OpenGLSceneView *viewPerspective;

    ManipulatorType _currentManipulator;
public:
    MyDocument();
    ~MyDocument();

    void setManipulated(IOpenGLManipulating *value);
    Mesh2 *currentMesh();
    void editItems();
    void editMesh(MeshSelectionMode mode);    

    ManipulatorType currentManipulator();
    void setCurrentManipulator(ManipulatorType value);

    void allItemsAction(string actionName, function<void ()> action);
    void meshAction(string actionName, function<void ()> action);
    void meshOnlyAction(string actionName, function<void ()> action);

    virtual void manipulationStartedInView(OpenGLSceneView *view);
    virtual void manipulationEndedInView(OpenGLSceneView *view);
    virtual void selectionChangedInView(OpenGLSceneView *view);

    virtual void willChangeSelection();
    virtual void didChangeSelection();

    void setViews(OpenGLSceneView *left,
                  OpenGLSceneView *top,
                  OpenGLSceneView *front,
                  OpenGLSceneView *perspective);

    void setNeedsDisplayExceptView(OpenGLSceneView *except);
    void setNeedsDisplayOnAllViews();

    void undo();
    void redo();
    void addItem(MeshType meshType, uint steps);
    void removeItem(MeshType meshType, uint steps);
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

#endif
