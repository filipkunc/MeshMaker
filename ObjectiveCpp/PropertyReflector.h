//
//  PropertyReflector.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/15/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#import "CachedProperty.h"

// This class uses runtime information to get all properties
// of any NSObject descendant.
// I used information from 
// http://developer.apple.com/mac/library/documentation/cocoa/Conceptual/ObjCRuntimeGuide/

// PropertyReflector implements NSTableViewDataSource and NSTableViewDelegate for 
// providing similar editation capabilities like .NET PropertyGrid class
// http://msdn.microsoft.com/en-us/library/system.windows.forms.propertygrid.aspx

#ifdef MAC_OS_X_VERSION_10_6
@interface PropertyReflector : NSObject <NSTableViewDataSource, NSTableViewDelegate>
#else
@interface PropertyReflector : NSObject
#endif
{
	id reflectedObject;
	NSMutableArray *cachedProperties;
	NSTableView *tableView;
}

@property (readwrite, retain) id reflectedObject;

- (id)initWithTableView:(NSTableView *)aTableView;
- (CachedProperty *)propertyAtIndex:(int)index;
- (id)propertyValueAtIndex:(int)index;
- (void)setPropertyValue:(id)value atIndex:(int)index;

@end
