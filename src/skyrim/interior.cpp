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
		parsegrup(CellPersistentChildren, loaded_cell, loaded_cell.persistent);
		parsegrup(CellTemporaryChildren, loaded_cell, loaded_cell.temporary);
	}

	CELL Interior::getcell(const char *name)
	{
		CELL cell;
		Grup A, B, C, D;
		grup top = esp_top_grup(get_plugins()[1], "CELL");
		bool stop = false;
		A(top).foreach(0, [&](unsigned int i) {
		B(A.getgrup(i)).foreach(2, [&](unsigned int j) {
		C(B.getgrup(j)).foreach(3, [&](unsigned int &k) {
			record *rcd = C.get<record *>(k);
			grup *grp = C.get<grup *>(k + 1);
			Record obj = rcd;
			Grup arr = grp;
			if (obj.hasEditorId(name))
			{
				cell.record = obj.rcd;
				// printf("foreach found your interior `%s`\n", editorId);
				cell.persistent = arr.amount() >= 1 ? arr.getgrup(0) : 0;
				cell.temporary = arr.amount() >= 2 ? arr.getgrup(1) : 0;
				stop = true;
			}
			k += 1;
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
			record *rcd = array.get<record *>(i);
			Object obj = rcd;
			if (obj.oftype(REFR))
			{
				Ref *ref = new Ref(obj.rcd);
				refs.push_back(ref);
				const char *editorId = obj.editorId();
				if (editorId)
					editorIds.emplace(editorId, ref);
				if (ref->baseObject.valid() && ref->baseObject.oftypeany({WEAP, MISC}))
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
		array(loaded_cell.persistent).foreach(CellPersistentChildren, [&](unsigned int i) {
			Record object(array.getrecord(i));
			if (*object.base() == 0x0000003B) //  "Marker"
			{
				// Place at any XMarker
				float *locationalData = object.data<float *>(_DATA_);
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