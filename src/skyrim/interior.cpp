#include <skyrim_units>

#include <skyrim/skyrim.h>
#include <skyrim/interior.h>
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
	static SKCell capture_cell(SKRecord wrcd, SKGrup wgrp)
	{
		SKCell cell;
		cell.wrcd = wrcd;
		cell.persistent = wgrp.get<grup *>(0);
		cell.temporary = wgrp.get<grup *>(1);
		return cell;
	}

	SKInterior::SKInterior(const char *edId)
	{
		this->edId = edId;
		Group *group = new Group();
	}

	SKInterior::~SKInterior()
	{
		Unload();
	}

	void SKInterior::Load()
	{
		printf("-- loading the dungeon --\n");
		cell = SkyrimGetCellInterior(edId, 5);
		Subgroup(cell.persistent, 8);
		Subgroup(cell.temporary, 9);
	}

	void SKInterior::Unload()
	{
		for (Ref *ref : refs)
			delete ref;
	}
	
	void SKInterior::Subgroup(SKGrup wgrp, int group_type)
	{
		auto showLabelsFor = {
			Doors,
			Furniture,
			Books,
			Containers,
			Armor,
			Weapons,
			Ammo,
			Misc,
			Alchemy,
			Ingredients
		};
		if (!wgrp.valid())
			return;
		wgrp.foreach([&](unsigned int i) {
			SKRecord wrcd = wgrp.get<RCD>(i);
			if (wrcd.sig(REFR))
			{
				Ref *ref = new Ref(wrcd.rcd);
				refs.push_back(ref);
				const char *edId = wrcd.editorId();
				if (edId)
					edIds.emplace(edId, ref);
				if (ref->baseObject.valid())
				{
					if (ref->baseObject.sigany( showLabelsFor ))
						Refs::labelled.push_back(ref);
					else if (ref->baseObject.sig( MSTT ))
						mstts.push_back(ref);
				}
			}
			return false;
		}, group_type);
		put_cam_on_random_xmarker();
	}
	
	void SKInterior::put_cam_on_random_xmarker()
	{
		if (alreadyTeleported)
			return;
		SKGrup wgrp = cell.persistent;
		wgrp.foreach([&](unsigned int i) {
			SKRecord wrcd = wgrp.get<RCD>(i);
			if (*(wrcd.base()) == 0x0000003B)
			{
				// printf("found random xmarker for camera\n");
				float *locationalData = wrcd.data<float *>("DATA");
				personCam->pos = cast_vec3(locationalData);
				personCam->pos.z += EYE_HEIGHT;
				personCam->yaw = cast_vec3(locationalData + 3).z;
				alreadyTeleported = true;
				return true;
			}
			return false;
		});
	}

	

	void SKInterior::Update()
	{
		Refs::Nearby();

		for (Ref *mstt : mstts)
			mstt->model->Misty();
	}

} // namespace dark