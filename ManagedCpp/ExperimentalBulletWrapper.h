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
#include "../PureCpp/GL_ShapeDrawerClone.h"
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
	vector<Transform> *transforms;

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
		transforms = new vector<Transform>();
	}

	~BulletWrapperHelper()
	{
		delete collisionConfiguration;
		delete dispatcher;
		delete broadphase;
		delete solver;
		delete dynamicsWorld;
		delete worldImporter;
		delete selection;
		delete transforms;
	}

	bool LoadFile(const char *fileName)
	{
		if (worldImporter->loadFile(fileName))
		{
			for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
			{
				selection->push_back(NO);
				btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[i];
				btVector3 pos = colObj->getWorldTransform().getOrigin();
				btQuaternion quat = colObj->getWorldTransform().getRotation();
				transforms->push_back(Transform(pos, quat));
			}
			return true;
		}
		return false;
	}

	void Draw(uint index, bool selected)
	{
		btScalar m[16];
		
		btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
		btRigidBody *body = btRigidBody::upcast(colObj);
		
		if (body && body->getMotionState())
		{
			btDefaultMotionState *myMotionState = (btDefaultMotionState *)body->getMotionState();
			myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
		}
		else
		{
			colObj->getWorldTransform().getOpenGLMatrix(m);
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

	Vector3D GetPosition(uint index)
	{
		return transforms->at(index).position;
	}

	void SetPosition(Vector3D position, uint index)
	{
		Transform &transform = transforms->at(index);
		transform.position = position;
		//Matrix4x4 m = transform.ToMatrix();
		
		btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
		//colObj->getWorldTransform().setFromOpenGLMatrix(m.m);
		colObj->getWorldTransform().setOrigin(transform.ToBulletVector3());
	}

	Quaternion GetRotation(uint index)
	{
		return transforms->at(index).rotation;
	}

	void SetRotation(Quaternion rotation, uint index)
	{
		Transform &transform = transforms->at(index);
		transform.rotation = rotation;
		//Matrix4x4 m = transform.ToMatrix();
		
		btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
		//colObj->getWorldTransform().setFromOpenGLMatrix(m.m);
		colObj->getWorldTransform().setRotation(transform.ToBulletQuaternion());
	}
};

#pragma managed

namespace ManagedCpp
{
	public ref class ExperimentalBulletWrapper : OpenGLManipulatingModelItem
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

		virtual Vector3D GetPosition(uint index);
		virtual Quaternion GetRotation(uint index);
		virtual Vector3D GetScale(uint index);
		virtual void SetPosition(Vector3D position, uint index);
		virtual void SetRotation(Quaternion rotation, uint index);
		virtual void SetScale(Vector3D scale, uint index);
		virtual void MoveBy(Vector3D offset, uint index);
		virtual void RotateBy(Quaternion offset, uint index);
		virtual void ScaleBy(Vector3D offset, uint index);
	};
}