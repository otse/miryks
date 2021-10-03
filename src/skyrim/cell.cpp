#include <skyrim_units>

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
	Interior *ginterior = nullptr;

	Cell get_interior_cell(const char *editorId, int plugin)
	{
		// todo figure out how to chain this javascript-style
		Cell cell;
		GRUP top = esp_top_grup(get_plugins()[plugin], "CELL");
		Grup A, B, C;
		A(top).loop([&](unsigned int i) {
		return B(A.get_grup(i)).loop([&](unsigned int j) {
		return C(B.get_grup(j)).loop([&](unsigned int &k) {
			Record recordw = C.get_record(k);
			Grup grupw = C.get_grup(++k);
			if (recordw.editor_id(editorId))
			{
				cell.wrcd = recordw;
				cell.persistent = grupw.get_grup(0);
				cell.temporary = grupw.get_grup(1);
				return true;
			}
			return false;
		}, 3);}, 2);}, 0);
		return cell;
	}

	Record get_world_space(const char *editorId)
	{
		return Record();
	}

	Interior::Interior(const char *edId) : edId(edId)
	{
		// Group *group = new Group();
	}

	Interior::~Interior()
	{
		Unload();
	}

	void Interior::Load()
	{
		printf("-- loading the ginterior --\n");
		cell = get_interior_cell(edId, 5);
		Subgroup(cell.persistent, 8);
		Subgroup(cell.temporary, 9);
	}

	void Interior::Unload()
	{
		for (Ref *ref : refs)
			delete ref;
	}
	
	auto LABELS = { Doors, Furniture, Books, Containers, Armor, Weapons, Ammo, Misc, Alchemy, Ingredients };

	void Interior::Subgroup(Grup grupw, int group_type)
	{
		if (!grupw.valid())
			return;
		grupw.loop([&](unsigned int &i) {
			Record recordw = grupw.get_record(i);
			if (recordw.is_type(REFR))
			{
				Ref *ref = new Ref(recordw.rcd);
				refs.push_back(ref);
				const char *edId = recordw.editor_id();
				if (edId)
					edIds.emplace(edId, ref);
				if (ref->baseObject.valid())
				{
					if (ref->baseObject.is_types( LABELS ))
						Refs::labelled.push_back(ref);
					else if (ref->baseObject.is_type( MSTT ))
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
		Grup grupw = cell.persistent;
		grupw.loop([&](unsigned int i) {
			Record recordw = grupw.get_record(i);
			if (*(recordw.base()) == 0x0000003B)
			{
				// printf("found random xmarker for camera\n");
				float *locationalData = recordw.data<float *>("DATA");
				personCam->pos = cast_vec3(locationalData);
				personCam->pos.z += EYE_HEIGHT;
				personCam->yaw = cast_vec3(locationalData + 3).z;
				alreadyTeleported = true;
				return true;
			}
			return false;
		});
	}

	

	void Interior::Update()
	{
		Refs::Nearby();

		for (Ref *mstt : mstts)
			mstt->model->Misty();
	}

	Exterior::Exterior()
	{
        Load();
	}

	Exterior::~Exterior()
	{
		Unload();
	}

	void Exterior::Load()
	{
		
	}

	void Exterior::Unload()
	{
        
	}
} // namespace dark