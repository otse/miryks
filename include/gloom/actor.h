#ifndef GLOOM_ACTOR_H
#define GLOOM_ACTOR_H

#include <Gloom/Dark2.h>

#include "object.h"

namespace gloom
{
	extern KeyFrames *draugrAttack;
	extern KeyFrames *humanIdle;

	Record *GetRace(const char *);
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

	class BZZ
	{
		
	};

	class Human
	{
		Actor *hat, *head, *body, *hands, *feet;

	public:
		CSphere *csphere;
		Group *group;
		DrawGroup *drawGroup = nullptr;
		
		Human();

		void Step();

		void Place(const char *);
	};

	class Player
	{
	public:
		Human *human;
		DrawGroup *drawGroup = nullptr;
		
		Player();

		void Step();
	};

} // namespace gloom

#endif