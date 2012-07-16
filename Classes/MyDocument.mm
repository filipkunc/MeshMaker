//
//  MyDocument.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#import "MyDocument.h"
#import "ItemManipulationState.h"
#import "IndexedItem.h"

#import <string>
#import <sstream>
#import "rapidxml.hpp"

using namespace std;
using namespace rapidxml;

template <typename T>
vector<T> *ReadValues(string s)
{
    vector<T> *values = new vector<T>();
    
    istringstream ss(s);
    
    T value;
    
    while (ss >> value)
    {
        values->push_back(value);
        
        NSLog(@"%.2f", (float)value);
    }
        
    NSLog(@"finish read");
    
    return values;
}

namespace TMD
{

#define MODEL_VERSION "TMD v.3.1"

struct model_header
{
    char version[10];
	int no_meshes;
	int no_materials;
};

struct anim_header
{
	int no_anims;
	float frame_speed;
	float ticks_per_frame;
	float end_time;
};

struct face
{
    unsigned int vertID[3];
	unsigned int texID[3];
	Vector3D face_normal;
};

struct material
{
    Vector4D color;
	char texture_file[255];	
};

struct mesh_desc
{
    int materialID;	
	int no_faces;
    int no_vertices;
	int no_texcoords;
	int start_animID;
	int end_animID;
};

struct mesh
{
	face * faces;
	Vector3D * vertices;
	Vector3D * normals;
	Vector2D * tex_coords;
};

struct anim_desc
{
    int no_vertices;
	float time_value;
};

// snimek animace
struct anim
{
    Vector3D * vertices;
	Vector3D * normals;
};
    
};

@implementation MyDocument

- (id)init
{
    self = [super init];
    if (self) 
	{
        // Add your subclass-specific initialization here.
        // If an error occurs here, send a [self release] message and return nil.
		items = [[ItemCollection alloc] init];
		itemsController = [[OpenGLManipulatingController alloc] init];
		meshController = [[OpenGLManipulatingController alloc] init];
		
		[itemsController setModel:items];
		manipulated = itemsController;

		[itemsController addSelectionObserver:self];
		[meshController addSelectionObserver:self];
		
		manipulationFinished = YES;
		oldManipulations = nil;
		oldMeshState = nil;
		
		views = [[NSMutableArray alloc] init];
		oneView = nil;
        
        currentManipulator = ManipulatorTypeDefault;
    }
    return self;
}
						   
- (void)dealloc
{
	[itemsController removeSelectionObserver:self];
	[meshController removeSelectionObserver:self];
}

- (void)awakeFromNib
{
    //[NSColor setIgnoresAlpha:NO];
    
	[editModePopUp selectItemWithTag:0];
	
	[views addObject:viewTop];
	[views addObject:viewLeft];
	[views addObject:viewFront];
	[views addObject:viewPerspective];
	
	for (OpenGLSceneView *view in views)
	{ 
		[view setManipulated:manipulated]; 
		[view setDisplayed:itemsController];
		[view setDelegate:self];
	};
	
	[viewTop setCameraMode:CameraModeTop];
	[viewLeft setCameraMode:CameraModeLeft];
	[viewFront setCameraMode:CameraModeFront];
	[viewPerspective setCameraMode:CameraModePerspective];
}

- (enum ManipulatorType)currentManipulator
{
    return currentManipulator;
}

- (void)setCurrentManipulator:(enum ManipulatorType)value;
{
    currentManipulator = value;
    [itemsController setCurrentManipulator:value];
    [meshController setCurrentManipulator:value];
}

- (NSApplicationPresentationOptions)window:(NSWindow *)window willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
    return proposedOptions;    
}

- (void)setNeedsDisplayExceptView:(OpenGLSceneView *)view
{
	for (OpenGLSceneView *v in views)
	{ 
		if (v != view)
			[v setNeedsDisplay:YES]; 
	}
}

- (void)setNeedsDisplayOnAllViews
{
	for (OpenGLSceneView *v in views)
	{
		[v setNeedsDisplay:YES];
	}
}

- (id<OpenGLManipulating>)manipulated
{
	return manipulated;
}

- (void)setManipulated:(id<OpenGLManipulating>)value
{
	manipulated = value;
	
	for (OpenGLSceneView *view in views)
	{ 
		[view setManipulated:value];
		[view setNeedsDisplay:YES];
	};

	if (manipulated == itemsController)
	{
		[editModePopUp selectItemWithTag:EditModeItems];
	}
	else if (manipulated == meshController)
	{
		int meshTag = [self currentMesh]->selectionMode() + 1;
		[editModePopUp selectItemWithTag:meshTag];
	}
}

- (Mesh2 *)currentMesh
{
	if (manipulated == meshController)
		return [(Item *)[meshController model] mesh];
    if (manipulated == itemsController)
        return [(ItemCollection *)[itemsController model] currentMesh];
	return nil;
}

- (MyDocument *)prepareUndoWithName:(NSString *)actionName
{
	NSUndoManager *undo = [self undoManager];
	MyDocument *document = [undo prepareWithInvocationTarget:self];
	if (![undo isUndoing])
		[undo setActionName:actionName];
	return document;
}

- (void)addItemWithType:(enum MeshType)type steps:(uint)steps;
{
	Item *item = [[Item alloc] init];
	Mesh2 *mesh = [item mesh];
    mesh->make(type, steps);
	
	NSString *name = Mesh2::descriptionOfMeshType(type);
	
	MyDocument *document = [self prepareUndoWithName:[NSString stringWithFormat:@"Add %@", name]];
	[document removeItemWithType:type steps:steps];
	
	[items addItem:item];
    [textureBrowserWindowController setItems:items];	
	[itemsController changeSelection:NO];
	[items setSelected:YES atIndex:[items count] - 1];
	[itemsController updateSelection];
	[self setManipulated:itemsController];
}

- (void)removeItemWithType:(enum MeshType)type steps:(uint)steps
{
	NSString *name = Mesh2::descriptionOfMeshType(type);
	
	MyDocument *document = [self prepareUndoWithName:[NSString stringWithFormat:@"Remove %@", name]];
	[document addItemWithType:type steps:steps];
		
	[items removeLastItem];
    [textureBrowserWindowController setItems:items];
	[itemsController changeSelection:NO];
	[self setManipulated:itemsController];
}

- (float)selectionX
{
	return [manipulated selectionX];
}

- (float)selectionY
{
	return [manipulated selectionY];
}

- (float)selectionZ
{
	return [manipulated selectionZ];
}

- (void)setSelectionX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setSelectionX:value];
	[self manipulationEndedInView:nil];
}

