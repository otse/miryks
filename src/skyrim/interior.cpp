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

	Interior *getInterior(const char *interiorId, int plugin)
	{
		Interior *interior = nullptr;
		printf("get interior\n");
		Grup top = esp_top(get_plugins()[plugin], "CELL");
		top.dive(2, [&](Grup<> &g) {
			Record record = g.get_record();
			Grup grup = g.next().get_grup();
			if (record.editor_id(interiorId))
			{
				interior = new Interior(record, grup);
				return true;
			}
			return false;
		}, 3);
		return interior;
	}

	Interior::~Interior()
	{
		Unload();
	}

	void Interior::Load()
	{
		Subgroup(persistent, 8);
		Subgroup(temporary, 9);
	}

	void Interior::Unload()
	{
		for (Ref *ref : refs)
			delete ref;
	}
	
	auto LABELS = { Doors, Furniture, Books, Containers, Armor, Weapons, Ammo, Misc, Alchemy, Ingredients };

	void Interior::Subgroup(Grup<> grupw, int group_type)
	{
		if (!grupw.valid())
			return;
		grupw.loop([&](Grup<> &g) {
			Record record = g.get_record();
			if (record.is_type(REFR))
			{
				Ref *ref = new Ref(record.rcd);
				refs.push_back(ref);
				const char *edId = record.editor_id();
				if (edId)
					edIds.emplace(edId, ref);
				if (ref->baseObject.valid())
				{
					if (ref->baseObject.is_types( LABELS ))
						Refs::labelled.push_back(ref);
					else if (ref->baseObject.is_type( MSTT ))
						mstts.push_back(ref);
				}
			}
			return false;
		}, group_type);
		put_cam_on_random_xmarker();
	}
	
	void Interior::put_cam_on_random_xmarker()
	{
		if (alreadyTeleported)
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
				alreadyTeleported = true;
				return true;
			}
			return false;
		});
	}
	
	void Interior::Update()
	{
		Refs::Nearby();

		for (Ref *mstt : mstts)
			mstt->model->Misty();
	}
}