//
//  FPSelectionWindowController.h
//  MeshMaker
//
//  Created by Filip Kunc on 2/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import "FPCurveView.h"

@protocol FPSelectionWindowDelegate <NSObject, FPCurveViewDelegate>

@property (readwrite, assign) BOOL selectThrough;
@property (readwrite, assign) BOOL useSoftSelection;
- (void)weightsChanged;

@end

@interface FPSelectionWindowController : NSWindowController

@property (weak) IBOutlet FPCurveView *curveView;
@property (readwrite, weak) id<FPSelectionWindowDelegate> delegate;
@property (readwrite, assign) BOOL selectThrough;
@property (readwrite, assign) BOOL useSoftSelection;

@end
