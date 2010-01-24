//
//  PropertyGridAppDelegate.h
//  PropertyGrid
//
//  Created by Filip Kunc on 1/23/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "PropertyReflector.h"

@interface PropertyGridAppDelegate : NSObject <NSApplicationDelegate> 
{
    NSWindow *window;
	NSTableView *tableView;
	PropertyReflector *propertyReflector;
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSTableView *tableView;

@end
