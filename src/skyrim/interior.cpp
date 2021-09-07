#include <skyrim_units>

#include "skyrim.h"
#include "interior.h"
#include "mesh.h"
#include "grup.h"

#include <algorithm>
#include <cctype>
#include <string>

#include <renderer/renderer.h>
#include <renderer/texture.h>
#include <renderer/camera.h>
#include <renderer/lights.h>

namespace skyrim
{
	static Cell capture_cell(Record wrcd, Grup wgrp)
	{
		Cell cell;
		cell.wrcd = wrcd;
		cell.persistent = wgrp.get<grup *>(0);
		cell.temporary = wgrp.get<grup *>(1);
		return cell;
	}

	Interior::Interior(const char *edId)
	{
		this->edId = edId;
		Group *group = new Group();
	}

	Interior::~Interior()
	{
		Unload();
	}

	void Interior::Load()
	{
		printf("-- loading the dungeon --\n");
		cell = skyrim_get_interior_cell(edId, MY_ESP);
		Subgroup(cell.persistent, 8);
		Subgroup(cell.temporary, 9);
	}

	void Interior::Unload()
	{
		for (Ref *ref : refs)
			delete ref;
	}
	
	void Interior::Subgroup(Grup wgrp, int group_type)
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
			Record wrcd = wgrp.get<record *>(i);
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
	
	void Interior::put_cam_on_random_xmarker()
	{
		if (alreadyTeleported)
			return;
		Grup wgrp = cell.persistent;
		wgrp.foreach([&](unsigned int i) {
			Record wrcd = wgrp.get<record *>(i);
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

	

	void Interior::Update()
	{
		Refs::Nearby();

		for (Ref *mstt : mstts)
			mstt->Step();
	}

} // namespace dark