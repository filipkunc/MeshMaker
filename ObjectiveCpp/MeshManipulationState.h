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
	vector<Vector3D> *vertices;
}

- (id)initWithMesh:(Mesh *)mesh;
- (void)applyManipulationToMesh:(Mesh *)mesh;

@end
