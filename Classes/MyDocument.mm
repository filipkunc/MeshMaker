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
    
    stringstream ss(s);
    while (!ss.eof())
    {
        T value;
        ss >> value;
        values->push_back(value);
    }
    
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

		[itemsController addTransformationObserver:self];
		[meshController addTransformationObserver:self];
		
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
	[itemsController removeTransformationObserver:self];
	[meshController removeTransformationObserver:self];
}

- (void)awakeFromNib
{
	[editModePopUp selectItemWithTag:0];
	[viewModePopUp selectItemWithTag:0];
	
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
		int meshTag = [[self currentMesh] selectionMode] + 1;
		[editModePopUp selectItemWithTag:meshTag];
	}
}

- (Mesh *)currentMesh
{
	if (manipulated == meshController)
		return (Mesh *)[meshController model];
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
	Mesh *mesh = [item mesh];
	[mesh makeMeshWithType:type steps:steps];
	
	NSString *name = [Mesh descriptionOfMeshType:type];
	
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
	NSString *name = [Mesh descriptionOfMeshType:type];
	
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

- (void)setNilValueForKey:(NSString *)key
{
	[self setValue:[NSNumber numberWithFloat:0.0f] forKey:key];
}

- (void)observeValueForKeyPath:(NSString *)keyPath 
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	// Is it right way to do it? I really don't know, but it works.
	[self willChangeValueForKey:keyPath];
	[self didChangeValueForKey:keyPath];
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
	[meshController setModel:[item mesh]];
	
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
	}
	else if (manipulated == meshController)
	{
		MyDocument *document = [self prepareUndoWithName:@"Mesh Manipulation"];
		[document swapMeshStateWithOld:oldMeshState current:[items currentMeshState] actionName:@"Mesh Manipulation"];
		oldMeshState = nil;
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
		[[item mesh] setSelectionMode:mode];
		[meshController setModel:[item mesh]];
		[meshController setPosition:[item position] 
						   rotation:[item rotation] 
							  scale:[item scale]];
		[self setManipulated:meshController];
	}
}

- (void)editItems
{
	[[self currentMesh] setSelectionMode:MeshSelectionModeVertices];
	[itemsController setModel:items];
	[itemsController setPosition:Vector3D()
						rotation:Quaternion()
						   scale:Vector3D(1, 1, 1)];
	[self setManipulated:itemsController];
}

- (IBAction)changeEditMode:(id)sender
{
	EditMode mode = (EditMode)[[editModePopUp selectedItem] tag];
    Mesh *currentMesh = [self currentMesh];
    
    if (currentMesh == nil)
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
	ViewMode mode = (ViewMode)[[viewModePopUp selectedItem] tag];
	for (OpenGLSceneView *view in views)
	{ 
        if (mode == ViewModeUnwrap)
            Mesh2::setUnwrapped(true);
        else
            Mesh2::setUnwrapped(false);
        
		[view setViewMode:mode];
	}
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
		[self meshActionWithName:@"Merge" block:^ { [[self currentMesh] mergeSelected]; }]; 
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
    [self meshOnlyActionWithName:@"Split" block:^ { [[self currentMesh] splitSelected]; }];
}

- (IBAction)flipSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Flip" block:^ { [[self currentMesh] flipSelected]; }];
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
    [self meshOnlyActionWithName:@"Subdivision" block:^ { [[self currentMesh] loopSubdivision]; }];
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
    [self meshOnlyActionWithName:@"Detach" block:^ { [[self currentMesh] detachSelected]; }];
}

- (IBAction)extrudeSelected:(id)sender
{
    [self meshOnlyActionWithName:@"Extrude" block:^ { [[self currentMesh] extrudeSelected]; }];
}

- (IBAction)cleanTexture:(id)sender
{
    [[self currentMesh] cleanTexture];
    [self setNeedsDisplayOnAllViews];
}

- (IBAction)setBaseColorFromBrush:(id)sender
{
    [[self currentMesh] setColor:self.brushColor];
}

