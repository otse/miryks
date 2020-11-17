#include <gloom/libs>

#include <gloom/actor.h>
#include <gloom/skeleton.h>
#include <gloom/level.h>

#include <opengl/group.h>
#include <opengl/scene.h>

namespace gloom
{
	Object GetRace(const char *raceId)
	{
		grup *top = esp_get_top_grup(get_plugins()[0], "RACE");

		cassert(top, "no race top grup");

		for (size_t i = 0; i < top->mixed.size; i++)
		{
			if (strcmp(Object(top->mixed.records[i], 1).Get<char *>("EDID"), raceId) == 0)
				return Object(top->mixed.records[i], 0);
		}
		cassert(false, "GetRace");
		return Object(nullptr, 0);
	}

	void ExportRaceBehavior(const char *list) {
		
		/*
		pseudo section!


		*/

	}

	Actor::Actor(const char *raceId)
	{
		printf("actor of race %s\n", raceId);

		Object race = GetRace(raceId);

		// cassert(race.Count("ANAM") == 2, "race count anam");

		auto ANAM = race.Get<char *>("ANAM", 0);

		auto MODL = race.Get<unsigned short *>("MODL", 2);
		auto MODLtext = race.Get<char *>("MODL", 2);

		printf("MODL %u %s\n", *MODL, MODLtext);
		cassert(ANAM, "no actor race anam sub");

		Skeleton *skelly = new Skeleton();

		skelly->Load(ANAM);
		skelly->Construct();

		auto ref = dungeon->refEditorIDs.find("gloomdraugrskelly");

		if (ref != dungeon->refEditorIDs.end())
		{
			printf("make skelly drawgroup!\n");
			DrawGroup *drawGroup = new DrawGroup(skelly->baseBone->group, ref->second->matrix);

			scene->Add(drawGroup);
		}
	}

} // namespace gloom