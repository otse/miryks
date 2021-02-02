#include <Gloom/Level.h>
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
	// Todo, Again the c arrays and for loops are dreadful. Will wrap in a high level c++ solution soon.
	Cell /*Level::*/ GetCell(const char *editorId)
	{
		return Cell{false};

		// Look below for rewrite with objectarray wrapper

		Cell cell = {false};

		Grup *top = esp_get_top_grup(get_plugins()[1], "CELL");

		cassert(top, "no cells top grup");

		for (unsigned int i = 0; i < top->mixed.size; i++)
		{
			Grup *A = (Grup *)top->mixed.elements[i];
			for (unsigned int j = 0; j < A->mixed.size; j++)
			{
				Grup *B = (Grup *)A->mixed.elements[j];
				for (unsigned int k = 0; k < B->mixed.size; k += 2)
				{
					Record *object = B->mixed.records[k];
					Grup *grup = B->mixed.grups[k + 1];
					cassert(object->hed->type == *(unsigned int *)"CELL", "not a cell");
					const char *cellEdid = (char *)object->fields.subrecords[0]->data;
					//printf("Found cell %s\n", cellEdid);
					if (0 == strcmp(cellEdid, editorId))
					{
						printf("Found your cell %s\n", cellEdid);
						cell.cell = object;
						//cassert(grup->mixed.size >= 2, "cell lacks 2 following grups");
						if (grup->mixed.size >= 1)
							cell.persistent = grup->mixed.grups[0];
						if (grup->mixed.size >= 2)
							cell.non_persistent = grup->mixed.grups[1];
						return cell;
					}
				}
			}
		}

		return cell;
	}

	Cell Level::GetCell(const char *editorId)
	{
		Cell cell = {false};

		Grup *top = esp_get_top_grup(get_plugins()[1], "CELL");

		cassert(top, "no cells top grup ?!");

		/*
		b foreach i j k 0 0 0
		b foreach i j k 0 1 0
		b foreach i j k 0 2 0
		b foreach i j k 0 3 0
		b foreach i j k 1 0 0
		b foreach i j k 1 1 0
		b foreach i j k 2 0 0
		b foreach i j k 2 1 0
		b foreach i j k 3 0 0
		*/
		bool stop = false;

		ObjectArray(top).ForEach(GRUP, stop, [&](ObjectArray &oa, size_t &i) {
			
			ObjectArray(oa.GetGrup(i)).ForEach(GRUP, stop, [&](ObjectArray &oa, size_t &j) {

				ObjectArray(oa.GetGrup(j)).ForEach(0, stop, [&](ObjectArray &oa, size_t &k) {

					Object object(oa.GetRecord(k));
					Grup *grup = oa.GetGrup(k + 1);
					ObjectArray C(grup);
					cassert(object.Is("CELL"), "not a cell");
					const char *editorId2 = object.Get<const char *>("EDID", 0);
					if (0 == strcmp(editorId, editorId2))
					{
						cell.cell = object.record;
						printf("ForEach found your cell %s\n", editorId);
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

		//return cell;
		for (unsigned int i = 0; i < top->mixed.size; i++)
		{
			Grup *A = (Grup *)top->mixed.elements[i];
			for (unsigned int j = 0; j < A->mixed.size; j++)
			{
				Grup *B = (Grup *)A->mixed.elements[j];
				for (unsigned int k = 0; k < B->mixed.size; k += 2)
				{
					printf("b foreach i j k %u %u %u\n", i, j, k);
					Record *object = B->mixed.records[k];
					Grup *grup = B->mixed.grups[k + 1];
					cassert(object->hed->type == *(unsigned int *)"CELL", "not a cell");
					const char *cellEdid = (char *)object->fields.subrecords[0]->data;
					//printf("Found cell %s\n", cellEdid);
					if (0 == strcmp(cellEdid, editorId))
					{
						printf("Found your cell %s\n", cellEdid);
						cell.cell = object;
						//cassert(grup->mixed.size >= 2, "cell lacks 2 following grups");
						if (grup->mixed.size >= 1)
							cell.persistent = grup->mixed.grups[0];
						if (grup->mixed.size >= 2)
							cell.non_persistent = grup->mixed.grups[1];
						return cell;
					}
				}
			}
		}

		return cell;
	}

	Level::~Level()
	{
		loadedCell;
		Unload();
	}

	Level::Level(const char *edid)
	{
		Group *group = new Group();
		Cell cell = GetCell(edid);
		LoadCell(cell);
	}

	void Level::LoadCell(Cell &cell)
	{
		loadedCell = cell;
		ParseGrup(cell, cell.persistent);
		ParseGrup(cell, cell.non_persistent);
	}

	void Level::ParseGrup(Cell &cell, Grup *grup)
	{
		if (grup == nullptr)
			return;
		for (unsigned int i = 0; i < grup->mixed.size; i++)
		{
			void *element = grup->mixed.elements[i];
			cassert(*(char *)element == RECORD, "mixed non record");
			if (grup->mixed.records[i]->hed->type != espwrd "REFR")
				continue;
			cassert(grup->mixed.records[i]->hed->type == espwrd "REFR", "not a refr");
			auto ref = new Ref((Record *)element);
			refs.push_back(ref);
			const char *EDID = ref->self->Get<const char *>("EDID");
			if (EDID)
				refEditorIDs.emplace(EDID, ref);
			if (ref->base &&
					ref->base->record->hed->type == espwrd "WEAP" ||
				ref->base->record->hed->type == espwrd "MISC")
			{
				//printf("weap misc item");
				iterables.push_back(ref);
			}
		}

		static bool spawned = false;
		auto ref = refEditorIDs.find("darkshackspawn");
		if (ref != refEditorIDs.end() && !spawned)
		{
			auto DATA = ref->second->self->Get<float *>("DATA");
			first_person_camera->pos = ref->second->matrix[3];
			first_person_camera->pos.z += EYE_HEIGHT;
			first_person_camera->fyaw = cast_vec_3(DATA + 3)->z;
			spawned = true;
		}
	}

	// Nice
	void Level::Unload()
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

	// Perfect
	void Level::Update()
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