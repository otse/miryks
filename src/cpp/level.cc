#include "level.h"

extern "C"
{
#include "c/esp.h"
}

#include "opengl/texture"
#include "opengl/types"

Level::Level()
{
	int l;
	Group *group = new Group();

	return;

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
}

void Level::Update()
{
}