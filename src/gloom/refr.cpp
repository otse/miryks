#include <gloom/refr.h>
#include <gloom/object.h>

#include <gloom/mesh.h>
#include <gloom/files.h>

#include <gloom/libs>

#include <algorithm>
#include <cctype>
#include <string>

#include <opengl/texture.h>
#include <opengl/camera.h>
#include <opengl/pointlight.h>
#include <opengl/types.h>

#include <imgui.h>
#include <gui/extra.h>

namespace gloom
{
	Ref::Ref(::record *record)
	{
		mesh = nullptr;
		drawGroup = nullptr;
		pointlight = nullptr;

		self = new Object(record);
		Go();
	}

	Ref::~Ref()
	{
		scene->Remove(drawGroup);
		scene->Remove(pointlight);
		delete drawGroup;
		delete pointlight;
	}

	void Ref::Go()
	{
		matrix = mat4(1.0);

		mat4 translation(1.0), rotation(1.0), scale(1.0);

		if (self->EDID)
		{
			//EDID = self->EDID;
		}
		if (self->XSCL)
		{
			scale = glm::scale(mat4(1.0), vec3(*self->XSCL));
		}
		if (self->DATA)
		{
			//DATA = (float *)self->DATA;
			vec3 pos = *cast_vec_3(((float *)self->DATA));
			vec3 rad = *cast_vec_3(((float *)self->DATA + 3));

			translation = translate(mat4(1.0f), pos);

			//glm::quat rot = glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f))
			rotation = rotate(rotation, -rad.x, vec3(1, 0, 0));
			rotation = rotate(rotation, -rad.y, vec3(0, 1, 0));
			rotation = rotate(rotation, -rad.z, vec3(0, 0, 1));

			matrix = translation * rotation * scale;
		}
		if (self->NAME)
		{
			::record *rbase = esp_brute_record_by_form_id(*self->NAME);
			base = new Object(rbase);

			cassert(rbase, "ref cant find name baseRecord");

			if (base->IsAny({"STAT", "ALCH", "CONT", "ARMO", "WEAP", "FLOR", "MISC"}))
			{
				for (int i = 0; i < base->record->fields.size; i++)
				{
					subrecord *field = base->record->fields.subrecords[i];
					if (field->hed->type != espwrd "MODL")
						continue;
					mesh = Mesh::GetStored(field->data);
					if (!mesh)
					{
						std::string data = (char *)field->data;
						data = "meshes\\" + data;
						std::transform(data.begin(), data.end(), data.begin(),
									   [](unsigned char c) { return std::tolower(c); });
						//printf("stat baseRecord has a modl %s\n", data.c_str());
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

				if (base->Is("WEAP"))
				{
					//WEAP = new Weap(baseRecord);
				}
			}
			else if (base->Is("LIGH"))
			{
				//Ligh LIGH(baseRecord);

				pointlight = new PointLight;
				scene->Add(pointlight);

				if (base->EDID)
				{
					//printf("ligh edid %s\n", LIGH.EDID);
				}
				if (base->DATA)
				{
					int time = *(int *)base->DATA;
					pointlight->distance = *((unsigned int *)base->DATA + 1);
					unsigned int rgb = *((unsigned int *)base->DATA + 2);
					unsigned char r = (rgb >> (8 * 0)) & 0xff;
					unsigned char g = (rgb >> (8 * 1)) & 0xff;
					unsigned char b = (rgb >> (8 * 2)) & 0xff;
					pointlight->color = vec3(r, g, b) / 255.f;
				}
				if (base->FNAM)
				{
					float fade = *base->FNAM;
					fade = 1 / fade;
					pointlight->decay = fade;
				}

				//point_light->decay = _j["Falloff Exponent"];
			}
		}

		if (mesh)
		{
			if (base->record->hed->formId != 0x32)
			{
				drawGroup = new DrawGroup(mesh->baseGroup, matrix);
				scene->Add(drawGroup);
			}
			aabb = AABB::mult(mesh->aabb, matrix);
		}
		if (pointlight)
		{
			pointlight->matrix = matrix;
		}
	}

	float Ref::GetDistance()
	{
		mat4 mate = translate(mat4(1), aabb.center());

		float distance = glm::length(vec3(mate[3]) - camera->pos);

		return distance;
	}

	bool Ref::DisplayAsItem()
	{
		if (GetDistance() > 600 / ONE_SKYRIM_UNIT_IN_CM)
			return false;

		char *itemName = "Sometihng";
		if (base->FULL)
		{
			itemName = base->FULL;
		}

		vec3 original = vec3(0);

		mat4 mate = translate(mat4(1), aabb.center());
		//mate = translate(mate, vec3(0, 0, 50 / ONE_SKYRIM_UNIT_IN_CM));

		mat4 model = camera->projection * camera->view * mate;
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
		if (base->DESC)
		{
			ImGui::TextWrapped(base->DESC);
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

} // namespace gloom