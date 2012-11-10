//
//  ScriptWindowController.m
//  MeshMaker
//
//  Created by Filip Kunc on 10/19/12.
//
//

#import "ScriptWindowController.h"
#import <WebKit/WebKit.h>
#import <MGSFragaria/MGSFragaria.h>
#import <MGSFragaria/SMLSyntaxColouring.h>

@interface ScriptWindowController () <NSWindowDelegate, NSTableViewDataSource, NSTableViewDelegate>
{
    IBOutlet NSView *editorView;
    IBOutlet NSTextView *outputView;
    IBOutlet NSTableView *scriptsTableView;
    WebView *scriptView;
    WebScriptObject *scriptObject;
    NSMutableArray *scripts;
    
    MGSFragaria *fragaria;
}

- (IBAction)newScript:(id)sender;
- (IBAction)runScript:(id)sender;

@end

@implementation ScriptWindowController

@synthesize delegate, scripts;

- (id)init
{
    self = [super initWithWindowNibName:@"ScriptWindowController"];
    if (self)
    {
        scriptView = [[WebView alloc] init];
        scripts = [[NSMutableArray alloc] init];
        NSURL *scriptsDirectoryURL = [self scriptsDirectoryURL];
        
        NSArray *keys = @[NSURLIsDirectoryKey, NSURLIsPackageKey, NSURLLocalizedNameKey];
        
        NSDirectoryEnumerator *enumerator = [[NSFileManager defaultManager]
                                             enumeratorAtURL:scriptsDirectoryURL
                                             includingPropertiesForKeys:keys
                                             options:(NSDirectoryEnumerationSkipsPackageDescendants |
                                                      NSDirectoryEnumerationSkipsHiddenFiles)
                                             errorHandler:^(NSURL *url, NSError *error) {
                                                 // Handle the error.
                                                 // Return YES if the enumeration should continue after the error.
                                                 return NO;
                                             }];
        for (NSURL *url in enumerator)
        {
            if ([url isFileURL])
            {
                [scripts addObject:[[url lastPathComponent] stringByDeletingPathExtension]];
            }
        }
        
        if (scripts.count == 0)
            [scripts addObject:@"Script1"];
        
        [scriptView setUIDelegate:self];
        [[scriptView mainFrame] loadHTMLString:@"" baseURL:NULL];
        scriptObject = [scriptView windowScriptObject];
    }
    return self;
}

- (NSURL*)scriptsDirectoryURL
{
    NSFileManager* sharedFileManager = [NSFileManager defaultManager];
    NSArray* possibleURLs = [sharedFileManager URLsForDirectory:NSApplicationSupportDirectory
                                                      inDomains:NSUserDomainMask];
    NSURL* appSupportDir = nil;
    NSURL* appDirectory = nil;
    
    if ([possibleURLs count] >= 1)
    {
        // Use the first directory (if multiple are returned)
        appSupportDir = [possibleURLs objectAtIndex:0];
    }
    
    // If a valid app support directory exists, add the
    // app's bundle ID to it to specify the final directory.
    if (appSupportDir)
    {
        NSString* appBundleID = [[NSBundle mainBundle] bundleIdentifier];
        appDirectory = [appSupportDir URLByAppendingPathComponent:appBundleID];
    }
    
    NSURL *scriptsDirectoryURL = [appDirectory URLByAppendingPathComponent:@"Scripts"];
    NSError *error = nil;
    if (![sharedFileManager createDirectoryAtURL:scriptsDirectoryURL withIntermediateDirectories:YES attributes:nil error:&error])
        NSLog(@"Create Scripts directory failed: %@", [error localizedDescription]);
    
    return scriptsDirectoryURL;
}

