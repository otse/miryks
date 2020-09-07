#include "level.h"
#include "mesh.h"

#include <algorithm>
#include <cctype>
#include <string>

#include "files"

#include "opengl/renderable"
#include "opengl/texture"
#include "opengl/camera"
#include "opengl/pointlight"
#include "opengl/types"

namespace dark2
{
	Cell Level::GetCell(const char *editorId)
	{
		Cell cell = {false};

		Grup *top = esp_get_top_grup(testMod, "CELL");

		Assert(top, "no cells top grup");

		for (int i = 0; i < top->mixed.size; i++)
		{
			Grup *A = (Grup *)top->mixed.elements[i];
			for (int j = 0; j < A->mixed.size; j++)
			{
				Grup *B = (Grup *)A->mixed.elements[j];
				for (int k = 0; k < B->mixed.size; k += 2)
				{
					Record *record = (Record *)B->mixed.elements[k];
					Grup *grup = (Grup *)B->mixed.elements[k + 1];
					Assert(record->head->type == *(unsigned int *)"CELL", "not a cell");
					const char *cellEdid = (char *)((Subrecord *)record->fields.elements[0])->data;
					//printf("Found cell %s\n", cellEdid);
					if (0 == strcmp(cellEdid, editorId))
					{
						printf("Found your cell %s\n", cellEdid);
						cell.cell = record;
						cell.persistent = (Grup *)grup->mixed.elements[0];
						cell.non_persistent = (Grup *)grup->mixed.elements[1];
						return cell;
					}
				}
			}
		}

		return cell;
	}

	Level::Level()
	{
		Group *group = new Group();

		Cell cell = GetCell("Dark2Schmuck");

		LoadCell(cell);
	}

	void Level::ParseGrup(Cell &cell, Grup *grup)
	{
		for (int i = 0; i < grup->mixed.size; i++)
		{
			void *element = grup->mixed.elements[i];
			Assert(*(char *)element == RECORD, "cell mixed has non record");
			auto ref = new Reference;
			ref->SetData((Record *)element);
			if (ref->EDID)
				editorIds.insert({ref->EDID, ref});
		}

		auto ref = editorIds.find("darkshackspawn");
		if (ref != editorIds.end())
		{
			first_person_camera->pos = ref->second->matrix[3];
			first_person_camera->pos.z += 170 / ONE_SKYRIM_UNIT_IN_CM;
			first_person_camera->fyaw = cast_vec_3(ref->second->DATA + 3)->z;
		}
	}

	void Level::LoadCell(Cell &cell)
	{
		//ParseGrup(cell, cell.persistent);
		ParseGrup(cell, cell.non_persistent);
	}

	void Level::Update()
	{
	}

	Reference::Reference()
	{
		Group *group = new Group();
	}

	void Reference::SetData(Record *record)
	{
		matrix = mat4(1.0);

		mat4 translation(1.0), rotation(1.0), scale(1.0);

		for (int i = 0; i < record->fields.size; i++)
		{
			Subrecord *field = (Subrecord *)record->fields.fields[i];
			if (field->head->type == *(unsigned int *)"EDID")
			{
				this->EDID = (char *)field->data;
			}
			if (field->head->type == *(unsigned int *)"XSCL")
			{
				scale = glm::scale(mat4(1.0), vec3(*(float *)field->data));
			}
			if (field->head->type == *(unsigned int *)"DATA")
			{
				this->DATA = (float *)field->data;
				vec3 pos, rad;
				pos = *cast_vec_3((float *)field->data);
				rad = *cast_vec_3((float *)field->data + 3);
				//printf("Reference has DATA pos %f %f %f\n", pos.x, pos.y, pos.z);
				//printf("Reference has DATA rad %f %f %f\n", rad.x, rad.y, rad.z);

				translation = translate(mat4(1.0f), pos);

				//glm::quat rot = glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f))
				rotation = rotate(rotation, -rad.x, vec3(1, 0, 0));
				rotation = rotate(rotation, -rad.y, vec3(0, 1, 0));
				rotation = rotate(rotation, -rad.z, vec3(0, 0, 1));
			}
			if (field->head->type == *(unsigned int *)"NAME")
			{
				unsigned int formId = *(unsigned int *)field->data;

				Record *base = esp_brute_record_by_form_id(formId);
				Assert(base, "ref cant find name base");
				if (base->head->type == *(unsigned int *)"STAT" ||
					base->head->type == *(unsigned int *)"CONT" ||
					base->head->type == *(unsigned int *)"ARMO" ||
					base->head->type == *(unsigned int *)"WEAP" ||
					base->head->type == *(unsigned int *)"MISC")
				{
					for (int i = 0; i < base->fields.size; i++)
					{
						Subrecord *field = (Subrecord *)base->fields.fields[i];
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
					float radius = 400 / ONE_SKYRIM_UNIT_IN_CM;
					vec3 color = vec3(1, 0, 0);
					for (int i = 0; i < base->fields.size; i++)
					{
						if (field->head->type == *(unsigned int *)"DATA")
						{
							radius = *(float *)field->data;
							//color.x = (field->data + 8 + 2);
							//color.y = (field->data + 8 + 3);
							//color.z = (field->data + 8 + 4);
						}
					}

					pl = new PointLight;
					pl->distance = radius;

					//point_light->decay = _j["Falloff Exponent"];

					pl->color = vec3(color);
					//pl->color /= 255.f;
					scene->Add(pl);
				}
			}
		}

		matrix = translation * rotation * scale;

		if (mesh)
		{
			Renderable *object = new Renderable(matrix, mesh->baseGroup);
			scene->Add(object);
		}
		if (pl)
		{
			printf("add a ligh\n");

			pl->matrix = matrix;
			scene->Add(pl);
		}
	}

} // namespace dark2