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

@interface FPTextureBrowserWindowController : NSWindowController
{
    ItemCollection *items;
    TextureCollection *textures;
}

- (void)setItems:(ItemCollection *)anItems textures:(TextureCollection *)aTextures;

@end