- (void)windowDidLoad
{
    [super windowDidLoad];

    // create an instance
	fragaria = [[MGSFragaria alloc] init];
	
	[fragaria setObject:self forKey:MGSFODelegate];
	
	// define our syntax definition
    [fragaria setObject:@"JavaScript" forKey:MGSFOSyntaxDefinitionName];
	
	// embed editor in editorView
	[fragaria embedInView:editorView];
	
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    [defaults setObject:@YES forKey:MGSFragariaPrefsAutocompleteSuggestAutomatically];
    [defaults setObject:@NO forKey:MGSFragariaPrefsLineWrapNewDocuments];
    
    NSFont *codeFont = [NSFont fontWithName:@"Monaco" size:14];
    
    [defaults setObject:[NSArchiver archivedDataWithRootObject:codeFont] forKey:MGSFragariaPrefsTextFont];
    [defaults setObject:@0.1 forKey:MGSFragariaPrefsAutocompleteAfterDelay];
    
    [fragaria setSyntaxColoured:YES];
    [fragaria setShowsLineNumbers:YES];
    
    SMLSyntaxColouring *syntaxColouring = [[fragaria docSpec] valueForKey:ro_MGSFOSyntaxColouring];
    
    NSArray *keywords = @[
        // JavaScript keywords
        @"break",
        @"else",
        @"new",
        @"var",
        @"case",
        @"finally",
        @"return",
        @"void",
        @"catch",
        @"for",
        @"switch",
        @"while",
        @"continue",
        @"function",
        @"this",
        @"with",
        @"default",
        @"if",
        @"throw",
        @"delete",
        @"in",
        @"try",
        @"do",
        @"instanceof",
        @"typeof",
        @"null",
        @"true",
        @"false",
    ];
        
    NSArray *autocompleteWords = @[
        // JavaScript
        @"alert",
        @"toString",
        // MeshMaker only
        @"items",
        @"at",
        @"count",
        @"vertexIterator",
        @"triQuadIterator",
        @"edgeIterator",
        @"vertexCount",
        @"triQuadCount",
        @"makeTexCoords",
        @"makeEdges",
        @"updateSelection",
        @"addVertex",
        @"addTriangle",
        @"addQuad",
        @"removeTriQuad",
        @"selected",
        @"setSelected",
        @"index",
        @"setIndex",
        @"edgeCount",
        @"isQuad",
        @"vertex",
        @"edge",
        @"setVertex",
        @"setEdge",
        @"vertexNotInEdge",
        @"x",
        @"y",
        @"z",
        @"setX",
        @"setY",
        @"setZ",
        @"half",
        @"setHalf",
        @"triangle",
        @"setTriangle",
        @"oppositeVertex",
        @"finished",
        @"moveStart",
        @"moveNext",
        @"removeDegeneratedTriangles",
        @"removeNonUsedVertices",
        @"removeNonUsedTexCoords",
        @"removeSelected",
        @"mergeSelected",
        @"splitSelected",
        @"detachSelected",
        @"duplicateSelectedTriangles",
        @"flipSelected",
        @"flipAllTriangles",
        @"extrudeSelectedTriangles",
        @"triangulate",
        @"triangulateSelectedQuads",
        @"openSubdivision",
        @"loopSubdivision",
        @"setSelectionModeVertices",
        @"setSelectionModeTriQuads",
        @"setSelectionModeEdges",
    ];
    
    keywords = [keywords sortedArrayUsingSelector:@selector(compare:)];
    autocompleteWords = [autocompleteWords sortedArrayUsingSelector:@selector(compare:)];
    
    NSArray *keywordsAndAutocompleteWords = [keywords arrayByAddingObjectsFromArray:autocompleteWords];
    
    keywordsAndAutocompleteWords = [keywordsAndAutocompleteWords sortedArrayUsingSelector:@selector(compare:)];
    
    [syntaxColouring setValue:keywords forKey:@"keywords"];
    [syntaxColouring setValue:autocompleteWords forKey:@"autocompleteWords"];
    [syntaxColouring setValue:keywordsAndAutocompleteWords forKey:@"keywordsAndAutocompleteWords"];
    
    [outputView setFont:codeFont];
    [outputView setString:@"Output:\n"];
    
    [[self window] setDelegate:self];
}

- (void)windowWillClose:(NSNotification *)notification
{
    [self savedCodeOfScriptAtIndex:scriptsTableView.selectedRow];
}

- (void)webView:(WebView *)sender runJavaScriptAlertPanelWithMessage:(NSString *)message
{
    NSString *outputText = [NSString stringWithFormat:@"%@%@\n", [outputView string], message];
    [outputView setString:outputText];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    return scripts.count;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    return scripts[row];
}

- (void)deleteSelected:(id)sender
{
    NSInteger index = scriptsTableView.selectedRow;
    if (index >= 0 && index < (NSInteger)scripts.count)
    {
        [scriptsTableView deselectAll:nil];
        [self removeScriptWithName:(NSString *)scripts[index]];
        [scripts removeObjectAtIndex:index];
        [scriptsTableView reloadData];
    }
}

- (NSIndexSet *)tableView:(NSTableView *)tableView selectionIndexesForProposedSelection:(NSIndexSet *)proposedSelectionIndexes
{
    NSInteger index = scriptsTableView.selectedRow;
    [self savedCodeOfScriptAtIndex:index];
    
    return proposedSelectionIndexes;
}

- (void)tableView:(NSTableView *)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    if ([scripts containsObject:object])
        return;

    [self renameScriptWithName:(NSString *)scripts[row] to:(NSString *)object];

    scripts[row] = object;
}

- (NSURL *)scriptURLWithName:(NSString *)name
{
    NSString *scriptName = [name stringByAppendingPathExtension:@"js"];
    NSURL *scriptURL = [[self scriptsDirectoryURL] URLByAppendingPathComponent:scriptName isDirectory:NO];
    return scriptURL;
}

