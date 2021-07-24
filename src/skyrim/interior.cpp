#include <skyrim_units>
#include <lib.h>

#include "skyrim.h"
#include "interior.h"
#include "mesh.h"
#include "record.h"
#include "recordarray.h"

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
		parsegrup(loaded_cell, loaded_cell.persistent);
		parsegrup(loaded_cell, loaded_cell.non_persistent);
	}

	CELL Interior::getcell(const char *name)
	{
		#define TOP 0
		#define INTERIOR_CELL_BLOCK 2
		#define INTERIOR_CELL_SUB_BLOCK 3
		CELL cell;
		Objects top, block, subblock;
		bool stop = false;
		top(esp_top_grup(get_plugins()[1], __CELL__)).foreach([&](unsigned int i) {
			assert(top.hed().group_type == TOP);

			block(top.getgrup(i)).foreach([&](unsigned int j) {
				assert(block.hed().group_type == INTERIOR_CELL_BLOCK);

				subblock(block.getgrup(j)).foreach([&](unsigned int &k) {
					assert(subblock.hed().group_type == INTERIOR_CELL_SUB_BLOCK);

					Object object = subblock.getobject(k);
					ObjectArray D = subblock.getobjectarray(k + 1);
					const char *editorId = getEditorId(object);
					if (0 == strcmp(name, editorId))
					{
						cell.record = object.record;
						// printf("foreach found your interior `%s`\n", editorId);
						cell.persistent = D.amount() >= 1 ? D.getgrup(0) : 0;
						cell.non_persistent = D.amount() >= 2 ? D.getgrup(1) : 0;
						stop = true;
						return true;
					}
					k += 1;
					return false;
				});
				return stop;
			});
			return stop;
		});
		return cell;
	}

	static void PlaceCameraDud(Interior *);

	void Interior::parsegrup(CELL &cell, Grup *grup)
	{
		if (grup == nullptr)
			return;
		Objects array;
		array(grup).foreach([&](unsigned int i) {
			Object object = array.getobject(i);
			if (object.oftype(__REFR__))
			{
				Ref *ref = new Ref(object.record);
				refs.push_back(ref);
				const char *editorId = getEditorId(object);
				if (editorId)
					editorIds.emplace(editorId, ref);
				if (ref->baseObject.valid() && ref->baseObject.oftypeany({__WEAP__, __MISC__}))
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
		Objects array;
		array(loaded_cell.persistent).foreach([&](unsigned int i) {
			Object object = array.getobject(i);
			if (*getbaseid(object) == 0x0000003B) //  "Marker"
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