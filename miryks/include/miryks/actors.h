#pragma once

#include <miryks/miryks.hpp>
#include <miryks/model.h>

namespace miryks
{
	class Monster
	{
	public:
		record race;
		ModelSkinned *modelSkinned;
		SkinnedMesh *skinnedMesh;
		skeleton *skel;
		animation *anim;
		DrawGroup *drawGroup;
		Monster(const char *, const char *);
		void Place(const char *);
		void SetAnim(const char *);
		void Step();
	};

	class Char
	{
	public:
		record race;
		skeleton *skel;
		animation *anim;
		SkinnedMesh *hat, *head, *body, *hands, *feet;
		DrawGroup *drawGroup;
		Char(const char * = "ImperialRace");
		void SetAnim(const char *);
		void Step();
		void Place(const char *);
	};
}