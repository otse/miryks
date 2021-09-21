#pragma once

#include <skyrim/mesh.h>
#include <skyrim/grup.h>

namespace skyrim
{
	class Creature
	{
	public:
		MeshSkinned *meshSkinned = nullptr;
		SkinnedMesh *skinnedMesh = nullptr;
		Skeleton *skeleton = nullptr;
		Animation *animation = nullptr;
		DrawGroup *drawGroup = nullptr;
		Creature(const char *, const char *);
		void Place(const char *);
		void SetAnimation(const char *);
		void Step();
		Record race;
	};

	class Character
	{
	public:
		Record race;
		Skeleton *skeleton = nullptr;
		SkinnedMesh *hat, *head, *body, *hands, *feet;
		DrawGroup *drawGroup = nullptr;
		Character(const char * = "ImperialRace");
		void Step();
		void Place(const char *);
	};

	class Player : public Character
	{
	public:
		bool thirdPerson = false;
		vec3 pose;
		float yaw;
		DrawGroup *drawGroup = nullptr;
		ViewerCamera *thirdPersonCamera = nullptr;
		Player();
		void toggleView();
		void Step();
		void move();
	};
}