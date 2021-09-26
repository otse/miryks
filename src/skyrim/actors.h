#pragma once

#include <skyrim/model.h>
#include <skyrim/grup.h>

namespace skyrim
{
	class Monster
	{
	public:
		ModelSkinned *modelSkinned;
		SkinnedMesh *skinnedMesh;
		Skel *skel;
		Anim *anim;
		DrawGroup *drawGroup;
		Monster(const char *, const char *);
		void Place(const char *);
		void SetAnimation(const char *);
		void Step();
		Record race;
	};

	class Char
	{
	public:
		Record race;
		Skel *skel;
		Anim *anim;
		SkinnedMesh *hat, *head, *body, *hands, *feet;
		DrawGroup *drawGroup;
		Char(const char * = "ImperialRace");
		void SetAnimation(const char *);
		void Step();
		void Place(const char *);
	};

	class Player : public Char
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