- (void)removeScriptWithName:(NSString *)name
{
    NSURL *url = [self scriptURLWithName:name];
    NSError *error = nil;
    if (![[NSFileManager defaultManager] removeItemAtURL:url error:&error])
        NSLog(@"Remove script failed: %@", [error localizedDescription]);
}

- (void)renameScriptWithName:(NSString *)oldName to:(NSString *)newName
{
    NSURL *oldURL = [self scriptURLWithName:oldName];
    NSURL *newURL = [self scriptURLWithName:newName];
    NSError *error = nil;
    if (![[NSFileManager defaultManager] moveItemAtURL:oldURL toURL:newURL error:&error])
        NSLog(@"Rename script failed: %@", [error localizedDescription]);
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
    NSString *code = [self loadedCodeOfScriptAtIndex:scriptsTableView.selectedRow];
    [fragaria setString:code];
}

- (void)newScript:(id)sender
{
    NSUInteger numberSuffix = scripts.count;
    NSString *name;
    do
    {
        numberSuffix++;
        name = [NSString stringWithFormat:@"Script%lu", numberSuffix];
    }
    while ([scripts containsObject:name]);
    
    [scripts addObject:name];
    [scriptsTableView reloadData];
}

- (NSString *)loadedCodeOfScriptAtIndex:(NSInteger)index
{
    if (index >= 0 && index < (NSInteger)scripts.count)
    {
        NSURL *scriptURL = [self scriptURLWithName:(NSString *)scripts[index]];
        NSError *error = nil;
        NSString *code = [NSString stringWithContentsOfURL:scriptURL encoding:NSUTF8StringEncoding error:&error];
        if (error)
        {
            NSLog(@"Script content error: %@", [error localizedDescription]);
            return @"";
        }
        return code;
    }
    return @"";
}

- (NSString *)savedCodeOfScriptAtIndex:(NSInteger)index
{
    NSString *code = [fragaria string];
    
    if (index >= 0 && index < (NSInteger)scripts.count)
    {
        NSURL *scriptURL = [self scriptURLWithName:(NSString *)scripts[index]];
        NSError *error = nil;

        [code writeToURL:scriptURL atomically:YES encoding:NSUTF8StringEncoding error:&error];
        if (error)
            NSLog(@"Script content error: %@", [error localizedDescription]);
    }
    
    return code;
}

- (void)runScriptWithName:(NSString *)scriptName
{
    NSURL *scriptURL = [self scriptURLWithName:scriptName];
    NSError *error = nil;
    NSString *code = [NSString stringWithContentsOfURL:scriptURL encoding:NSUTF8StringEncoding error:&error];
    if (error)
    {
        NSLog(@"Script content error: %@", [error localizedDescription]);
        return;
    }
    [self runScriptCode:code];
}

- (void)runScriptCode:(NSString *)code
{
    NSString* script = [NSString stringWithFormat:@"try { %@ } catch (e) { e.toString() }", code];
    [scriptObject setValue:delegate.items forKey:@"items"];
    [outputView setString:@"Output:\n"];
    
    [delegate allItemsActionWithName:@"Script Action On Items" block:^
    {
        for (Item *item in delegate.items)
        {
            item.mesh->resetTriangleCache();
            item.mesh->resetAlgorithmData();
        }
        
        id data = [scriptObject evaluateWebScript:script];
        if (![data isMemberOfClass:[WebUndefined class]])
        {
            [outputView setString:[NSString stringWithFormat:@"Output:\n%@", [data description]]];
            NSLog(@"%@", data);
        }
        
        [delegate updateManipulatedSelection];
    }];

    [delegate setNeedsDisplayOnAllViews];
}

- (IBAction)runScript:(id)sender
{
    NSString *code = [self savedCodeOfScriptAtIndex:scriptsTableView.selectedRow];
    [self runScriptCode:code];
}

- (void)menuNeedsUpdate:(NSMenu *)menu
{
    if (menu != [delegate.scriptPullDown menu])
        return;
    
    NSMenuItem *firstItem = [menu itemAtIndex:0];
    [menu removeAllItems];
    [menu addItem:firstItem];
    
    NSArray *scriptArray = [self scripts];
    NSUInteger index = 1;
    for (NSString *script in scriptArray)
    {
        NSMenuItem *item = [menu addItemWithTitle:script action:@selector(runScriptAction:) keyEquivalent:@""];
        item.target = self;
        item.keyEquivalentModifierMask = NSCommandKeyMask;
        item.keyEquivalent = [NSString stringWithFormat:@"%lu", index];
        
        if (index != 0 && ++index > 9)
            index = 0;
    }
}

- (IBAction)runScriptAction:(id)sender
{
    NSString *scriptName = [[delegate.scriptPullDown selectedItem] title];
    [self runScriptWithName:scriptName];
}


@end
