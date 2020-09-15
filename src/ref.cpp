#include "ref.h"
#include "mesh.h"

#include <algorithm>
#include <cctype>
#include <string>

extern "C"
{
//#include "putc.h"
#include "bsa.h"
#include <nifp/nifp.h>
#include "esp.h"
}

#include "files.h"

#include <opengl/renderable.h>
#include <opengl/texture.h>
#include <opengl/camera.h>
#include <opengl/pointlight.h>
#include <opengl/types.h>

#include <imgui.h>
#include <gui/extra.h>

constexpr char test_expr[] = "EDID";

#define Fields object->fields

namespace dark2
{
	Refr::Refr(record *record)
	{
		auto array = &record->fields;
		for (int i = 0; i < array->size; i++)
		{
			auto field = (array->subrecords[i]);
			if (field->hed->type == espwrd "EDID")
				EDID = ((char *)field->data);
			if (field->hed->type == espwrd "XSCL")
				XSCL = ((float *)field->data);
			if (field->hed->type == espwrd "NAME")
				NAME = ((unsigned int *)field->data);
			if (field->hed->type == espwrd "DATA")
				DATA = ((unsigned char *)field->data);
		}
	}

	Ligh::Ligh(record *record)
	{
		auto array = &record->fields;
		for (int i = 0; i < array->size; i++)
		{
			auto field = (array->subrecords[i]);
			if (field->hed->type == espwrd "EDID")
				EDID = ((char *)field->data);
			if (field->hed->type == espwrd "FNAM")
				FNAM = ((float *)field->data);
			if (field->hed->type == espwrd "DATA")
				DATA = ((unsigned char *)field->data);
		}
	}

	Weap::Weap(record *record)
	{
		auto array = &record->fields;
		for (int i = 0; i < array->size; i++)
		{
			auto field = (array->subrecords[i]);
			if (field->hed->type == espwrd "EDID")
				EDID = ((char *)field->data);
			if (field->hed->type == espwrd "FULL")
				FULL = ((char *)field->data);
			if (field->hed->type == espwrd "DESC")
				DESC = ((char *)field->data);
		}
	}

	Ref::Ref()
	{
		Group *group = new Group();
	}

	Ref::~Ref()
	{
		scene->Remove(renderable);
		scene->Remove(pointlight);
		delete renderable;
		delete pointlight;
	}

