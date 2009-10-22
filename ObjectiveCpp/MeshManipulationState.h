//
//  MeshManipulationState.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
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
