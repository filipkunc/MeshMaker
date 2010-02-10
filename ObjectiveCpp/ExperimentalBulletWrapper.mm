//
//  ExperimentalBulletWrapper.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/31/10.
//  For license see LICENSE.TXT
//

#import "ExperimentalBulletWrapper.h"

@implementation ExperimentalBulletWrapper

@synthesize dynamicsWorld;

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
		shapeDrawer = new GL_ShapeDrawer();
		selection = new vector<BOOL>();
		
		if (worldImporter->loadFile([fileName cStringUsingEncoding:NSASCIIStringEncoding]))
		{
			for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
			{
				selection->push_back(NO);
			}
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
	delete selection;
	[super dealloc];	
}

#pragma mark OpenGLManipulatingModel implementation

- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection withMode:(enum ViewMode)mode
{
	btScalar m[16];
	btMatrix3x3	rot;
	rot.setIdentity();
	
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	btRigidBody *body = btRigidBody::upcast(colObj);
	
	if (body && body->getMotionState())
	{
		btDefaultMotionState *myMotionState = (btDefaultMotionState *)body->getMotionState();
		myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
		rot = myMotionState->m_graphicsWorldTrans.getBasis();
	}
	else
	{
		colObj->getWorldTransform().getOpenGLMatrix(m);
		rot = colObj->getWorldTransform().getBasis();
	}
	btVector3 wireColor(1.0f, 1.0f, 0.5f); //wants deactivation
	if (index & 1) 
		wireColor = btVector3(0.0f, 0.0f, 1.0f);
	///color differently for active, sleeping, wantsdeactivation states
	if (colObj->getActivationState() == 1) //active
	{
		if (index & 1)
			wireColor += btVector3(1.0f, 0.0f, 0.0f);
		else
			wireColor += btVector3(0.5f, 0.0f, 0.0f);
	}
	if (colObj->getActivationState() == 2) //ISLAND_SLEEPING
	{
		if (index & 1)
			wireColor += btVector3(0.0f, 1.0f, 0.0f);
		else
			wireColor += btVector3(0.0f, 0.5f, 0.0f);
	}
	
	btVector3 aabbMin,aabbMax;
	dynamicsWorld->getBroadphase()->getBroadphaseAabb(aabbMin, aabbMax);
	
	aabbMin -= btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);
	aabbMax += btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);
	
	shapeDrawer->enableTexture(true);
	
	if (!forSelection && [self isSelectedAtIndex:index])
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor, 0, aabbMin, aabbMax);
		glDisable(GL_POLYGON_OFFSET_FILL);
		wireColor = btVector3(0.5f, 0.5f, 0.5f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor, 0, aabbMin, aabbMax);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	{
		shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor, 0, aabbMin, aabbMax);
	}
}

- (uint)count
{ 
	return (uint)dynamicsWorld->getNumCollisionObjects();
}

- (void)removeSelected 
{
	NSLog(@"removeSelected is not supported");
}

- (void)cloneSelected
{
	NSLog(@"cloneSelected is not supported");
}

- (void)setSelected:(BOOL)selected atIndex:(uint)index 
{
	selection->at(index) = selected;
}

- (BOOL)isSelectedAtIndex:(uint)index
{ 
	return selection->at(index);
}

#pragma mark OpenGLManipulatingModelItem implementation

- (Vector3D)positionAtIndex:(uint)index
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	btVector3 position = colObj->getWorldTransform().getOrigin();
	return Vector3D(position);
}

- (Quaternion)rotationAtIndex:(uint)index
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	btQuaternion rotation = colObj->getWorldTransform().getRotation();
	return Quaternion(rotation);
}

- (Vector3D)scaleAtIndex:(uint)index
{
	return Vector3D(1, 1, 1); // ignored
}

- (void)setPosition:(Vector3D)position atIndex:(uint)index
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	colObj->getWorldTransform().setOrigin(btVector3(position.x, position.y, position.z));
}

- (void)setRotation:(Quaternion)rotation atIndex:(uint)index
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	colObj->getWorldTransform().setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
}

- (void)setScale:(Vector3D)scale atIndex:(uint)index
{
	// ignored
}

- (void)moveByOffset:(Vector3D)offset atIndex:(uint)index
{
	Vector3D position = [self positionAtIndex:index];
	position += offset;
	[self setPosition:position atIndex:index];
}

- (void)rotateByOffset:(Quaternion)offset atIndex:(uint)index
{
	Quaternion rotation = [self rotationAtIndex:index];
	rotation = offset * rotation;
	[self setRotation:rotation atIndex:index];
}

- (void)scaleByOffset:(Vector3D)offset atIndex:(uint)index
{
	// ignored
}

@end
