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
	Cell capture_cell(Record wrcd, Grup wgrp)
	{
		Cell cell;
		cell.wrcd = wrcd;
		cell.persistent = wgrp.get<grup *>(0);
		cell.temporary = wgrp.get<grup *>(1);
		return cell;
	}

	Cell find_cell_foreach(const char *name)
	{
		Cell cell;
		Grup a, b, c;
		grupp top = esp_top_grup(get_plugins()[MY_ESP], Cells);
		bool stop = false;
		a(top).foreach(0, [&](unsigned int i) {
		b(a.get<grup *>(i)).foreach(2, [&](unsigned int j) {
		c(b.get<grup *>(j)).foreach(3, [&](unsigned int &k) {
			Record wrcd = c.get<record *>(k);
			Grup wgrp = c.get<grup *>(++k);
			if (wrcd.hasEditorId(name)) {
				printf("foreach found\n");
				cell = capture_cell(wrcd, wgrp);
				stop = true;
			}
			return stop;
			});
			return stop;
			});
			return stop;
		});
		return cell;
	}

	Interior::Interior(const char *edId)
	{
		editorId = edId;
		Group *group = new Group();
	}

	void Interior::load()
	{
		cell = find_cell_foreach(editorId);
		subgroup(cell.persistent, 8);
		subgroup(cell.temporary, 9);
	}
	
	void Interior::subgroup(Grup wgrp, int group_type)
	{
		auto things = {
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
		wgrp.foreach(group_type, [&](unsigned int i) {
			Record wrcd = wgrp.get<record *>(i);
			if (wrcd.sig(REFR))
			{
				Ref *ref = new Ref(wrcd.rcd);
				refs.push_back(ref);
				const char *edId = wrcd.editorId();
				if (edId)
					editorIds.emplace(edId, ref);
				if (ref->baseObject.valid())
				{
					if (ref->baseObject.sigany( things ))
						labels.push_back(ref);
					else if (ref->baseObject.sig( MSTT ))
						mstts.push_back(ref);
				}
			}
			return false;
		});
		put_cam_on_random_xmarker();
	}
	
	// todo horrible
	void Interior::put_cam_on_random_xmarker()
	{
		if (alreadyTeleported)
			return;
		Grup wgrp = cell.persistent;
		wgrp.foreach(8, [&](unsigned int i) {
			Record wrcd = wgrp.get<record *>(i);
			if (*(wrcd.base()) == 0x0000003B)
			{
				float *locationalData = wrcd.data<float *>("DATA");
				// printf("found random xmarker for camera\n");
				personCam->pos = *cast_vec_3(locationalData);
				personCam->pos.z += EYE_HEIGHT;
				personCam->yaw = cast_vec_3(locationalData + 3)->z;
				alreadyTeleported = true;
				return true;
			}
			return false;
		});
	}
	
	Interior::~Interior()
	{
		unload();
	}

	void Interior::unload()
	{
		for (auto it = refs.begin(); it != refs.end(); ++it)
		{
			Ref *ref = *it;
			delete ref;
		}
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