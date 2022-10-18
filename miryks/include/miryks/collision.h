#pragma once

#include <opengl/scene_graph.hpp>

#include "btBulletDynamicsCommon.h"

#if 0
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#endif

namespace miryks
{
	namespace collision
	{
		class base
		{
		public:
			btCollisionShape *colShape = nullptr;
			btRigidBody *rigidBody = nullptr;
			base() {}
			btTransform get_world_transform();
			void set_position(btVector3);
			void remove();
		};
		class box : public base
		{
		public:
			box(record *, float [3], float [3]);
		};
		class orb : public base
		{
		public:
			orb(GroupDrawer *);
		};
		class capsule : public base
		{
		public:
			float half = 15;
			float height = 100;
			capsule(GroupDrawer *);
			void step();
			void gravitate();
		};
		class solid : public base
		{
		public:
			btTriangleMesh *triangleMesh;
			solid(GroupDrawer *);
		};
		extern btDefaultCollisionConfiguration *collisionConfiguration;
		extern btCollisionDispatcher *dispatcher;
		extern btSequentialImpulseConstraintSolver *solver;
		extern btDiscreteDynamicsWorld *dynamicsWorld;
		extern btAlignedObjectArray<btCollisionShape *> collisionShapes;
		void init();
		void simulate(float);
		btVector3 glm_to_bt(vec3 &);
		vec3 bt_to_glm(btVector3 &);
	}
}