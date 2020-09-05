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
	Cell Level::GetCell(const char *edid)
	{
		Cell cell;

		Grup *top = esp_get_top_grup(testMod, "CELL");

		Assert(top, "plugin has no top cells?");

		for (int i = 0; i < top->mixed.size; i++)
		{
			Grup *block = (Grup *)top->mixed.elements[i];
			for (int j = 0; j < block->mixed.size; j++)
			{
				Grup *subBlock = (Grup *)block->mixed.elements[j];
				for (int k = 0; k < subBlock->mixed.size; k += 2)
				{
					Record *cell = (Record *)subBlock->mixed.elements[k];
					Grup *grup = (Grup *)subBlock->mixed.elements[k + 1];
					Assert(cell->head->type == *(unsigned int *)"CELL", "level getcell isnt a cell");
					printf("Found cell %s\n", ((Subrecord *)cell->fields.elements[0])->data);
				}
			}
		}

		return cell;
	}

	Level::Level()
	{
		Group *group = new Group();

		GetCell("DarkSchmuck");
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