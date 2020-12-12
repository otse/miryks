#ifndef GLOOM_ACTOR_H
#define GLOOM_ACTOR_H

#include <gloom/dark2.h>

#include "object.h"

namespace gloom
{
	Object GetRace(const char *);
	//void Export(const char *);
	void ExportRaceHkxToKf(const char *);

	class Actor
	{
	public:
		Actor(const char *, const char *);

		Mesh *mesh = nullptr;
		SkinnedMesh *smesh = nullptr;
		Skeleton *skeleton = nullptr;
		Animation *animation = nullptr;

		DrawGroup *drawGroup = nullptr;
		
		float merry = 0;
		
		void Step();

		void PutDown(const char *);
	};

} // namespace gloom

#endif