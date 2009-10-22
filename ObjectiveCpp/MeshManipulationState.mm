//
//  MeshManipulationState.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MeshManipulationState.h"

@implementation MeshManipulationState

- (id)initWithMesh:(Mesh *)mesh
{
	self = [super init];
	if (self)
	{
		vertices = new vector<Vector3D>(*mesh->vertices);
	}
	return self;
}

- (void)dealloc
{
	delete vertices;
	[super dealloc];
}

- (void)applyManipulationToMesh:(Mesh *)mesh
{
	*(mesh->vertices) = *vertices;
}

@end
