#include <gloom/level.h>
#include <gloom/mesh.h>
#include <gloom/object.h>
#include <gloom/libs>

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
	Cell Level::GetCell(const char *editorId)
	{
		Cell cell = {false};

		grup *top = esp_get_top_grup(get_plugins()[1], "CELL");

		cassert(top, "no cells top grup");

		for (int i = 0; i < top->mixed.size; i++)
		{
			grup *A = (grup *)top->mixed.elements[i];
			for (int j = 0; j < A->mixed.size; j++)
			{
				grup *B = (grup *)A->mixed.elements[j];
				for (int k = 0; k < B->mixed.size; k += 2)
				{
					record *object = B->mixed.records[k];
					grup *grup = B->mixed.grups[k + 1];
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

	void Level::ParseGrup(Cell &cell, grup *grup)
	{
		if (grup == nullptr)
			return;
		for (int i = 0; i < grup->mixed.size; i++)
		{
			void *element = grup->mixed.elements[i];
			cassert(*(char *)element == RECORD, "mixed non record");
			if (grup->mixed.records[i]->hed->type != espwrd "REFR")
				continue;
			cassert(grup->mixed.records[i]->hed->type == espwrd "REFR", "not a refr");
			auto ref = new Ref((record *)element);
			refs.push_back(ref);
			if (ref->self->EDID)
				refEditorIDs.emplace(ref->self->EDID, ref);
			if (ref->base && ref->base->record->hed->type == espwrd "WEAP" ||
				ref->base->record->hed->type == espwrd "MISC")
			{
				printf("weap misc item");
				iterables.push_back(ref);
			}
		}

		static bool spawned = false;
		auto ref = refEditorIDs.find("darkshackspawn001");
		if (ref != refEditorIDs.end() && !spawned)
		{
			first_person_camera->pos = ref->second->matrix[3];
			first_person_camera->pos.z += EYE_HEIGHT;
			first_person_camera->fyaw = cast_vec_3((float *)ref->second->self->DATA + 3)->z;
			spawned = true;
		}
	}

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