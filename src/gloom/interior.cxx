#include <skyrim_units>
#include <libs>
#include <Gloom/Interior.h>
#include <Gloom/Util.h>

#include <Gloom/Mesh.h>
#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

#include <algorithm>
#include <cctype>
#include <string>

// #include <OpenGL/renderable.h>
#include <OpenGL/Texture.h>
#include <OpenGL/Camera.h>
#include <OpenGL/Lights.h>
#include <OpenGL/Types.h>

namespace gloom
{
	Interior::Interior(const char *edid)
	{
		editorId = edid;
		Group *group = new Group();
	}

	void Interior::LoadCell()
	{
		loadedCell = GetCell(editorId);
		ParseGrup(loadedCell, loadedCell.persistent);
		ParseGrup(loadedCell, loadedCell.non_persistent);
	}

	Cell Interior::GetCell(const char *name)
	{
		Cell cell;

		Grup *top = esp_get_top_grup(get_plugins()[1], "CELL");

		bool stop = false;

		Objects(top).ForEach(0, stop, [&](Objects &objects, unsigned int i) {
			Objects(objects.GetGrup(i)).ForEach(0, stop, [&](Objects &objects, unsigned int j) {
				Objects(objects.GetGrup(j)).ForEach(0, stop, [&](Objects &objects, unsigned int &k) {
					Object object(objects.GetRecord(k));
					Grup *grup = objects.GetGrup(k + 1);
					const char *editorId = GetEditorId(object);
					if (0 == strcmp(name, editorId))
					{
						Objects C(grup);
						cell.record = object.record;
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

	static void PlaceCameraDud(Interior *);

	void Interior::ParseGrup(Cell &cell, Grup *grup)
	{
		if (grup == nullptr)
			return;
		bool stop = false;
		auto func = [&](Objects &objects, unsigned int i) {
			Record *record = objects.GetRecord(i);
			Object object(record);
			if (object.IsType("REFR"))
			{
				Ref *ref = new Ref(record);
				refs.push_back(ref);
				const char *editorId = GetEditorId(object);
				if (editorId)
					editorIds.emplace(editorId, ref);
				if (ref->baseObject.Valid() && ref->baseObject.IsTypeAny({"WEAP", "MISC"}))
				{
					iterables.push_back(ref);
				}
			}
		};
		Objects(grup).ForEach(0, stop, func);
		PlaceCamera();
	}

	void Interior::PlaceCamera()
	{
		if (alreadyTeleported)
			return;
		bool stop = false;
		auto func = [&](Objects &objects, unsigned int i) {
			Object object(objects.GetRecord(i));
			if (*GetBaseId(object) == 0x0000003B) //  "Marker"
			{
				// Place at any XMarker
				float *locationalData = object.Data<float *>("DATA");
				printf(" xmarker ! \n");
				first_person_camera->pos = *cast_vec_3(locationalData);
				first_person_camera->pos.z += EYE_HEIGHT;
				first_person_camera->fyaw = cast_vec_3(locationalData + 3)->z;
				alreadyTeleported = true;
				stop = true;
			}
		};
		Objects(loadedCell.persistent).ForEach(0, stop, func);
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