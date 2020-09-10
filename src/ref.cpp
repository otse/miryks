#include "ref.h"
#include "mesh.h"

#include <algorithm>
#include <cctype>
#include <string>

extern "C"
{
//#include "putc.h"
#include "bsa.h"
#include "nif.h"
#include "esp.h"
}

#include "files.h"

#include "opengl/renderable"
#include "opengl/texture"
#include "opengl/camera"
#include "opengl/pointlight"
#include "opengl/types"

constexpr char test_expr[] = "EDID";

#define Fields object->fields

namespace dark2
{
	REFR::REFR(Record *record)
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

	LIGH::LIGH(Record *record)
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

	void Ref::SetData(Record *refr)
	{
		REFR REFR(refr);
		matrix = mat4(1.0);

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
			unsigned int formId = *REFR.NAME;

			Record *base = esp_brute_record_by_form_id(formId);
			Assert(base, "ref cant find name base");
			if (base->hed->type == espwrd "STAT" ||
				//base->hed->type == espwrd"FURN" ||
				base->hed->type == espwrd "ALCH" ||
				base->hed->type == espwrd "CONT" ||
				base->hed->type == espwrd "ARMO" ||
				base->hed->type == espwrd "WEAP" ||
				base->hed->type == espwrd "MISC")
			{
				for (int i = 0; i < base->fields.size; i++)
				{
					subrecord *field = base->fields.subrecords[i];
					if (field->hed->type != espwrd "MODL")
						continue;
					mesh = Mesh::GetStored(field->data);
					if (!mesh)
					{
						std::string data = (char *)field->data;
						data = "meshes\\" + data;
						std::transform(data.begin(), data.end(), data.begin(),
									   [](unsigned char c) { return std::tolower(c); });
						//printf("stat base has a modl %s\n", data.c_str());
						Rc *rc = bsa_find(meshes, data.c_str());
						if (rc)
						{
							//printf("found a rc %p\n", rc);
							Nif *nif = nif_saved(rc);
							if (nif == NULL)
							{
								nif = LoadNif(rc);
								nif_save(rc, nif);
							}
							mesh = new Mesh;
							mesh->Construct(nif);
							Mesh::Store(field->data, mesh);
						}
					}
				}
			}
			else if (base->hed->type == espwrd "LIGH")
			{
				LIGH LIGH(base);

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
			renderable = new Renderable(matrix, mesh->baseGroup);
			scene->Add(renderable);
		}
		if (pointlight)
		{
			pointlight->matrix = matrix;
		}
	}

} // namespace dark2