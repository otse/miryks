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

namespace miryks
{
	struct maker;

	struct maker : record
	{
		bool operator<=(grup_closure<maker> &target)
		{
			Interior *interior = (Interior *)target.some_value;
			if (this->is_reference())
			{
				Reference *reference = new Reference(*this);
				interior->refs.push_back(reference);
				if (this->editor_id())
					interior->edIds.emplace(this->editor_id(), reference);
				/*if (reference->baseObject.valid())
				{
					if (reference->baseObject.is_types(thingsWithLabels))
						Refs::labelled.push_back(reference);
					else if (reference->baseObject.is_type(MSTT))
						interior->mstts.push_back(reference);
				}*/
			}
			return false;
		}
	};
	
	Interior *dungeon = nullptr;

	Interior *get_interior(const char *cellId, int num)
	{
		return new Interior(get_interior_cell(cellId, num));
	}

	Interior::Interior(record_and_grup rng) : Cell(rng)
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
		run_struct_on_grup<maker>(temporary, (void *)this);
		PutCam();
		return this;
	}

	void Interior::Unload()
	{
		for (Reference *ref : refs)
			delete ref;
		refs.clear();
		Refs::labelled.clear();
		mstts.clear();
	}

	auto thingsWithLabels = {Doors, Furniture, Books, Containers, Armor, Weapons, Ammo, Misc, Alchemy, Ingredients};

	inline vec2 cast_vec2(void *f) { return *reinterpret_cast<vec2 *>(f); }
	inline vec3 cast_vec3(void *f) { return *reinterpret_cast<vec3 *>(f); }
	inline vec4 cast_vec4(void *f) { return *reinterpret_cast<vec4 *>(f); }
	inline bvec4 cast_bvec4(void *u) { return *reinterpret_cast<bvec4 *>(u); }
	inline mat3 cast_mat3(void *f) { return *reinterpret_cast<mat3 *>(f); }
	inline mat4 cast_mat4(void *f) { return *reinterpret_cast<mat4 *>(f); }

	void Interior::PutCam()
	{
		printf("putcam");
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
		//Refs::Nearby();

		//for (Reference *mstt : mstts)
		//	mstt->model->Misty();
	}
}