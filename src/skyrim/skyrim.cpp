#include <skyrim/skyrim.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>
#include <skyrim/cell.h>

namespace skyrim {

	char *editme;

	record get_race(const char *raceId)
	{
		closure < record_with_id > closure;
		(grup<>(races, 0)) <= (closure, raceId);
		return closure.match;
	}
	
}