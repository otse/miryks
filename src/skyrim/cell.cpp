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
	Interior *ginterior = nullptr;
	WorldSpace *gworldSpace = nullptr;

	Interior *getInterior(const char *interiorId, int plugin)
	{
		printf("get interior\n");
		Interior *interior = nullptr;
		Grup top = esp_top(get_plugins()[plugin], "CELL");
		top.loop([&](Grup &grup, unsigned int &i) {
			Record record = grup.get_record(i);
			Grup grup2 = grup.get_grup(++i);
			if (record.editor_id(interiorId)) {
				interior = new Interior(record, grup2);
				return true;
			}
			return false;
		}, 3, 2);
		return interior;
	}

	WorldSpace *getWorldSpace(const char *worldSpaceId, int plugin)
	{
		printf("get worldspace\n");
		WorldSpace *worldSpace = nullptr;
		Grup top = esp_top(get_plugins()[plugin], "WRLD");
		top.loop([&](Grup &grup, unsigned int &i) {
			Record record = grup.get_record(i);
			Grup grup2 = grup.get_grup(++i);
			if (record.editor_id(worldSpaceId)) {
				worldSpace = new WorldSpace(record, grup2);
				return true;
			} 
			return false;
		}, 0);
		return worldSpace;
	}

	Cell::Cell(Record r, Grup g) {
		record = r;
		persistent = g.get_grup(0);
		temporary = g.get_grup(1);
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

	void Interior::Subgroup(Grup grupw, int group_type)
	{
		if (!grupw.valid())
			return;
		grupw.loop([&](Grup &grup, unsigned int &i) {
			Record recordw = grupw.get_record(i);
			if (recordw.is_type(REFR))
			{
				Ref *ref = new Ref(recordw.rcd);
				refs.push_back(ref);
				const char *edId = recordw.editor_id();
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
		Grup grupw = persistent;
		grupw.loop([&](Grup &grup, unsigned int i) {
			Record recordw = grup.get_record(i);
			if (*(recordw.base()) == 0x0000003B)
			{
				// printf("found random xmarker for camera\n");
				float *locationalData = recordw.data<float *>("DATA");
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

	Exterior::Exterior()
	{
        Load();
	}

	Exterior::~Exterior()
	{
		Unload();
	}

	void Exterior::Load()
	{
		
	}

	void Exterior::Unload()
	{
        
	}
} // namespace dark