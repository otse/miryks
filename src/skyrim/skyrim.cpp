#include <skyrim/skyrim.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>

namespace skyrim {

	char *editme;

	SKRecord SkyrimGetRace(const char *raceId)
	{
		SKRecord race;
		SKGrup array;
		GRUP top = esp_top_grup(get_plugins()[0], "RACE");
		array(top).foreach([&](unsigned int &i) {
			SKRecord object = array.get<RCD>(i);
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

	SKCell SkyrimGetCellInterior(const char *edId, int plugin)
	{
		SKCell cell;
		SKGrup a, b, c;
		GRUP top = esp_top_grup(get_plugins()[plugin], "CELL");
		a(top).foreach([&](unsigned int i) {
		return b(a.get<GRUP>(i)).foreach([&](unsigned int j) {
		return c(b.get<GRUP>(j)).foreach([&](unsigned int &k) {
			SKRecord wrcd = c.get<RCD>(k);
			SKGrup wgrp = c.get<GRUP>(++k);
			if (wrcd.HasId(edId))
			{
				cell.wrcd = wrcd;
				cell.persistent = wgrp.get<GRUP>(0);
				cell.temporary = wgrp.get<GRUP>(1);
				return true;
			}
			return false;
		}, 3);}, 2);}, 0);
		return cell;
	}
	
}