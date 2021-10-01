#include <skyrim/skyrim.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>

namespace skyrim {

	char *editme;

	Record get_race(const char *raceId)
	{
		Record race;
		Grup array;
		GRUP top = esp_top_grup(get_plugins()[0], "RACE");
		array(top).foreach([&](unsigned int &i) {
			Record object = array.get_record(i);
			if (object.editor_id(raceId))
			{
				race = object.rcd;
				return true;
			}
			return false;
		}, 0);
		assertc(race.valid());
		return race;
	}

	Record get_world_space(const char *editorId) {
		return Record();
	}

	CellCapture get_interior_cell(const char *editorId, int plugin)
	{
		CellCapture cell;
		Grup a, b, c;
		GRUP top = esp_top_grup(get_plugins()[plugin], "CELL");
		a(top).foreach([&](unsigned int i) {
		return b(a.get_grup(i)).foreach([&](unsigned int j) {
		return c(b.get_grup(j)).foreach([&](unsigned int &k) {
			Record wrcd = c.get_record(k);
			Grup wgrp = c.get_grup(++k);
			if (wrcd.editor_id(editorId))
			{
				cell.wrcd = wrcd;
				cell.persistent = wgrp.get_grup(0);
				cell.temporary = wgrp.get_grup(1);
				return true;
			}
			return false;
		}, 3);}, 2);}, 0);
		return cell;
	}
	
}