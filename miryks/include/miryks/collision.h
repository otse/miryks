#pragma once

#include <opengl/scene_graph.hpp>

#include "btBulletDynamicsCommon.h"

namespace miryks
{
	namespace collision
	{
		class base
		{
		public:
			btRigidBody *rigidBody;
			base() {}
			btTransform get_world_transform();
		};
		class orb : public base
		{
		public:
			btCollisionShape *colShape;
			orb(GroupDrawer *);
		};
		class solid : public base
		{
		public:
			btTriangleMesh *triangleMesh;
			btCollisionShape *collisionShape;
			solid(GroupDrawer *);
		};
		extern btDefaultCollisionConfiguration *collisionConfiguration;
		extern btCollisionDispatcher *dispatcher;
		extern btSequentialImpulseConstraintSolver *solver;
		extern btDiscreteDynamicsWorld *dynamicsWorld;
		extern btAlignedObjectArray<btCollisionShape *> collisionShapes;
		void init();
		void simulate();
		btVector3 glm_to_bt(vec3 &);
		vec3 bt_to_glm(btVector3 &);
	}
}