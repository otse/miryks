// big include blob !!

// needs a big cleanup sometime

#include <miryks/reference.h>

#include <miryks/record.h>
#include <miryks/grup.h>
#include <miryks/model.h>
#include <miryks/trash.h>

#include <algorithm>
#include <cctype>
#include <string>

#include <opengl/texture.h>
#include <opengl/camera.h>
#include <opengl/lights.h>
#include <opengl/group.h>
#include <opengl/drawgroup.h>
#include <opengl/renderer.h>

#include <imgui.h>
#include <panels.h>

namespace miryks
{
	Reference::Reference(Record refr) : Record(refr)
	{
		model = nullptr;
		drawGroup = nullptr;
		pointLight = nullptr;
		spotLight = nullptr;
		Go();
	}

	Reference::~Reference()
	{
		if (drawGroup && drawGroup->parent)
			drawGroup->parent->Remove(drawGroup);
		sceneDef->pointLights.Remove(pointLight);
		sceneDef->spotLights.Remove(spotLight);
		delete drawGroup;
		delete pointLight;
		delete spotLight;
	}

	void Reference::Step()
	{
		if (baseObject.is_type("MSTT"))
		{
			if(model)
				model->Step();
		}
	}

	void Reference::Go()
	{
		matrix = mat4(1.0);

		translation = rotation = scale = mat4(1.0);

		formId baseId = base();

		auto XSCL = data<float *>("XSCL");
		auto locationalData = data<float *>("DATA");

		if (XSCL)
			scale = glm::scale(mat4(1.0), vec3(*XSCL));

		ForLocationalData(locationalData);
		ForBaseId(baseId);
	}

	// bad
	void Reference::ForLocationalData(float *locationalData)
	{
		if (!locationalData)
			return;

		vec3 pos = cast_vec3(locationalData);
		vec3 rad = cast_vec3(locationalData + 3);

		translation = translate(mat4(1.0f), pos);

		rotation = glm::rotate(rotation, -rad.x, vec3(1, 0, 0));
		rotation = glm::rotate(rotation, -rad.y, vec3(0, 1, 0));
		rotation = glm::rotate(rotation, -rad.z, vec3(0, 0, 1));

		matrix = translation * rotation * scale;
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
	void Reference::ForBaseId(formId baseId)
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
		
		std::vector<const char *> things = {
			Statics,
			Plants,
			Doors,
			Furniture,
			Books,
			Containers,
			Armor,
			Weapons,
			Ammo,
			Misc,
			Alchemy,
			Ingredients
		};
		
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
				model = create_simple_model_from_modl(modl);
			}
			if (baseObject.is_type(CONT))
			{
				container = new Container(baseObject);
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
			auto edId = baseObject.editor_id();
			
			printf("ref.cpp cant become %.4s edid %s\n", (char *)&baseObject.rhed().sgn, edId);
			
			baseObject = nullptr; // invalidate
		}

		if (model)
		{
			// bad
			if (baseObject.r->hed->formId != 0x32)
			{
				drawGroup = new DrawGroupFlatSorted(model->baseGroup, matrix);
				sceneDef->bigGroup->Add(drawGroup);
				int i = 0;
				for (auto thing : Things)
				{
					if (*(unsigned int *)thing == baseObject.rhed().sgn) {
						drawGroup->mask = 1 << i;
						break;
					}
					i ++;
				}
			}
		}
	}

	float Reference::GetDistance() const
	{
		if (drawGroup == nullptr)
			return 0;
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

	bool myfunction(Reference *l, Reference *r)
	{
		return l->GetDistance() < r->GetDistance();
	}

	// very bad namespace
	namespace Refs {
		Reference *handRef = nullptr;
		std::vector<Reference *> labelled;
		bool labelingEnabled = true;
		vec3 projected;
		void Init()
		{
		}
		void Nearby()
		{
			if (!labelingEnabled)
				return;
				
			std::sort(labelled.begin(), labelled.end(), [](const Reference *l, const Reference *r) -> bool {
				return l->GetDistance() < r->GetDistance();
			});

			handRef = nullptr;

			for (Reference *ref : labelled)
				if (ref->DisplayAsItem())
					return;
		}

		void Activate() {
			if (handRef)
				handRef->Use();
		}
	};


	bool Reference::Use() {
		printf("Use %s\n", baseObject.editor_id());
		if (baseObject.is_type(CONT) && container)
			container->Activate();
		return true;
	}

	// horrible imgui vomit
	bool Reference::DisplayAsItem()
	{
		float dist = GetDistance() * CM_TO_SKYRIM_UNITS;

		if (dist > 40)
			return false;

		if (drawGroup == nullptr)
			return false;


		Refs::handRef = this;

		auto EDID = baseObject.data<char *>("EDID");
		auto FULL = baseObject.data<char *>("FULL");
		auto DESC = baseObject.data<const char *>("DESC");

		const char *itemName = "Something";
		if (FULL)
			itemName = FULL;

		vec3 original = vec3(0);

		mat4 mate = glm::translate(mat4(1), drawGroup->aabb.center());

		mat4 model = cameraCur->projection * cameraCur->view * mate;
		mat4 projection = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
		vec4 viewport(0.0f, 0.0f, width, height);

		vec3 projected = glm::project(original, model, projection, viewport);
		Refs::projected = projected;
		//vec3 unprojected = glm::unProject(projected, model, projection, viewport);

		ImGui::SetNextWindowPos(ImVec2(width - projected.x, projected.y));
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;
		static bool open = true;
		//char s[100];
		ImGui::Begin("##ItemThing", &open, flags);
		ImGui::PushFont(font2);
		//ImGui::PushStyleColor(IMGUI_FON)
		//ImGui::TextColored(ImVec4(1, 1, 1, 1), EDID);
		//ImGui::NewLine();
		ImGui::TextColored(ImVec4(1, 1, 1, 1), itemName);

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