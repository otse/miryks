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
#include <ogl/scene_graph.hpp>

namespace miryks
{
	struct VHGT
	{
		float offset;
		char bytes[1089];
		char unknown[3];
		int end[];
	};

	struct ATXT
	{
		unsigned int formid;
		uint8_t quadrant;
		char unknown;
		uint16_t layers;
	};

	struct BTXT
	{
		unsigned int formid;
		uint8_t quadrant;
		char unknown;
		uint16_t layers;
	};

	struct VCLR
	{
		char vertex_color[1089][3];
	};

	struct DATA
	{
		float x;
	};

	land::land(record land, exterior *exte) : record(land), exte(exte)
	{
		auto datah = data<DATA *>("DATA");
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
		constexpr float fgrid = grid;

		geometry->Clear(grid * grid, 1);

		float heightmap[33][33] = {{0}};
		float offset = vhgt->offset * 8;
		printf("land offset %f\n", offset);
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

		const float center = 0;

		const float trepeat = 33 / 2;
#if 0
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
#endif

		const int spacing = grid / 2;

		// create 8 piece triangle inward pointing land square
		float square[8][3][2] = {{{0}}};
		square[0][0][0] = 0;
		square[0][0][1] = 0;
		square[0][1][0] = 1;
		square[0][1][1] = 0;
		square[0][2][0] = 1;
		square[0][2][1] = 1;

		square[1][0][0] = 0;
		square[1][0][1] = 0;
		square[1][1][0] = 1;
		square[1][1][1] = 1;
		square[1][2][0] = 0;
		square[1][2][1] = 1;

		square[2][0][0] = 0;
		square[2][0][1] = 1;
		square[2][1][0] = 1;
		square[2][1][1] = 1;
		square[2][2][0] = 0;
		square[2][2][1] = 2;

		square[3][0][0] = 0;
		square[3][0][1] = 2;
		square[3][1][0] = 1;
		square[3][1][1] = 1;
		square[3][2][0] = 1;
		square[3][2][1] = 2;

		square[4][0][0] = 1;
		square[4][0][1] = 2;
		square[4][1][0] = 1;
		square[4][1][1] = 1;
		square[4][2][0] = 2;
		square[4][2][1] = 2;

		square[5][0][0] = 1;
		square[5][0][1] = 1;
		square[5][1][0] = 2;
		square[5][1][1] = 1;
		square[5][2][0] = 2;
		square[5][2][1] = 2;

		square[6][0][0] = 2;
		square[6][0][1] = 1;
		square[6][1][0] = 1;
		square[6][1][1] = 1;
		square[6][2][0] = 2;
		square[6][2][1] = 0;

		square[7][0][0] = 1;
		square[7][0][1] = 0;
		square[7][1][0] = 2;
		square[7][1][1] = 0;
		square[7][2][0] = 1;
		square[7][2][1] = 1;

		float colors[8][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}};

		//printf(" heightmap 0 = %f\n", heightmap[0][0] - 2048.0);
		float gridsize = 32 * 32;
		geometry->Clear(8 * 3 * gridsize, 8 * 3 * gridsize);

		constexpr float breadth = 4096.f;
		constexpr float div = breadth / 32.0F;

		printf(" exte %i\n", exte);
		float X = breadth * exte->xclc->x;
		float Y = breadth * exte->xclc->y;
		printf(" X Y %f %f\n", X, Y);

		int vertex = 0;
		for (int y = 0; y < 32; y+=2)
		{
			for (int x = 0; x < 32; x+=2)
			{
				for (int triangle = 0; triangle < 8; triangle++)
				{
					float x_ = x;
					float y_ = y;
					//printf("triangle %i\n", triangle);
					Vertex &a = geometry->vertices[vertex + 0];
					Vertex &b = geometry->vertices[vertex + 1];
					Vertex &c = geometry->vertices[vertex + 2];
					float x1 = square[triangle][0][0];
					float y1 = square[triangle][0][1];
					float x2 = square[triangle][1][0];
					float y2 = square[triangle][1][1];
					float x3 = square[triangle][2][0];
					float y3 = square[triangle][2][1];
					a.position = vec3((x1 + x) * div + X, (y1 + y) * div + Y, heightmap[(int)x1+x][(int)y1+y]);
					b.position = vec3((x2 + x) * div + X, (y2 + y) * div + Y, heightmap[(int)x2+x][(int)y2+y]);
					c.position = vec3((x3 + x) * div + X, (y3 + y) * div + Y, heightmap[(int)x3+x][(int)y3+y]);
					// a.color = vec4(colors[triangle][0], colors[triangle][1], colors[triangle][2], 1);
					// b.color = vec4(colors[triangle][0], colors[triangle][1], colors[triangle][2], 1);
					// c.color = vec4(colors[triangle][0], colors[triangle][1], colors[triangle][2], 1);
					a.uv = vec2(x1, y1);
					b.uv = vec2(x2, y2);
					c.uv = vec2(x3, y3);
					geometry->elements.insert(
						geometry->elements.end(),
						{(unsigned int)vertex + 0,
						 (unsigned int)vertex + 1,
						 (unsigned int)vertex + 2});
					vertex += 3;
				}
			}
		}

#if 0
		// create squares
		for (int y = 0; y < 16; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				int q = y * 16 + x;
				int w = (y + 1) * 16 + x;
				int e = (y + 1) * 16 + x + 1;
				int r = y * 16 + x;
				Vertex &a = geometry->vertices[q];
				Vertex &b = geometry->vertices[w];
				Vertex &c = geometry->vertices[e];
				Vertex &d = geometry->vertices[r];
				float X = x * div - center;
				float Y = y * div - center;
				a.position = vec3(X, Y, heightmap[x][y] - 2048.0);
				/*b.position = vec3(X, Y, heightmap[x][y] - 2048.0);
				c.position = vec3(X, Y, heightmap[x][y] - 2048.0);
				d.position = vec3(X, Y, heightmap[x][y] - 2048.0);*/
				if (vclr)
				{
					char *bytes = vclr->vertex_color[q];
					a.color = vec4(
						bytes[0],
						bytes[1],
						bytes[2],
						1); // / 255.f;
				}
				a.uv = vec2(x / (fgrid - 1) * trepeat, y / (fgrid - 1) * trepeat);
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
#endif

		geometry->SetupMesh();
		printf("adding land geometry to bigGroup\n");

		group->UpdateSideways();
		groupDrawer = new GroupDrawer(group, mat4(1.0));
		groupDrawer->UpdateSideways();
		sceneDef->bigGroup->Add(groupDrawer);
	}
}