#include <skyrim/skyrim.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>
#include <skyrim/cell.h>

namespace skyrim {

	char *editme;

	record get_race(const char *raceId)
	{
		closure < record_with_id > capture ((void *)raceId);
		grup< races > (0) / capture;
		return capture.last;
	}
	
}