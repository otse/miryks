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
		void SetAnim(const char *);
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
		void SetAnim(const char *);
		void Step();
		void Place(const char *);
	};
}