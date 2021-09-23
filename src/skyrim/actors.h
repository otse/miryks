#pragma once

#include <skyrim/mesh.h>
#include <skyrim/grup.h>

namespace skyrim
{
	class Creature
	{
	public:
		MeshSkinned *meshSkinned;
		SkinnedMesh *skinnedMesh;
		Skeleton *skeleton;
		Animation *animation;
		DrawGroup *drawGroup;
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
		Skeleton *skeleton;
		Animation *animation;
		SkinnedMesh *hat, *head, *body, *hands, *feet;
		DrawGroup *drawGroup;
		Character(const char * = "ImperialRace");
		void SetAnimation(const char *);
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