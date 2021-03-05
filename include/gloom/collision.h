#ifndef GLOOM_COLLIDER_H
#define GLOOM_COLLIDER_H

// part of gloom

#include <Gloom/Gloom.h>

#include <btBulletDynamicsCommon.h>

//#include <Gloom/Skeleton.h>

namespace gloom
{
	void collision_init();
	void collision_simulate();
	void collision_cleanup();

	class CSphere
	{
	public:
		btCollisionShape *colShape;
		btRigidBody *body;
		CSphere(vec3);
		vec3 GetWorldTransform();
	};

	class CShape
	{
	public:
		btCollisionShape *colShape;
		btRigidBody *body;
		CShape(vec3);
	};

	class Collider
	{
	public:
		Collider();

		bool doo;

		//static void MoveTo(Collider *, glm::vec3 tendency);
	};
} // namespace gloom

#endif