- (void)setSelectionY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setSelectionY:value];
	[self manipulationEndedInView:nil];
}

- (void)setSelectionZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setSelectionZ:value];
	[self manipulationEndedInView:nil];
}

- (BOOL)selectionColorEnabled
{
    return manipulated.selectionColorEnabled;
}

- (NSColor *)selectionColor
{
    return manipulated.selectionColor;
}

- (void)setSelectionColor:(NSColor *)selectionColor
{
    [self meshOnlyActionWithName:@"Change Color" block:^ { manipulated.selectionColor = selectionColor; }];
}

- (void)setNilValueForKey:(NSString *)key
{
	[self setValue:[NSNumber numberWithFloat:0.0f] forKey:key];
}

- (void)observeValueForKeyPath:(NSString *)keyPath 
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
    if (manipulationFinished)
    {
        [self willChangeValueForKey:keyPath];
        [self didChangeValueForKey:keyPath];
    }
}

- (void)swapManipulationsWithOld:(NSMutableArray *)old current:(NSMutableArray *)current
{
	NSAssert([old count] == [current count], @"old count == current count");
	[items setCurrentManipulations:old];
	
	MyDocument *document = [self prepareUndoWithName:@"Manipulations"];	
	[document swapManipulationsWithOld:current current:old];
	
	[itemsController updateSelection];
	[self setManipulated:itemsController];
}

- (void)swapAllItemsWithOld:(NSMutableArray *)old
					current:(NSMutableArray *)current
				 actionName:(NSString *)actionName
{
	[items setAllItems:old];

	MyDocument *document = [self prepareUndoWithName:actionName];
	[document swapAllItemsWithOld:current
						  current:old
					   actionName:actionName];
	
	[itemsController updateSelection];
	[self setManipulated:itemsController];
}

- (void)swapMeshStateWithOld:(MeshState *)old 
					 current:(MeshState *)current 
				  actionName:(NSString *)actionName
{
	[items setCurrentMeshState:old];
	Item *item = [items itemAtIndex:[old itemIndex]];
	
    [meshController setModel:item];
    [meshController setPosition:[item position] 
                       rotation:[item rotation] 
                          scale:[item scale]];
    
	MyDocument *document = [self prepareUndoWithName:actionName];
	[document swapMeshStateWithOld:current
						   current:old
						actionName:actionName];
	
	[itemsController updateSelection];
	[meshController updateSelection];
    
	[self setManipulated:meshController];
}

- (void)allItemsActionWithName:(NSString *)actionName block:(void (^)())action
{
	MyDocument *document = [self prepareUndoWithName:actionName];
	NSMutableArray *oldItems = [items allItems];

	action();
    
    [textureBrowserWindowController setItems:items];
	
	NSMutableArray *currentItems = [items allItems];
	[document swapAllItemsWithOld:oldItems 
						  current:currentItems
					   actionName:actionName];	
}

- (void)meshActionWithName:(NSString *)actionName block:(void (^)())action
{
	MyDocument *document = [self prepareUndoWithName:actionName];
	MeshState *oldState = [items currentMeshState];
	
	action();
	
	MeshState *currentState = [items currentMeshState];
	[document swapMeshStateWithOld:oldState 
						   current:currentState
						actionName:actionName];
}

- (void)manipulationStartedInView:(OpenGLSceneView *)view
{
	manipulationFinished = NO;
	
	if (manipulated == itemsController)
	{
		oldManipulations = [items currentManipulations];
	}
	else if (manipulated == meshController)
	{
		oldMeshState = [items currentMeshState];
	}
}

- (void)manipulationEndedInView:(OpenGLSceneView *)view
{
	manipulationFinished = YES;
	
	if (manipulated == itemsController)
	{
		MyDocument *document = [self prepareUndoWithName:@"Manipulations"];
		[document swapManipulationsWithOld:oldManipulations current:[items currentManipulations]];
		oldManipulations = nil;
        
        [itemsController willChangeSelection];
        [itemsController didChangeSelection];
	}
	else if (manipulated == meshController)
	{
		MyDocument *document = [self prepareUndoWithName:@"Mesh Manipulation"];
		[document swapMeshStateWithOld:oldMeshState current:[items currentMeshState] actionName:@"Mesh Manipulation"];
		oldMeshState = nil;
        
        [meshController willChangeSelection];
        [meshController didChangeSelection];
	}
	
	[self setNeedsDisplayExceptView:view];
}

- (NSColor *)brushColor
{
    return [texturePaintToolWindowController brushColor];
}

- (float)brushSize
{
    return [texturePaintToolWindowController brushSize];
}

- (void)selectionChangedInView:(OpenGLSceneView *)view
{
	[self setNeedsDisplayExceptView:view];
}

- (IBAction)addPlane:(id)sender
{
    [self addItemWithType:MeshTypePlane steps:0];
}

- (IBAction)addCube:(id)sender
{
	[self addItemWithType:MeshTypeCube steps:0];
}

- (IBAction)addCylinder:(id)sender
{
	itemWithSteps = MeshTypeCylinder;
	[addItemWithStepsSheetController beginSheetWithProtocol:self];
}

- (IBAction)addSphere:(id)sender
{
	itemWithSteps = MeshTypeSphere;
	[addItemWithStepsSheetController beginSheetWithProtocol:self];
}

- (void)addIcosahedron:(id)sender
{
    [self addItemWithType:MeshTypeIcosahedron steps:0];
}

- (void)addItemWithSteps:(uint)steps
{
	[self addItemWithType:itemWithSteps steps:steps];
}

- (void)editMeshWithMode:(enum MeshSelectionMode)mode
{
	NSInteger index = [itemsController lastSelectedIndex];
	if (index > -1)
	{
		Item *item = [items itemAtIndex:index];
        [item mesh]->setSelectionMode(mode);
		[meshController setModel:item];
		[meshController setPosition:[item position] 
						   rotation:[item rotation] 
							  scale:[item scale]];
		[self setManipulated:meshController];
	}
}

- (void)editItems
{
    Mesh2 *currentMesh = [self currentMesh];
    if (currentMesh)
        currentMesh->setSelectionMode(MeshSelectionModeVertices);
	[itemsController setModel:items];
	[itemsController setPosition:Vector3D()
						rotation:Quaternion()
						   scale:Vector3D(1, 1, 1)];
	[self setManipulated:itemsController];
}

