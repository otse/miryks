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
		#if 0
		Grup top = esp_top(get_plugins()[0], "RACE");
		top.loop([&](any &temp) {
			Record record = temp.u.r;
			if (record.editor_id(raceId))
			{
				race = record;
				return true;
			}
			return false;
		}, Top);
		assertc(race.valid());
		#endif
		return race;
	}
	
}