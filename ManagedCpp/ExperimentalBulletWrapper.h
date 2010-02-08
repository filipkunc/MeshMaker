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
#include <vector>
using namespace std;
#include "../PureCpp/Enums.h"
#include "OpenGLManipulatingModel.h"

#include "btBulletFile.h"
#include "Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletCollision/BroadphaseCollision/btDispatcher.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "LinearMath/btIDebugDraw.h"
#include "LinearMath/btDefaultMotionState.h"
#include "Demos/OpenGL/GL_ShapeDrawer.h"
using namespace bParse;

class BulletWrapperHelper
{
public:
	btDefaultCollisionConfiguration *collisionConfiguration;
	btCollisionDispatcher *dispatcher;
	btDbvtBroadphase *broadphase;
	btSequentialImpulseConstraintSolver *solver;
	btDynamicsWorld *dynamicsWorld;
	btBulletWorldImporter *worldImporter;
	GL_ShapeDrawer *shapeDrawer;
	vector<CocoaBool> *selection;

	BulletWrapperHelper()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		broadphase = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, -10, 0));	
		worldImporter = new btBulletWorldImporter(dynamicsWorld);
		shapeDrawer = new GL_ShapeDrawer();
		selection = new vector<CocoaBool>();
	}

	bool LoadFile(const char *fileName)
	{
		if (worldImporter->loadFile(fileName))
		{
			for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
			{
				selection->push_back(NO);
			}
			return true;
		}
		return false;
	}

	void Draw(uint index, bool selected)
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
		
		if (selected)
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

	~BulletWrapperHelper()
	{
		delete collisionConfiguration;
		delete dispatcher;
		delete broadphase;
		delete solver;
		delete dynamicsWorld;
		delete worldImporter;
		delete shapeDrawer;
		delete selection;
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
		void StepSimulation(btScalar timeStep);

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