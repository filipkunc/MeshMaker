//
//  ScriptWindowController.h
//  MeshMaker
//
//  Created by Filip Kunc on 10/19/12.
//
//

#import <Cocoa/Cocoa.h>
#import "ItemCollection.h"

@protocol ScriptDelegate <NSObject>

@property (readonly) ItemCollection *items;

- (void)allItemsActionWithName:(NSString *)actionName block:(void (^)())action;
- (void)setNeedsDisplayOnAllViews;

@end

@interface ScriptWindowController : NSWindowController
{

}

@property (readwrite, weak) id<ScriptDelegate> delegate;
@property (readonly) NSArray *scripts;

- (void)runScriptWithName:(NSString *)scriptName;

@end
