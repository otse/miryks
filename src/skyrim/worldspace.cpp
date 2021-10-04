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
	
	WorldSpace *getWorldSpace(const char *worldSpaceId, int plugin)
	{
		WorldSpace *worldSpace = nullptr;
		Grup top = esp_top(get_plugins()[plugin], "WRLD");
		top.loop([&](Grup<> &g) {
			Record record = g.get_record();
			Grup grup = g.next().get_grup();
			if (record.editor_id(worldSpaceId)) {
				worldSpace = new WorldSpace(record, grup);
				return true;
			} 
			return false;
		}, 0);
		return worldSpace;
	}

	void WorldSpace::Load()
	{
		grup.index = 2;
		grup.dive(2, [&](Grup<> &g) {
			
			return true;
		}, 2);
	}

}