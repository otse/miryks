#include <gloom/libs>

#include <gloom/actor.h>

namespace gloom
{
	Actor::Actor(const char *race) : race(race)
	{
		printf("actor of race %s", race);

		grup *top = esp_get_top_grup(get_plugins()[0], "RACE");

		cassert(top, "no race top grup");

		for (int i = 0; i < top->mixed.size; i++)
		{
			Object *object = new Object(top->mixed.records[i], 1);
			char *EDID = object->Get<char *>("EDID");
			printf("race entry record %s\n", EDID);
		}


		//for (int i = 0; i < array->size; i++)
		//{
		//	Race *self = new Race();
		//}
	}
} // namespace gloom