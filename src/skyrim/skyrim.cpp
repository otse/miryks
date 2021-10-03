#include <skyrim/skyrim.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>
#include <skyrim/cell.h>

namespace skyrim {

	char *editme;

	// Grup &Grup::end = Grup();

	Record get_race(const char *raceId)
	{
		Record race;
		GRUP top = esp_top(get_plugins()[0], "RACE");
		Grup(top).loop([&](Grup &grup, unsigned int i) {
			Record recordw = grup.get_record(i);
			if (recordw.editor_id(raceId))
			{
				race = recordw.rcd;
				return true;
			}
			return false;
		}, 0);
		assertc(race.valid());
		return race;
	}
	
}