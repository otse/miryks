// big include blob !!

// needs a big cleanup sometime

#include <miryks/miryks.hpp>
#include <miryks/model.h>
#include <miryks/collision.h>

#include <dark/trash.h>
#include <dark/my_reference.h>

#include <algorithm>
#include <cctype>
#include <string>

#include <opengl/texture.h>
#include <opengl/camera.h>
#include <opengl/lights.h>
#include <opengl/scene_graph.hpp>
#include <opengl/renderer.h>

#include <imgui.h>
#include <panels.h>

using namespace miryks;

namespace dark
{
	my_reference::my_reference(record refr) : reference(refr)
	{
		model = nullptr;
		groupDrawer = nullptr;
		pointLight = nullptr;
		spotLight = nullptr;
		Go();
		itemfinder::pool.push_back(this);
	}

	my_reference::~my_reference()
	{
		if (groupDrawer && groupDrawer->parent)
			groupDrawer->parent->Remove(groupDrawer);
		sceneDef->pointLights.Remove(pointLight);
		sceneDef->spotLights.Remove(spotLight);
		delete groupDrawer;
		delete pointLight;
		delete spotLight;

		if (collider)
			collider->remove();
	}

	void my_reference::Step()
	{
		if (baseObject.is_type("MSTT"))
		{
			if (model)
				model->Step();
		}
	}

	void my_reference::Go()
	{
		matrix = mat4(1.0);

		translation = rotation = scale = mat4(1.0);

		const unsigned int *baseId = base();

		auto xscl = data<float *>("XSCL");
		if (xscl) {
			orientation.scale = vec3(*xscl);
		}
		auto locationalData = data<float *>("DATA");

		ForLocationalData(locationalData);
		ForBaseId(baseId);
	}

	// bad
	void my_reference::ForLocationalData(float *locationalData)
	{
		if (!locationalData)
			return;
		orientation.position = reinterpret_vec3(locationalData);
		orientation.rotation = reinterpret_vec3(locationalData + 3);
		orientation.Compose();
		matrix = orientation.matrix;
	}

	// bad
	Model *create_simple_model_from_modl(const char *modl)
	{
		static std::map<const char *, Model *> map;
		auto has = map.find(modl);
		if (has != map.end())
			return has->second;
		Model *model = new Model(get_res(modl));
		map.emplace(modl, model);
		return model;
	}

	// for boo baba
	void my_reference::ForBaseId(const unsigned int *baseId)
	{
		if (!baseId)
			return;

		unsigned int id = *baseId;

		if (id == 0x0005AD9E) // Gold ingots to Orichalum ingots
			id = 0x0005AD99;

		baseObject = esp_get_form_id(id);

		if (!baseObject.rvalid())
		{
			char hex[10];
			snprintf(hex, 10, "%08X", id);
			printf("\ncant find form_id %s %u\n\n", hex, id);
			return;
		}

		std::vector<const char *> things = {
			Activators,
			Statics,
			Plants,
			Flora,
			Doors,
			Furniture,
			Books,
			Containers,
			Armor,
			Weapons,
			Ammo,
			Misc,
			Alchemy,
			Ingredients};

		// horrile if statements
		if (baseObject.is_types(things))
		{
			auto modl = baseObject.data<const char *>("MODL", 0);
			if (!modl)
			{
				printf("um no modl here\n");
			}
			else
			{
				if (baseObject.is_types({Misc, Alchemy, Armor, Weapons, Ingredients}))
					global_no_dust = true;
				if (strstr(baseObject.editor_id(), "cobweb"))
					global_is_cobweb = true;
				model = create_simple_model_from_modl(modl);
				global_no_dust = false;
				global_is_cobweb = false;
			}
			if (baseObject.is_type(CONT))
			{
				// container = new Container(baseObject);
			}
			if (baseObject.r->hed->formId == 0x21)
			{
				printf("big yellow collision marker\n");
				struct XPRM
				{
					float bounds[3];
					float color[3];
					float unknown;
					uint32_t unknown2;
				} * xprm;
				xprm = data<XPRM *>("XPRM");

				auto locationalData = data<float *>("DATA");

				collider = new miryks::collision::box(this, locationalData, xprm->bounds);
				printf("xprm %s bounds %f %f %f\n", editor_id(), xprm->bounds[0] * 2, xprm->bounds[1] * 2, xprm->bounds[2] * 2);

				// this is a collision marker
			}
		}
		else if (baseObject.is_type(MSTT))
		{
			// printf("this is a mstt\n");

			auto modl = baseObject.data<const char *>("MODL", 0);

			model = create_simple_model_from_modl(modl);
		}
		else if (baseObject.is_type(LIGH))
		{
			// decent light code !
			struct Struct
			{
				unsigned int time, radius, color, flags;
				float falloffExponent, FOV, nearClip, period, intensityAmplitude, movementAmplitude;
				unsigned int value;
				float weight;
			};

			float fade = 1;
			vec3 color(0, 0, 1);

			Light *light = nullptr;

			auto edId = baseObject.editor_id();
			auto DATA = baseObject.data<int *>("DATA");
			auto FNAM = baseObject.data<float *>("FNAM");

			Struct *data = (Struct *)DATA;

			if (edId)
			{
				// printf("ligh edid %s\n", LIGH.baseId);
			}
			if (data)
			{
				unsigned int rgb = data->color;
				unsigned char r = (rgb >> (8 * 0)) & 0xff;
				unsigned char g = (rgb >> (8 * 1)) & 0xff;
				unsigned char b = (rgb >> (8 * 2)) & 0xff;
				color = vec3(r, g, b) / 255.f;
			}
			if (FNAM)
			{
				fade = *FNAM;
				fade = 1 / fade;
			}

			if (data->flags & 0x0400)
			{
				light = spotLight = new spotlight;
				// printf("data fov %f\n", data->FOV);
				spotLight->angle = radians(data->FOV);
				sceneDef->spotLights.Add(spotLight);
				printf(" spotLight ! %f \n", spotLight->angle);
			}
			else
			{
				light = pointLight = new pointlight;
				sceneDef->pointLights.Add(pointLight);
				if (data->flags & 0x1)
					0; // pointLight->shadow->enabled = true;
			}

			if (light)
			{
				light->color = color;
				light->decay = fade;
				light->distance = (float)data->radius;
				light->matrix = matrix;
			}

			// point_light->decay = _j["Falloff Exponent"];
		}
		else
		{
			auto edId = baseObject.editor_id();

			printf("ref.cpp cant become %.4s edid %s\n", (char *)&baseObject.rhed().sgn, edId);

			baseObject = nullptr; // invalidate
		}

		if (model)
		{
			// bad
			if (baseObject.r->hed->formId != 0x32)
			{
				char type[50];
				snprintf(type, 50, "%.4s", (char *)&baseObject.rhed().sgn);

				groupDrawer = new GroupDrawer(model->baseGroup, matrix);
				groupDrawer->name = "(REFR)";
				groupDrawer->name += type;
				//groupDrawer->scale = orientation.scale.x;
				sceneDef->bigGroup->Add(groupDrawer);
				if (!baseObject.is_types({MSTT, TREE}))
				{
					if (baseObject.is_types({Misc, Alchemy, Armor, Weapons, Ingredients}))
						collider = new miryks::collision::movable_box(groupDrawer);
					else
						collider = new miryks::collision::solid(groupDrawer);
				}
				int i = 0;
				for (auto thing : Things)
				{
					if (*(unsigned int *)thing == baseObject.rhed().sgn)
					{
						groupDrawer->mask = 1 << i;
						break;
					}
					i++;
				}
			}
		}
	}

