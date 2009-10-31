//
//  ItemManipulationState.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/5/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "Item.h"

@interface ItemManipulationState : NSObject
{
	uint itemIndex;
	Vector3D *position;
	Quaternion *rotation;
	Vector3D *scale;
}

@property (readonly) uint itemIndex;

- (id)initWithItem:(Item *)item index:(uint)index;
- (void)applyManipulationToItem:(Item *)item;

@end
