//
//  ItemManipulationState.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/5/09.
//  For license see LICENSE.TXT
//

#import "ItemManipulationState.h"

@implementation ItemManipulationState

@synthesize itemIndex;

- (id)initWithItem:(Item *)item index:(uint)index;
{
	self = [super init];
	if (self)
	{
		position = new Vector3D(item->position);
		rotation = new Quaternion(item->rotation);
		scale = new Vector3D(item->scale);
		itemIndex = index;
	}
	return self;
}

- (void)dealloc
{
	delete position;
	delete rotation;
	delete scale;
}

- (void)applyManipulationToItem:(Item *)item
{
    item->position = *position;
    item->rotation = *rotation;
    item->scale = *scale;
    item->selected = true;
}

@end
