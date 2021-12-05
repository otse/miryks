#include <miryks/miryks.h>
#include <miryks/cell.h>
#include <miryks/model.h>
#include <miryks/grup.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <ranges>

#include <opengl/renderer.h>
#include <opengl/texture.h>
#include <opengl/camera.h>
#include <opengl/lights.h>

const char *cells = Cells;

namespace miryks
{
	Interior *dungeon = nullptr;

	Interior *get_interior(const char *cellId, int num)
	{
		auto match =
		(grup<0,
		grup<2,
		grup<3>>> (cells)).find_combo(cellId);
		return new Interior(match);
	}

	Interior::Interior(record_and_grup &rng) : Cell(rng.one, rng.two)
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
		Sift(temporary, cell_temporary_children);
		PutCam();
		return this;
	}

	void Interior::Unload()
	{
		for (Reference *ref : refs)
			delete ref;
	}

	auto thingsWithLabels = {Doors, Furniture, Books, Containers, Armor, Weapons, Ammo, Misc, Alchemy, Ingredients};

	struct maker : record
	{
		bool operator<=(
			grup_closure<maker> &rhs)
		{
			Interior *interior = (Interior *)rhs.pointer;
			if (this->is_type(REFR))
			{
				Reference *reference = new Reference(*this);
				interior->refs.push_back(reference);
				if (this->editor_id())
					interior->edIds.emplace(this->editor_id(), reference);
				if (reference->baseObject.valid())
				{
					if (reference->baseObject.is_types(thingsWithLabels))
						Refs::labelled.push_back(reference);
					else if (reference->baseObject.is_type(MSTT))
						interior->mstts.push_back(reference);
				}
			}
			return false;
		}
		using record::record; // needed for template inheritance
	};

	void Interior::Sift(grup<> &subgroup, int group_type)
	{
		//grup_closure<maker> make_records(this);

		subgroup.rewind();
		subgroup.perform<maker>(this);
	}

	inline vec2 cast_vec2(void *f) { return *reinterpret_cast<vec2 *>(f); }
	inline vec3 cast_vec3(void *f) { return *reinterpret_cast<vec3 *>(f); }
	inline vec4 cast_vec4(void *f) { return *reinterpret_cast<vec4 *>(f); }
	inline bvec4 cast_bvec4(void *u) { return *reinterpret_cast<bvec4 *>(u); }
	inline mat3 cast_mat3(void *f) { return *reinterpret_cast<mat3 *>(f); }
	inline mat4 cast_mat4(void *f) { return *reinterpret_cast<mat4 *>(f); }

	void Interior::PutCam()
	{
		for (auto ref : refs | std::views::reverse)
		{
			if (*ref->base() == 0x00000032) // coc marker heading
			{
				float *locationalData = ref->data<float *>("DATA");
				personCam->pos = cast_vec3(locationalData);
				personCam->pos.z += EYE_HEIGHT;
				personCam->yaw = cast_vec3(locationalData + 3).z;
				dontTeleport = true;
				break;
			}
		}
#if 0
		if (dontTeleport)
			return;
		if (!persistent.valid())
			return;
		closure<visitor> me;
		me.pointer = (void*)this;
		subgroup.rewind();
		subgroup(me);
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