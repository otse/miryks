#pragma once

#include <dark/dark.h>

#include <skyrim/grup.h>

namespace dark
{
	extern Keyframes *draugrAttack;
	extern Keyframes *humanIdle, *humanForward, *humanLeft, *humanRight, *humanBack;

	crecordp GetRace(const char *);
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
		DrawGroup *draw_group = nullptr;
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
		DrawGroup *draw_group = nullptr;
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
		DrawGroup *draw_group = nullptr;
		ViewerCamera *thirdPersonCamera = nullptr;
		Player();
		void toggleView();
		void step();
		void move();
	};

}