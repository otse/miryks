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
		}, Top);
		return ws;
	}

	void WorldSpace::Init()
	{
		sceneDef->ambient = vec3(180.0 / 255.0);
		personCam->pos = vec3(0, 0, -2048.0);
		DiscoverAllCells();
		LoadExterior(0, 0);
	}

	void WorldSpace::DiscoverAllCells()
	{
		printf("DiscoverAllCells\n");
		grup.index = 2; // ignore first two world children
		grup.dive([&](Grup<> &g) {
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
				printf("loading exterior %i %i\n", x, y);
				exterior->Init();
			}
		}
	}

	void Exterior::Init()
	{
		Subgroup(persistent, CellPersistentChildren);
		Subgroup(temporary, CellTemporaryChildren);
	}
	
	void Exterior::Subgroup(Grup<> subgroup, int group_type)
	{
		if (!subgroup.valid()) {
			return;
		}
		subgroup.loop([&](Grup<> &g) {
			Record refr = g.get_record();
			if (refr.is_type(REFR))
			{
				Reference *reference = new Reference(refr);
				reference->cell = (Cell *)this;
				worldSpace->references.push_back(reference);
			}
			else if (refr.is_type("LAND"))
			{
				printf("this is land\n");
				assertc(land == nullptr);
				land = new Land(g.get_record());
				land->exterior = this;
			}
			return false;
		}, group_type);
	}

	Land::Land(Record land) : Record(land)
	{
		exterior = nullptr;
		printf("lets try make a land mesh\n");
	}

}