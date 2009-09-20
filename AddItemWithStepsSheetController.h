#import <Cocoa/Cocoa.h>

@protocol AddItemWithStepsProtocol

- (void)addItemWithSteps:(NSUInteger)steps;

@end

@interface AddItemWithStepsSheetController : NSObject 
{
	IBOutlet NSWindow *documentWindow;
	IBOutlet NSPanel *sheet;
	
	id<AddItemWithStepsProtocol> documentProtocol;
	
	NSUInteger steps;
}

@property (readwrite, assign) NSUInteger steps;

- (void)beginSheetWithProtocol:(id<AddItemWithStepsProtocol>)addItemWithSteps;
- (IBAction)addItem:(id)sender;

@end
