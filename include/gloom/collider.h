#ifndef GLOOM_COLLIDER_H
#define GLOOM_COLLIDER_H

#include <Gloom/Dark2.h>

//#include <Gloom/Skeleton.h>

namespace gloom
{
	class Collider
	{
	public:
		Collider();

        bool doo;

		static void MoveTo(Collider *, glm::vec3 tendency);

	};
}

#endif