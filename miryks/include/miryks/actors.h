#pragma once

#include <miryks/miryks.hpp>
#include <miryks/sound.h>
#include <miryks/collision_forwards.h>
#include <miryks/model.h>

namespace miryks
{
	class Monster
	{
	public:
		Monster(const char *, const char *);
		record race;
		reference *start_marker;
		skeleton *skel;
		sound *alcoveexit;
		orientation orientation;
		SkinnedMesh *helmet2;
		ModelSkinned *modelSkinned;
		SkinnedMesh *skinnedMesh;
		collision::capsule *capsule;
		GroupDrawer *groupDrawer;
		void SetAnim(const char *);
		virtual void Place(const char *);
		virtual void Step();
	};

	class Draugr : public Monster
	{
	public:
		Draugr(const char *);
		SkinnedMesh *helmet;
		animation *idle;
		bool wearHelmet;
		virtual void Setup();
		virtual void Step();
		void SetupCollision();
	};

	class DraugrAlcove : public Draugr
	{
	public:
		DraugrAlcove(const char *);
		reference *end_marker;
		animation *alcove_idle;
		animation *alcove_wake;
		float sleeping;
		float transition;
		bool wake;
		virtual void Place(const char *);
		virtual void Setup();
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