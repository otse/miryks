#ifndef GLOOM_ACTOR_H
#define GLOOM_ACTOR_H

// part of gloom

#include <Gloom/Gloom.h>

#include "object.h"

namespace gloom
{
	extern Keyframes *draugrAttack;
	extern Keyframes *humanIdle;

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

		void step();

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

		void step();

		void Place(const char *);
	};

	class Player
	{
	public:
		vec3 pose;
		float yaw;
		Player();
		Human *human;
		DrawGroup *drawGroup = nullptr;
		
		ViewerCamera *thirdPersonCamera = nullptr;

		bool thirdPerson = false;
		void toggleView();
		

		void step();
		void move();
	};

} // namespace gloom

#endif