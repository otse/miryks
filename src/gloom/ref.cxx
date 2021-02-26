#include <skyrim_units>
#include <libs>
#include <Gloom/Ref.h>

#include <Gloom/Util.h>
#include <Gloom/Object.h>
#include <Gloom/Mesh.h>
#include <Gloom/Files.h>

#include <algorithm>
#include <cctype>
#include <string>

#include <OpenGL/Texture.h>
#include <OpenGL/Camera.h>
#include <OpenGL/Lights.h>
#include <OpenGL/Group.h>
#include <OpenGL/DrawGroup.h>
#include <OpenGL/Types.h>

#include <imgui.h>
#include <gui/extra.h>

namespace gloom
{
	Ref::Ref(::Record *record) : selfObject(record)
	{
		mesh = nullptr;
		drawGroup = nullptr;
		pointLight = nullptr;
		spotLight = nullptr;
		Go();
	}

	Ref::~Ref()
	{
		scene->drawGroups.Remove(drawGroup);
		scene->pointLights.Remove(pointLight);
		scene->spotLights.Remove(spotLight);
		delete drawGroup;
		delete pointLight;
		delete spotLight;
	}

	void Ref::Go()
	{
		matrix = mat4(1.0);

		translation = rotation = scale = mat4(1.0);

		auto baseId = GetBaseId(selfObject);
		auto editorId = GetEditorId(selfObject);
		auto XSCL = selfObject.Data<float *>("XSCL");
		auto locationalData = selfObject.Data<float *>("DATA");

		if (editorId)
			this->editorId = editorId;

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

	void Ref::forBaseId(unsigned int *baseId)
	{
		if (!baseId)
			return;

		unsigned int id = *baseId;

		if (id == 0x0005AD9E) // Gold ingots to Orichalum ingots
			id = 0x0005AD99;

		baseObject.Set(esp_get_form_id(id));

		cassert(baseObject.Valid(), "cant find refs Name-BaseId record");

		if (baseObject.IsTypeAny({"STAT", "DOOR", "ALCH", "CONT",
								  "ARMO", "WEAP", "FLOR", "TREE", "MISC"}))
		{
			mesh = util::GrantMesh(baseObject);
		}
		else if (baseObject.IsType("LIGH"))
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

			auto editorId = GetEditorId(baseObject);
			auto DATA = baseObject.Data<int *>("DATA");
			auto FNAM = baseObject.Data<float *>("FNAM");

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
				scene->spotLights.Add(spotLight);
				printf(" spotLight ! %f \n", spotLight->angle);
			}
			else
			{
				light = pointLight = new PointLight;
				scene->pointLights.Add(pointLight);
				if (data->flags & 0x1)
					0; // pointLight->shadow->enabled = true;
			}

			if (light)
			{
				light->color = color;
				light->decay = fade;
				light->distance = data->radius;
				light->matrix = matrix;
			}

			//point_light->decay = _j["Falloff Exponent"];
		}

		if (mesh)
		{
			if (baseObject.record->hed->formId != 0x32)
			{
				drawGroup = new DrawGroupSortable(mesh->baseGroup, matrix);
				scene->drawGroups.Add(drawGroup);
			}
		}
	}

	float Ref::GetDistance()
	{
		float distance = glm::length(drawGroup->aabb.center() - vec3(first_person_camera->hands->matrixWorld[3]));

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
		cassert(ret == Z_OK, "esp zlib");
		rec->actualSize = realSize;
		Count.uncompress++;
	}*/

	// Todo, Omg !

	bool Ref::DisplayAsItem()
	{
		float dist = GetDistance() * CM_TO_SKYRIM_UNITS;

		// printf(" display as item %f\n", dist);

		if (dist > 40)
		{
			return false;
		}

		auto FULL = baseObject.Data<char *>("FULL");
		auto DESC = baseObject.Data<const char *>("DESC");

		char *itemName = "Something";
		if (FULL)
		{
			itemName = FULL;
		}

		vec3 original = vec3(0);

		mat4 mate = glm::translate(mat4(1), drawGroup->aabb.center());

		mat4 model = camera->projection * camera->view * mate;
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

} // namespace gloom