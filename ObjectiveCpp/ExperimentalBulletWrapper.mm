//
//  ExperimentalBulletWrapper.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/31/10.
//  For license see LICENSE.TXT
//

#import "ExperimentalBulletWrapper.h"

@implementation ExperimentalBulletWrapper

- (id)initWithFileName:(NSString *)fileName
{
	self = [super init];
	if (self)
	{
		// from DemoApplication
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		broadphase = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, -10, 0));	
		worldImporter = new btBulletWorldImporter(dynamicsWorld);
		debugDrawer = new ExperimentalDebugDrawImplementation();
		
		if (worldImporter->loadFile([fileName cStringUsingEncoding:NSASCIIStringEncoding]))
		{
			dynamicsWorld->setDebugDrawer(debugDrawer);
			return self;
		}
		// loading failed
		[self dealloc];
		return nil;
	}
	return self;
}

- (void)dealloc
{
	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;
	delete solver;
	delete dynamicsWorld;
	delete worldImporter;
	delete debugDrawer;
	
	[super dealloc];	
}

- (void)debugDraw
{
	dynamicsWorld->debugDrawWorld();
}

#pragma mark OpenGLManipulatingModel implementation

- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection withMode:(enum ViewMode)mode
{
	if (!forSelection)
	{
		[self debugDraw];
	}
}

- (uint)count
{ 
	return 1U; 
}

- (void)removeSelected { }
- (void)cloneSelected { }
- (void)setSelected:(BOOL)selected atIndex:(uint)index { }
- (BOOL)isSelectedAtIndex:(uint)index { return NO; }

@end
