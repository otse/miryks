#include "level.h"

#include "files"

extern "C"
{
#include "c/esp.h"
}

#include "opengl/texture"
#include "opengl/types"

namespace dark2
{
	Cell Level::GetCell(const char *editorId)
	{
		Cell cell = { false };

		Grup *top = esp_get_top_grup(testMod, "CELL");

		Assert(top, "no cells top grup");

		for (int i = 0; i < top->mixed.size; i++)
		{
			Grup *block = (Grup *)top->mixed.elements[i];
			for (int j = 0; j < block->mixed.size; j++)
			{
				Grup *subBlock = (Grup *)block->mixed.elements[j];
				for (int k = 0; k < subBlock->mixed.size; k += 2)
				{
					Record *record = (Record *)subBlock->mixed.elements[k];
					Grup *grup = (Grup *)subBlock->mixed.elements[k + 1];
					Assert(record->head->type == *(unsigned int *)"CELL", "isnt a cell");
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
	}

	/*Grup *cells = dark2::skyrim->grups.grups[57];
	for (int i = 0; i < cells->grups.size; i++)
	{
		Grup *block = cells->grups.grups[i];
		for (int j = 0; j < block->grups.size; j++)
		{
			Grup *subblock = block->grups.grups[j];
			for (int k = 0; k < subblock->mixed.size; k++)
			{
				void *any = subblock->mixed.elements[k];
				if (*(espnum *) any == GRUP)
				{
					printf("a grup");
				}
			}
		}
	}*/

	void Level::Update()
	{
	}
} // namespace dark2