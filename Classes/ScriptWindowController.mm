//
//  ScriptWindowController.m
//  MeshMaker
//
//  Created by Filip Kunc on 10/19/12.
//
//

#import "ScriptWindowController.h"
#import <WebKit/WebKit.h>

@interface ScriptWindowController () <NSWindowDelegate, NSTableViewDataSource, NSTableViewDelegate>
{
    IBOutlet WebView *editorView;
    IBOutlet NSTextView *outputView;
    IBOutlet NSTableView *scriptsTableView;
    WebScriptObject *editorObject;
    WebView *scriptView;
    WebScriptObject *scriptObject;
    NSMutableArray *scripts;
}

- (IBAction)newScript:(id)sender;
- (IBAction)runScript:(id)sender;

@end

@implementation ScriptWindowController

@synthesize delegate;

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

    [scriptView setUIDelegate:self];
    [[scriptView mainFrame] loadHTMLString:@"" baseURL:NULL];
	scriptObject = [scriptView windowScriptObject];
    
    NSURL *url = [[NSBundle mainBundle] URLForResource:@"editor" withExtension:@"html"];
    [editorView setFrameLoadDelegate:self];
    [[editorView mainFrame] loadRequest:[NSURLRequest requestWithURL:url]];
   
    [outputView setFont:[NSFont fontWithName:@"Monaco" size:14.0]];
    [outputView setString:@"Output:\n"];
    
    [[self window] setDelegate:self];
}

- (void)windowWillClose:(NSNotification *)notification
{
    [self savedCodeOfScriptAtIndex:scriptsTableView.selectedRow];
}

- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
    if (sender == editorView && frame == [editorView mainFrame])
        editorObject = (WebScriptObject *)[[editorView windowScriptObject] valueForKey:@"editor"];
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

    [editorObject callWebScriptMethod:@"setValue" withArguments:@[code]];
    [editorObject callWebScriptMethod:@"clearSelection" withArguments:nil];
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
    NSString *code = (NSString *)[editorObject callWebScriptMethod:@"getValue" withArguments:nil];
    
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

- (IBAction)runScript:(id)sender
{
    NSString *code = [self savedCodeOfScriptAtIndex:scriptsTableView.selectedRow];
    NSString* script = [NSString stringWithFormat:@"try { %@ } catch (e) { e.toString() }", code];
    
    [scriptObject setValue:delegate.items forKey:@"items"];
    [outputView setString:@"Output:\n"];
    
    [delegate allItemsActionWithName:@"Script Action" block:^
    {
        for (Item *item in delegate.items)
            item.mesh->beforeScriptAction();
        
        id data = [scriptObject evaluateWebScript:script];
        if (![data isMemberOfClass:[WebUndefined class]])
        {
            [outputView setString:[NSString stringWithFormat:@"Output:\n%@", [data description]]];
            NSLog(@"%@", data);
        }
        
        for (Item *item in delegate.items)
            item.mesh->afterScriptAction();
    }];
    
    [delegate setNeedsDisplayOnAllViews];
}

@end
