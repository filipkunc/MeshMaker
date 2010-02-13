//
//  ExperimentalBulletWrapper.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/31/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <vector>
using namespace std;
#import "OpenGLManipulatingModel.h"

#import "btBulletFile.h"
#import "btBulletWorldImporter.h"
#import "btDynamicsWorld.h"
#import "btDiscreteDynamicsWorld.h"
#import "btDispatcher.h"
#import "btDefaultCollisionConfiguration.h"
#import "btDbvtBroadphase.h"
#import "btSequentialImpulseConstraintSolver.h"
#import "btIDebugDraw.h"
#import "btDefaultMotionState.h"
#import "GL_ShapeDrawerClone.h"
using namespace bParse;

class Transform
{
public:
	Vector3D position;
	Quaternion rotation;
	
	Transform(const btVector3& position, const btQuaternion& rotation)
	{
		this->position = Vector3D(position.x(), position.y(), position.z());
		this->rotation = Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w());
	}
	
	Matrix4x4 ToMatrix()
	{
		Matrix4x4 t, r;
		t.Translate(position);
		rotation.ToMatrix(r);
		return t * r;
	}
	
	btVector3 ToBulletVector3()
	{
		return btVector3(position.x, position.y, position.z);
	}
	
	btQuaternion ToBulletQuaternion()
	{
		return btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
	}
};

@interface ExperimentalBulletWrapper : NSObject <OpenGLManipulatingModelItem>
{
	btDefaultCollisionConfiguration *collisionConfiguration;
	btCollisionDispatcher *dispatcher;
	btDbvtBroadphase *broadphase;
	btSequentialImpulseConstraintSolver *solver;
	btDynamicsWorld *dynamicsWorld;
	btBulletWorldImporter *worldImporter;
	GL_ShapeDrawer *shapeDrawer;
	vector<BOOL> *selection;
	vector<Transform> *transforms;
}

@property (readonly, assign) btDynamicsWorld *dynamicsWorld;

- (id)initWithFileName:(NSString *)fileName;

@end
