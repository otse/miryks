#pragma once

#include <miryks/model.h>
#include <miryks/grup.h>

namespace miryks
{
	class Monster
	{
	public:
		record_copy race;
		ModelSkinned *modelSkinned;
		SkinnedMesh *skinnedMesh;
		Skel *skel;
		Anim *anim;
		drawgroup *drawGroup;
		Monster(const char *, const char *);
		void Place(const char *);
		void SetAnim(const char *);
		void Step();
	};

	class Char
	{
	public:
		record_copy race;
		Skel *skel;
		Anim *anim;
		SkinnedMesh *hat, *head, *body, *hands, *feet;
		drawgroup *drawGroup;
		Char(const char * = "ImperialRace");
		void SetAnim(const char *);
		void Step();
		void Place(const char *);
	};
}