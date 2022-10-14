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
#include <opengl/scene_graph.hpp>

/*
landscape works
but this is a messy source file
good luck
to explain it shortly, landscaping uses vertex attributes
then texture splats with glsl mix
it uses a single, optional base layer and multiple alpha layers
its not difficult once you get it
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

#pragma pack(push, 1)
	struct BTXT
	{
		unsigned int formid;
		uint8_t quadrant;
		char unknown;
		uint16_t layer;
	};

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
		csubrecord *vtxt;
		int num_sub_structs;
		const char *tx00;
	};

	struct base_layer
	{
		BTXT *header;
		record ltex;
		record txst;
		const char *tx00;
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

		bool debug = false;
		//if (exte->xclc->x == -1 && exte->xclc->y == 0)
		//	debug = true;

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
			std::vector<base_layer> base_layers;
			std::vector<alpha_layer> alpha_layers;
		} quadrants[4];

		for (int i = 0; i < 20; i++)
		{
			auto btxt = data<BTXT *>("BTXT", i);
			if (btxt)
			{
				quadrant &current = quadrants[btxt->quadrant];
				record ltex = esp_get_form_id(btxt->formid);
				auto tnam = ltex.data<unsigned int *>("TNAM");
				record txst = esp_get_form_id(*tnam);
				auto tx00 = txst.data<const char *>("TX00");

				current.base_layers.push_back({btxt, ltex, txst, tx00});
				
				printf("found base layer header #%i\n");
				printf("found base layer, txst edid is %s\n", txst.editor_id());
				printf("found base layer, txst tx00 is %s\n", tx00);
			
			}
			if (!btxt)
				break;
		}

		for (int i = 0; i < 30; i++)
		{
			auto header = data<ATXT *>("ATXT", i);

			// auto alpha_layer_data = data<VTXT *>("VTXT", i);
			auto vtxt = find("VTXT", i);
			if (header && vtxt)
			{
				const int num_sub_structs = vtxt->hed->size / sizeof(VTXT_);

				quadrant &current = quadrants[header->quadrant];
				// printf("alpha texture form id %i\n", header->formid);
				//  grup_iter<0> landscape_textures("LTEX", 0);
				record ltex = esp_get_form_id(header->formid);
				const char *tx00 = "landscape\\dirt02.dds";
				if (ltex.rvalid())
				{
					auto tnam = ltex.data<unsigned int *>("TNAM");
					record txst = esp_get_form_id(*tnam);
					tx00 = txst.data<const char *>("TX00");
					//if (debug)
					//	printf("quadrant %i layer %i tx00 is %s\n", header->quadrant, header->layer, tx00);
				}
				current.alpha_layers.push_back({header, vtxt, num_sub_structs, tx00});

				// printf("landscape texture name %s\n", landscape_texture.editor_id());
				//  printf("amount sub structs in vtxt: %i\n", num_sub_structs);
			}
			if (!header)
				break;
		}

		const int grid = 33;
		constexpr float fgrid = grid;

		float heightmap[33][33] = {{0}};
		vec3 normals[33][33] = {{vec3(0.f)}};
		vec3 colors[33][33] = {{vec3(1.f)}};

		float offset = vhgt->offset * 8;
		//if (debug)
		//	printf("land offset %f\n", offset);
		float row_offset = 0;

		if (vnml)
		{
			for (int i = 0; i < 1089; i++)
			{
				int row = i / 33;
				int column = i % 33;
				int j = i * 3;
				float a = vnml->bytes[j + 0] / 255.0f;
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
				float a = vclr->bytes[j + 0] / 255.0f;
				float b = vclr->bytes[j + 1] / 255.0f;
				float c = vclr->bytes[j + 2] / 255.0f;
				//if (debug)
				//	printf("vcolor\n %f\n %f\n %f\n", a, b, c);
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

		//if (debug)
		//	printf(" exte %i\n", exte);
		float X = breadth * exte->xclc->x;
		float Y = breadth * exte->xclc->y;
		//if (debug)
		//	printf(" X Y %f %f\n", X, Y);

		// 0 = bottom left. 1 = bottom right. 2 = upper-left. 3 = upper-right

		ivec2 quadrant_offsets[4] = {
			{0, 0},
			{1, 0},
			{0, 1},
			{1, 1},
		};

		for (int w = 0; w < 4; w++)
		{
			int vertex = 0;
			quadrant &current = quadrants[w];
			vec2 half = quadrant_offsets[w] * 4096 / 2;
			ivec2 offset = quadrant_offsets[w] * 16;

			group[w] = new GroupBounded;
			group[w]->name = "Quadrant";
			Geometry *geometry = new Geometry();
			const int seventeen = 17 * 17;
			geometry->Clear(8 * 3 * seventeen, 8 * 3 * seventeen);
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
			float layers[8][17 * 17] = {{1}};

			material->map2 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map3 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map4 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map5 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map6 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map7 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");
			material->map8 = GetProduceTexture("textures\\landscape\\dirt02_n.dds");

			for (auto &alpha_layer : current.alpha_layers)
			{
				switch (alpha_layer.header->layer)
				{
				case 0:
					if (debug)
						printf("layer 0 is %s\n", alpha_layer.tx00);
					material->map2 = GetProduceTexture((std::string("textures\\") + alpha_layer.tx00).c_str());
					break;
				case 1:
					if (debug)
						printf("layer 1 is %s\n", alpha_layer.tx00);
					material->map3 = GetProduceTexture((std::string("textures\\") + alpha_layer.tx00).c_str());
					break;
				case 2:
					if (debug)
						printf("layer 2 is %s\n", alpha_layer.tx00);
					material->map4 = GetProduceTexture((std::string("textures\\") + alpha_layer.tx00).c_str());
					break;
				case 3:
					if (debug)
						printf("layer 3 is %s\n", alpha_layer.tx00);
					material->map5 = GetProduceTexture((std::string("textures\\") + alpha_layer.tx00).c_str());
					break;
				case 4:
					if (debug)
						printf("layer 4 is %s\n", alpha_layer.tx00);
					material->map6 = GetProduceTexture((std::string("textures\\") + alpha_layer.tx00).c_str());
					break;
				}
				for (int i = 0; i < alpha_layer.num_sub_structs; i++)
				{
					VTXT_ &substruct = ((VTXT_ *)alpha_layer.vtxt->data)[i];
					//if (debug)
					//	printf("set layer %i pos %i to %f\n", alpha_layer.header->layer, substruct.pos, substruct.opacity);
					layers[alpha_layer.header->layer][substruct.pos] = substruct.opacity;

					//printf("([%i][%i] = %f)\n", alpha_layer.header->layer, substruct.pos, substruct.opacity);
				}
			}

			if (current.base_layers.size())
			{
				printf("We Have A Base Layer\n");
				base_layer &base = current.base_layers[0];
				material->map = GetProduceTexture((std::string("textures\\") + base.tx00).c_str());
			}

			for (int y = 0; y < 16; y += 2)
			{
				for (int x = 0; x < 16; x += 2)
				{
					for (int triangle = 0; triangle < 8; triangle++)
					{
						auto set_eight_opacities = [&](Vertex &v, int vertex2)
						{
							v.skin_index.x = layers[0][vertex2];
							// if (debug)
							//	printf("s_e_o [%i][%i] = %f\n", 0, vertex2, layers[0][vertex2]);
							//  if (debug && v.skin_index.x)
							//	printf("v.skin_index.x = %f\n", layers[0][vertex2]);
							v.skin_index.y = layers[1][vertex2];
							v.skin_index.z = layers[2][vertex2];
							v.skin_index.w = layers[3][vertex2];

							v.skin_weight.x = layers[4][vertex2];
							v.skin_weight.y = layers[5][vertex2];
							v.skin_weight.z = layers[6][vertex2];
							v.skin_weight.w = layers[7][vertex2];
						};

						// printf("triangle %i\n", triangle);
						Vertex &a = geometry->vertices[vertex + 0];
						Vertex &b = geometry->vertices[vertex + 1];
						Vertex &c = geometry->vertices[vertex + 2];
						int tx1 = square[triangle][0][0];
						int ty1 = square[triangle][0][1];
						int tx2 = square[triangle][1][0];
						int ty2 = square[triangle][1][1];
						int tx3 = square[triangle][2][0];
						int ty3 = square[triangle][2][1];
						int x1_ = tx1 + x + offset.x;
						int x2_ = tx2 + x + offset.x;
						int x3_ = tx3 + x + offset.x;
						int y1_ = ty1 + y + offset.y;
						int y2_ = ty2 + y + offset.y;
						int y3_ = ty3 + y + offset.y;
						a.position = vec3((tx1 + x) * div + X + half.x, (ty1 + y) * div + Y + half.y, heightmap[x1_][y1_]);
						b.position = vec3((tx2 + x) * div + X + half.x, (ty2 + y) * div + Y + half.y, heightmap[x2_][y2_]);
						c.position = vec3((tx3 + x) * div + X + half.x, (ty3 + y) * div + Y + half.y, heightmap[x3_][y3_]);
						auto get_vertex_numeral = [](int x_, int y_)
						{
							//return 289 - (17 - (y_ + 1) * 17) + (x_); // this caused two days of payne
							return (y_ * 17) + x_;
						};
						//if (debug)
						//	printf("get vertex numeral tx1 %i x %i = %i\n", tx1, x, get_vertex_numeral(tx1 + x, ty1 + y));
						// printf("tx1 + x (tx + x, ty + y) = %i \n", get_vertex_numeral(tx1 + x, ty1 + y));
						set_eight_opacities(a, get_vertex_numeral(tx1 + x, ty1 + y));
						set_eight_opacities(b, get_vertex_numeral(tx2 + x, ty2 + y));
						set_eight_opacities(c, get_vertex_numeral(tx3 + x, ty3 + y));
						a.uv = vec2(tx1, ty1) / 2.f;
						b.uv = vec2(tx2, ty2) / 2.f;
						c.uv = vec2(tx3, ty3) / 2.f;
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
			// if (debug)
			//	printf("num vertices: %i\n", vertex);
			//if (debug)
			//	printf("end of quadrant\n");
			geometry->SetupMesh();
			group[w]->UpdateSideways();
		}
		if (debug)
			printf("adding land geometry to bigGroup\n");

		groupDrawer = new GroupDrawer(nullptr, mat4(1.0));
		groupDrawer->name = "Land";
		groupDrawer->Add(group[0]);
		groupDrawer->Add(group[1]);
		groupDrawer->Add(group[2]);
		groupDrawer->Add(group[3]);
		groupDrawer->UpdateSideways();
		sceneDef->bigGroup->Add(groupDrawer);
	}
}