#include <miryks/collision.h>

namespace miryks
{
	namespace collision
	{
		btDefaultCollisionConfiguration *collisionConfiguration;
		btCollisionDispatcher *dispatcher;
		btSequentialImpulseConstraintSolver *solver;
		btDiscreteDynamicsWorld *dynamicsWorld;
		btAlignedObjectArray<btCollisionShape *> collisionShapes;

		void init()
		{
			/// collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
			collisionConfiguration = new btDefaultCollisionConfiguration();

			/// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
			dispatcher = new btCollisionDispatcher(collisionConfiguration);

			/// btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
			btBroadphaseInterface *overlappingPairCache = new btDbvtBroadphase();

			/// the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
			solver = new btSequentialImpulseConstraintSolver;

			dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

			dynamicsWorld->setGravity(btVector3(0, 0, -10));

			btCollisionShape *groundShape = new btBoxShape(btVector3(btScalar(5000.), btScalar(5000.), btScalar(25.)));

			collisionShapes.push_back(groundShape);

			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(0, 0, 0));

			btScalar mass(0.);
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				groundShape->calculateLocalInertia(mass, localInertia);

			// using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState *myMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
			btRigidBody *body = new btRigidBody(rbInfo);

			// add the body to the dynamics world
			dynamicsWorld->addRigidBody(body);

			// new orb();
		}

		void simulate()
		{
			dynamicsWorld->stepSimulation(1.f / 60.f, 10);

			for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
			{
				btCollisionObject *obj = dynamicsWorld->getCollisionObjectArray()[j];
				btRigidBody *body = btRigidBody::upcast(obj);
				btTransform trans;
				if (body && body->getMotionState())
				{
					body->getMotionState()->getWorldTransform(trans);
				}
				else
				{
					trans = obj->getWorldTransform();
				}
			}
		}

		btTransform base::get_world_transform()
		{
			btRigidBody *body = rigidBody; // btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = rigidBody->getWorldTransform();
			}
			return trans;
		}

		struct triangle_collector
		{
			struct triangle
			{
				vec3 a, b, c;
			};
			std::vector<triangle> triangles;
		};

		void visit_group_geometry(triangle_collector &, Group *, mat4);

		void start_visit(triangle_collector &collector, GroupDrawer *drawer)
		{
			visit_group_geometry(collector, drawer->target, drawer->matrixWorld);
		}

		void visit_group_geometry(triangle_collector &collector, Group *group, mat4 left)
		{
			mat4 place = left * group->matrixWorld;
			if (group->geometry)
			{
				for (int i = 0; i < group->geometry->vertices.size(); i += 3)
				{
					auto a = place * vec4(group->geometry->vertices[i + 0].position, 1);
					auto b = place * vec4(group->geometry->vertices[i + 1].position, 1);
					auto c = place * vec4(group->geometry->vertices[i + 2].position, 1);
					collector.triangles.push_back({vec3(a), vec3(b), vec3(c)});
					printf("t");
				}
			}
			for (Group *child : group->childGroups)
				visit_group_geometry(collector, child, place);
		}

		solid::solid(GroupDrawer *drawer)
		{
			triangleMesh = new btTriangleMesh();

			triangle_collector collector;
			start_visit(collector, drawer);

			for (auto &t : collector.triangles)
			{
				//printf("add triangle\n");
				triangleMesh->addTriangle(
					glm_to_bt(t.a),
					glm_to_bt(t.b),
					glm_to_bt(t.c));
			}

			collisionShape = new btBvhTriangleMeshShape(triangleMesh, true);
			
			btTransform startTransform;
			startTransform.setIdentity();

			btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);

			btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, collisionShape, btVector3(0, 0, 0));
			rigidBody = new btRigidBody(rbInfo);
			rigidBody->setFriction(btScalar(0.95));

			dynamicsWorld->addRigidBody(rigidBody);
		}

		btVector3 glm_to_bt(vec3 &vec)
		{
			btVector3 btVec;
			btVec[0] = vec.x;
			btVec[1] = vec.y;
			btVec[2] = vec.z;
			return btVec;
		}

		vec3 bt_to_glm(btVector3 &btVec)
		{
			vec3 vec;
			vec.x = btVec[0];
			vec.y = btVec[1];
			vec.z = btVec[2];
			return vec;
		}

		orb::orb(GroupDrawer *drawer)
		{
			printf("new orb \n");
			// create a dynamic rigidbody

			// btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
			this->colShape = new btSphereShape(btScalar(1.));
			collisionShapes.push_back(colShape);

			/// Create Dynamic Objects
			btTransform startTransform;
			startTransform.setIdentity();

			btScalar mass(1.f);

			// rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				colShape->calculateLocalInertia(mass, localInertia);

			vec3 vec = vec3(drawer->matrix[3]);
			startTransform.setOrigin(glm_to_bt(vec));

			// using motionstate is recommended, it provides interpolation capabilities,
			// and only synchronizes 'active' objects
			btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
			this->rigidBody = new btRigidBody(rbInfo);
			rigidBody->setFriction(btScalar(0.5));

			dynamicsWorld->addRigidBody(rigidBody);
		}
	}
}