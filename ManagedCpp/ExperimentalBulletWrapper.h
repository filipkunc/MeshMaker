/*
 *  ExperimentalBulletWrapper.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 04/02/10.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../PureCpp/Enums.h"
#include "OpenGLManipulatingModel.h"
//#include "MarshalHelpers.h"

#include "btBulletFile.h"
#include "Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletCollision/BroadphaseCollision/btDispatcher.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "LinearMath/btIDebugDraw.h"
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
		//NSLog(@"%s", warningString); 
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

class BulletWrapperHelper
{
public:
	btDefaultCollisionConfiguration *collisionConfiguration;
	btCollisionDispatcher *dispatcher;
	btDbvtBroadphase *broadphase;
	btSequentialImpulseConstraintSolver *solver;
	btDynamicsWorld *dynamicsWorld;
	btBulletWorldImporter *worldImporter;
	ExperimentalDebugDrawImplementation *debugDrawer;

	BulletWrapperHelper(const char *fileName)
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		broadphase = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, -10, 0));	
		worldImporter = new btBulletWorldImporter(dynamicsWorld);
		debugDrawer = new ExperimentalDebugDrawImplementation();

		if (worldImporter->loadFile(fileName))
		{
			dynamicsWorld->setDebugDrawer(debugDrawer);
			//return true;
		}
	}

	~BulletWrapperHelper()
	{
		delete collisionConfiguration;
		delete dispatcher;
		delete broadphase;
		delete solver;
		delete dynamicsWorld;
		delete worldImporter;
		delete debugDrawer;
	}
};

#pragma managed

namespace ManagedCpp
{
	public ref class ExperimentalBulletWrapper : OpenGLManipulatingModel
	{
	private:
		BulletWrapperHelper *wrapper;
	public:
		ExperimentalBulletWrapper(String ^fileName);
		void DebugDraw();

		virtual property uint Count { uint get(); }
		virtual	CocoaBool IsSelected(uint index);
		virtual	void SetSelected(CocoaBool selected, uint index);
		virtual	void Draw(uint index, CocoaBool forSelection, ViewMode mode);
		virtual	void CloneSelected();
		virtual	void RemoveSelected();
		
		//@optional
		virtual	void WillSelect();
		virtual	void DidSelect();
	};
}