#import <Cocoa/Cocoa.h>

@protocol AddCylinderProtocol

- (void)addCylinderWithSteps:(int)steps;

@end

@interface AddCylinderSheetController : NSObject 
{
	IBOutlet NSWindow *documentWindow;
	IBOutlet NSPanel *sheet;
	
	id<AddCylinderProtocol> documentProtocol;
	
	int steps;
}

@property (readwrite, assign) int steps;

- (void)beginSheetWithProtocol:(id<AddCylinderProtocol>)addCylinderProtocol;
- (IBAction)addCylinder:(id)sender;

@end
