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

	Interior *get_interior(const char *interiorId, int num)
	{
		printf("get_interior\n");
		Interior *interior = nullptr;
		Grup top(CELL);
		top.dive([&](Grup<> &g) {
			Record cell = g.record();
			Grup<> grup = g.grup();
			if (cell.editor_id(interiorId))
			{
				printf("found interior\n");
				interior = new Interior(cell, grup);
				return true;
			}
			return false;
		}, {
			Top, InteriorCellBlock, InteriorCellSubBlock
		});
		if (!interior)
			printf("fatal can't find interior\n");
		return interior;
	}

	Interior::Interior(Record cell, Grup<> &grup) : Cell(cell, grup)
	{
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
		if (!subgroup.valid()) {
			return;
		}
		subgroup.loop([&](Grup<> &g) {
			Record refr = g.record();
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
			Record record = g.record();
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