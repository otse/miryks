#include "ref.h"
#include "mesh.h"

#include <algorithm>
#include <cctype>
#include <string>

extern "C"
{
#include "c.h"
#include "bsa.h"
#include "nif.h"
#include "esp.h"
}

#include "files"

#include "opengl/renderable"
#include "opengl/texture"
#include "opengl/camera"
#include "opengl/pointlight"
#include "opengl/types"

constexpr char test_expr[] = "EDID";

#define X *(unsigned int *)

namespace dark2
{
	REFR::REFR(Record *record)
	{
		esp_array_each(record->fields, each_field);
	}

	void each_field(Subrecord *field, void *data)
	{
		REFR *REFR = (REFR *)data;
		if (field->hed->type == *(unsigned int *)"EDID")
			EDID = ((char *)sub->data);
		if (field->hed->type == *(unsigned int *)"XSCL")
			XSCL = ((float *)sub->data);
		if (field->hed->type == *(unsigned int *)"NAME")
			NAME = ((unsigned int *)sub->data);
		if (field->hed->type == *(unsigned int *)"DATA")
			DATA = ((unsigned char *)sub->data);
	}

	Ref::Ref()
	{
		Group *group = new Group();
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
			vec3 pos, rad;
			pos = *cast_vec_3(((float *)REFR.DATA));
			rad = *cast_vec_3(((float *)REFR.DATA + 3));

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
			if (base->hed->type == X "STAT" ||
				//base->hed->type == X"FURN" ||
				base->hed->type == X "ALCH" ||
				base->hed->type == X "CONT" ||
				base->hed->type == X "ARMO" ||
				base->hed->type == X "WEAP" ||
				base->hed->type == X "MISC")
			{
				for (int i = 0; i < base->fields.size; i++)
				{
					Field *field = base->fields.fields[i];
					if (field->hed->type != X "MODL")
						continue;
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
							nif = make_nif(rc);
							nif_save(rc, nif);
						}
						mesh = new Mesh;
						mesh->Construct(nif);
					}
				}
			}
			else if (base->hed->type == X "LIGH")
			{
				pl = new PointLight;
				scene->Add(pl);

				for (int i = 0; i < base->fields.size; i++)
				{
					Field *field = base->fields.fields[i];
					if (field->hed->type == X "EDID")
					{
						printf("ligh edid %s\n", field->data);
					}
					else if (field->hed->type == X "DATA")
					{
						int time = *(int *)field->data;
						pl->distance = *((unsigned int *)field->data + 1);
						unsigned int rgb = *((unsigned int *)field->data + 2);
						unsigned char r = (rgb >> (8 * 0)) & 0xff;
						unsigned char g = (rgb >> (8 * 1)) & 0xff;
						unsigned char b = (rgb >> (8 * 2)) & 0xff;
						pl->color = vec3(r, g, b) / 255.f;
					}
				}

				//point_light->decay = _j["Falloff Exponent"];
			}
		}

		if (mesh)
		{
			Renderable *object = new Renderable(matrix, mesh->baseGroup);
			scene->Add(object);
		}
		if (pl)
		{
			pl->matrix = matrix;
		}
	}

} // namespace dark2