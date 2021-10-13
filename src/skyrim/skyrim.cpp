#include <skyrim/skyrim.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>
#include <skyrim/cell.h>

namespace skyrim {

	char *editme;

	// Grup &Grup::end = Grup();

	Record GetRace(const char *raceId)
	{
		Record race;
		Grup top = esp_top(get_plugins()[0], "RACE");
		top.loop([&](Grup<> &g) {
			Record record = g.record();
			if (record.editor_id(raceId))
			{
				race = record;
				return true;
			}
			return false;
		}, Top);
		assertc(race.valid());
		return race;
	}
	
}