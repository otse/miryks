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
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/camera.h>
#include <renderer/lights.h>
#include <renderer/drawgroup.h>

namespace skyrim
{
	WorldSpace *gworldSpace = nullptr;
	
	WorldSpace *GetWorldSpace(const char *id, int plugin)
	{
		WorldSpace *ws = nullptr;
		Grup top = esp_top(get_plugins()[plugin], "WRLD");
		top.loop([&](Grup<> &g) {
			Record wrld = g.record();
			Grup<> grup = g.next().grup();
			if (wrld.editor_id(id)) {
				ws = new WorldSpace(wrld, grup);
				return true;
			} 
			return false;
		}, Top);
		return ws;
	}

	void WorldSpace::Init()
	{
		sceneDef->ambient = vec3(180.0 / 255.0);
		personCam->pos = vec3(0, 0, -2048.0);
		DiscoverAllCells();
		LoadExterior(0, 0);
	}

	void WorldSpace::DiscoverAllCells()
	{
		printf("DiscoverAllCells\n");
		grup.index = 2; // ignore first two world children
		grup.dive([&](Grup<> &g) {
			Record cell = g.record();
			Grup<> grup = g.next().grup();
			Exterior *exterior = new Exterior(cell, grup);
			exterior->worldSpace = this;
			exteriors.push_back(exterior);
			return false;
		}, {
			WorldChildren,
			ExteriorCellBlock,
			ExteriorCellSubBlock
		});
	}

	void WorldSpace::LoadExterior(int x, int y)
	{
		for (Exterior *exterior : exteriors)
		{
			if (exterior->xclc->x == x && exterior->xclc->y == y)
			{
				printf("loading exterior %i %i\n", x, y);
				exterior->Init();
			}
		}
	}

	void Exterior::Init()
	{
		Subgroup(persistent, CellPersistentChildren);
		Subgroup(temporary, CellTemporaryChildren);
	}
	
	void Exterior::Subgroup(Grup<> subgroup, int group_type)
	{
		if (!subgroup.valid()) {
			return;
		}
		subgroup.loop([&](Grup<> &g) {
			Record refr = g.record();
			if (refr.is_type(REFR))
			{
				Reference *reference = new Reference(refr);
				reference->cell = (Cell *)this;
				worldSpace->references.push_back(reference);
				if (refr.editor_id("darkworldheading"))
				{
					float *loc = refr.data<float *>("DATA");
					personCam->pos = cast_vec3(loc);
					personCam->pos.z += EYE_HEIGHT;
					personCam->yaw = cast_vec3(loc + 3).z;
					cameraCur = personCam;
				}
			}
			else if (refr.is_type("LAND"))
			{
				printf("this is land\n");
				assertc(land == nullptr);
				land = new Land(g.record());
				land->exterior = this;
			}
			return false;
		}, group_type);
	}

	Land::Land(Record land) : Record(land)
	{
		exterior = nullptr;
		printf("lets try make a land mesh\n");
		vhgt = data<VHGT *>("VHGT");
		group = new GroupBounded;

		Geometry *geometry = new Geometry();
		group->geometry = geometry;
		
		Material *material = new Material;
		material->defines += "#define DONT_USE_NORMAL_MAP\n";
		material->defines += "#define DONT_USE_SPECULAR_MAP\n";
		
		material->map = GetProduceTexture("textures\\landscape\\dirt02.dds");
		material->normalMap = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
		material->color = vec3(1.f);
		material->src = &simple;
		geometry->material = material;

		const int grid = 33;
		const float fgrid = grid;

		geometry->Clear(grid * grid, 1);

		float heightmap[33][33] = {{0}};

		float offset = 0;
		float row_offset = 0;

		for (int i = 0; i < 1089; i++)
		{
			float value = vhgt->bytes[i] * 8;

			int r = i / 33;
        	int c = i % 33;

			if (c == 0)
			{
				row_offset = 0;
				offset += value;
			}
			else
			{
				row_offset += value;
			}

			heightmap[c][r] = offset + row_offset;
		}

		const float breadth = 4096.f;
		const float div = breadth / fgrid;
		const float center = 0;

		const float trepeat = 33 / 4;

		for (int y = 0; y < grid; y++)
		{
			for (int x = 0; x < grid; x++)
			{
				int z = y*grid+x;
				Vertex &vertex = geometry->vertices[z];
				float X = x * div - center;
				float Y = y * div - center;
				vertex.position = vec3(X, Y, heightmap[x][y] - 2048.0);
				vertex.color = vec4(1.0);
				vertex.uv = vec2(x/(fgrid-1)*trepeat, y/(fgrid-1)*trepeat);
			}
		}

		for (int y = 0; y < grid - 1; y ++)
		{
			for (int x = 0; x < grid - 1; x ++)
			{
				unsigned int a = y*grid+x;
				unsigned int b = y*grid+(x+1);
				unsigned int c = (y+1)*grid+(x+1);
				unsigned int d = (y+1)*grid+x;
				geometry->elements.insert(geometry->elements.end(), { a, b, c });
				geometry->elements.insert(geometry->elements.end(), { d, b, c });
			}
		}

		geometry->SetupMesh();
		printf("adding land geometry to bigGroup");

		group->Update();
		drawGroup = new DrawGroupFlatSorted(group, mat4(1.0));
		drawGroup->Update();
		sceneDef->bigGroup->Add(drawGroup);
	}

}