- (IBAction)changeEditMode:(id)sender
{
	EditMode mode = (EditMode)[[editModePopUp selectedItem] tag];
    Mesh2 *currentMesh = [self currentMesh];
    
    if (!currentMesh)
        [editModePopUp selectItemWithTag:EditModeItems];
    
	switch (mode)
	{
		case EditModeItems:
			[self editItems];
			break;
		case EditModeVertices:
			[self editMeshWithMode:MeshSelectionModeVertices];
			break;
		case EditModeTriangles:
			[self editMeshWithMode:MeshSelectionModeTriangles];
			break;
		case EditModeEdges:
			[self editMeshWithMode:MeshSelectionModeEdges];
			break;
	}
}

- (IBAction)changeViewMode:(id)sender
{
	ViewMode mode = (ViewMode)[sender tag];
    
    [manipulated setViewMode:mode];
    
    [self setNeedsDisplayOnAllViews];
}

- (IBAction)mergeSelected:(id)sender
{
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == itemsController)
	{
		[self allItemsActionWithName:@"Merge" block:^ { [items mergeSelectedItems]; }];
	}
	else if (manipulated == meshController)
	{
		[self meshActionWithName:@"Merge" block:^ { [self currentMesh]->mergeSelected(); }]; 
	}
	
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)unionSelected:(id)sender
{
    if ([manipulated selectedCount] <= 0)
		return;
    
    if (manipulated == itemsController)
	{
		[self allItemsActionWithName:@"CSG Union" block:^ { [items csgOperationOnSelectedItems:CsgUnion]; }];
	}
    
    [manipulated updateSelection];
    [self setNeedsDisplayOnAllViews];
}

- (IBAction)subtractSelected:(id)sender
{
    if ([manipulated selectedCount] <= 0)
		return;
    
    if (manipulated == itemsController)
	{
		[self allItemsActionWithName:@"CSG Subtract" block:^ { [items csgOperationOnSelectedItems:CsgSubtract]; }];
	}
    
    [manipulated updateSelection];
    [self setNeedsDisplayOnAllViews];
}

- (IBAction)intersectSelected:(id)sender
{
    if ([manipulated selectedCount] <= 0)
		return;
    
    if (manipulated == itemsController)
	{
		[self allItemsActionWithName:@"CSG Intersect" block:^ { [items csgOperationOnSelectedItems:CsgIntersect]; }];
	}
    
    [manipulated updateSelection];
    [self setNeedsDisplayOnAllViews];
}

- (void)meshOnlyActionWithName:(NSString *)actionName block:(void (^)())action
{    
    if ([self currentMesh] == nil)
        return;
	
	BOOL startManipulation = NO;
	if (!manipulationFinished)
	{
		startManipulation = YES;
		[self manipulationEndedInView:nil];
	}
	
    [self meshActionWithName:actionName block:action];
	
    [manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
	
	if (startManipulation)
	{
		[self manipulationStartedInView:nil];
	}
}

- (IBAction)splitSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Split" block:^ { [self currentMesh]->splitSelected(); }];
}

- (IBAction)flipSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Flip" block:^ { [self currentMesh]->flipSelected(); }];
}

- (IBAction)duplicateSelected:(id)sender
{	
	if ([manipulated selectedCount] <= 0)
		return;
	
	BOOL startManipulation = NO;
	if (!manipulationFinished)
	{
		startManipulation = YES;
		[self manipulationEndedInView:nil];
	}
	
	if (manipulated == itemsController)
	{
		NSMutableArray *selection = [items currentSelection];
		MyDocument *document = [self prepareUndoWithName:@"Duplicate"];
		[document undoDuplicateSelected:selection];
		[manipulated duplicateSelected];
	}
	else if (manipulated == meshController)
	{
		[self meshActionWithName:@"Duplicate" block:^ { [manipulated duplicateSelected]; }];
	}
	
    [manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
	
	if (startManipulation)
	{
		[self manipulationStartedInView:nil];
	}
}

- (void)redoDuplicateSelected:(NSMutableArray *)selection
{
	[self setManipulated:itemsController];
	[items setCurrentSelection:selection];
	[manipulated duplicateSelected];
	
	MyDocument *document = [self prepareUndoWithName:@"Duplicate"];
	[document undoDuplicateSelected:selection];
	
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (void)undoDuplicateSelected:(NSMutableArray *)selection
{	
	[self setManipulated:itemsController];
	uint duplicatedCount = [selection count];
	[items removeItemsInRange:NSMakeRange([items count] - duplicatedCount, duplicatedCount)];
	[items setCurrentSelection:selection];

	MyDocument *document = [self prepareUndoWithName:@"Duplicate"];
	[document redoDuplicateSelected:selection];
		
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)deleteSelected:(id)sender
{
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == itemsController)
	{
		NSMutableArray *currentItems = [items currentItems];
		MyDocument *document = [self prepareUndoWithName:@"Delete"];
		[document undoDeleteSelected:currentItems];
		[manipulated removeSelected];
        [textureBrowserWindowController setItems:items];
	}
	else if (manipulated == meshController)
	{
		[self meshActionWithName:@"Delete" block:^ { [manipulated removeSelected]; }];
	}
	
	[self setNeedsDisplayOnAllViews];
}

- (void)redoDeleteSelected:(NSMutableArray *)selectedItems
{
	[self setManipulated:itemsController];
	[items setSelectionFromIndexedItems:selectedItems];
	[manipulated removeSelected];
    [textureBrowserWindowController setItems:items];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document undoDeleteSelected:selectedItems];

	[itemsController updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (void)undoDeleteSelected:(NSMutableArray *)selectedItems
{
	[self setManipulated:itemsController];
	[items setCurrentItems:selectedItems];
    [textureBrowserWindowController setItems:items];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document redoDeleteSelected:selectedItems];
	
	[itemsController updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)subdivision:(id)sender
{
    [self meshOnlyActionWithName:@"Subdivision" block:^ { [self currentMesh]->loopSubdivision(); }];
}

+ (BOOL)softSelection
{
    return Mesh2::useSoftSelection();
}

+ (void)setSoftSelection:(BOOL)value
{
    Mesh2::setUseSoftSelection(value);
}

- (IBAction)softSelection:(id)sender
{
    NSMenuItem *menuItem = (NSMenuItem *)sender;
    [MyDocument setSoftSelection:![MyDocument softSelection]];
    [menuItem setState:[MyDocument softSelection]];
}

- (IBAction)changeManipulator:(id)sender
{
	self.currentManipulator = (ManipulatorType)[sender tag];
	for (OpenGLSceneView *view in views)
	{ 
		[view setCurrentManipulator:currentManipulator]; 
	}
}

- (IBAction)selectAll:(id)sender
{
	[[self manipulated] changeSelection:YES];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)invertSelection:(id)sender
{
	[[self manipulated] invertSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)hideSelected:(id)sender
{
	[[self manipulated] hideSelected];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)unhideAll:(id)sender
{
	[[self manipulated] unhideAll];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)detachSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Detach" block:^ { [self currentMesh]->detachSelected(); }];
}

- (IBAction)extrudeSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Extrude" block:^ { [self currentMesh]->extrudeSelectedTriangles(); }];
}

- (IBAction)cleanTexture:(id)sender
{
    Mesh2 *mesh = [self currentMesh];
    if (mesh)
    {
        mesh->cleanTexture();
        [self setNeedsDisplayOnAllViews];
    }
}

- (IBAction)resetTexCoords:(id)sender
{
    [self meshOnlyActionWithName:@"Reset Texture Coordinates" block:^ 
    { 
        Mesh2 *mesh = [self currentMesh];
        mesh->resetTriangleCache();
        mesh->makeTexCoords();
        mesh->makeEdges();
    }];
}

- (IBAction)viewTexturePaintTool:(id)sender
{
    [texturePaintToolWindowController showWindow:nil];
}

- (IBAction)viewTextureBrowser:(id)sender
{
    [textureBrowserWindowController setItems:items];
    [textureBrowserWindowController showWindow:nil];
}

- (BOOL)texturePaintEnabled
{
    if (texturePaintToolWindowController.isWindowLoaded)
    {
        if (texturePaintToolWindowController.window.isVisible)
            return YES;
    }
    return NO;
}

- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"MyDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
    [super windowControllerDidLoadNib:aController];
    // Add any code here that needs to be executed once the windowController has loaded the document's window.
}

