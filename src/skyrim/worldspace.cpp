#include <skyrim_units>

#include <skyrim/skyrim.h>
#include <skyrim/cell.h>
#include <skyrim/model.h>
#include <skyrim/grup.h>

#include <algorithm>
#include <cctype>
#include <string>

#include <renderer/renderer.h>
#include <renderer/texture.h>
#include <renderer/camera.h>
#include <renderer/lights.h>

namespace skyrim
{
	WorldSpace *gworldSpace = nullptr;
	
	WorldSpace *GetWorldSpace(const char *id, int plugin)
	{
		WorldSpace *ws = nullptr;
		Grup top = esp_top(get_plugins()[plugin], "WRLD");
		top.loop([&](Grup<> &g) {
			Record wrld = g.get_record();
			Grup grup = g.next().get_grup();
			if (wrld.editor_id(id)) {
				ws = new WorldSpace(wrld, grup);
				return true;
			} 
			return false;
		}, 0);
		return ws;
	}

	void WorldSpace::Load()
	{
		printf("ws load\n");
		grupw.index = 2;
		grupw.dive(2, [&](Grup<> &g) {
			Record cell = g.get_record();
			Grup grup = g.next().get_grup();
			new Cell(cell, grup);
			return true;
		}, 5);
	}

}