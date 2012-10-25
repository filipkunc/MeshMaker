//
//  ScriptWindowController.m
//  MeshMaker
//
//  Created by Filip Kunc on 10/19/12.
//
//

#import "ScriptWindowController.h"
#import <WebKit/WebKit.h>

@interface ScriptWindowController ()
{
    IBOutlet WebView *editorView;
    IBOutlet NSTextView *outputView;
    WebScriptObject *editorObject;
    WebView *scriptView;
    WebScriptObject *scriptObject;
}

- (IBAction)runScript:(id)sender;

@end

@implementation ScriptWindowController

@synthesize items;

- (id)init
{
    self = [super initWithWindowNibName:@"ScriptWindowController"];
    if (self)
    {
        scriptView = [[WebView alloc] init];
    }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    [[scriptView mainFrame] loadHTMLString:@"" baseURL:NULL];
    [scriptView setUIDelegate:self];
	scriptObject = [scriptView windowScriptObject];
	[scriptObject setValue:items forKey:@"items"];
    
    NSURL *url = [[NSBundle mainBundle] URLForResource:@"editor" withExtension:@"html"];
    [[editorView mainFrame] loadRequest:[NSURLRequest requestWithURL:url]];
    editorObject = [editorView windowScriptObject];
    [outputView setFont:[NSFont fontWithName:@"Monaco" size:14.0]];
}

- (void)webView:(WebView *)sender runJavaScriptAlertPanelWithMessage:(NSString *)message
{
    NSString *outputText = [NSString stringWithFormat:@"%@%@\n", [outputView string], message];
    [outputView setString:outputText];
}

- (IBAction)runScript:(id)sender
{
    NSString *code = (NSString *)[editorObject evaluateWebScript:@"editor.getValue();"];
	NSString* script = [NSString stringWithFormat:@"try { %@ } catch (e) { e.toString() }", code];
    
    [outputView setString:@""];
    
    for (Item *item in items)
        item.mesh->beforeScriptAction();
    
    id data = [scriptObject evaluateWebScript:script];
	if (![data isMemberOfClass:[WebUndefined class]])
    {
        [outputView setString:[data description]];
		NSLog(@"%@", data);
    }
    //[scriptObject evaluateWebScript:script];
    
    for (Item *item in items)
        item.mesh->afterScriptAction();
}

@end