#pragma mark Archivation

+ (BOOL)autosavesInPlace
{
    return YES;
}

- (void)windowWillEnterVersionBrowser:(NSNotification *)notification
{
    NSWindow *window = (NSWindow *)[notification object];
    [window.toolbar setVisible:NO];
}

- (void)windowDidExitVersionBrowser:(NSNotification *)notification
{
    NSWindow *window = (NSWindow *)[notification object];
    [window.toolbar setVisible:YES];
}

- (BOOL)readFromFileWrapper:(NSFileWrapper *)dirWrapper ofType:(NSString *)typeName error:(NSError *__autoreleasing *)outError
{
    if ([typeName isEqualToString:@"model3D"])
        return [self readFromModel3D:[dirWrapper regularFileContents]];
    
    if ([typeName isEqualToString:@"Collada"])
        return [self readFromCollada:[dirWrapper regularFileContents]];
    
    if ([typeName isEqualToString:@"TMD"])
        return [self readFromTMD:[dirWrapper regularFileContents]];
    
    NSFileWrapper *wrapper;
    NSData *data;
    
    wrapper = [[dirWrapper fileWrappers] objectForKey:@"Geometry.model3D"];
    data = [wrapper regularFileContents];
    [self readFromModel3D:data];
    
    [[dirWrapper fileWrappers] enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) 
    {
        NSString *textureName = (NSString *)key;
        if ([textureName hasSuffix:@".png"])
        {
            textureName = [textureName stringByDeletingPathExtension];
            
            NSFileWrapper *wrapper = (NSFileWrapper *)obj;
            NSData *data = [wrapper regularFileContents];
            
            NSImage *image = [[NSImage alloc] initWithData:data];
            int index = [textureName substringFromIndex:@"Texture".length].integerValue;
            Item *item = [items itemAtIndex:index];
            FPTexture *texture = item.mesh->texture();
            [texture setCanvas:image];
        }        
    }];
    
    return YES;
}

- (NSFileWrapper *)fileWrapperOfType:(NSString *)typeName error:(NSError *__autoreleasing *)outError
{
    if ([typeName isEqualToString:@"model3D"])
        return [[NSFileWrapper alloc] initRegularFileWithContents:[self dataOfModel3D]];
    
    if ([typeName isEqualToString:@"Collada"])
        return [[NSFileWrapper alloc] initRegularFileWithContents:[self dataOfCollada]];
    
    NSFileWrapper *dirWrapper = [[NSFileWrapper alloc] initDirectoryWithFileWrappers:nil];
    
    [dirWrapper addRegularFileWithContents:[self dataOfModel3D]
                         preferredFilename:@"Geometry.model3D"];
    
    int i = 0;
    
    for (Item *item in items)
    {
        NSImage *image = item.mesh->texture().canvas;
        NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithData:[image TIFFRepresentation]];
        
        NSData *imageData = [bitmap representationUsingType:NSPNGFileType properties:nil];

        
        [dirWrapper addRegularFileWithContents:imageData 
                             preferredFilename:[NSString stringWithFormat:@"Texture%.2i.png", i]];
        
        i++;
    }
    
    return dirWrapper;
}

