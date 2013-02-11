//
//  FPSelectionWindowController.h
//  MeshMaker
//
//  Created by Filip Kunc on 2/10/13.
//
//

#import <Cocoa/Cocoa.h>

@protocol FPSelectionWindowDelegate <NSObject>

@property (readwrite, assign) BOOL selectThrough;
@property (readwrite, assign) BOOL useSoftSelection;

@end

@interface FPSelectionWindowController : NSWindowController

@property (readwrite, weak) id<FPSelectionWindowDelegate> delegate;
@property (readwrite, assign) BOOL selectThrough;
@property (readwrite, assign) BOOL useSoftSelection;

@end
