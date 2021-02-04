#include <Gloom/Interior.h>
#include <Gloom/Mesh.h>
#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

#include <libs>

#include <algorithm>
#include <cctype>
#include <string>

#include <opengl/renderable.h>
#include <opengl/texture.h>
#include <opengl/camera.h>
#include <opengl/pointlight.h>
#include <opengl/types.h>

namespace gloom
{
	Interior::Interior(const char *edid)
	{
		Group *group = new Group();
		Cell cell = GetCell(edid);
		LoadCell(cell);
	}

	void Interior::LoadCell(Cell &cell)
	{
		loadedCell = cell;
		ParseGrup(cell, cell.persistent);
		ParseGrup(cell, cell.non_persistent);
	}

	Cell Interior::GetCell(const char *name)
	{
		Cell cell;

		Grup *top = esp_get_top_grup(get_plugins()[1], "CELL");

		bool stop = false;

		Objects(top).ForEach(0, stop, [&](Objects &oa, size_t i) {
			Objects(oa.GetGrup(i)).ForEach(0, stop, [&](Objects &oa, size_t j) {
				Objects(oa.GetGrup(j)).ForEach(0, stop, [&](Objects &oa, size_t &k) {
					Object object(oa.GetRecord(k));
					Grup *grup = oa.GetGrup(k + 1);
					const char *editorId = object.Get<const char *>("EDID", 0);
					if (0 == strcmp(name, editorId))
					{
						Objects C(grup);
						cell.cell = object.record;
						printf("ForEach found your interior `%s`\n", editorId);
						if (C.Size() >= 1)
							cell.persistent = C.GetGrup(0);
						if (C.Size() >= 2)
							cell.non_persistent = C.GetGrup(1);
						stop = true;
					}
					k += 1;
				});
			});
		});

		return cell;
	}

	static void PlaceCamera(Interior *);

	void Interior::ParseGrup(Cell &cell, Grup *grup)
	{
		if (grup == nullptr)
			return;

		bool stop = false;

		Objects(grup).ForEach(0, stop, [&](Objects &oa, size_t i) {
			Record *record = oa.GetRecord(i);
			Object object(record);
			cassert(object.Type("REFR"), "fus ro dah");
			Ref *ref = new Ref(record);
			refs.push_back(ref);
			const char *editorId = object.Get<const char *>("EDID", 0);
			if (editorId)
				editorIds.emplace(editorId, ref);
			if (ref->baseObject && ref->baseObject->TypeAny({"WEAP", "MISC"}))
			{
				iterables.push_back(ref);
			}
		});

		PlaceCamera(this);
	}

	static void PlaceCamera(Interior *interior) {
		static bool spawned = false;
		auto ref = interior->editorIds.find("darkshackspawn");
		if (ref != interior->editorIds.end() && !spawned)
		{
			auto DATA = ref->second->self->Get<float *>("DATA");
			first_person_camera->pos = ref->second->matrix[3];
			first_person_camera->pos.z += EYE_HEIGHT;
			first_person_camera->fyaw = cast_vec_3(DATA + 3)->z;
			spawned = true;
		}
	}

	Interior::~Interior()
	{
		Unload();
	}

	void Interior::Unload()
	{
		for (auto it = refs.begin(); it != refs.end(); ++it)
		{
			Ref *ref = *it;
			delete ref;
		}
	}

	bool myfunction(Ref *l, Ref *r)
	{
		return l->GetDistance() < r->GetDistance();
	}

	void Interior::Update()
	{
		std::vector<Ref *> closest = iterables;
		std::sort(iterables.begin(), iterables.end(), myfunction);

		for (auto it = closest.begin(); it != closest.end(); ++it)
		{
			Ref *ref = *it;

			if (ref->DisplayAsItem())
				return;
		}
	}

} // namespace gloom