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
			Grup<> grup = g.next().get_grup();
			if (wrld.editor_id(id)) {
				ws = new WorldSpace(wrld, grup);
				return true;
			} 
			return false;
		}, 0);
		return ws;
	}

	void WorldSpace::Init()
	{
		DiscoverAllCells();
		LoadExterior(0, 0);
	}

	void WorldSpace::DiscoverAllCells()
	{
		printf("ws load\n");
		grupw.index = 2;
		grupw.dive([&](Grup<> &g) {
			Record cell = g.get_record();
			Grup<> grup = g.next().get_grup();
			Exterior *exterior = new Exterior(cell, grup);
			exterior->worldSpace = this;
			exteriors.push_back(exterior);
			return false;
		}, {
			WorldChildren,
			ExteriorCellBlock,
			ExteriorCellSubBlock
		});
	}

	void WorldSpace::LoadExterior(int x, int y)
	{
		for (Exterior *exterior : exteriors)
		{
			if (exterior->xclc->x == x && exterior->xclc->y == y)
			{
				printf("loading exterior %i %i", x, y);
				exterior->Init();
			}
		}
	}

	void Exterior::Init()
	{
		//Subgroup(persistent, 8);
		//Subgroup(temporary, 9);
	}
	
	void Exterior::Subgroup(Grup<> grupw, int group_type)
	{
		if (!grupw.valid())
			return;
		grupw.loop([&](Grup<> &g) {
			Record refr = g.get_record();
			if (refr.is_type(REFR))
			{
				Reference *reference = new Reference(refr);
				worldSpace->references.push_back(reference);
			}
			return false;
		}, group_type);
	}

	Land::Land(Record land) : Record(land)
	{

	}

}