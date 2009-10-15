#import <Cocoa/Cocoa.h>

@protocol AddItemWithStepsProtocol

- (void)addItemWithSteps:(uint)steps;

@end

@interface AddItemWithStepsSheetController : NSObject 
{
	IBOutlet NSWindow *documentWindow;
	IBOutlet NSPanel *sheet;
	
	id<AddItemWithStepsProtocol> documentProtocol;
	
	uint steps;
}

@property (readwrite, assign) uint steps;

- (void)beginSheetWithProtocol:(id<AddItemWithStepsProtocol>)addItemWithSteps;
- (IBAction)addItem:(id)sender;

@end
