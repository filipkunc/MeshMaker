//
//  ExperimentalBulletWrapper.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/31/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
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
using namespace bParse;

class ExperimentalDebugDrawImplementation : public btIDebugDraw
{
private:
	int debugMode;
public:
	ExperimentalDebugDrawImplementation()
	{
		debugMode = DBG_MAX_DEBUG_DRAW_MODE;
	}
	
	virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
	{
		glColor3f(color.x(), color.y(), color.z());
		
		glBegin(GL_LINES);
		glVertex3f(from.x(), from.y(), from.z());
		glVertex3f(to.x(), to.y(), to.z());
		glEnd();
	}
	
	virtual void drawContactPoint(const btVector3 &PointOnB,
								  const btVector3 &normalOnB,
								  btScalar distance,
								  int lifeTime,
								  const btVector3 &color)
	{
		// ignored
	}
	
	virtual void reportErrorWarning(const char *warningString) 
	{ 
		NSLog(@"%s", warningString); 
	}
	
	virtual void draw3dText(const btVector3 &location, const char *textString)
	{ 
		// ignored
	}  
	
	virtual void setDebugMode(int debugMode) 
	{ 
		this->debugMode = debugMode; 
	}
	
	virtual int	getDebugMode() const 
	{ 
		return this->debugMode; 
	}
};

@interface ExperimentalBulletWrapper : NSObject <OpenGLManipulatingModel>
{
	btDefaultCollisionConfiguration *collisionConfiguration;
	btCollisionDispatcher *dispatcher;
	btDbvtBroadphase *broadphase;
	btSequentialImpulseConstraintSolver *solver;
	btDynamicsWorld *dynamicsWorld;
	btBulletWorldImporter *worldImporter;
	
	ExperimentalDebugDrawImplementation *debugDrawer;
}

@property (readonly, assign) btDynamicsWorld *dynamicsWorld;

- (id)initWithFileName:(NSString *)fileName;
- (void)debugDraw;

@end
