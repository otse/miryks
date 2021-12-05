#pragma once

#include <miryks/model.h>
#include <miryks/grup.h>

namespace miryks
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
		void SetAnim(const char *);
		void Step();
		record race;
	};

	class Char
	{
	public:
		record race;
		Skel *skel;
		Anim *anim;
		SkinnedMesh *hat, *head, *body, *hands, *feet;
		DrawGroup *drawGroup;
		Char(const char * = "ImperialRace");
		void SetAnim(const char *);
		void Step();
		void Place(const char *);
	};
}