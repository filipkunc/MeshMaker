//
//  MeshFullState.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 11/22/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "Mesh.h"

@interface MeshFullState : NSObject
{
	uint itemIndex;
	vector<Vector3D> *vertices;
	vector<Triangle> *triangles;
	vector<BOOL> *selected;
	enum MeshSelectionMode selectionMode;
}

@property (readonly, assign) uint itemIndex;

- (id)initWithMesh:(Mesh *)mesh itemIndex:(uint)index;
- (void)applyFullToMesh:(Mesh *)mesh;

@end
