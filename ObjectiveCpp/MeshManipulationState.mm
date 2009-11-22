//
//  MeshManipulationState.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/17/09.
//  For license see LICENSE.TXT
//

#import "MeshManipulationState.h"

@implementation MeshManipulationState

@synthesize itemIndex;

- (id)initWithMesh:(Mesh *)mesh itemIndex:(uint)index
{
	self = [super init];
	if (self)
	{
		itemIndex = index;
		vertices = new vector<Vector3D>(*mesh->vertices);
		selected = new vector<BOOL>(*mesh->selected);
		selectionMode = [mesh selectionMode];
	}
	return self;
}

- (void)dealloc
{
	delete vertices;
	delete selected;
	[super dealloc];
}

- (void)applyManipulationToMesh:(Mesh *)mesh
{
	*(mesh->vertices) = *vertices;
	*(mesh->selected) = *selected;
	mesh->selectionMode = selectionMode;
	[mesh makeMarkedVertices];
}

@end
