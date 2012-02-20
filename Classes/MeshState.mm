//
//  MeshState.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 11/22/09.
//  For license see LICENSE.TXT
//

#import "MeshState.h"

@implementation MeshState

@synthesize itemIndex;

- (id)initWithMesh:(Mesh *)mesh itemIndex:(uint)index
{
	self = [super init];
	if (self)
	{
		itemIndex = index;
		vertices = new vector<Vector3D>();
        texCoords = new vector<Vector3D>();
		triangles = new vector<Triangle>();
		selection = new vector<bool>();
        
        mesh->mesh->toIndexRepresentation(*vertices, *texCoords, *triangles);
        mesh->mesh->getSelection(*selection);
        
		selectionMode = [mesh selectionMode];
	}
	return self;
}

- (void)dealloc
{
	delete vertices;
	delete triangles;
	delete selection;
}

- (void)applyToMesh:(Mesh *)mesh
{
    mesh->mesh->setSelectionMode(selectionMode);
    mesh->mesh->fromIndexRepresentation(*vertices, *texCoords, *triangles);
    mesh->mesh->setSelection(*selection);
}

@end

