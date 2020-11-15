#include <gloom/libs>

#include <gloom/actor.h>

namespace gloom
{
	Race::Race(::record *record) : Object(record)
	{
	}

	Actor::Actor(const char *race) : race(race)
	{
		printf("actor of race %s", race);

		grup *top = esp_get_top_grup(get_plugins()[0], "RACE");

		cassert(top, "no race top grup");

		//for (int i = 0; i < array->size; i++)
		//{
		//	Race *self = new Race();
		//}
	}
} // namespace gloom