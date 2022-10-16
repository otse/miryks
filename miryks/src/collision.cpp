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

			dynamicsWorld->setGravity(btVector3(0, 0, -1000));

			btCollisionShape *groundShape = new btBoxShape(btVector3(btScalar(5000.), btScalar(5000.), btScalar(25.)));

			collisionShapes.push_back(groundShape);

			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(0, 0, -50));

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
			//dynamicsWorld->addRigidBody(body);

			// new orb();
		}

		void simulate(float delta)
		{
			dynamicsWorld->stepSimulation(delta, 10);

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
			if (!drawer->target)
				return;
			//if (drawer->target->collision)
				visit_group_geometry(collector, drawer->target, drawer->matrix);
		}

		void visit_group_geometry(triangle_collector &collector, Group *group, mat4 left)
		{
			if (!group->collision) {
				//printf("group %s has no collision\n", group->name.c_str());
				return;
			}
			if (group->geometry)
			{
				mat4 place = left * group->matrixWorld;
				quat q = quat(place);
				for (int i = 0; i < group->geometry->elements.size(); i += 3)
				{
					auto a = group->geometry->vertices[group->geometry->elements[i + 0]];
					auto b = group->geometry->vertices[group->geometry->elements[i + 1]];
					auto c = group->geometry->vertices[group->geometry->elements[i + 2]];
					auto d = place * vec4(a.position, 1);
					auto e = place * vec4(b.position, 1);
					auto f = place * vec4(c.position, 1);
					collector.triangles.push_back({vec3(f), vec3(e), vec3(d)});
				}
			}
			for (Group *child : group->childGroups)
				visit_group_geometry(collector, child, left);
		}

		solid::solid(GroupDrawer *drawer)
		{
			triangleMesh = new btTriangleMesh();

			triangle_collector collector;

			start_visit(collector, drawer);

			if (!collector.triangles.size())
				return;

			for (auto &t : collector.triangles)
			{
				triangleMesh->addTriangle(
					glm_to_bt(t.a),
					glm_to_bt(t.b),
					glm_to_bt(t.c));
			}

			// add empty triangle or crash
			//triangleMesh->addTriangle(btVector3(0.f, 0.f, 0.f), btVector3(0.f, 0.f, 0.f), btVector3(0.f, 0.f, 0.f));

			collisionShape = new btBvhTriangleMeshShape(triangleMesh, true);

			btScalar mass(0.);

			btTransform startTransform;
			startTransform.setIdentity();

			btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);

			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, btVector3(0, 0, 0));
			rigidBody = new btRigidBody(rbInfo);
			rigidBody->setFriction(btScalar(1.0f));

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
			colShape = new btSphereShape(btScalar(30.));
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
			rigidBody->setFriction(btScalar(1.0));

			dynamicsWorld->addRigidBody(rigidBody);
		}

		capsule::capsule(GroupDrawer *drawer)
		{
			printf("new capsule \n");
			// create a dynamic rigidbody

			// btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
			//colShape = new btCapsuleShape(25, 100);
			colShape = new btSphereShape(btScalar(20));
			//colShape = new btBoxShape(btVector3(30., 30, 30));

			collisionShapes.push_back(colShape);

			/// Create Dynamic Objects
			btTransform startTransform;
			startTransform.setIdentity();
			btQuaternion q = startTransform.getRotation();
			printf("capsule start quaternion is %f %f %f %f\n", q.x(), q.y(), q.z(), q.w());

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
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, colShape, localInertia);
			rbInfo.m_startWorldTransform = startTransform;
			rigidBody = new btRigidBody(rbInfo);
			rigidBody->setFriction(btScalar(0.5f));
			rigidBody->setDamping(btScalar(0.99f), btScalar(0.99f));

			dynamicsWorld->addRigidBody(rigidBody);
		}

		void capsule::gravitate() {
			btVector3 down = btVector3(0, 0, -100);
			//rigidBody->applyCentralImpulse(down);

			btTransform trans;
			trans = rigidBody->getWorldTransform();
			btVector3 position = trans.getOrigin();

			btVector3 Start = position, End = (position - btVector3(0, 0, 21));

			btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
			//RayCallback.m_collisionFilterMask = FILTER_CAMERA;

			// Perform raycast
			dynamicsWorld->rayTest(Start, End, RayCallback);
			if(RayCallback.hasHit()) {
				End = RayCallback.m_hitPointWorld;
			}
			else
			{
				rigidBody->applyCentralImpulse(down);
			}
		}

		void capsule::step() {
			btTransform trans;
			trans = rigidBody->getWorldTransform();
			btVector3 position = trans.getOrigin();
			//trans.setRotation(btQuaternion(0.f, 0.f, 0.f, 1.f));
			//rigidBody->setWorldTransform(trans);
			rigidBody->setGravity(btVector3(0, 0, 0));

			//printf("capsule is %f %f %f\n", position.x(), position.y(), position.z());


			/*if (rigidBody->getMotionState())
			{
				rigidBody->getMotionState()->getWorldTransform(trans);
				trans.setRotation(btQuaternion(0.f, 0.f, 0.f, 1.f));
				rigidBody->getMotionState()->setWorldTransform(trans);
			}*/

			btVector3 Start = position, End = (position - btVector3(0, 0, 21));

			btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
			//RayCallback.m_collisionFilterMask = FILTER_CAMERA;

			// Perform raycast
			dynamicsWorld->rayTest(Start, End, RayCallback);
			if(RayCallback.hasHit()) {
				// printf("hit");
				End = RayCallback.m_hitPointWorld;
				rigidBody->setGravity(btVector3(0, 0, 0));
				//Normal = RayCallback.m_hitNormalWorld;
			}
			else
			{
				rigidBody->setGravity(btVector3(0, 0, -1000));
				//btVector3 dir = btVector3(0, 0, -10);
				//rigidBody->applyCentralImpulse(dir);
			}
		}
	}
}