- (BOOL)readFromTMD:(NSData *)data
{
    MemoryReadStream *stream = [[MemoryReadStream alloc] initWithData:data];

    TMD::model_header desc;
    
    [stream readBytes:&desc length:sizeof(TMD::model_header)];
	if (strncmp(desc.version, MODEL_VERSION, sizeof(MODEL_VERSION)) != 0)
	{
		desc.no_materials = 0;
		desc.no_meshes = 0;
        return NO;
	}
    
    TMD::anim_header a_desc;
    [stream readBytes:&a_desc length:sizeof(TMD::anim_header)];
    TMD::material *materials = new TMD::material[desc.no_materials];
	int *textureIDs = new int[desc.no_materials];
    TMD::mesh_desc *m_descs = new TMD::mesh_desc[desc.no_meshes];
    TMD::mesh *meshes = new TMD::mesh[desc.no_meshes];
    
	int i, j;
	for (i = 0; i < desc.no_materials; i++)
	{
        [stream readBytes:&materials[i] length:sizeof(TMD::material)];
		textureIDs[i] = 0;
	}
	for (i = 0; i < desc.no_meshes; i++)
	{
        [stream readBytes:&m_descs[i] length:sizeof(TMD::mesh_desc)];
		meshes[i].faces = new TMD::face[m_descs[i].no_faces];
	    for (j = 0; j < m_descs[i].no_faces; j++)
	    {
            [stream readBytes:&meshes[i].faces[j] length:sizeof(TMD::face)];
	    }
		meshes[i].vertices = new Vector3D[m_descs[i].no_vertices];
		meshes[i].normals = new Vector3D[m_descs[i].no_vertices];
		meshes[i].tex_coords = new Vector2D[m_descs[i].no_texcoords];
	    for (j = 0; j < m_descs[i].no_vertices; j++)
	    {
            [stream readBytes:&meshes[i].vertices[j] length:sizeof(Vector3D)];
            [stream readBytes:&meshes[i].normals[j] length:sizeof(Vector3D)];
	    }
		for (j = 0; j < m_descs[i].no_texcoords; j++)
	    {
            [stream readBytes:&meshes[i].tex_coords[j] length:sizeof(Vector2D)];
        }
    }
    
    ItemCollection *newItems = [[ItemCollection alloc] init];
    
    for (i = 0; i < desc.no_meshes; i++)
    {
        Item *item = [[Item alloc] init];
        Mesh2 *itemMesh = [item mesh];
        
        vector<Vector3D> vertices;
        vector<Vector3D> texCoords;
        vector<Triangle> triangles;
        
        for (j = 0; j < m_descs[i].no_faces; j++)
        {
            Triangle triangle;
            for (int k = 0; k < 3; k++)
            {
                triangle.vertexIndices[k] = meshes[i].faces[j].vertID[k];
                triangle.texCoordIndices[k] = meshes[i].faces[j].texID[k];
            }
            FlipTriangle(triangle);
            triangles.push_back(triangle);
        }
        
        for (j = 0; j < m_descs[i].no_vertices; j++)
	    {
            vertices.push_back(meshes[i].vertices[j]);
        }
        
        for (j = 0; j < m_descs[i].no_texcoords; j++)
	    {
            Vector3D texCoord;
            texCoord.x = meshes[i].tex_coords[j].x;
            texCoord.y = meshes[i].tex_coords[j].y;
            texCoord.z = 0.0f;
            texCoords.push_back(texCoord);
        }
        
        itemMesh->fromIndexRepresentation(vertices, texCoords, triangles);
        
        [newItems addItem:item];
    }
    
    items = newItems;
    [itemsController setModel:items];
    [itemsController updateSelection];
    [self setManipulated:itemsController];
    
    delete [] materials;
    delete [] textureIDs;
    delete [] m_descs;
    for (i = 0; i < desc.no_meshes; i++)
    {
        delete [] meshes[i].faces;
        delete [] meshes[i].vertices;
        delete [] meshes[i].normals;
        delete [] meshes[i].tex_coords;
    }
    delete [] meshes;
	
    return YES;
}

- (BOOL)readFromModel3D:(NSData *)data
{
    MemoryReadStream *stream = [[MemoryReadStream alloc] initWithData:data];
    
    unsigned int version = 0;
    
    [stream readBytes:&version length:sizeof(unsigned int)];
    
    if (version < 1 || version > 2)
        return NO;
    
    [stream setVersion:version];
    ItemCollection *newItems = [[ItemCollection alloc] initWithReadStream:stream];
    items = newItems;
    [itemsController setModel:items];
    [itemsController updateSelection];
    [self setManipulated:itemsController];

    return YES;
}

- (NSData *)dataOfModel3D
{
    NSMutableData *data = [[NSMutableData alloc] init];
    MemoryWriteStream *stream = [[MemoryWriteStream alloc] initWithData:data];
    
    unsigned int version = 2;
    [stream setVersion:version];
    [stream writeBytes:&version length:sizeof(unsigned int)];
    [items encodeWithWriteStream:stream];
    return data;
}

- (void)readMesh:(Mesh2 *)itemMesh fromXml:(xml_node< > *)meshXml
{
    string positionsString = meshXml->first_node("source")->first_node("float_array")->value();
    vector<float> *points = ReadValues<float>(positionsString);
    
    string uvCoordsString = meshXml->first_node("source")->next_sibling()->next_sibling()->first_node("float_array")->value();
    vector<float> *uvCoords = ReadValues<float>(uvCoordsString);
    
    xml_node< > *triNode = meshXml->first_node("source")->next_sibling("triangles")->first_node();
    
    int inputTypesCount = 0;
    string trianglesString;
    
    while (true)
    {
        if (strcmp(triNode->name(), "p") == 0)
        {
            trianglesString = triNode->value();
            break;
        }
        else
        {
            triNode = triNode->next_sibling();
            inputTypesCount++;
        }
    }
    
    vector<uint> *indices = ReadValues<uint>(trianglesString);
    
    vector<Vector3D> vertices;
    vector<Vector3D> texCoords;
    vector<Triangle> triangles;
    
    uint pointsSize = points->size();
    
    for (uint i = 0; i < pointsSize; i += 3)
    {
        Vector3D point;
        for (uint j = 0; j < 3; j++)
            point[j] = points->at(i + j);
        
        vertices.push_back(point);
    }
    
    for (uint i = 0; i < uvCoords->size(); i += 2)
    {
        Vector3D uvCoord;
        for (uint j = 0; j < 2; j++)
            uvCoord[j] = (*uvCoords)[i + j];
        
        texCoords.push_back(uvCoord);
    }
    
    vector<uint> &trianglesRef = *indices;
    
    NSLog(@"trianglesRef.size(): %lu", trianglesRef.size());
    
    for (uint i = 0; i < trianglesRef.size(); i += inputTypesCount * 3)
    {
        uint vertexIndices[3];
        uint texCoordIndices[3];
        
        for (uint j = 0; j < 3; j++)
        {
            vertexIndices[j] = trianglesRef.at(i + j * inputTypesCount);
            texCoordIndices[j] = trianglesRef.at(i + j * inputTypesCount + inputTypesCount - 1);
        }
        
        AddTriangle(triangles, vertexIndices, texCoordIndices);
    }
    
    itemMesh->fromIndexRepresentation(vertices, texCoords, triangles);
    
    delete points;
    delete uvCoords;
    delete indices;
}