	float my_reference::GetDistance() const
	{
		if (groupDrawer == nullptr)
			return 0;
		float distance = glm::length(groupDrawer->aabb.center() - vec3(personCam->hands->matrixWorld[3]));

		return distance;
	}

	/*void uncompress_subrecord(char *data)
	{
		char *src = data;
		const unsigned int realSize = *(unsigned int *)src;
		const unsigned int size = field->hed->size - 4;
		src += 4;
		rec->buf = malloc(realSize * sizeof(char));
		int ret = uncompress(rec->buf, (uLongf*)&realSize, src, size);
		assertm(ret == Z_OK, "esp zlib");
		rec->actualSize = realSize;
		Count.uncompress++;
	}*/

	// Todo, Omg !

	bool myfunction(my_reference *l, my_reference *r)
	{
		return l->GetDistance() < r->GetDistance();
	}

	bool my_reference::use()
	{
		printf(" use %s\n", baseObject.editor_id());

		/*if (baseObject.is_type(CONT) && container)
			container->Activate();*/
		return true;
	}

#if 1
	// horrible imgui vomit
	bool my_reference::DisplayAsItem()
	{
		float dist = GetDistance() * ONE_CENTIMETER_IN_SKYRIM_UNITS;

		if (dist > 40)
			return false;

		if (groupDrawer == nullptr)
			return false;

		if (!baseObject.is_types({Furniture, Books, Containers, Armor, Weapons, Ammo, Misc, Alchemy, Ingredients, Flora}))
			return false;

		itemfinder::handRef = this;

		auto EDID = baseObject.data<char *>("EDID");
		auto FULL = baseObject.data<char *>("FULL");
		auto DESC = baseObject.data<const char *>("DESC");

		const char *itemName = "Something";
		if (FULL)
			itemName = FULL;

		vec3 original = vec3(0);

		mat4 mate = glm::translate(mat4(1), groupDrawer->aabb.center());

		mat4 model = cameraCur->projection * cameraCur->view * mate;
		mat4 projection = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
		vec4 viewport(0.0f, 0.0f, width, height);

		vec3 projected = glm::project(original, model, projection, viewport);
		itemfinder::projected = projected;
		// vec3 unprojected = glm::unProject(projected, model, projection, viewport);

		ImGui::SetNextWindowPos(ImVec2(width - projected.x, projected.y));
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;
		static bool open = true;
		// char s[100];
		ImGui::Begin("##ItemThing", &open, flags);
		ImGui::PushFont(font2);
		// ImGui::PushStyleColor(IMGUI_FON)
		// ImGui::TextColored(ImVec4(1, 1, 1, 1), EDID);
		// ImGui::NewLine();
		ImGui::TextColored(ImVec4(1, 1, 1, 1), itemName);

		ImGui::PopFont();
		ImGui::End();
		// Vec2 boo(x, y);

		return true;
	}
#endif

	/*
	Matrix m = _refr->_matrix;
	m.setTrans(m.getTrans() + Vec3(0, 0, 15 * ONE_SKYRIM_UNIT_IN_CM));

	Vec3 pos = _refr->record->_node->getBound().center() * m;

	const auto inv = camera_->getViewMatrix();
	const auto pro = camera_->getProjectionMatrix();

	Vec3 proj = pos * inv * pro;

	float x = ((proj.x() / 2 + .5)) * width_ / 2.0f;
	float y = (1 - (proj.y() / 2 + .5)) * height_ / 2.0f;
	Vec2 boo(x, y);

	std::string str = "";
	str = str + "move_item(" + std::to_string(x) + ", " + std::to_string(y) + ");";

	tub_->eval(str);
	*/

} // namespace dark