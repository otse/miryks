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

struct REFR
{
	char *EDID;
	float *XSCL;
	unsigned int *NAME;
	struct
	{
		float position[3], rotation[3];
	} * DATA;
	REFR(Record *record)
	{
		for (int i = 0; i < record->fields.size; i++)
		{
			Field *sub = record->fields.fields[i];
			if (sub->head->type == X"EDID")
				EDID = ((char *)sub->data);
			if (sub->head->type == X"XSCL")
				XSCL = ((float *)sub->data);
			if (sub->head->type == X"NAME")
				NAME = ((unsigned int *)sub->data);
		};
	}
};

namespace dark2
{
	Reference::Reference()
	{
		Group *group = new Group();
	}

	void Reference::SetData(Record *refr)
	{
		REFR REFR(refr);
		matrix = mat4(1.0);

		mat4 translation(1.0), rotation(1.0), scale(1.0);

		for (int i = 0; i < refr->fields.size; i++)
		{
			Field *sub = refr->fields.fields[i];
			if (sub->head->type == X"EDID")
			{
				this->EDID = (char *)sub->data;
			}
			else if (sub->head->type == X"XSCL")
			{
				scale = glm::scale(mat4(1.0), vec3(*(float *)sub->data));
			}
			else if (sub->head->type == X"DATA")
			{
				this->DATA = (float *)sub->data;
				vec3 pos, rad;
				pos = *cast_vec_3((float *)sub->data);
				rad = *cast_vec_3((float *)sub->data + 3);

				translation = translate(mat4(1.0f), pos);

				//glm::quat rot = glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f))
				rotation = rotate(rotation, -rad.x, vec3(1, 0, 0));
				rotation = rotate(rotation, -rad.y, vec3(0, 1, 0));
				rotation = rotate(rotation, -rad.z, vec3(0, 0, 1));

				matrix = translation * rotation * scale;
			}
			else if (sub->head->type == X"NAME")
			{
				unsigned int formId = *(unsigned int *)sub->data;

				Record *base = esp_brute_record_by_form_id(formId);
				Assert(base, "ref cant find name base");
				if (base->head->type == X"STAT" ||
					//base->head->type == *(unsigned int *)"FURN" ||
					base->head->type == X"ALCH" ||
					base->head->type == X"CONT" ||
					base->head->type == X"ARMO" ||
					base->head->type == X"WEAP" ||
					base->head->type == X"MISC")
				{
					for (int i = 0; i < base->fields.size; i++)
					{
						Field *field = base->fields.fields[i];
						if (field->head->type != *(unsigned int *)"MODL")
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
				else if (base->head->type == *(unsigned int *)"LIGH")
				{
					pl = new PointLight;
					scene->Add(pl);

					for (int i = 0; i < base->fields.size; i++)
					{
						Field *field = base->fields.fields[i];
						if (field->head->type == *(unsigned int *)"EDID")
						{
							printf("ligh edid %s\n", field->data);
						}
						else if (field->head->type == *(unsigned int *)"DATA")
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