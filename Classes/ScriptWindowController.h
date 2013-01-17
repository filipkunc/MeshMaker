//
//  ScriptWindowController.h
//  MeshMaker
//
//  Created by Filip Kunc on 10/19/12.
//
//

#pragma once

#import <Cocoa/Cocoa.h>
#import "ItemCollection.h"
#import "JSWrappers.h"

@protocol ScriptDelegate <NSObject>

@property (readonly) ItemCollection *items;
@property (readonly) NSPopUpButton *scriptPullDown;

- (void)allItemsActionWithName:(NSString *)actionName block:(void (^)())action;
- (void)updateManipulatedSelection;
- (void)setNeedsDisplayOnAllViews;

@end

@interface ScriptWindowController : NSWindowController <NSMenuDelegate>
{

}

@property (readwrite, weak) id<ScriptDelegate> delegate;
@property (readonly) NSArray *scripts;

- (void)runScriptWithName:(NSString *)scriptName;
- (IBAction)runScriptAction:(id)sender;

@end
