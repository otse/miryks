#include <skyrim_units>
#include <lib.h>

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
	Interior::Interior(const char *edid)
	{
		editorId = edid;
		Group *group = new Group();
	}

	void Interior::loadcell()
	{
		loaded_cell = getcell(editorId);
		parsegrup(8, loaded_cell, loaded_cell.persistent);
		parsegrup(9, loaded_cell, loaded_cell.temporary);
	}

	CELL Interior::getcell(const char *name)
	{
		CELL cell;
		Grup A, B, C, D;
		grupp top = esp_top_grup(get_plugins()[3], "CELL");
		bool stop = false;
		A(top).foreach(0, [&](unsigned int i) {
		B(A.getgrup(i)).foreach(2, [&](unsigned int j) {
		C(B.getgrup(j)).foreach(3, [&](unsigned int &k) {
			Record wrcd = C.getrecord(k);
			Grup wgrp = C.getgrup(++k);
			if (wrcd.hasEditorId(name))
			{
				cell.record = wrcd.rcd;
				printf("foreach found your interior `%s`\n", name);
				cell.persistent = wgrp.mixed().size >= 1 ? wgrp.getgrup(0) : 0;
				cell.temporary = wgrp.mixed().size >= 2 ? wgrp.getgrup(1) : 0;
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

	static void PlaceCameraDud(Interior *);

	void Interior::parsegrup(int group_type, CELL &cell, cgrupp grp)
	{
		if (grp == nullptr)
			return;
		Grup wgrp;
		wgrp(grp).foreach(group_type, [&](unsigned int i) {
			Record wrcd = wgrp.getrecord(i);
			if (wrcd.sig(REFR))
			{
				Ref *ref = new Ref(wrcd.rcd);
				refs.push_back(ref);
				const char *editorId = wrcd.editorId();
				if (editorId)
					editorIds.emplace(editorId, ref);
				if (ref->baseObject.valid() && ref->baseObject.sigany({WEAP, MISC}))
				{
					iterables.push_back(ref);
				}
			}
			return false;
		});
		placecamera();
	}

	void parsegrup_no_wrapper(cgrupp grp)
	{
		for (int i = 0; i < grp->mixed->size; i++)
		{
			crecordp rcd = (crecordp)grp->mixed->elements[i];
			if (rcd->hed->sgn == *(unsigned int *)"REFR")
			{
				// make Ref
			}
		}
	}
	
	void Interior::placecamera()
	{
		if (alreadyTeleported)
			return;
		Grup wgrp;
		wgrp(loaded_cell.persistent).foreach(8, [&](unsigned int i) {
			Record wrcd = wgrp.getrecord(i);
			if (*(wrcd.base()) == 0x0000003B) //  "Marker"
			{
				// Place at any XMarker
				float *locationalData = wrcd.data<float *>(_DATA_);
				// printf(" xmarker ! \n");
				first_person_camera->pos = *cast_vec_3(locationalData);
				first_person_camera->pos.z += EYE_HEIGHT;
				first_person_camera->yaw = cast_vec_3(locationalData + 3)->z;
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
		std::vector<Ref *> closest = iterables;
		std::sort(iterables.begin(), iterables.end(), myfunction);

		for (auto it = closest.begin(); it != closest.end(); ++it)
		{
			Ref *ref = *it;

			if (ref->displayAsItem())
				return;
		}
	}

} // namespace dark