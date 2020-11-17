#ifndef GLOOM_ACTOR_H
#define GLOOM_ACTOR_H

#include <gloom/dark2.h>

#include "object.h"

namespace gloom
{
	Object GetRace(const char *);
	//void Export(const char *);
	void ExportRaceBehavior(const char *);

	class Actor
	{
	public:
		Actor(const char *);


	};

} // namespace gloom

#endif