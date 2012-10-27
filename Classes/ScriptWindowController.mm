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

@synthesize delegate;

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
    
    [scriptObject setValue:delegate.items forKey:@"items"];
    [outputView setString:@""];
    
    [delegate allItemsActionWithName:@"Script Action" block:^
    {
        for (Item *item in delegate.items)
            item.mesh->beforeScriptAction();
        
        id data = [scriptObject evaluateWebScript:script];
        if (![data isMemberOfClass:[WebUndefined class]])
        {
            [outputView setString:[data description]];
            NSLog(@"%@", data);
        }
        
        for (Item *item in delegate.items)
            item.mesh->afterScriptAction();
    }];
    
    [delegate setNeedsDisplayOnAllViews];
}

@end