	void Ref::SetData(record *refr)
	{
		REFR = Refr(refr);

		matrix = mat4(1.0);

		unsigned int formIdName = 0;

		mat4 translation(1.0), rotation(1.0), scale(1.0);

		if (REFR.EDID)
		{
			EDID = REFR.EDID;
		}
		if (REFR.XSCL)
		{
			scale = glm::scale(mat4(1.0), vec3(*REFR.XSCL));
		}
		if (REFR.DATA)
		{
			DATA = (float *)REFR.DATA;
			vec3 pos = *cast_vec_3(((float *)REFR.DATA));
			vec3 rad = *cast_vec_3(((float *)REFR.DATA + 3));

			translation = translate(mat4(1.0f), pos);

			//glm::quat rot = glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f))
			rotation = rotate(rotation, -rad.x, vec3(1, 0, 0));
			rotation = rotate(rotation, -rad.y, vec3(0, 1, 0));
			rotation = rotate(rotation, -rad.z, vec3(0, 0, 1));

			matrix = translation * rotation * scale;
		}
		if (REFR.NAME)
		{
			formIdName = *REFR.NAME;

			baseObject = esp_brute_record_by_form_id(formIdName);

			Assert(baseObject, "ref cant find name baseObject");

			if (baseObject->hed->type == espwrd "STAT" ||
				//baseObject->hed->type == espwrd"FURN" ||
				baseObject->hed->type == espwrd "ALCH" ||
				baseObject->hed->type == espwrd "CONT" ||
				baseObject->hed->type == espwrd "ARMO" ||
				baseObject->hed->type == espwrd "WEAP" ||
				baseObject->hed->type == espwrd "FLOR" ||
				baseObject->hed->type == espwrd "MISC")
			{
				for (int i = 0; i < baseObject->fields.size; i++)
				{
					subrecord *field = baseObject->fields.subrecords[i];
					if (field->hed->type != espwrd "MODL")
						continue;
					mesh = Mesh::GetStored(field->data);
					if (!mesh)
					{
						std::string data = (char *)field->data;
						data = "meshes\\" + data;
						std::transform(data.begin(), data.end(), data.begin(),
									   [](unsigned char c) { return std::tolower(c); });
						//printf("stat baseObject has a modl %s\n", data.c_str());
						rc *rc = bsa_find_more(data.c_str(), 0x1);
						if (rc)
						{
							//printf("found a rc %p\n", rc);
							nifp *nif = nifp_saved(rc);
							if (nif == NULL)
							{
								nif = loadNif(rc);
								nifp_save(rc, nif);
							}
							mesh = new Mesh;
							mesh->Construct(nif);
							Mesh::Store(field->data, mesh);
						}
					}
				}

				if (baseObject->hed->type == espwrd "WEAP")
				{
					WEAP = new Weap(baseObject);
				}
			}
			else if (baseObject->hed->type == espwrd "LIGH")
			{
				Ligh LIGH(baseObject);

				pointlight = new PointLight;
				scene->Add(pointlight);

				if (LIGH.EDID)
				{
					//printf("ligh edid %s\n", LIGH.EDID);
				}
				if (LIGH.DATA)
				{
					int time = *(int *)LIGH.DATA;
					pointlight->distance = *((unsigned int *)LIGH.DATA + 1);
					unsigned int rgb = *((unsigned int *)LIGH.DATA + 2);
					unsigned char r = (rgb >> (8 * 0)) & 0xff;
					unsigned char g = (rgb >> (8 * 1)) & 0xff;
					unsigned char b = (rgb >> (8 * 2)) & 0xff;
					pointlight->color = vec3(r, g, b) / 255.f;
				}
				if (LIGH.FNAM)
				{
					float fade = *LIGH.FNAM;
					fade = 1 / fade;
					pointlight->decay = fade;
				}

				//point_light->decay = _j["Falloff Exponent"];
			}
		}

		if (mesh)
		{
			if (formIdName != 0x32)
			{
				renderable = new Renderable(matrix, mesh->baseGroup);
				scene->Add(renderable);
			}
		}
		if (pointlight)
		{
			pointlight->matrix = matrix;
		}
	}

	float Ref::GetDistance()
	{
		float distance = glm::length(vec3(matrix[3]) - camera->pos);

		return distance;
	}

	bool Ref::DisplayAsItem()
	{
		if (GetDistance() > 250)
		{
			return false;
		}

		char *itemName = "Sometihng";
		if (WEAP)
		{
			itemName = WEAP->FULL + 0;
		}

		vec3 original = vec3(0);

		mat4 model = camera->projection * camera->view * matrix;
		mat4 projection = frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
		vec4 viewport(0.0f, 0.0f, width, height);

		vec3 projected = glm::project(original, model, projection, viewport);
		//vec3 unprojected = glm::unProject(projected, model, projection, viewport);

		ImGui::SetNextWindowPos(ImVec2(width - projected.x, projected.y));
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;
		static bool open = true;
		//char s[100];
		ImGui::Begin("##Item", &open, flags);
		ImGui::PushFont(font2);
		ImGui::Text(itemName);
		if (WEAP && WEAP->DESC)
		{
			ImGui::TextWrapped(WEAP->DESC);
		}
		ImGui::PopFont();
		ImGui::End();
		//Vec2 boo(x, y);

		return true;
	}

	/*
	Matrix m = _refr->_matrix;
	m.setTrans(m.getTrans() + Vec3(0, 0, 15 / ONE_SKYRIM_UNIT_IN_CM));

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

} // namespace dark2