#ifndef GLOOM_ACTOR_H
#define GLOOM_ACTOR_H

// part of gloom

#include <dark/dark.h>

#include <dark/skyrim/object.h>

namespace dark
{
	extern Keyframes *draugrAttack;
	extern Keyframes *humanIdle, *humanForward, *humanLeft, *humanRight, *humanBack;

	Record *GetRace(const char *);
	//void Export(const char *);
	void ExportRaceHkxToKf(const char *);
	Keyframes *GetKeyframes(const char *);

	class BodyPart
	{
	public:
		BodyPart(const char *, const char *);
		Mesh *mesh = nullptr;
		SkinnedMesh *smesh = nullptr;
		Skeleton *skeleton = nullptr;
		Animation *animation = nullptr;
		DrawGroup *drawGroup = nullptr;
		void step();
		void PutDown(const char *);
	};

	class BZZ
	{
		
	};

	class Human
	{
		BodyPart *hat, *head, *body, *hands, *feet;
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
		bool thirdPerson = false;
		vec3 pose;
		float yaw;
		Human *human;
		DrawGroup *drawGroup = nullptr;
		ViewerCamera *thirdPersonCamera = nullptr;
		Player();
		void toggleView();
		void step();
		void move();
	};

} // namespace dark

#endif