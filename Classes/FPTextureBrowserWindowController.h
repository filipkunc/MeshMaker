//
//  FPTextureBrowserWindowController.h
//  MeshMaker
//
//  Created by Filip Kunc on 4/14/12.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "ItemCollection.h"
#import "TextureCollection.h"

@protocol FPTextureBrowserDelegate <NSObject>

@property (readonly) ItemCollection *items;
@property (readonly) TextureCollection *textures;

- (void)setNeedsDisplayOnAllViews;

@end

@interface FPTextureBrowserWindowController : NSWindowController

@property (readwrite, weak) id<FPTextureBrowserDelegate> delegate;

- (void)reloadData;

@end
