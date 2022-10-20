#pragma once

#include <miryks/miryks.hpp>
#include <miryks/collision_forwards.h>
#include <miryks/model.h>

namespace miryks
{
	class Monster
	{
	public:
		record race;
		SkinnedMesh *helmet2;
		ModelSkinned *modelSkinned;
		SkinnedMesh *skinnedMesh;
		collision::capsule *capsule;
		skeleton *skel;
		float yaw;
		GroupDrawer *groupDrawer;
		Monster(const char *, const char *);
		void Place(const char *);
		void SetAnim(const char *);
		virtual void Step();
	};

	class Draugr : public Monster
	{
	public:
		animation *alcove_idle;
		SkinnedMesh *helmet;
		animation *idle;
		float sleeping;
		bool alcove;
		bool wake;
		bool wearHelmet;
		Draugr(const char *);
		void Setup();
		void SetupCollision();
		virtual void Step();
	};

	class Char
	{
	public:
		record race;
		skeleton *skel;
		animation *anim;
		SkinnedMesh *hat, *hair, *head, *body, *hands, *feet;
		GroupDrawer *groupDrawer;
		Char(const char * = "ImperialRace");
		void Place(const char *);
		void SetAnim(const char *);
		void Step();
	};
}