#include <miryks/miryks.hpp>

#include <miryks/collision.h>

namespace miryks
{
	namespace collision
	{
		std::vector<base *> objects;

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
			// dynamicsWorld->addRigidBody(body);

			// new orb();
		}

		void base::remove()
		{
			//printf("going to remove collision object\n");
			if (rigidBody)
			{
				dynamicsWorld->removeRigidBody(rigidBody);
				delete rigidBody->getCollisionShape();
				delete rigidBody;
				delete this;
			}
			return;
			int i;
			for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
			{
				btCollisionObject *obj = dynamicsWorld->getCollisionObjectArray()[i];
				dynamicsWorld->removeCollisionObject(obj);
				delete obj;
			}
		}

		void simulate(float delta)
		{
			dynamicsWorld->stepSimulation(delta, 10);

			/*for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
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
			}*/

			for (auto base : objects)
			{
				base->step();
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
			// if (drawer->target->collision)
			visit_group_geometry(collector, drawer->target, drawer->matrix);
		}

		void start_visit_dynamic(triangle_collector &collector, GroupDrawer *drawer)
		{
			if (!drawer->target)
				return;
			visit_group_geometry(collector, drawer->target, mat4(1.0));
		}

		void visit_group_geometry(triangle_collector &collector, Group *group, mat4 left)
		{
			if (!group->collision)
			{
				// printf("group %s has no collision\n", group->name.c_str());
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
					vec_to_vec(t.a),
					vec_to_vec(t.b),
					vec_to_vec(t.c));
			}

			// add empty triangle or crash
			// triangleMesh->addTriangle(btVector3(0.f, 0.f, 0.f), btVector3(0.f, 0.f, 0.f), btVector3(0.f, 0.f, 0.f));

			colShape = new btBvhTriangleMeshShape(triangleMesh, true);
			collisionShapes.push_back(colShape);

			btScalar mass(0.);

			btTransform startTransform;
			startTransform.setIdentity();

			btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);

			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, btVector3(0, 0, 0));
			rigidBody = new btRigidBody(rbInfo);
			rigidBody->setFriction(btScalar(1.0f));

			dynamicsWorld->addRigidBody(rigidBody);
		}

		void movable_box::step()
		{
			auto bound = dynamic_cast<GroupBounded *>(drawer->target);
			btTransform trans;
			rigidBody->getMotionState()->getWorldTransform(trans);
			quat q = quat_to_quat(trans.getRotation());
			vec3 vec = vec_to_vec(trans.getOrigin());
			mat4 matrix;
			mat4 translation = glm::translate(mat4(1.0f), vec);
			mat4 rotation = mat4(q);
			//mat4 scale = glm::scale(mat4(1.0), vec3(drawer->scale));
			matrix = translation * rotation;// * scale;
			// todo we cancel scale like this
			matrix = translate(matrix, -bound->aabb.center());
			//drawer->matrix[3] = vec4(vec_to_vec(trans.getOrigin()), 1);
			drawer->matrix = matrix;
			drawer->UpdateSideways();
		}

		base::~base()
		{
			vector_safe_remove<base *>(this, objects);
		}

		movable_box::movable_box(GroupDrawer *drawer)
		{
			auto bound = dynamic_cast<GroupBounded *>(drawer->target);

			if (!bound)
				return;

			Aabb &aabb = bound->aabb;
			this->drawer = drawer;
			objects.push_back(this);

			vec3 halfExtents = aabb.max - aabb.center();

			btVector3 vec = btVector3(halfExtents.x, halfExtents.y, halfExtents.z);
			colShape = new btBoxShape(vec);

			collisionShapes.push_back(colShape);

			btScalar mass(0.1f);

			bool isDynamic = mass != 0.f;

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				colShape->calculateLocalInertia(mass, localInertia);

			btTransform startTransform;
			startTransform.setIdentity();

			vec3 start = vec3(drawer->matrix[3]);
			quat qua = quat(drawer->matrix);
			start += bound->aabb.center();
			startTransform.setOrigin(vec_to_vec(start));
			startTransform.setRotation(quat_to_quat(qua));

			btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);

			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
			rigidBody = new btRigidBody(rbInfo);
			rigidBody->setFriction(btScalar(0.5f));
			// rigidBody->setGravity(btVector3(0, 0, 0));
			rigidBody->setDamping(btScalar(0.2f), btScalar(0.0f));

			dynamicsWorld->addRigidBody(rigidBody);
		}

		btVector3 vec_to_vec(vec3 vec)
		{
			btVector3 btVec;
			btVec[0] = vec[0];
			btVec[1] = vec[1];
			btVec[2] = vec[2];
			return btVec;
		}

		vec3 vec_to_vec(btVector3 btVec)
		{
			vec3 vec;
			vec[0] = btVec[0];
			vec[1] = btVec[1];
			vec[2] = btVec[2];
			return vec;
		}

		btQuaternion quat_to_quat(quat a)
		{
			btQuaternion b;
			b[0] = a[0];
			b[1] = a[1];
			b[2] = a[2];
			b[3] = a[3];
			return b;
		}

		quat quat_to_quat(btQuaternion a)
		{
			quat b;
			b[0] = a[0];
			b[1] = a[1];
			b[2] = a[2];
			b[3] = a[3];
			return b;
		}

		mat3 mat3_to_mat3(btMatrix3x3 a)
		{
			mat3 b = mat3(1.0);
			b[0] = vec_to_vec(a.getRow(0));
			b[3] = vec_to_vec(a.getRow(1));
			b[6] = vec_to_vec(a.getRow(2));
			/*b[1] = a[1];
			b[2] = a[2];
			b[3] = a[3];
			b[4] = a[4];
			b[5] = a[5];
			b[6] = a[6];
			b[7] = a[7];
			b[8] = a[8];*/
			return b;
		}

		void base::set_position(btVector3 vec)
		{
			btTransform trans;
			trans = rigidBody->getWorldTransform();
			trans.setOrigin(vec);
			rigidBody->setWorldTransform(trans);
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
			startTransform.setOrigin(vec_to_vec(vec));

			// using motionstate is recommended, it provides interpolation capabilities,
			// and only synchronizes 'active' objects
			btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
			rigidBody = new btRigidBody(rbInfo);
			rigidBody->setFriction(btScalar(1.0));

			dynamicsWorld->addRigidBody(rigidBody);
		}

		capsule::capsule()
		{
		}

		void capsule::set_position(vec3 in)
		{
			in += vec3(0, 0, half * 2 + height / 2);
			btVector3 convert = vec_to_vec(in);
			btTransform trans;
			trans = rigidBody->getWorldTransform();
			trans.setOrigin(convert);
			rigidBody->setWorldTransform(trans);
			rigidBody->activate();
		}

		void capsule::make(vec3 pos)
		{
			printf("new capsule \n");
			// create a dynamic rigidbody

			// btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
			colShape = new btSphereShape(btScalar(half * 2));
			colShape = new btCapsuleShape(half * 2, height);
			// colShape = new btBoxShape(btVector3(30., 30, 30));

			collisionShapes.push_back(colShape);

			/// Create Dynamic Objects
			btTransform startTransform;
			startTransform.setIdentity();
			btQuaternion q = startTransform.getRotation();

			btScalar mass(1.f);

			// rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				colShape->calculateLocalInertia(mass, localInertia);

			startTransform.setOrigin(vec_to_vec(pos));
			// Point shape up
			startTransform.setRotation(btQuaternion(1.0f, 0.f, 0.f, 1.0f));

			// using motionstate is recommended, it provides interpolation capabilities,
			// and only synchronizes 'active' objects
			btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, colShape, localInertia);
			rbInfo.m_startWorldTransform = startTransform;
			rigidBody = new btRigidBody(rbInfo);
			rigidBody->setFriction(btScalar(0.3f));
			rigidBody->setDamping(btScalar(0.99f), btScalar(0.0f));
			rigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

			dynamicsWorld->addRigidBody(rigidBody);
		}

		box::box(record *rc, float position[3], float bounds[3])
		{
			printf("new box\n");
			// vec3
			colShape = new btBoxShape(btVector3(bounds[0], bounds[1], bounds[2]));

			// collisionShapes.push_back(colShape);
			btTransform startTransform;
			startTransform.setIdentity();
			startTransform.setOrigin(btVector3(position[0], position[1], position[2]));
			btScalar mass(0.f);
			btVector3 localInertia(0, 0, 0);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, colShape, localInertia);
			rbInfo.m_startWorldTransform = startTransform;
			rigidBody = new btRigidBody(rbInfo);
			dynamicsWorld->addRigidBody(rigidBody);
		}

		void capsule::gravitate()
		{
			btVector3 down = btVector3(0, 0, -100);
			// rigidBody->applyCentralImpulse(down);

			btTransform trans;
			trans = rigidBody->getWorldTransform();
			btVector3 position = trans.getOrigin();

			btVector3 Start = position, End = (position - btVector3(0, 0, 21));

			btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
			// RayCallback.m_collisionFilterMask = FILTER_CAMERA;

			// Perform raycast
			dynamicsWorld->rayTest(Start, End, RayCallback);
			if (RayCallback.hasHit())
			{
				End = RayCallback.m_hitPointWorld;
			}
			else
			{
				rigidBody->applyCentralImpulse(down);
			}
		}

		void capsule::step()
		{
			return;
			// Keep shape straight
			btTransform trans;
			trans = rigidBody->getWorldTransform();
			btVector3 position = trans.getOrigin();
			auto shape = rigidBody->getCollisionShape();
			auto q = trans.getRotation();
			// printf("capsule start quaternion is %f %f %f %f\n", q.x(), q.y(), q.z(), q.w());
			trans.setRotation(btQuaternion(0.f, 0.f, 0.f, 1.f));
			rigidBody->setWorldTransform(trans);
			// rigidBody->setGravity(btVector3(0, 0, 0));

			// printf("capsule is %f %f %f\n", position.x(), position.y(), position.z());

			/*if (rigidBody->getMotionState())
			{
				rigidBody->getMotionState()->getWorldTransform(trans);
				trans.setRotation(btQuaternion(0.f, 0.f, 0.f, 1.f));
				rigidBody->getMotionState()->setWorldTransform(trans);
			}*/

			return;

			btVector3 Start = position, End = (position - btVector3(0, 0, 21));

			btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
			// RayCallback.m_collisionFilterMask = FILTER_CAMERA;

			// Perform raycast
			dynamicsWorld->rayTest(Start, End, RayCallback);
			if (RayCallback.hasHit())
			{
				// printf("hit");
				End = RayCallback.m_hitPointWorld;
				rigidBody->setGravity(btVector3(0, 0, 0));
				// Normal = RayCallback.m_hitNormalWorld;
			}
			else
			{
				rigidBody->setGravity(btVector3(0, 0, -1000));
				// btVector3 dir = btVector3(0, 0, -10);
				// rigidBody->applyCentralImpulse(dir);
			}
		}
	}
}