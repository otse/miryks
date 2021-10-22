#include <skyrim/skyrim.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>
#include <skyrim/cell.h>

namespace skyrim {

	char *editme;

	record get_race(const char *raceId)
	{
		grup< 0, any >
			declaration;
		
		declaration.plugin = 0;
		declaration.top = "RACE";

		capture<record_with_id>
			find_race;

		find_race.pointer = (void *)raceId;

		declaration.at_any(find_race);

		return find_race.last;
	}
	
}