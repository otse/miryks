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
	Interior *dungeon = nullptr;

	Interior *GetInterior(const char *interiorId, int plugin)
	{
		Interior *interior = nullptr;
		printf("get interior\n");
		Grup top = esp_top(get_plugins()[plugin], "CELL");
		top.dive([&](Grup<> &g) {
			Record cell = g.get_record();
			Grup<> grup = g.next().get_grup();
			if (cell.editor_id(interiorId))
			{
				interior = new Interior(cell, grup);
				return true;
			}
			return false;
		}, {
			Top,
			InteriorCellBlock,
			InteriorCellSubBlock
		});
		return interior;
	}

	Interior::~Interior()
	{
		Unload();
	}

	void Interior::Init()
	{
		Subgroup(persistent, CellPersistentChildren);
		Subgroup(temporary, CellTemporaryChildren);
	}

	void Interior::Unload()
	{
		for (Reference *ref : refs)
			delete ref;
	}
	
	auto LABELS = { Doors, Furniture, Books, Containers, Armor, Weapons, Ammo, Misc, Alchemy, Ingredients };

	void Interior::Subgroup(Grup<> grupw, int group_type)
	{
		if (!grupw.valid())
			return;
		grupw.loop([&](Grup<> &g) {
			Record refr = g.get_record();
			if (refr.is_type(REFR))
			{
				Reference *reference = new Reference(refr);
				refs.push_back(reference);
				const char *edId = refr.editor_id();
				if (edId)
					edIds.emplace(edId, reference);
				if (reference->baseObject.valid())
				{
					if (reference->baseObject.is_types( LABELS ))
						Refs::labelled.push_back(reference);
					else if (reference->baseObject.is_type( MSTT ))
						mstts.push_back(reference);
				}
			}
			return false;
		}, group_type);
		put_cam_on_random_xmarker();
	}
	
	void Interior::put_cam_on_random_xmarker()
	{
		if (dontTeleport)
			return;
		if (!persistent.valid())
			return;
		persistent.loop([&](Grup<> &g) {
			Record record = g.get_record();
			if (*(record.base()) == 0x0000003B)
			{
				// printf("found random xmarker for camera\n");
				float *locationalData = record.data<float *>("DATA");
				personCam->pos = cast_vec3(locationalData);
				personCam->pos.z += EYE_HEIGHT;
				personCam->yaw = cast_vec3(locationalData + 3).z;
				dontTeleport = true;
				return true;
			}
			return false;
		});
	}
	
	void Interior::Update()
	{
		Refs::Nearby();

		for (Reference *mstt : mstts)
			mstt->model->Misty();
	}
}