//
//  ScriptWindowController.h
//  MeshMaker
//
//  Created by Filip Kunc on 10/19/12.
//
//

#import <Cocoa/Cocoa.h>
#import "ItemCollection.h"

@interface ScriptWindowController : NSWindowController
{

}

@property (readwrite, retain) ItemCollection *items;

@end
