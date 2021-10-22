#include <skyrim/skyrim.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>
#include <skyrim/cell.h>

namespace skyrim {

	char *editme;

	struct check : record
	{
		bool operator()(
			capture<check> &capture)
		{
			return this->editor_id((const char *)capture.pointer);
		}
	};

	record get_race(const char *raceId)
	{
		grup< 0, any >
			declaration;
		
		declaration.plugin = 0;
		declaration.top = "RACE";

		capture<check>
			find_race((void *)raceId);

		declaration.at_any(find_race);

		return find_race.last;
	}
	
}