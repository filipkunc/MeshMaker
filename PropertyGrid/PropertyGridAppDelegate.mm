//
//  PropertyGridAppDelegate.m
//  PropertyGrid
//
//  Created by Filip Kunc on 1/23/10.
//  For license see LICENSE.TXT
//

#import "PropertyGridAppDelegate.h"
#import "MyTestObject.h"

@implementation PropertyGridAppDelegate

@synthesize window, tableView;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification 
{
	// Insert code here to initialize your application 
	propertyReflector = [[PropertyReflector alloc] initWithTableView:tableView];
	[propertyReflector setReflectedObject:[MyTestObject new]];
}

@end
