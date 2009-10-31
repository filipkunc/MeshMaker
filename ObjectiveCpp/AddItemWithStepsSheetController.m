//
//  AddItemWithStepsSheetController.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 9/20/09.
//  For license see LICENSE.TXT
//

#import "AddItemWithStepsSheetController.h"

@implementation AddItemWithStepsSheetController

@synthesize steps;

- (id)init
{
	self = [super init];
	if (self != nil)
	{
		steps = 5;
	}
	return self;
}

- (void)beginSheetWithProtocol:(id<AddItemWithStepsProtocol>)addItemWithSteps
{
	documentProtocol = addItemWithSteps;
	
	if (sheet == nil) 
	{
        NSBundle *myBundle = [NSBundle bundleForClass:[self class]];
        NSNib *nib = [[NSNib alloc] initWithNibNamed:@"AddItemWithStepsSheet" bundle:myBundle];
		
        BOOL success = [nib instantiateNibWithOwner:self topLevelObjects:nil];
		
		[nib release];
		
        if (success != YES) 
		{
			NSLog(@"beginSheet failed");
            // should present error
            return;
        }
    }
		
	[NSApp beginSheet:sheet
	   modalForWindow:documentWindow
		modalDelegate:self
	   didEndSelector:@selector(sheetDidEnd:returnCode:contextInfo:) 
		  contextInfo:NULL];
}

- (IBAction)addItem:(id)sender
{
	[documentProtocol addItemWithSteps:steps];
	[NSApp endSheet:sheet];
}

- (void)sheetDidEnd:(NSWindow *)sheetWindow
		 returnCode:(int)returnCode
		contextInfo:(void  *)contextInfo 
{
    [sheetWindow orderOut:self];
}

@end
