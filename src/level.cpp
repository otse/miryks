#include "level.h"
#include "mesh.h"

#include <algorithm>
#include <cctype>
#include <string>

extern "C"
{
#include "bsa.h"
#include "nif.h"
#include "esp.h"
}

#include "opengl/renderable"
#include "opengl/texture"
#include "opengl/camera"
#include "opengl/pointlight"
#include "opengl/types"



namespace dark2
{
	Cell Level::GetCell(const char *editorId)
	{
		Cell cell = {false};

		grup *top = esp_get_top_grup(padstow, "CELL");

		Assert(top, "no cells top grup");

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
					Assert(object->hed->type == *(unsigned int *)"CELL", "not a cell");
					const char *cellEdid = (char *)object->fields.subrecords[0]->data;
					//printf("Found cell %s\n", cellEdid);
					if (0 == strcmp(cellEdid, editorId))
					{
						printf("Found your cell %s\n", cellEdid);
						cell.cell = object;
						cell.persistent = grup->mixed.grups[0];
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
		for (int i = 0; i < grup->mixed.size; i++)
		{
			void *element = grup->mixed.elements[i];
			Assert(*(char *)element == RECORD, "mixed non record");
			Assert(grup->mixed.records[i]->hed->type == *(unsigned int *)"REFR", "not a refr");
			auto ref = new Ref;
			refs.push_back(ref);
			ref->SetData((Record *)element);
			if (ref->EDID)
				editorIds.insert({ref->EDID, ref});
		}

		static bool spawned = false;
		auto ref = editorIds.find("darkshackspawn001");
		if (ref != editorIds.end() && !spawned)
		{
			first_person_camera->pos = ref->second->matrix[3];
			first_person_camera->pos.z += 170 / ONE_SKYRIM_UNIT_IN_CM;
			first_person_camera->fyaw = cast_vec_3(ref->second->DATA + 3)->z;
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

	void Level::Update()
	{
	}

} // namespace dark2