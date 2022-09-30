extern "C"
{
#include <half.h>
}

#include <miryks/model.h>

#include <opengl/shader.h>
#include <opengl/texture.h>
#include <opengl/renderer.h>

#include <map>

namespace miryks
{
	ModelSkinned::ModelSkinned(const char *modl) : Model()
	{
		first = true;
		model = get_ni(get_res(modl));
		assertc(model);
		Construct();
	}

	void ni_node_omit_callback(RD rd, NiNode *block)
	{
		// theres too many hint nodes in a skinned mesh that we dont care about
		if (rd->current != 0)
			return;
		Model *mesh = (Model *)rd->data;
		Group *group = mesh->MakeNewGroup(rd);
		matrix_from_common(group, block->common);
	}

	void ModelSkinned::Construct()
	{
		rundown *rd = calloc_rd();
		rd->ni = model;
		rd->data = this;
		rd->ni_node_callback = ni_node_omit_callback;
		rd->bs_tri_shape_callback = bs_tri_shape_callback;
		rd->bs_dynamic_tri_shape_callback = bs_dynamic_tri_shape_callback;
		rd->ni_skin_instance_callback = ni_skin_instance_callback;
		rd->ni_skin_data_callback = ni_skin_data_callback;
		rd->ni_skin_partition_callback = ni_skin_partition_callback;
		rd->bs_lighting_shader_property_callback = bs_lighting_shader_property_callback;
		rd->bs_effect_shader_property_callback = bs_effect_shader_property_callback;
		rd->bs_shader_texture_set_callback = bs_shader_texture_set_callback;
		rd->ni_alpha_property_callback = ni_alpha_property_callback;
		nif_rd(rd);
		free_rd(&rd);
		baseGroup->UpdateSideways();
	}

	void ModelSkinned::Update(skeleton *skeleton)
	{
		int cache = 0;
		for (NiRef ref : shapes__)
		{
			// Group *group = mesh->groups[index];
			// todo: support dynamic tri shapes
			BSTriShape *shape;
			BSDynamicTriShape *dynamic_shape;
			NiSkinInstance *nsi;
			NiSkinPartition *nsp;

			if(0 == strcmp(nif_get_block_type(model, ref), BSTriShapeS))
			{
				shape = (BSTriShape *)nif_get_block(model, ref);
				nsi = (NiSkinInstance *)nif_get_block(model, shape->refs->skin);
				nsp = (NiSkinPartition *)nif_get_block(model, nsi->A->skin_partition);
			}
			else if (0 == strcmp(nif_get_block_type(model, ref), BSDynamicTriShapeS))
			{
				dynamic_shape = (BSDynamicTriShape *)nif_get_block(model, ref);
				nsi = (NiSkinInstance *)nif_get_block(model, dynamic_shape->bs_tri_shape->refs->skin);
				nsp = (NiSkinPartition *)nif_get_block(model, nsi->A->skin_partition);
			}

			for (unsigned int k = 0; k < nsp->A->num_partitions; k++)
			{
				SkinPartition *partition = nsp->partitions[k];
				Group *group = groups[ref]->childGroups[k];
				Material *material = group->geometry->material;
				material->boneMatrices.clear();
				material->boneMatrices.reserve(partition->nums->bones);
				material->bindMatrix = group->matrixWorld;
				for (unsigned short i = 0; i < partition->nums->bones; i++)
				{
					if (first)
					{
						unsigned short j = partition->bones[i];
						NiNode *node = (NiNode *)nif_get_block(model, nsi->bones[j]);
						char *name = nif_get_string(model, node->common->F->name);
						auto has = skeleton->bonesNamed.find(name);
						if (has == skeleton->bonesNamed.end())
						{
							bonesCached.push_back(nullptr);
							continue;
						}
						bone *bon = has->second;
						bonesCached.push_back(bon);
					}
					else
					{
						bone *bon = bonesCached[cache++];
						if (bon == nullptr)
							material->boneMatrices.push_back(mat4(1.0));
						else
							material->boneMatrices.push_back(bon->matrixWorld * inverse(bon->rest));
					}
					// Group *node_group = groups[nsi->bones[partition->bones[i]]];
					// node_group->matrixWorld = bone->group->matrixWorld;
				}
			}
		}
		first = false;
	}

	void ModelSkinned::Step(skeleton *skeleton)
	{
		if (first)
			Update(skeleton);
		Update(skeleton);
	}

	void ni_skin_instance_callback(RD rd, NiSkinInstance *block)
	{
		ModelSkinned *modelSkinned = (ModelSkinned *)rd->data;
		//assertc(0 == strcmp(nif_get_block_type(modelSkinned->model, rd->parent), BSTriShapeS));
		//modelSkinned->lastShape = rd->parent;
	}

	static vec3 bytestofloat(void *in)
	{
		float xf, yf, zf;
		unsigned char *vec = (unsigned char *)in;
		xf = (float)((double(vec[0]) / 255.0) * 2.0 - 1.0);
		yf = (float)((double(vec[1]) / 255.0) * 2.0 - 1.0);
		zf = (float)((double(vec[2]) / 255.0) * 2.0 - 1.0);
		return vec3(xf, yf, zf);
	}

	static vec2 halftexcoord(void *in)
	{
		unsigned short *uv = (unsigned short *)in;
		union
		{
			float f;
			uint32_t i;
		} u, v;
		u.i = half_to_float(uv[0]);
		v.i = half_to_float(uv[1]);
		return vec2(u.f, v.f);
	}

