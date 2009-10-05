//
//  ItemManipulationState.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Item.h"

@interface ItemManipulationState : NSObject
{
	NSUInteger itemIndex;
	Vector3D *position;
	Quaternion *rotation;
	Vector3D *scale;
}

@property (readonly) NSUInteger itemIndex;

- (id)initWithItem:(Item *)item index:(NSUInteger)index;
- (void)applyManipulationToItem:(Item *)item;

@end
