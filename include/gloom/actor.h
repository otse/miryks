#ifndef GLOOM_ACTOR_H
#define GLOOM_ACTOR_H

#include <gloom/dark2.h>

#include "object.h"

namespace gloom
{
	class Race : public Object {
		Race(::record *);

		
	};

	class Actor
	{
	public:
		Actor(const char *);

		const char *race;
	};

} // namespace gloom

#endif