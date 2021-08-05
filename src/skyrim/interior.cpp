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
		myCell = find_cell_loop(editorId);
		parsegrup(8, myCell, myCell.persistent);
		parsegrup(9, myCell, myCell.temporary);
	}

	static void PlaceCameraDud(Interior *);

	void Interior::parsegrup(int group_type, Cell &cell, Grup wgrp)
	{
		if (!wgrp.valid())
			return;
		wgrp.foreach(group_type, [&](unsigned int i) {
			Record wrcd = wgrp.get<record *>(i);
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
		Grup wgrp = myCell.persistent;
		wgrp.foreach(8, [&](unsigned int i) {
			Record wrcd = wgrp.get<record *>(i);
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