- (BOOL)readFromCollada:(NSData *)data
{
    NSString* xmlData = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    
    int length = [xmlData lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
    char *textBuffer = new char [length + 1];
    memset(textBuffer, 0, length + 1);
    memcpy(textBuffer, [xmlData UTF8String], length);
    
    xml_document< > document;
    document.parse<0>(textBuffer);
    
    xml_node< > *geometries = document.first_node()->first_node("library_geometries");
    xml_node< > *visualScenes = document.first_node()->first_node("library_visual_scenes")->first_node("visual_scene");
    
    ItemCollection *newItems = [[ItemCollection alloc] init];
    
    for (xml_node< > *node = visualScenes->first_node("node"); node; node = node->next_sibling())
    {
        xml_node< > *instanceGeometry = node->first_node("instance_geometry");
        if (instanceGeometry != NULL)
        {
            Item *item = [[Item alloc] init];
            
            xml_attribute< > *url = instanceGeometry->first_attribute("url");
            char *urlValue = url->value();
            urlValue++; // Skipping '#'
            
            for (xml_node< > *geometry = geometries->first_node("geometry"); geometry; geometry = geometry->next_sibling())
            {
                if (strcmp(urlValue, geometry->first_attribute("id")->value()) == 0)
                {
                    [self readMesh:item.mesh fromXml:geometry->first_node("mesh")];
                    break;
                }
            }
            
            xml_node< > *translate = node->first_node("translate");
            if (translate != NULL)
            {
                float x, y, z;
                sscanf(translate->value(), "%f %f %f", &x, &y, &z);
                item.position = Vector3D(x, y, z);
            }
            
            [newItems addItem:item];
        }
    }
    
    items = newItems;
    [itemsController setModel:items];
    [itemsController updateSelection];
    [self setManipulated:itemsController];
    
    delete [] textBuffer;
    
    return YES;
}

- (NSData *)dataOfCollada
{
    NSMutableString *colladaXml = [[NSMutableString alloc] init];
    
    [colladaXml appendString:@"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"];
    [colladaXml appendString:@"<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n"];
    {
        [colladaXml appendString:@"<asset>\n"];
        {
            [colladaXml appendString:@"<contributor>\n"];
            {
                NSString *version =[[[NSBundle mainBundle] infoDictionary] valueForKey:@"CFBundleVersion"];
                [colladaXml appendFormat:@"<authoring_tool>MeshMaker %@</authoring_tool>\n", version];
            }
            [colladaXml appendString:@"</contributor>\n"];
            [colladaXml appendString:@"<created>2011-01-23T15:41:29Z</created>\n"];     // TODO: fill real date-time
            [colladaXml appendString:@"<modified>2011-01-23T15:41:29Z</modified>\n"];   // TODO: fill real date-time
            [colladaXml appendString:@"<up_axis>Y_UP</up_axis>\n"];
        }
        [colladaXml appendString:@"</asset>\n"];
        
        [colladaXml appendString:@"<library_cameras>\n"];
        {
            [colladaXml appendString:@"<camera id=\"Camera-Camera\" name=\"Camera\">\n"];
            {
                [colladaXml appendString:@"<optics>\n"];
                {
                    [colladaXml appendString:@"<technique_common>\n"];
                    {
                        [colladaXml appendString:@"<perspective>\n"];
                        {
                            [colladaXml appendString:@"<xfov sid=\"HFOV\">39.5978</xfov>\n"];       // TODO: fill real HFOV
                            [colladaXml appendString:@"<yfov sid=\"YFOV\">26.9915</yfov>\n"];       // TODO: fill real YFOV
                            [colladaXml appendString:@"<znear sid=\"near_clip\">0.01</znear>\n"];   // TODO: fill real near_clip
                            [colladaXml appendString:@"<zfar sid=\"far_clip\">10000</zfar>\n"];     // TODO: fill real far_clip
                        }
                        [colladaXml appendString:@"</perspective>\n"];
                    }
                    [colladaXml appendString:@"</technique_common>\n"];
                }
            [colladaXml appendString:@"</optics>\n"];
            }
            [colladaXml appendString:@"</camera>\n"];
        }
        [colladaXml appendString:@"</library_cameras>\n"];
        
        [colladaXml appendString:@"<library_materials>\n"];
        {
            [colladaXml appendString:@"<material id=\"Material-Default\" name=\"Default\">\n"];
            {
                [colladaXml appendString:@"<instance_effect url=\"#Effect-Default\" />\n"];
            }
            [colladaXml appendString:@"</material>\n"];
        }
        [colladaXml appendString:@"</library_materials>\n"];
        
        [colladaXml appendString:@"<library_effects>\n"];
        {
            [colladaXml appendString:@"<effect id=\"Effect-Default\" name=\"Default\">\n"];
            {
                [colladaXml appendString:@"<profile_COMMON>\n"];
                {
                    [colladaXml appendString:@"<technique sid=\"common\">\n"];
                    {
                        [colladaXml appendString:@"<phong>\n"];
                        {
                            [colladaXml appendString:@"<diffuse>\n"];
                            {
                                [colladaXml appendString:@"<color sid=\"diffuse_effect_rgb\">0.8 0.8 0.8 1</color>\n"];
                            }
                            [colladaXml appendString:@"</diffuse>\n"];
                            
                            [colladaXml appendString:@"<specular>\n"];
                            {
                                [colladaXml appendString:@"<color sid=\"specular_effect_rgb\">0.2 0.2 0.2 1</color>\n"];
                            }
                            [colladaXml appendString:@"</specular>\n"];
                        }
                        [colladaXml appendString:@"</phong>\n"];
                    }
                    [colladaXml appendString:@"</technique>\n"];
                }
                [colladaXml appendString:@"</profile_COMMON>\n"];
            }
            [colladaXml appendString:@"</effect>\n"];
        }
        [colladaXml appendString:@"</library_effects>\n"];
        
        [colladaXml appendString:@"<library_geometries>\n"];
        {
            int itemID = 0;
            for (Item *item in items)
            {
                vector<Vector3D> vertices;
                vector<Vector3D> texCoords;
                vector<Vector3D> normals;
                vector<Triangle> triangles;
                
                item.mesh->toIndexRepresentation(vertices, texCoords, triangles);
                
                const FPList<TriangleNode, Triangle2> &trianglesRef = item.mesh->triangles();
                for (TriangleNode *node = trianglesRef.begin(), *end = trianglesRef.end(); node != end; node = node->next())
                    normals.push_back(node->data().vertexNormal);
                
                [colladaXml appendFormat:@"<geometry id=\"Geometry-Mesh_%i\" name=\"Mesh_%i\">\n", itemID, itemID];
                {
                    [colladaXml appendString:@"<mesh>\n"];
                    {
                        // positions
                        [colladaXml appendFormat:@"<source id=\"Geometry-Mesh_%i-positions\" name=\"positions\">\n", itemID];
                        {
                            [colladaXml appendFormat:@"<float_array id=\"Geometry-Mesh_%i-positions-array\" count=\"%li\">\n",
                                itemID, vertices.size() * 3];
                            {
                                for (uint i = 0; i < vertices.size(); i++)
                                    [colladaXml appendFormat:@"%f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z];                                    
                            }
                            [colladaXml appendString:@"</float_array>\n"];
                            
                            [colladaXml appendString:@"<technique_common>\n"];
                            { 
                                [colladaXml appendFormat:@"<accessor count=\"%li\" source=\"#Geometry-Mesh_%i-positions-array\" stride=\"3\">\n",
                                    vertices.size(), itemID];
                                {
                                    [colladaXml appendString:@"<param name=\"X\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"Y\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"Z\" type=\"float\" />\n"];
                                }
                                [colladaXml appendString:@"</accessor>\n"];
                            }
                            [colladaXml appendString:@"</technique_common>\n"];
                        }
                        [colladaXml appendString:@"</source>\n"];
                        
                        // normals
                        [colladaXml appendFormat:@"<source id=\"Geometry-Mesh_%i-normals\" name=\"normals\">\n", itemID];
                        {
                            [colladaXml appendFormat:@"<float_array id=\"Geometry-Mesh_%i-normals-array\" count=\"%li\">\n",
                             itemID, normals.size() * 3];
                            {
                                for (uint i = 0; i < normals.size(); i++)
                                    [colladaXml appendFormat:@"%f %f %f\n", normals[i].x, normals[i].y, normals[i].z]; 
                            }
                            [colladaXml appendString:@"</float_array>\n"];
                            
                            [colladaXml appendString:@"<technique_common>\n"];
                            { 
                                [colladaXml appendFormat:@"<accessor count=\"%li\" source=\"#Geometry-Mesh_%i-normals-array\" stride=\"3\">\n",
                                    normals.size(), itemID];
                                {
                                    [colladaXml appendString:@"<param name=\"X\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"Y\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"Z\" type=\"float\" />\n"];
                                }
                                [colladaXml appendString:@"</accessor>\n"];
                            }
                            [colladaXml appendString:@"</technique_common>\n"];
                        }
                        [colladaXml appendString:@"</source>\n"];
                        
                        // texture
                        [colladaXml appendFormat:@"<source id=\"Geometry-Mesh_%i-Texture\" name=\"Texture\">\n", itemID];
                        {
                            [colladaXml appendFormat:@"<float_array id=\"Geometry-Mesh_%i-Texture-array\" count=\"%li\">\n",
                             itemID, texCoords.size() * 2];
                            {
                                for (uint i = 0; i < texCoords.size(); i++)
                                    [colladaXml appendFormat:@"%f %f\n", texCoords[i].x, texCoords[i].y];
                            }
                            [colladaXml appendString:@"</float_array>\n"];
                            
                            [colladaXml appendString:@"<technique_common>\n"];
                            { 
                                [colladaXml appendFormat:@"<accessor count=\"%li\" source=\"#Geometry-Mesh_%i-Texture-array\" stride=\"2\">\n",
                                    texCoords.size(), itemID];
                                {
                                    [colladaXml appendString:@"<param name=\"S\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"T\" type=\"float\" />\n"];
                                }
                                [colladaXml appendString:@"</accessor>\n"];
                            }
                            [colladaXml appendString:@"</technique_common>\n"];
                        }
                        [colladaXml appendString:@"</source>\n"];
                        
                        [colladaXml appendFormat:@"<vertices id=\"Geometry-Mesh_%i-vertices\">\n", itemID];
                        {
                            [colladaXml appendFormat:@"<input semantic=\"POSITION\" source=\"#Geometry-Mesh_%i-positions\" />\n", itemID];
                        }
                        [colladaXml appendString:@"</vertices>\n"];
                        
                        [colladaXml appendFormat:@"<triangles count=\"%li\" material=\"Material-Default\">\n", triangles.size()];
                        {
                            [colladaXml appendFormat:@"<input semantic=\"VERTEX\" source=\"#Geometry-Mesh_%i-vertices\" offset=\"0\" />\n", itemID];
                            [colladaXml appendFormat:@"<input semantic=\"NORMAL\" source=\"#Geometry-Mesh_%i-normals\" offset=\"1\" />\n", itemID];
                            [colladaXml appendFormat:@"<input semantic=\"TEXCOORD\" source=\"#Geometry-Mesh_%i-Texture\" offset=\"2\" set=\"0\" />\n", itemID];
                            
                            [colladaXml appendString:@"<p>"];
                            {
                                uint normalIndex = 0U;
                                
                                for (uint i = 0; i < triangles.size(); i++)
                                {
                                    const Triangle &t = triangles[i];
                                    
                                    for (uint j = 0; j < 3; j++)
                                    {
                                        [colladaXml appendFormat:@"%i %i %i ", t.vertexIndices[j], normalIndex, t.texCoordIndices[j]];
                                    }
                                    normalIndex++;
                                }
                            }
                            [colladaXml appendString:@"</p>\n"];
                        }
                        [colladaXml appendString:@"</triangles>\n"];
                    }
                    [colladaXml appendString:@"</mesh>\n"];
                }
                [colladaXml appendString:@"</geometry>\n"];
                
                itemID++;
            }
        }
        [colladaXml appendString:@"</library_geometries>\n"];
        
        [colladaXml appendString:@"<library_lights>\n"];
        {
            [colladaXml appendString:@"<light id=\"Light-Render\" name=\"Render\">\n"];
            {
                [colladaXml appendString:@"<technique_common>\n"];
                {
                    [colladaXml appendString:@"<ambient>\n"];
                    {
                        [colladaXml appendString:@"<color sid=\"ambient_light_rgb\">0.05 0.05 0.05</color>\n"];
                    }
                    [colladaXml appendString:@"</ambient>\n"];
                }
                [colladaXml appendString:@"</technique_common>\n"];
            }
            [colladaXml appendString:@"</light>\n"];
            [colladaXml appendString:@"<light id=\"Light-Directional_Light\" name=\"Directional_Light\">\n"];
            {
                [colladaXml appendString:@"<technique_common>\n"];
                {
                    [colladaXml appendString:@"<directional>\n"];
                    {
                        [colladaXml appendString:@"<color sid=\"directional_light_rgb\">1 1 1</color>\n"];
                    }
                    [colladaXml appendString:@"</directional>\n"];
                }
                [colladaXml appendString:@"</technique_common>\n"];
            }
            [colladaXml appendString:@"</light>\n"];
        }
        [colladaXml appendString:@"</library_lights>\n"];
        
        [colladaXml appendString:@"<library_visual_scenes>\n"];
        {
            [colladaXml appendString:@"<visual_scene id=\"DefaultScene\">\n"];
            {
                [colladaXml appendString:@"<node id=\"RenderNode\" name=\"Render\" type=\"NODE\">\n"];
                {
                    [colladaXml appendString:@"<instance_light url=\"#Light-Render\" />\n"];
                }
                [colladaXml appendString:@"</node>\n"];
                
                int itemID = 0;
                for (Item *item in items)
                {
                    [colladaXml appendFormat:@"<node id=\"Geometry-MeshNode_%i\" name=\"Mesh_%i\" type=\"NODE\">\n", itemID, itemID];
                    {
                        // TODO: fill rotation and scale too
                        
                        [colladaXml appendFormat:@"<translate sid=\"Position_%i\">%f %f %f</translate>\n", 
                            itemID, item.position.x, item.position.y, item.position.z];
                        
                        /*[colladaXml appendFormat:@"<rotate sid=\"RotationY\">0 1 0 0</rotate>\n"];
                        [colladaXml appendFormat:@"<rotate sid=\"RotationX\">1 0 0 -5</rotate>\n"];
                        [colladaXml appendFormat:@"<rotate sid=\"RotationZ\">0 0 1 0</rotate>\n"];*/
                        [colladaXml appendFormat:@"<instance_geometry url=\"#Geometry-Mesh_%i\">\n", itemID];
                        {
                            [colladaXml appendString:@"<bind_material>\n"];
                            {
                                [colladaXml appendString:@"<technique_common>\n"];
                                {
                                   [colladaXml appendString:@"<instance_material symbol=\"Material-Default\" target=\"#Material-Default\" />\n"];
                                }
                                [colladaXml appendString:@"</technique_common>\n"];
                            }
                            [colladaXml appendString:@"</bind_material>\n"];
                        }
                        [colladaXml appendString:@"</instance_geometry>\n"];
                    }
                    [colladaXml appendString:@"</node>\n"];
                    itemID++;
                }
                
                [colladaXml appendString:@"<node id=\"Camera-CameraNode\" name=\"Camera\" type=\"NODE\">\n"];
                {
                    [colladaXml appendString:@"<translate sid=\"Position\">0 0.75 10</translate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"RotationY\">0 1 0 0</rotate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"RotationX\">1 0 0 -5</rotate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"RotationZ\">0 0 1 0</rotate>\n"];
                    [colladaXml appendString:@"<instance_camera url=\"#Camera-Camera\" />\n"];
                }
                [colladaXml appendString:@"</node>\n"];
                
                [colladaXml appendString:@"<node id=\"Light-Directional_LightNode\" name=\"Directional_Light\" type=\"NODE\">\n"];
                {
                    [colladaXml appendString:@"<translate sid=\"Position__2_\">-2 2 2</translate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"Rotation__2_Y\">0 1 0 -45</rotate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"Rotation__2_X\">1 0 0 -30</rotate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"Rotation__2_Z\">0 0 1 0</rotate>\n"];
                    [colladaXml appendString:@"<instance_light url=\"#Light-Directional_Light\" />\n"];
                }
                [colladaXml appendString:@"</node>\n"];
            }
            [colladaXml appendString:@"</visual_scene>\n"];
        }
        [colladaXml appendString:@"</library_visual_scenes>\n"];
        
        [colladaXml appendString:@"<scene>\n"];
        {
            [colladaXml appendString:@"<instance_visual_scene url=\"#DefaultScene\" />\n"];
        }
        [colladaXml appendString:@"</scene>\n"];
    }
    [colladaXml appendString:@"</COLLADA>\n"];

    return [colladaXml dataUsingEncoding:NSUTF8StringEncoding];
}

#pragma mark Splitter sync

- (CGFloat)splitView:(NSSplitView *)splitView 
constrainSplitPosition:(CGFloat)proposedPosition 
		 ofSubviewAt:(NSInteger)dividerIndex
{
	if (oneView)
		return 0.0f;
	
	return proposedPosition;
}

// fix for issue four-views works independently on Mac version
- (void)splitViewDidResizeSubviews:(NSNotification *)notification
{
	if (oneView)
		return;
	
	NSSplitView *splitView = (NSSplitView *)[notification object];
	if (splitView == mainSplit)
		return;
	
	NSView *topSubview0 = (NSView *)[[topSplit subviews] objectAtIndex:0];
	NSView *topSubview1 = (NSView *)[[topSplit subviews] objectAtIndex:1];
	
	NSView *bottomSubview0 = (NSView *)[[bottomSplit subviews] objectAtIndex:0];
	NSView *bottomSubview1 = (NSView *)[[bottomSplit subviews] objectAtIndex:1];
	
	// we are interested only in width change
	if (fabsf([bottomSubview0 frame].size.width - [topSubview0 frame].size.width) >= 1.0f)
	{
		if (splitView == topSplit)
		{
			[bottomSubview0 setFrame:[topSubview0 frame]];
			[bottomSubview1 setFrame:[topSubview1 frame]];
		}
		else
		{
			[topSubview0 setFrame:[bottomSubview0 frame]];
			[topSubview1 setFrame:[bottomSubview1 frame]];
		}
	}
}

- (void)collapseSplitView:(NSSplitView *)splitView
{
	[[[splitView subviews] objectAtIndex:0] setFrame:NSZeroRect];
	/*if ([splitView isVertical])
	{
		NSRect frame = [[[splitView subviews] objectAtIndex:1] frame];
		if (frame.size.width < 1.0f)
		{
			frame.size.width = 2.0f;
			[[[splitView subviews] objectAtIndex:1] setFrame:frame];
		}
	}
	else 
	{
		NSRect frame = [[[splitView subviews] objectAtIndex:1] frame];
		if (frame.size.height < 1.0f)
		{
			frame.size.height = 2.0f;
			[[[splitView subviews] objectAtIndex:1] setFrame:frame];
		}
	}*/
}

- (void)swapCamerasBetweenFirst:(OpenGLSceneView *)first second:(OpenGLSceneView *)second
{
	Camera firstCamera = [first camera];
	Camera secondCamera = [second camera];
	CameraMode firstMode = [first cameraMode];
	CameraMode secondMode = [second cameraMode];
	[second setCameraMode:firstMode];
	[first setCameraMode:secondMode];
	[second setCamera:firstCamera];
	[first setCamera:secondCamera];
}

- (void)toggleOneViewFourView:(id)sender
{
	if (oneView)
	{
		if (oneView != viewPerspective)
		{
			[self swapCamerasBetweenFirst:oneView second:viewPerspective];
		}
		NSRect frame = [viewPerspective frame];
		[[[mainSplit subviews] objectAtIndex:0] setFrame:frame];
		oneView = nil;
		return;
	}
	
	NSWindow *window = [viewPerspective window];
	NSPoint point = [window convertScreenToBase:[NSEvent mouseLocation]];
	
	NSView *hittedView = [[[window contentView] superview] hitTest:point];
	
	for (OpenGLSceneView *view in views)
	{
		if (view == hittedView)
		{
			oneView = view;
			
			[self collapseSplitView:mainSplit];
            [self collapseSplitView:topSplit];
			[self collapseSplitView:bottomSplit];			
			
			if (oneView != viewPerspective)
			{
				[self swapCamerasBetweenFirst:oneView second:viewPerspective];
			}
			return;
		}
	}	
}

@end
