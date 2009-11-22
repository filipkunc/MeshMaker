//
//  MeshFullState.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 11/22/09.
//  For license see LICENSE.TXT
//

#import "MeshFullState.h"

@implementation MeshFullState

@synthesize itemIndex;

- (id)initWithMesh:(Mesh *)mesh itemIndex:(uint)index
{
	self = [super init];
	if (self)
	{
		itemIndex = index;
		vertices = new vector<Vector3D>(*mesh->vertices);
		triangles = new vector<Triangle>(*mesh->triangles);
		selected = new vector<BOOL>(*mesh->selected);
		selectionMode = [mesh selectionMode];
	}
	return self;
}

- (void)dealloc
{
	delete vertices;
	delete triangles;
	delete selected;
	[super dealloc];
}

- (void)applyFullToMesh:(Mesh *)mesh
{
	*(mesh->vertices) = *vertices;
	*(mesh->triangles) = *triangles;
	*(mesh->selected) = *selected;
	mesh->selectionMode = selectionMode;
	if (selectionMode == MeshSelectionModeEdges)
		[mesh makeEdges];
	[mesh makeMarkedVertices];
}

@end

