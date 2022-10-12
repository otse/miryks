#include <miryks/miryks.hpp>
#include <miryks/model.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>

#include <opengl/renderer.h>
#include <opengl/texture.h>
#include <opengl/material.h>
#include <opengl/shader.h>
#include <opengl/camera.h>
#include <opengl/lights.h>
#include <ogl/scene_graph.hpp>

/*
this is a messy source file
may not deserve eventual cleanup
*/

namespace miryks
{
	struct VHGT
	{
		float offset;
		char bytes[1089];
		char unknown[3];
		int end[];
	};

	struct VNML
	{
		unsigned char bytes[3267];
	};

	struct VCLR
	{
		unsigned char bytes[3267];
	};

	/*struct ATXT
	{
		unsigned int formid;
		uint8_t quadrant;
		char unknown;
		uint16_t layers;
	};*/

	struct DATA
	{
		float x;
	};

	struct BTXT
	{
		unsigned int formid;
		uint8_t quadrant;
		char unknown;
		uint16_t layer;
	};

#pragma pack(push, 1)
	struct ATXT
	{
		unsigned int formid;
		uint8_t quadrant;
		char unknown;
		uint16_t layer;
	};

	struct VTXT_
	{
		unsigned short int pos;
		char unkown[2];
		float opacity;
	};

	struct alpha_layer
	{
		ATXT *header;
		csubrecord *subrecord;
		int num_sub_structs;
		record landscape_texture;
	};

	struct base_layer
	{
		ATXT *header;
	};
#pragma pack(pop)

	land::land(record land, exterior *exte) : record(land), exte(exte)
	{
		auto datah = data<DATA *>("DATA");
		auto vhgt = data<VHGT *>("VHGT");
		auto vnml = data<VNML *>("VNML");
		auto atxt = data<ATXT *>("ATXT");
		auto btxt = data<BTXT *>("BTXT");
		auto vclr = data<VCLR *>("VCLR");

		if (!vhgt)
			return;

		/*group = new GroupBounded;

		Geometry *geometry = new Geometry();
		group->geometry = geometry;

		MaterialLand *material = new MaterialLand;
		material->landOffset = vec2(exte->xclc->x, exte->xclc->y);
		// material->defines += "#define DONT_USE_NORMAL_MAP\n";
		material->defines += "#define DONT_USE_SPECULAR_MAP\n";

		material->map2 = GetProduceTexture("textures\\landscape\\dirt02.dds");
		material->normalMap = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
		material->color = vec3(1.f);
		material->src = &landshader;
		geometry->material = material;*/

		// 0 = bottom left. 1 = bottom right. 2 = upper-left. 3 = upper-right.
		struct quadrant
		{
			std::vector<alpha_layer> alpha_layers;
		} quadrants[4];

		for (int i = 0; i < 20; i++)
		{
			auto btxt = data<BTXT *>("BTXT", i);
			if (btxt)
				printf("found base layer header #%i\n", i);
			if (!btxt)
				break;
		}

		for (int i = 0; i < 30; i++)
		{
			auto alpha_layer_header = data<ATXT *>("ATXT", i);

			// auto alpha_layer_data = data<VTXT *>("VTXT", i);
			auto subrecord = find("VTXT", i);
			if (alpha_layer_header && subrecord)
			{
				printf("found alpha layer header with data #%i\n", i);
				quadrant &current = quadrants[alpha_layer_header->quadrant];
				// printf("alpha texture form id %i\n", alpha_layer_header->formid);
				//  grup_iter<0> landscape_textures("LTEX", 0);
				record landscape_texture = esp_get_form_id(alpha_layer_header->formid);
				const int num_sub_structs = subrecord->hed->size / sizeof(VTXT_);
				current.alpha_layers.push_back({alpha_layer_header, subrecord, num_sub_structs, landscape_texture});
				//printf("landscape texture name %s\n", landscape_texture.editor_id());
				// printf("amount sub structs in vtxt: %i\n", num_sub_structs);
			}
			if (!alpha_layer_header)
				break;
		}

		int j = 0;
		for (auto layer : quadrants[0].alpha_layers)
		{
			j++;
			if (j == 0)
			{
				// material->map2 = GetProduceTexture("textures\\landscape\\dirt02.dds");
			}
			//auto header = layer.header;
			//auto data = layer.subrecord->data;

			for (int i = 0; i < layer.num_sub_structs; i++)
			{
				VTXT_ &substruct = ((VTXT_ *)layer.subrecord->data)[i];
				void *pointer1 = &((VTXT_ *)layer.subrecord->data)[i];
				void *pointer2 = &((VTXT_ *)layer.subrecord->data)[i + 1];
				// printf("pointer1 + pointer2 distance: %i, %i dist: %i\n", pointer1, pointer2, (int)pointer1 - (int)pointer2);
				printf("substruct #%i is pos %i opacity %f\n", i, substruct.pos, substruct.opacity);
			}
			for (int y = 0; y < 33; y++)
			{
				for (int x = 0; x < 33; x++)
				{
					// int row = i / 33;
					// int column = i % 33;
					// cell_ *cast = (cell_ *) data->bytes[y * 33 + x];
					// float ointment = data->bytes[y * 33 + x] / 255.f;
					// printf("alpha layer data pos %i is %f\n", cast->pos, cast->opacity);
					// material->ointments[y*33+x] = layer.data[y*33+x];
				}
			}
			break;
		}

		const int grid = 33;
		constexpr float fgrid = grid;

		float heightmap[33][33] = {{0}};
		vec3 normals[33][33] = {{vec3(0.f)}};
		vec3 colors[33][33] = {{vec3(0.f)}};
		float offset = vhgt->offset * 8;
		printf("land offset %f\n", offset);
		float row_offset = 0;

		if (vnml)
		{
			unsigned int x;
			std::stringstream ss;
			ss << std::hex << "fffefffe";
			ss >> x;
			for (int i = 0; i < 1089; i++)
			{
				int row = i / 33;
				int column = i % 33;
				int j = i * 3;
				float a = vnml->bytes[j] / 255.0f;
				float b = vnml->bytes[j + 1] / 255.0f;
				float c = vnml->bytes[j + 2] / 255.0f;
				// printf("normal\n %f\n %f\n %f\n", a, b, c);
				normals[column][row] = vec3(a, b, c);
			}
		}

		if (vclr)
		{
			for (int i = 0; i < 1089; i++)
			{
				int row = i / 33;
				int column = i % 33;
				int j = i * 3;
				float a = vclr->bytes[j] / 255.0f;
				float b = vclr->bytes[j + 1] / 255.0f;
				float c = vclr->bytes[j + 2] / 255.0f;
				printf("vcolor\n %f\n %f\n %f\n", a, b, c);
				colors[column][row] = vec3(a, b, c);
			}
		}

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

		// create 8 piece triangle inward pointing land square
		int square[8][3][2] = {{{0}}};
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

		float colors2[8][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}};

