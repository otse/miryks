#pragma once

#include <skyrim/model.h>
#include <skyrim/grup.h>

namespace skyrim
{
	class SKCreature
	{
	public:
		SKModelSkinned *modelSkinned;
		SKSkinnedMesh *skinnedMesh;
		SKSkeleton *skeleton;
		SKAnimation *animation;
		DrawGroup *drawGroup;
		SKCreature(const char *, const char *);
		void Place(const char *);
		void SetAnimation(const char *);
		void Step();
		SKRecord race;
	};

	class SKCharacter
	{
	public:
		SKRecord race;
		SKSkeleton *skeleton;
		SKAnimation *animation;
		SKSkinnedMesh *hat, *head, *body, *hands, *feet;
		DrawGroup *drawGroup;
		SKCharacter(const char * = "ImperialRace");
		void SetAnimation(const char *);
		void Step();
		void Place(const char *);
	};

	class Player : public SKCharacter
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