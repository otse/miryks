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
		grupp top = esp_top_grup(get_plugins()[1], "CELL");
		bool stop = false;
		A(top).foreach(0, [&](unsigned int i) {
		B(A.getgrup(i)).foreach(2, [&](unsigned int j) {
		C(B.getgrup(j)).foreach(3, [&](unsigned int &k) {
			record *rcd = C.get<record *>(k);
			grup *grp = C.get<grup *>(++k);
			Record Rcd = rcd;
			Grup Grp = grp;
			if (Rcd.hasEditorId(name))
			{
				cell.record = rcd;
				// printf("foreach found your interior `%s`\n", editorId);
				cell.persistent = Grp.size() >= 1 ? Grp.getgrup(0) : 0;
				cell.temporary = Grp.size() >= 2 ? Grp.getgrup(1) : 0;
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

	void Interior::parsegrup(int group_type, CELL &cell, const grup * grp)
	{
		if (grp == nullptr)
			return;
		Grup arr;
		arr(grp).foreach(group_type, [&](unsigned int i) {
			record *rcd = arr.get<record *>(i);
			Record Rcd = rcd;
			if (Rcd.sig(REFR))
			{
				Ref *ref = new Ref(Rcd.rcd);
				refs.push_back(ref);
				const char *editorId = Rcd.editorId();
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

	void Interior::placecamera()
	{
		if (alreadyTeleported)
			return;
		Grup array;
		array(loaded_cell.persistent).foreach(8, [&](unsigned int i) {
			record *rcd = array.get<record *>(i);
			Record Rcd = rcd;
			if (*(Rcd.base()) == 0x0000003B) //  "Marker"
			{
				// Place at any XMarker
				float *locationalData = Rcd.data<float *>(_DATA_);
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