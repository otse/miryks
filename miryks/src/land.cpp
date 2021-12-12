#include <miryks/miryks.hpp>
#include <miryks/model.h>

#include <algorithm>
#include <cctype>
#include <string>

#include <opengl/renderer.h>
#include <opengl/texture.h>
#include <opengl/material.h>
#include <opengl/shader.h>
#include <opengl/camera.h>
#include <opengl/lights.h>
#include <opengl/drawgroup.h>

namespace miryks
{
	struct VHGT
	{
		int32_t offset;
		char bytes[1089];
		char unknown[3];
		int end[];
	};

	struct ATXT {
		unsigned int formid;
		uint8_t quadrant;
		char unknown;
		uint16_t layers;
	};

	struct BTXT {
		unsigned int formid;
		uint8_t quadrant;
		char unknown;
		uint16_t layers;
	};

	struct VCLR {
		char vertex_color[1089][3];
	};

	land::land(record land) : record(land)
	{
		exterior = nullptr;
		
		auto vhgt = data<VHGT *>("VHGT");
		auto atxt = data<ATXT *>("ATXT");
		auto btxt = data<BTXT *>("BTXT");
		auto vclr = data<VCLR *>("VCLR");

		if (atxt)
			printf("atxt layers %u\n", atxt->layers);

		printf("btxt %i\n", btxt);

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
			int row = i / 33;
			int column = i % 33;
			if (column == 0)
			{
				row_offset = 0;
				offset += value;
			}
			else
				row_offset += value;

			heightmap[column][row] = offset + row_offset;
		}

		const float breadth = 4096.f;
		const float div = breadth / fgrid;
		const float center = 0;

		const float trepeat = 33 / 4;

		for (int y = 0; y < grid; y++)
		{
			for (int x = 0; x < grid; x++)
			{
				int z = y * grid + x;
				Vertex &vertex = geometry->vertices[z];
				float X = x * div - center;
				float Y = y * div - center;
				vertex.position = vec3(X, Y, heightmap[x][y] - 2048.0);
				if (vclr) {
					char *bytes = vclr->vertex_color[z];
					vertex.color = vec4(
						bytes[0],
						bytes[1],
						bytes[2],
						1
					);// / 255.f;
				}
				vertex.uv = vec2(x / (fgrid - 1) * trepeat, y / (fgrid - 1) * trepeat);
			}
		}

		for (int y = 0; y < grid - 1; y++)
		{
			for (int x = 0; x < grid - 1; x++)
			{
				unsigned int a = y * grid + x;
				unsigned int b = y * grid + (x + 1);
				unsigned int c = (y + 1) * grid + (x + 1);
				unsigned int d = (y + 1) * grid + x;
				geometry->elements.insert(geometry->elements.end(), {a, b, c});
				geometry->elements.insert(geometry->elements.end(), {d, b, c});
			}
		}

		geometry->SetupMesh();
		printf("adding land geometry to bigGroup\n");

		group->Update();
		drawGroup = new DrawGroupFlatSorted(group, mat4(1.0));
		drawGroup->Update();
		sceneDef->bigGroup->Add(drawGroup);
	}
}