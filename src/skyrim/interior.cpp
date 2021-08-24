#include <skyrim_units>

#include "skyrim.h"
#include "interior.h"
#include "mesh.h"
#include "grup.h"

#include <algorithm>
#include <cctype>
#include <string>

#include <renderer/types.h>
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

	Cell find_interior_cell(const char *edId, int plugin)
	{
		Cell cell;
		Grup a, b, c;
		grupp top = esp_top_grup(get_plugins()[plugin], Cells);
		a(top).foreach([&](unsigned int i) {
		return b(a.get<grup *>(i)).foreach([&](unsigned int j) {
		return c(b.get<grup *>(j)).foreach([&](unsigned int &k) {
			Record wrcd = c.get<record *>(k);
			Grup wgrp = c.get<grup *>(++k);
			if (wrcd.hasEditorId(edId)) {
				printf("interior found\n");
				cell = capture_cell(wrcd, wgrp);
				return true;
			}
			return false;
		}, 3);}, 2);}, 0);
		return cell;
	}

	Interior::Interior(const char *edId)
	{
		this->edId = edId;
		Group *group = new Group();
	}

	Interior::~Interior()
	{
		unload();
	}

	void Interior::load()
	{
		cell = find_interior_cell(edId, MY_ESP);
		subgroup(cell.persistent, 8);
		subgroup(cell.temporary, 9);
	}

	void Interior::unload()
	{
		for (Ref *ref : refs)
			delete ref;
	}
	
	void Interior::subgroup(Grup wgrp, int group_type)
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
						labels.push_back(ref);
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

	bool myfunction(Ref *l, Ref *r)
	{
		return l->getDistance() < r->getDistance();
	}

	void Interior::update()
	{
		std::vector<Ref *> closest = labels;
		std::sort(labels.begin(), labels.end(), myfunction);
		
		for (Ref *ref : closest)
			if (ref->displayAsItem())
				return;

		for (Ref *mstt : mstts)
			mstt->step();
	}

} // namespace dark