		// printf(" heightmap 0 = %f\n", heightmap[0][0] - 2048.0);
		float gridsize = 32 * 32;
		// geometry->Clear(8 * 3 * gridsize, 8 * 3 * gridsize);

		constexpr float breadth = 4096.f;
		constexpr float div = breadth / 32.0F;

		printf(" exte %i\n", exte);
		float X = breadth * exte->xclc->x;
		float Y = breadth * exte->xclc->y;
		printf(" X Y %f %f\n", X, Y);

		// 0 = bottom left. 1 = bottom right. 2 = upper-left. 3 = upper-right

		ivec2 quadrant_offsets[4] = {
			{0, 1},
			{1, 1},
			{0, 0},
			{1, 0},
		};

		for (int w = 0; w < 4; w++)
		{
			int vertex = 0;
			quadrant &current = quadrants[w];
			vec2 half = quadrant_offsets[w] * 4096 / 2;
			ivec2 offset = quadrant_offsets[w] * 16;

			group[w] = new GroupBounded;
			Geometry *geometry = new Geometry();
			geometry->Clear(8 * 3 * gridsize, 8 * 3 * gridsize);
			geometry->skinning = true; // our skinning attribs double as landscape-weights
			group[w]->geometry = geometry;
			MaterialLand *material = new MaterialLand;
			material->map = GetProduceTexture("textures\\landscape\\dirt02.dds");
			material->normalMap = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->color = vec3(1.f);
			material->landOffset = vec2(exte->xclc->x, exte->xclc->y);

			material->src = &landshader;
			geometry->material = material;

			float textures[7] = {0};
			float layers[7][17*17] = {{0}};

			material->map2 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map3 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map4 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map5 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map6 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map7 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map8 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");

			for (auto alpha_layer : current.alpha_layers)
			{
				switch(alpha_layer.header->layer) {
					case 0:
						printf("layer 0\n");
						material->map2 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
						break;
					case 1:
						printf("layer 0\n");
						material->map3 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
						break;
				}
				for (int i = 0; i < alpha_layer.num_sub_structs; i++)
				{
					VTXT_ &substruct = ((VTXT_ *)alpha_layer.subrecord->data)[i];
					layers[alpha_layer.header->layer][substruct.pos] = substruct.opacity;
				}

			}

			for (int y = 0; y < 16; y += 2)
			{
				for (int x = 0; x < 16; x += 2)
				{
					for (int triangle = 0; triangle < 8; triangle++)
					{
						auto set_eight_opacities = [&](Vertex &v, int vertex) {
							v.skin_index[0] = layers[0][vertex];
							v.skin_index[1] = layers[1][vertex];
							v.skin_index[2] = layers[2][vertex];
							v.skin_index[3] = layers[3][vertex];
							v.skin_weight[0] = layers[4][vertex];
							v.skin_weight[1] = layers[5][vertex];
							v.skin_weight[2] = layers[6][vertex];
						};
						
						// printf("triangle %i\n", triangle);
						Vertex &a = geometry->vertices[vertex + 0];
						Vertex &b = geometry->vertices[vertex + 1];
						Vertex &c = geometry->vertices[vertex + 2];
						float tx1 = square[triangle][0][0];
						float ty1 = square[triangle][0][1];
						float tx2 = square[triangle][1][0];
						float ty2 = square[triangle][1][1];
						float tx3 = square[triangle][2][0];
						float ty3 = square[triangle][2][1];
						int x1_ = tx1 + x + offset.x;
						int x2_ = tx2 + x + offset.x;
						int x3_ = tx3 + x + offset.x;
						int y1_ = ty1 + y + offset.y;
						int y2_ = ty2 + y + offset.y;
						int y3_ = ty3 + y + offset.y;
						a.position = vec3((tx1 + x) * div + X + half.x, (ty1 + y) * div + Y + half.y, heightmap[x1_][y1_]);
						b.position = vec3((tx2 + x) * div + X + half.x, (ty2 + y) * div + Y + half.y, heightmap[x2_][y2_]);
						c.position = vec3((tx3 + x) * div + X + half.x, (ty3 + y) * div + Y + half.y, heightmap[x3_][y3_]);
						auto get_vertex_numeral = [](int x, int y) {
							return 289 - (y * 17) + x;
						};
						;
						set_eight_opacities(a, get_vertex_numeral(tx1 + x, ty1 + y));
						set_eight_opacities(b, get_vertex_numeral(tx2 + x, ty2 + y));
						set_eight_opacities(c, get_vertex_numeral(tx3 + x, ty3 + y));
						a.uv = vec2(tx1, ty1);
						b.uv = vec2(tx2, ty2);
						c.uv = vec2(tx3, ty3);
						if (vclr)
						{
							a.color = vec4(colors[x1_][y1_], 1.f);
							b.color = vec4(colors[x2_][y2_], 1.f);
							c.color = vec4(colors[x3_][y3_], 1.f);
						}
						if (vnml)
						{
							a.normal = normals[x1_][y1_];
							b.normal = normals[x2_][y2_];
							c.normal = normals[x3_][y3_];
						}
						geometry->elements.insert(
							geometry->elements.end(),
							{(unsigned int)vertex + 0,
							 (unsigned int)vertex + 1,
							 (unsigned int)vertex + 2});
						vertex += 3;
					}
				}
			}
			printf("num vertices: %i\n", vertex);
			geometry->SetupMesh();
			group[w]->UpdateSideways();

			for (int i = 0; i < 17*17; i++) {
				auto &vertex = geometry->vertices[i];
				//vertex.position
				//geometry->vertices[i].skin_weight[3] = i;
			}
		}


		printf("adding land geometry to bigGroup\n");

		groupDrawer = new GroupDrawer(nullptr, mat4(1.0));
		groupDrawer->Add(group[0]);
		groupDrawer->Add(group[1]);
		groupDrawer->Add(group[2]);
		groupDrawer->Add(group[3]);
		groupDrawer->UpdateSideways();
		sceneDef->bigGroup->Add(groupDrawer);
	
	}
}