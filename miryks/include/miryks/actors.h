#pragma once

#include <miryks/miryks.hpp>
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
		skeleton *skel;
		animation *anim;
		GroupDrawer *groupDrawer;
		Monster(const char *, const char *);
		void Place(const char *);
		void SetAnim(const char *);
		virtual void Step();
	};

	class Draugr : public Monster
	{
	public:
		SkinnedMesh *helmet;
		bool wearHelmet;
		Draugr(const char *);
		void Setup();
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