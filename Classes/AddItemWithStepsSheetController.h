//
//  AddItemWithStepsSheetController.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 9/20/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>

@protocol AddItemWithStepsProtocol

- (void)addItemWithSteps:(unsigned int)steps;

@end

@interface AddItemWithStepsSheetController : NSObject 
{
	IBOutlet NSWindow *documentWindow;
	IBOutlet NSPanel *sheet;
    
	id<AddItemWithStepsProtocol> documentProtocol;
	
	unsigned int steps;
}

@property (readwrite, assign) unsigned int steps;

- (void)beginSheetWithProtocol:(id<AddItemWithStepsProtocol>)addItemWithSteps;
- (IBAction)addItem:(id)sender;

@end
