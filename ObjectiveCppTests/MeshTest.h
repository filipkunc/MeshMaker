//
//  MeshTest.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/30/09.
//  For license see LICENSE.TXT
//

#import <SenTestingKit/SenTestingKit.h>
#import "Mesh.h"

@interface MeshTest : SenTestCase 
{
	Mesh *mesh;
}

- (void)testMesh;
- (void)testMakeEdges;
- (void)testTurnEdges;
- (void)testArchivation;

@end
