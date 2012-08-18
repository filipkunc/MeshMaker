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

- (id)initWithMesh:(Mesh2 *)mesh itemIndex:(uint)index
{
	self = [super init];
	if (self)
	{
		itemIndex = index;
		vertices = new vector<Vector3D>();
        texCoords = new vector<Vector3D>();
		triangles = new vector<TriQuad>();
		selection = new vector<bool>();
        
        mesh->toIndexRepresentation(*vertices, *texCoords, *triangles);
        mesh->getSelection(*selection);
        
		selectionMode = mesh->selectionMode();
	}
	return self;
}

- (void)dealloc
{
	delete vertices;
	delete triangles;
	delete selection;
}

- (void)applyToMesh:(Mesh2 *)mesh
{
    mesh->fromIndexRepresentation(*vertices, *texCoords, *triangles);
    mesh->setSelectionMode(selectionMode);
    mesh->setSelection(*selection);
}

@end

