//
//  MeshTest.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/30/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MeshTest.h"


@implementation MeshTest

- (void)testMesh
{
	mesh = [[Mesh alloc] init];
	
	STAssertNotNil(mesh, @"mesh can't be nil");
	
	Vector3D expected = Vector3D(1, 2, 3);
	[mesh addVertex:expected];
	Vector3D actual = [mesh vertexAtIndex:0];
	
	STAssertTrue(actual == expected, @"addVertex or vertexAtIndex not working properly");
	
	[mesh release];
}

@end
