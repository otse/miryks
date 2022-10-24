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
		void init();
		void simulate(float);

		btVector3 vec_to_vec(vec3);
		vec3 vec_to_vec(btVector3);

		btQuaternion quat_to_quat(quat);
		quat quat_to_quat(btQuaternion);

		btMatrix3x3 mat3_to_mat3(mat3);
		mat3 mat3_to_mat3(btMatrix3x3);

		class base;
		class box;
		class orb;
		class capsule;
		class movable_box;
		class solid;
		extern std::vector<base *> objects;
		class base
		{
		public:
			btCollisionShape *colShape = nullptr;
			btRigidBody *rigidBody = nullptr;
			base() {}
			~base();
			btTransform get_world_transform();
			void set_position(btVector3);
			void remove();
			virtual void step() {};
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
			float height = 80;
			bool active = true;
			capsule();
			void make(vec3);
			void set_position(vec3);
			void gravitate();
			virtual void step();// {};
		};
		class movable_box : public base
		{
		public:
			GroupDrawer *drawer = nullptr;
			movable_box(GroupDrawer *);
			virtual void step();
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
	}
}