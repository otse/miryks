#include "level.h"

#include "files"

#include "opengl/texture"
#include "opengl/types"

namespace dark2
{
	Cell Level::GetCell(const char *editorId)
	{
		Cell cell = {false};

		Grup *top = esp_get_top_grup(testMod, "CELL");

		Assert(top, "no cells top grup");

		for (int i = 0; i < top->mixed.size; i++)
		{
			Grup *A = (Grup *)top->mixed.elements[i];
			for (int j = 0; j < A->mixed.size; j++)
			{
				Grup *B = (Grup *)A->mixed.elements[j];
				for (int k = 0; k < B->mixed.size; k += 2)
				{
					Record *record = (Record *)B->mixed.elements[k];
					Grup *grup = (Grup *)B->mixed.elements[k + 1];
					Assert(record->head->type == *(unsigned int *)"CELL", "not a cell");
					const char *cellEdid = (char *)((Subrecord *)record->fields.elements[0])->data;
					printf("Found cell %s\n", cellEdid);
					if (0 == strcmp(cellEdid, editorId))
					{
						printf("Found your cell %s\n", cellEdid);
						cell.cell = record;
						cell.persistent = (Grup *)grup->mixed.elements[0];
						cell.non_persistent = (Grup *)grup->mixed.elements[1];
						return cell;
					}
				}
			}
		}

		return cell;
	}

	Level::Level()
	{
		Group *group = new Group();

		Cell cell = GetCell("Dark2Schmuck");

		LoadCell(cell);
	}

	void Level::ParseGrup(Cell &cell, Grup *grup)
	{
		for (int i = 0; i < cell.non_persistent->mixed.size; i++)
		{
			void *element = cell.non_persistent->mixed.elements[i];
			Assert(*(char *)element == RECORD, "cell mixed has non record");
			auto ref = new Reference;
			ref->SetData((Record *)element);
			printf("Reference\n");
		}
	}

	void Level::LoadCell(Cell &cell)
	{
		ParseGrup(cell, cell.non_persistent);
	}

	void Level::Update()
	{
	}

	Reference::Reference()
	{
		Group *group = new Group();
	}

	void Reference::SetData(Record *record)
	{
		for (int i = 0; i < record->fields.size; i++)
		{
			void *element = record->fields.elements[i];
			Subrecord *field = (Subrecord *)element;
			if (field->head->type == *(unsigned int *)"EDID")
			{
				
			}
			if (field->head->type == *(unsigned int *)"DATA")
			{
				vec3 pos, rad;
				pos = *cast_vec_3((float *)field->data);
				rad = *cast_vec_3((float *)field->data + 3);
				printf("Reference has DATA pos %f %f %f\n", pos.x, pos.y, pos.z);
				printf("Reference has DATA rad %f %f %f\n", rad.x, rad.y, rad.z);

				matrix = mat4(1.0f);

				mat4 translation = translate(mat4(1.0f), pos);
				mat4 rotation = mat4(1.0f);
				mat4 scale = mat4(1.0f);

				rotation = rotate(rotation, rad.x, vec3(1, 0, 0));
				rotation = rotate(rotation, rad.y, vec3(0, 1, 0));
				rotation = rotate(rotation, rad.z, vec3(0, 0, 1));

				matrix = translation * rotation * scale;
			}
			if (field->head->type == *(unsigned int *)"NAME")
			{
				unsigned int formId = *(unsigned int *)field->data;
				printf("NAME %i\n", formId);
			}
		}


	}
} // namespace dark2