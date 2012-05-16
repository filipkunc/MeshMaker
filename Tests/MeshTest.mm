//
//  MeshTest.mm
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

@end

@implementation MeshTest

- (void)testSimpleList
{
    SimpleList<int> *list = new SimpleList<int>();
    list->removeAll();
    
    for (int i = 0; i < 10; i++)
    {
        list->add(i);
    }
    
    for (SimpleNode<int> *node = list->begin(), *end = list->end(); node != end; node = node->next())
    {
        printf("%i\n", node->data());
    }
    
    list->removeAll();
    
    for (SimpleNode<int> *node = list->begin(), *end = list->end(); node != end; node = node->next())
    {
        printf("%i\n", node->data());
    }
}

- (void)testMakeEdges
{
	mesh = [[Mesh alloc] init];
	
	STAssertNotNil(mesh, @"mesh can't be nil");
	
	[mesh makeMeshWithType:MeshTypeCube steps:0];

	STAssertEquals([mesh vertexCount], 8U, @"vertexCount in cube must be equal to 8");
	STAssertEquals([mesh triangleCount], 12U, @"triangleCount in cube must be equal to 12");
	STAssertEquals([mesh edgeCount], 18U, @"edgeCount in cube must be equal to 18");
}

@end
