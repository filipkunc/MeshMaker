//
//  MeshManipulationState.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/17/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "MathDeclaration.h"
#import "Mesh.h"

@interface MeshManipulationState : NSObject 
{
	uint itemIndex;
	vector<Vector3D> *vertices;
	vector<BOOL> *selected;
	enum MeshSelectionMode selectionMode;
}

@property (readonly, assign) uint itemIndex;

- (id)initWithMesh:(Mesh *)mesh itemIndex:(uint)index;
- (void)applyManipulationToMesh:(Mesh *)mesh;

@end
