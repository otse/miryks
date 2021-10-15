#include <skyrim/skyrim.h>
#include <skyrim/cell.h>
#include <skyrim/model.h>
#include <skyrim/grup.h>
#include <skyrim/grupt.h>

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

	Interior *get_interior(const char *id, int num)
	{
		printf("get_interior %s\n", id);
		Interior *interior = nullptr;
		GRUP top = esp_top(get_plugins()[5], "CELL");
		printf("grup top 5, cell %i\n", top);
		record_and_grup needle(id);
		grup_down<grup_down<grup_down<record_and_grup, 3>, 2>, 0> find(top);
		find.call(&needle);
		return interior;
	}

	Interior::Interior(constellation &cons) : Cell(cons.self, cons.childs)
	{
		printf("woo");
		sceneDef->ambient = vec3(50.f / 255.f);
	}

	Interior::~Interior()
	{
		Unload();
	}

	Interior *Interior::Init()
	{
		Subgroup(persistent, CellPersistentChildren);
		Subgroup(temporary, CellTemporaryChildren);
		return this;
	}

	void Interior::Unload()
	{
		for (Reference *ref : refs)
			delete ref;
	}
	
	auto LABELS = { Doors, Furniture, Books, Containers, Armor, Weapons, Ammo, Misc, Alchemy, Ingredients };

	void Interior::Subgroup(Grup<> subgroup, int group_type)
	{
		#if 0
		if (!subgroup.valid()) {
			return;
		}
		subgroup.loop([&](any &temp) {
			Record refr = temp.u.r;
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
		#endif
	}
	
	void Interior::put_cam_on_random_xmarker()
	{
		#if 0
		if (dontTeleport)
			return;
		if (!persistent.valid())
			return;
		persistent.loop([&](any &temp) {
			Record record = temp.u.r;
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
		#endif
	}
	
	void Interior::Update()
	{
		Refs::Nearby();

		for (Reference *mstt : mstts)
			mstt->model->Misty();
	}
}