	void ni_skin_data_callback(RD rd, NiSkinData *block)
	{
		ModelSkinned *modelSkinned = (ModelSkinned *)rd->data;
	}

	void ni_skin_partition_callback(RD rd, NiSkinPartition *block)
	{
		ModelSkinned *modelSkinned = (ModelSkinned *)rd->data;
		if (!block->vertex_data)
			return;
		unsigned int num_vertices = block->A->data_size / block->A->vertex_size;

		int vertices, uvs, normals, tangents, colors, skinned, eyedata, fullprec;
		nif_bs_vertex_desc(
			block->A->vertex_desc, &vertices, &uvs, &normals, &tangents, &colors, &skinned, &eyedata, &fullprec);

		for (unsigned int k = 0; k < block->A->num_partitions; k++)
		{
			SkinPartition *partition = block->partitions[k];
			if (!*partition->has_vertex_map)
				break;
			if (!*partition->has_faces)
				break;
			Group *group = new Group;
			Geometry *geometry = new Geometry();
			group->geometry = geometry;
			geometry->skinning = true;
			// adapt material from previous bstrishape
			printf("lastgroup %i %i\n", modelSkinned->lastGroup, modelSkinned->lastGroup->geometry);
			geometry->material = new Material(*modelSkinned->lastGroup->geometry->material);
			geometry->material->tangents = tangents;
			geometry->material->bones = partition->nums->bones;
			geometry->material->skinning = true;
			geometry->Clear(partition->nums->vertices, partition->nums->triangles * 3);
			std::map<unsigned short, unsigned short> remap;
			for (unsigned short i = 0; i < partition->nums->vertices; i++)
			{
				unsigned short j = partition->vertex_map[i];
				remap.emplace(j, i);
				Vertex vertex;
				if (vertices && uvs && normals && tangents && colors && skinned)
				{
					auto data = &((vertex_data_t *)block->vertex_data)[j];
					vertex.position = reinterpret_vec3(&data->vertex);
					vertex.uv = halftexcoord(&data->uv);
					vertex.normal = bytestofloat(&data->normal);
					vertex.tangent = bytestofloat(&data->tangent);
					vertex.color = vec4(reinterpret_bvec4(&data->colors)) / 255.f;
					vertex.skin_index = reinterpret_bvec4(&partition->bone_indices[i]);
					vertex.skin_weight = reinterpret_vec4(&partition->vertex_weights[i]);
				}
				else if (vertices && uvs && normals && tangents && !colors && skinned)
				{
					auto data = &((vertex_data_2_t *)block->vertex_data)[j];
					vertex.position = reinterpret_vec3(&data->vertex);
					vertex.uv = halftexcoord(&data->uv);
					vertex.normal = bytestofloat(&data->normal);
					vertex.tangent = bytestofloat(&data->tangent);
					vertex.skin_index = reinterpret_bvec4(&partition->bone_indices[i]);
					vertex.skin_weight = reinterpret_vec4(&partition->vertex_weights[i]);
				}
				else if (vertices && uvs && !normals && !tangents && colors && skinned)
				{
					auto data = &((vertex_data_3_t *)block->vertex_data)[j];
					vertex.position = reinterpret_vec3(&data->vertex);
					vertex.uv = halftexcoord(&data->uv);
					vertex.color = vec4(reinterpret_bvec4(&data->colors)) / 255.f;
					vertex.skin_index = reinterpret_bvec4(&partition->bone_indices[i]);
					vertex.skin_weight = reinterpret_vec4(&partition->vertex_weights[i]);
				}
				else if (!vertices && uvs && !normals && !tangents && colors && skinned && fullprec)
				{
					auto dynamic_shape = (BSDynamicTriShape *)nif_get_block(modelSkinned->model, modelSkinned->shapes__.back());

					printf("were a head with shape %i\n", modelSkinned->shapes__.back());
					auto data = &((vertex_data_4_t *)block->vertex_data)[j];
					vertex.position = reinterpret_vec3(&dynamic_shape->vertices[j]);
					vertex.uv = halftexcoord(&data->uv);
					vertex.color = vec4(reinterpret_bvec4(&data->colors)) / 255.f;
					vertex.skin_index = reinterpret_bvec4(&partition->bone_indices[i]);
					vertex.skin_weight = reinterpret_vec4(&partition->vertex_weights[i]);
				}
				// malehead.nif
				/*else if (!vertices && uvs && !normals && !tangents && colors && skinned && fullprec)
				{
					auto data = &((vertex_data_3_t *)block->vertex_data)[j];
					vertex.position = reinterpret_vec3(&data->vertex);
					vertex.uv = halftexcoord(&data->uv);
					vertex.color = vec4(reinterpret_bvec4(&data->colors)) / 255.f;
					vertex.skin_index = reinterpret_bvec4(&partition->bone_indices[i]);
					vertex.skin_weight = reinterpret_vec4(&partition->vertex_weights[i]);
				}*/
				geometry->vertices[i] = vertex;
			}
			for (unsigned short i = 0; i < partition->nums->triangles; i++)
			{
				auto triangle = partition->triangles[i];
				geometry->elements.insert(geometry->elements.end(),
										  {remap[triangle.a], remap[triangle.b], remap[triangle.c]});
			}
			geometry->SetupMesh();
			modelSkinned->lastGroup->Add(group);
			modelSkinned->lastGroup->UpdateSideways();
		}

	}

}