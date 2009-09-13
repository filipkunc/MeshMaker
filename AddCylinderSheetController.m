#import "AddCylinderSheetController.h"

@implementation AddCylinderSheetController

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

- (void)beginSheetWithProtocol:(id<AddCylinderProtocol>)addCylinderProtocol
{
	documentProtocol = addCylinderProtocol;
	
	if (sheet == nil) 
	{
        NSBundle *myBundle = [NSBundle bundleForClass:[self class]];
        NSNib *nib = [[NSNib alloc] initWithNibNamed:@"AddCylinderSheet" bundle:myBundle];
		
        BOOL success = [nib instantiateNibWithOwner:self topLevelObjects:nil];
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

- (IBAction)addCylinder:(id)sender
{
	[documentProtocol addCylinderWithSteps:steps];
	[NSApp endSheet:sheet];
}

- (void)sheetDidEnd:(NSWindow *)sheetWindow
		 returnCode:(int)returnCode
		contextInfo:(void  *)contextInfo 
{
    [sheetWindow orderOut:self];
}

@end
