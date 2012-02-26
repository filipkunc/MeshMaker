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
	// maximizing window on start
	[[viewPerspective window] performZoom:self];
	
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

- (NSApplicationPresentationOptions)window:(NSWindow *)window willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
    return proposedOptions | NSApplicationPresentationAutoHideToolbar;    
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
	[itemsController changeSelection:NO];
	[self setManipulated:itemsController];
}

- (float)positionX
{
	return [manipulated positionX];
}

- (float)positionY
{
	return [manipulated positionY];
}

- (float)positionZ
{
	return [manipulated positionZ];
}

- (float)rotationX
{
	return [manipulated rotationX];
}

- (float)rotationY
{
	return [manipulated rotationY];
}

- (float)rotationZ
{
	return [manipulated rotationZ];
}

- (float)scaleX
{
	return [manipulated scaleX];
}

- (float)scaleY
{
	return [manipulated scaleY];
}

- (float)scaleZ
{
	return [manipulated scaleZ];
}

- (void)setPositionX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setPositionX:value];
	[self manipulationEndedInView:nil];
}

- (void)setPositionY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setPositionY:value];
	[self manipulationEndedInView:nil];
}

- (void)setPositionZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setPositionZ:value];
	[self manipulationEndedInView:nil];
}

- (void)setRotationX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setRotationX:value];
	[self manipulationEndedInView:nil];
}

- (void)setRotationY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setRotationY:value];
	[self manipulationEndedInView:nil];
}

- (void)setRotationZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setRotationZ:value];
	[self manipulationEndedInView:nil];
}

- (void)setScaleX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setScaleX:value];
	[self manipulationEndedInView:nil];
}

- (void)setScaleY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setScaleY:value];
	[self manipulationEndedInView:nil];
}

- (void)setScaleZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setScaleZ:value];
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
        case EditModeQuadsTriangles:
            [self editMeshWithMode:MeshSelectionModeQuadsTriangles];
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

- (IBAction)splitSelected:(id)sender
{
    if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		[self meshActionWithName:@"Split" block:^ { [[self currentMesh] splitSelected]; }];
	}
	
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)flipSelected:(id)sender
{
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		[self meshActionWithName:@"Flip" block:^ { [[self currentMesh] flipSelected]; }];
	}
	
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
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
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document undoDeleteSelected:selectedItems];
	
	[itemsController updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (void)undoDeleteSelected:(NSMutableArray *)selectedItems
{
	[self setManipulated:itemsController];
	[items setCurrentItems:selectedItems];
	
	MyDocument *document = [self prepareUndoWithName:@"Delete"];
	[document redoDeleteSelected:selectedItems];
	
	[itemsController updateSelection];
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)mergeVertexPairs:(id)sender
{
	if ([manipulated selectedCount] <= 0)
		return;
	
	if (manipulated == meshController)
	{
		if ([[self currentMesh] selectionMode] == MeshSelectionModeVertices)
		{
			[self meshActionWithName:@"Merge Vertex Pairs" block:^ { [[self currentMesh] mergeVertexPairs]; }];
		}
	}
	[self setNeedsDisplayOnAllViews];
}

- (IBAction)subdivision:(id)sender
{
    Mesh *currentMesh;
    
    if (manipulated == meshController)
        currentMesh = [self currentMesh];
    else
        currentMesh = [items currentMesh];

    if (currentMesh)
    {
        [self meshActionWithName:@"Subdivision" block:^ { [currentMesh loopSubdivision]; }];
    }
	
	[manipulated updateSelection];
	[self setNeedsDisplayOnAllViews];
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
	ManipulatorType newManipulator = (ManipulatorType)[sender tag];
	for (OpenGLSceneView *view in views)
	{ 
		[view setCurrentManipulator:newManipulator]; 
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

- (void)detachSelected:(id)sender
{
    [[self currentMesh] detachSelected];
    [self setNeedsDisplayOnAllViews];
}

- (void)cleanTexture:(id)sender
{
    [[self currentMesh] cleanTexture];
    [self setNeedsDisplayOnAllViews];
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

- (BOOL)readFromCollada:(NSString *)fileName
{
    NSString* xmlData = [NSString stringWithContentsOfFile:fileName encoding:NSUTF8StringEncoding error:NULL];
    
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

- (BOOL)readFromModel3D:(NSString *)fileName
{
	const char *cFileName = [fileName cStringUsingEncoding:NSUTF8StringEncoding];
	ifstream fin;
	fin.open(cFileName, ios::in | ios::binary);
	if (fin.is_open())
	{
		ItemCollection *newItems = [[ItemCollection alloc] initWithFileStream:&fin];
		items = newItems;
		[itemsController setModel:items];
		[itemsController updateSelection];
		[self setManipulated:itemsController];
		fin.close();
		return YES;
	}
	fin.close();
	return NO;
}

- (void)writeToModel3D:(NSString *)fileName
{
	const char *cFileName = [fileName cStringUsingEncoding:NSUTF8StringEncoding];
	ofstream fout;
	fout.open(cFileName, ios::out | ios::binary);
	[items encodeWithFileStream:&fout];
	fout.close();
}

// these read/write methods are deprecated, I need to use newer methods

- (BOOL)readFromFile:(NSString *)fileName ofType:(NSString *)typeName
{
	if ([typeName isEqual:@"model3D"])
		return [self readFromModel3D:fileName];

    if ([typeName isEqualToString:@"Collada"])
        return [self readFromCollada:fileName];
	
    return NO;
}

- (BOOL)writeToFile:(NSString *)fileName ofType:(NSString *)typeName
{
	if ([typeName isEqual:@"model3D"])
	{
		[self writeToModel3D:fileName];
		return YES;
	}
	return NO;
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
