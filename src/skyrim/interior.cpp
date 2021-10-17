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

	Interior *get_interior(const char *id, int num)
	{
		Interior *interior = nullptr;
		grup<grup<grup<any, 3>, 2>, 0> constellation(Cells);
		record_and_grup boo(id);
		constellation(boo);
		interior = new Interior(boo);
		return interior;
	}

	Interior::Interior(record_and_grup &rng) : Cell(rng.bonnie, rng.clyde)
	{
		printf("persistent n: %i\n", persistent.mixed().size);
		printf("temporary n: %i\n", temporary.mixed().size);
		sceneDef->ambient = vec3(50.f / 255.f);
	}

	Interior::~Interior()
	{
		Unload();
	}

	Interior *Interior::Init()
	{
		//Sift(persistent, CellPersistentChildren);
		Sift(temporary, CellTemporaryChildren);
		return this;
	}

	void Interior::Unload()
	{
		for (Reference *ref : refs)
			delete ref;
	}

	auto LABELS = {Doors, Furniture, Books, Containers, Armor, Weapons, Ammo, Misc, Alchemy, Ingredients};

	struct make_references : any
	{
		record bonnie;
		Interior *interior;
		//make_references()
		//{
		//	// hint
		//	printf("hint\n");
		//}
		make_references(Interior *interior) : interior(interior)
		{
		}
		make_references(grup_wrapper &iterator)
		{
			bonnie = iterator.next_record();
		}
		bool operator()(make_references &target)
		{
			if (bonnie.is_type(REFR))
			{
				//printf("good");
				Reference *reference = new Reference(bonnie);
				//interior->refs.push_back(reference);
				//if (bonnie.editor_id())
				//	interior->edIds.emplace(bonnie.editor_id(), reference);
			}
			return false;
		}
	};

	void Interior::Sift(subgroup &subgroup, int group_type)
	{
		make_references boo(this);
		subgroup(boo);
#if 0
		if (!subgroup.valid()) {
			return;
		}
		subgroup.loop([&](any &temp) {
			Record refr = temp.u.r;
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
#endif
	}

	void Interior::put_cam_on_random_xmarker()
	{
#if 0
		if (dontTeleport)
			return;
		if (!persistent.valid())
			return;
		persistent.loop([&](any &temp) {
			Record record = temp.u.r;
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
#endif
	}

	void Interior::Update()
	{
		Refs::Nearby();

		for (Reference *mstt : mstts)
			mstt->model->Misty();
	}
}