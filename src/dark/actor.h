#pragma once

#include <dark/dark.h>

#include <skyrim/grup.h>

namespace dark
{
	class Actor
	{
	public:
		SkinnedMesh *skinnedMesh;
		Skeleton *skeleton;
		Animation *animation;
		DrawGroup *drawGroup;
		Record race;
		Actor(const char *, const char *);
		void Step();
		void Place(const char *);
	};

	class Human
	{
		BodyPart *hat, *head, *body, *hands, *feet;
	public:
		Group *group;
		DrawGroup *drawGroup = nullptr;
		Human();
		void Step();
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
		void Step();
		void move();
	};

}