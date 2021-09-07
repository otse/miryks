#include <skyrim/skyrim.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>

namespace skyrim {

	Record skyrim_get_race(const char *raceId)
	{
		Record race;
		Grup array;
		grupp top = esp_top_grup(get_plugins()[0], "RACE");
		array(top).foreach([&](unsigned int &i) {
			Record object = array.get<record *>(i);
			auto edId = object.editorId();
			if (strcmp(edId, raceId) == 0)
			{
				race = object.rcd;
				return true;
			}
			return false;
		}, 0);
		assertc(race.valid());
		return race;
	}

	Cell skyrim_get_interior_cell(const char *edId, int plugin)
	{
		Cell cell;
		Grup a, b, c;
		grupp top = esp_top_grup(get_plugins()[plugin], "CELL");
		a(top).foreach([&](unsigned int i) {
		return b(a.get<grup *>(i)).foreach([&](unsigned int j) {
		return c(b.get<grup *>(j)).foreach([&](unsigned int &k) {
			Record wrcd = c.get<record *>(k);
			Grup wgrp = c.get<grup *>(++k);
			if (wrcd.HasId(edId))
			{
				cell.wrcd = wrcd;
				cell.persistent = wgrp.get<grup *>(0);
				cell.temporary = wgrp.get<grup *>(1);
				return true;
			}
			return false;
		}, 3);}, 2);}, 0);
		return cell;
	}
	
}