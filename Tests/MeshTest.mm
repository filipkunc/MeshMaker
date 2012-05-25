//
//  MeshTest.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/30/09.
//  For license see LICENSE.TXT
//

#import <SenTestingKit/SenTestingKit.h>
#import "Mesh2.h"

@interface MeshTest : SenTestCase 
{
	Mesh2 *mesh;
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
	mesh = new Mesh2();
	
	STAssertTrue(mesh != NULL, @"mesh can't be NULL");
	
    mesh->makeCube();

	STAssertEquals(mesh->vertexCount(), 8, @"vertexCount in cube must be equal to 8");
	STAssertEquals(mesh->triangleCount(), 12, @"triangleCount in cube must be equal to 12");
	STAssertEquals(mesh->vertexEdgeCount(), 18, @"edgeCount in cube must be equal to 18");
}

@end
