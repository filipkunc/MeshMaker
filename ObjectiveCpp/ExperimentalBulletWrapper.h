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
#import "GL_ShapeDrawer.h"
using namespace bParse;

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
}

@property (readonly, assign) btDynamicsWorld *dynamicsWorld;

- (id)initWithFileName:(NSString *)fileName;

@end