- (IBAction)resetTexCoords:(id)sender
{
    [self meshOnlyActionWithName:@"Reset Texture Coordinates" block:^ { [[self currentMesh] resetTexCooords]; }];
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
    
    [[dirWrapper fileWrappers] enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
        
        NSString *textureName = (NSString *)key;
        if ([textureName hasSuffix:@".png"])
        {
            textureName = [textureName stringByDeletingPathExtension];
            
            NSFileWrapper *wrapper = (NSFileWrapper *)obj;
            NSData *data = [wrapper regularFileContents];
            
            NSImage *image = [[NSImage alloc] initWithData:data];
            int index = [textureName substringFromIndex:@"Texture".length].integerValue;
            Item *item = [items itemAtIndex:index];
            FPTexture *texture = item.mesh->mesh->texture();
            [texture setCanvas:image];
        }        
    }];
    
    return YES;
}

- (NSFileWrapper *)fileWrapperOfType:(NSString *)typeName error:(NSError *__autoreleasing *)outError
{
    if ([typeName isEqualToString:@"model3D"])
        return [[NSFileWrapper alloc] initRegularFileWithContents:[self dataOfModel3D]];
        
    
    NSFileWrapper *dirWrapper = [[NSFileWrapper alloc] initDirectoryWithFileWrappers:nil];
    
    [dirWrapper addRegularFileWithContents:[self dataOfModel3D]
                         preferredFilename:@"Geometry.model3D"];
    
    int i = 0;
    
    for (Item *item in items)
    {
        NSImage *image = item.mesh->mesh->texture().canvas;
        NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithData:[image TIFFRepresentation]];
        
        NSData *imageData = [bitmap representationUsingType:NSPNGFileType properties:nil];

        
        [dirWrapper addRegularFileWithContents:imageData 
                             preferredFilename:[NSString stringWithFormat:@"Texture%.2i.png", i]];
        
        i++;
    }
    
    return dirWrapper;
}

- (BOOL)readFromCollada:(NSData *)data
{
    NSString* xmlData = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    
    int length = [xmlData lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
    char *textBuffer = new char [length];
    memset(textBuffer, 0, length);
    memcpy(textBuffer, [xmlData UTF8String], length);
    
    // Insert code here to initialize your application
    xml_document< > document;
    document.parse<0>(textBuffer);
    
    xml_node< > *mesh = document.first_node()->first_node("library_geometries")->first_node("geometry")->first_node("mesh");
    
    string positionsString = mesh->first_node("source")->first_node("float_array")->value();
    vector<float> *points = ReadValues<float>(positionsString);
    
    string uvCoordsString = mesh->first_node("source")->next_sibling()->next_sibling()->next_sibling()->first_node("float_array")->value();
    vector<float> *uvCoords = ReadValues<float>(uvCoordsString);
    
    xml_node< > *triNode = mesh->first_node("source")->next_sibling("triangles")->first_node();
    
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
    
    ItemCollection *newItems = [[ItemCollection alloc] init];
    
    Item *item = [[Item alloc] init];
    Mesh *itemMesh = [item mesh];
    
    vector<Vector3D> vertices;
    vector<Vector3D> texCoords;
    vector<Triangle> triangles;
    
    for (uint i = 0; i < points->size(); i += 3)
    {
        Vector3D point;
        for (uint j = 0; j < 3; j++)
            point[j] = (*points)[i + j];

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
    
    for (uint i = 0; i < trianglesRef.size(); i += inputTypesCount * 3)
    {
        uint vertexIndices[3];
        uint texCoordIndices[3];
        
        for (uint j = 0; j < 3; j++)
        {
            vertexIndices[j] = trianglesRef[i + j * inputTypesCount];
            texCoordIndices[j] = trianglesRef[i + j * inputTypesCount + 3];
        }
                
        AddTriangle(triangles, vertexIndices, texCoordIndices);
    }

    itemMesh->mesh->fromIndexRepresentation(vertices, texCoords, triangles);
    
    [newItems addItem:item];
    items = newItems;
    [itemsController setModel:items];
    [itemsController updateSelection];
    [self setManipulated:itemsController];

    delete points;
    delete uvCoords;
    delete indices;
    delete [] textBuffer;

    return YES;
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
        Mesh *itemMesh = [item mesh];
        
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
        
        itemMesh->mesh->fromIndexRepresentation(vertices, texCoords, triangles);
        
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
