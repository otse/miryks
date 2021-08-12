#include <skyrim_units>
#include <lib.h>

#include <dark/ref.h>

#include <skyrim/grup.h>
#include <skyrim/mesh.h>
#include <core/files.h>

#include <algorithm>
#include <cctype>
#include <string>

#include <renderer/texture.h>
#include <renderer/camera.h>
#include <renderer/lights.h>
#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/types.h>

#include <imgui.h>
#include <gooey/gooey.h>

namespace dark
{
	Ref::Ref(crecordp rcd) : Record(rcd)
	{
		mesh = nullptr;
		drawGroup = nullptr;
		pointLight = nullptr;
		spotLight = nullptr;
		go();
	}

	Ref::~Ref()
	{
		sceneDef->drawGroups.Remove(drawGroup);
		sceneDef->pointLights.Remove(pointLight);
		sceneDef->spotLights.Remove(spotLight);
		delete drawGroup;
		delete pointLight;
		delete spotLight;
	}

	void Ref::go()
	{
		matrix = mat4(1.0);

		translation = rotation = scale = mat4(1.0);

		formId baseId = base();

		auto XSCL = data<float *>("XSCL");
		auto locationalData = data<float *>("DATA");

		if (XSCL)
			scale = glm::scale(mat4(1.0), vec3(*XSCL));

		forLocationalData(locationalData);
		forBaseId(baseId);
	}

	void Ref::forLocationalData(float *locationalData)
	{
		if (!locationalData)
			return;

		vec3 pos = *cast_vec_3(locationalData);
		vec3 rad = *cast_vec_3(locationalData + 3);

		translation = translate(mat4(1.0f), pos);

		rotation = glm::rotate(rotation, -rad.x, vec3(1, 0, 0));
		rotation = glm::rotate(rotation, -rad.y, vec3(0, 1, 0));
		rotation = glm::rotate(rotation, -rad.z, vec3(0, 0, 1));

		matrix = translation * rotation * scale;
	}

	void Ref::forBaseId(formId baseId)
	{
		if (!baseId)
			return;

		unsigned int id = *baseId;

		if (id == 0x0005AD9E) // Gold ingots to Orichalum ingots
			id = 0x0005AD99;

		baseObject = esp_get_form_id(id);

		if (!baseObject.valid())
		{
			char hex[10];
			snprintf(hex, 10, "%08X", id);
			printf("\ncant find form_id %s %u\n\n", hex, id);
			return;
		}
		else
		{
		}

		// "STAT", "DOOR", "FURN", "BOOK", "ALCH", "CONT", "ARMO", "WEAP", "FLOR", "TREE", "SLGM", "SCRL", "INGR", "MISC"
		
		if (baseObject.sigany({
			"STAT",
			"DOOR",
			"FURN",
			"BOOK",
			"CONT",
			"ARMO",
			"WEAP",
			"MISC",
			"ALCH",
			"INGR"
			}))
		{
#if SNPRINTF_FORM_ID
			printf("refr has base %s - %s\n", baseObject.rcd->form_id->hex, baseObject.editorId());
#endif
			auto modl = baseObject.data<const char *>("MODL", 0);
			if (!modl)
			{
				printf("um no modl here\n");
			}
			else
			{
				mesh = create_simple_mesh_from_modl(modl, true);
			}
		}
		else if (baseObject.sig(__LIGH__))
		{
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

			auto editorId = baseObject.editorId();
			auto DATA = baseObject.data<int *>(_DATA_);
			auto FNAM = baseObject.data<float *>(_FNAM_);

			Struct *data = (Struct *)DATA;

			if (editorId)
			{
				//printf("ligh edid %s\n", LIGH.baseId);
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
				light = spotLight = new SpotLight;
				//printf("data fov %f\n", data->FOV);
				spotLight->angle = radians(data->FOV);
				sceneDef->spotLights.Add(spotLight);
				printf(" spotLight ! %f \n", spotLight->angle);
			}
			else
			{
				light = pointLight = new PointLight;
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

			//point_light->decay = _j["Falloff Exponent"];
		}
		else
		{
			baseObject = nullptr; // invalidate
		}

		if (mesh)
		{
			if (baseObject.rcd->hed->formId != 0x32)
			{
				drawGroup = new DrawGroupSortable(mesh->baseGroup, matrix);
				sceneDef->drawGroups.Add(drawGroup);
			}
		}
	}

	float Ref::getDistance()
	{
		float distance = glm::length(drawGroup->aabb.center() - vec3(personCam->hands->matrixWorld[3]));

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

	bool Ref::displayAsItem()
	{
		float dist = getDistance() * CM_TO_SKYRIM_UNITS;

		// printf(" display as item %f\n", dist);

		if (dist > 40)
		{
			return false;
		}

		auto FULL = baseObject.data<char *>(_FULL_);
		auto DESC = baseObject.data<const char *>(_DESC_);

		const char *itemName = "Something";
		if (FULL)
		{
			itemName = FULL;
		}

		vec3 original = vec3(0);

		mat4 mate = glm::translate(mat4(1), drawGroup->aabb.center());

		mat4 model = cameraCur->projection * cameraCur->view * mate;
		mat4 projection = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
		vec4 viewport(0.0f, 0.0f, width, height);

		vec3 projected = glm::project(original, model, projection, viewport);
		//vec3 unprojected = glm::unProject(projected, model, projection, viewport);

		ImGui::SetNextWindowPos(ImVec2(width - projected.x, projected.y));
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;
		static bool open = true;
		//char s[100];
		ImGui::Begin("##Item", &open, flags);
		ImGui::PushFont(font3);
		//ImGui::PushStyleColor(IMGUI_FON)
		//ImGui::Text(itemName);
		ImGui::TextColored(ImVec4(1, 1, 1, 1), itemName);

		if (DESC)
		{
			ImGui::TextWrapped(DESC);
		}
		ImGui::PopFont();
		ImGui::End();
		//Vec2 boo(x, y);

		return true;
	}

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