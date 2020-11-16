#include <gloom/libs>

#include <gloom/actor.h>
#include <gloom/skeleton.h>

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

	Actor::Actor(const char *raceId)
	{
		printf("actor of race %s\n", raceId);

		Object race = GetRace(raceId);

		// cassert(race.Count("ANAM") == 2, "race count anam");

		auto ANAM = race.Get<char *>("ANAM", 0);

		cassert(ANAM, "no actor race anam sub");

		Skeleton *skelly = new Skeleton();
		
		skelly->Load(ANAM);
		skelly->Construct();

		if (ANAM)
			printf("race anam nif %s\n", ANAM);
	}

} // namespace gloom