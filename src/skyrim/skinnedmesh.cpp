extern "C"
{	
#include <half.h>
}

#include "mesh.h"

#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/types.h>

#include <map>

using namespace dark;

namespace skyrim
{
	SkinnedMesh::SkinnedMesh(Nif *nif, Skeleton *skeleton) : Mesh(nif),
		skeleton(skeleton)
	{
		assertm(skeleton, "smesh needs skeleton");
		construct();
	}

	void SkinnedMesh::construct()
	{
		Rd *rd = calloc_nifprd();
		rd->nif = nif;
		rd->data = this;
		rd->ni_node_callback = ni_node_callback;
		rd->bs_tri_shape_callback = bs_tri_shape_callback;
		rd->ni_skin_instance_callback = ni_skin_instance_callback;
		rd->ni_skin_data_callback = ni_skin_data_callback;
		rd->ni_skin_partition_callback = ni_skin_partition_callback;
		rd->bs_lighting_shader_property_callback = bs_lighting_shader_property_callback;
		rd->bs_effect_shader_property_callback = bs_effect_shader_property_callback;
		rd->bs_shader_texture_set_callback = bs_shader_texture_set_callback;
		rd->ni_alpha_property_callback = ni_alpha_property_callback;
		nif_rd(rd);
		free_nifprd(&rd);
		initial();
		baseGroup->Update();
	}
	
	void SkinnedMesh::initial()
	{
		for (NiRef ref : shapes__)
		{
			//Group *group = mesh->groups[index];
			auto shape = (BSTriShape *)nif_get_block(nif, ref);
			auto nsi = (NiSkinInstance *)nif_get_block(nif, shape->refs->skin);
			auto nsp = (NiSkinPartition *)nif_get_block(nif, nsi->A->skin_partition);
			
			for (unsigned int k = 0; k < nsp->A->num_partitions; k++)
			{
				SkinPartition *partition = nsp->partitions[k];
				Group *group = groups[ref]->groups[k];
				Material *material = group->geometry->material;
				material->boneMatrices.clear();
				material->boneMatrices.reserve(partition->nums->bones);
				material->bindMatrix = group->matrixWorld;
				for (unsigned short i = 0; i < partition->nums->bones; i++)
				{
					auto node = (NiNode *)nif_get_block(nif, nsi->bones[partition->bones[i]]);
					char *name = nif_get_string(nif, node->common->F->name);
					auto has = skeleton->bonesNamed.find(name);
					if (has == skeleton->bonesNamed.end())
					{
						material->boneMatrices.push_back(mat4(1.0));
						continue;
					}
					Bone *bone = has->second;
					material->boneMatrices.push_back(bone->group->matrixWorld * inverse(bone->rest));
					
					//Group *node_group = groups[nsi->bones[partition->bones[i]]];
					//node_group->matrixWorld = bone->group->matrixWorld;
				}
			}
		}
	}

	void SkinnedMesh::forward()
	{
		if (skeleton)
			skeleton->step();
		initial();
	}

	void ni_skin_instance_callback(Rd *rd, NiSkinInstance *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		assertc(0 == strcmp(nif_get_block_type(smesh->nif, rd->parent), BSTriShapeS));
		smesh->lastShape = rd->parent;
	}

	static vec3 bytestofloat(unsigned char *vec)
	{
		float xf, yf, zf;
		xf = (double( vec[0] ) / 255.0) * 2.0 - 1.0;
		yf = (double( vec[1] ) / 255.0) * 2.0 - 1.0;
		zf = (double( vec[2] ) / 255.0) * 2.0 - 1.0;
		return vec3(xf, yf, zf);
	}

	static vec2 halftexcoord(unsigned short *uv)
	{
		union { float f; uint32_t i; } u, v;
		u.i = half_to_float(uv[0]);
		v.i = half_to_float(uv[1]);
		return vec2(u.f, v.f);
	}

	vec4 halfweights(unsigned short *hu)
	{
		union { float f; uint32_t i; } a, b, c, d;
		a.i = half_to_float(hu[0]);
		b.i = half_to_float(hu[1]);
		c.i = half_to_float(hu[2]);
		d.i = half_to_float(hu[3]);
		return vec4(a.f, b.f, c.f, d.f);
	}

	void ni_skin_data_callback(Rd *rd, NiSkinData *block)
	{
		//printf("ni skin data\n");
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		return;
		/*
		Group *lastGroup = smesh->lastGroup;
		Group *group = smesh->make_new_group(rd);
		group->geometry = lastGroup->geometry;
		group->matrix = translate(group->matrix, gloomVec3(block->skin_transform->translation));
		group->matrix *= inverse(mat4(gloomMat3(block->skin_transform->rotation)));
		group->matrix = scale(group->matrix, vec3(block->skin_transform->scale));
		*/

	}
	void ni_skin_partition_callback(Rd *rd, NiSkinPartition *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		if (!block->vertex_data)
			return;
		unsigned int num_vertices = block->A->data_size / block->A->vertex_size;

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
			geometry->material = new Material(*smesh->lastGroup->geometry->material);
			geometry->material->bones = partition->nums->bones;
			geometry->material->skinning = true;
			geometry->material->modelSpaceNormals = true;
			geometry->Clear(partition->nums->vertices, partition->nums->triangles * 3);
			std::map<unsigned short, unsigned short> remap;
			for (unsigned short i = 0; i < partition->nums->vertices; i++)
			{
				unsigned short j = partition->vertex_map[i];
				remap.emplace(j, i);
				auto data = &block->vertex_data[j];
				Vertex vertex;
				vertex.position = gloomVec3(data->vertex);
				vertex.uv = halftexcoord((unsigned short *)&data->uv);
				vertex.normal = bytestofloat((unsigned char *)&data->normal);
				vertex.tangent = bytestofloat((unsigned char *)&data->tangent);
				auto bi = data->bone_indices;
				vertex.skin_index = vec4(bi.a, bi.b, bi.c, bi.d);
				vertex.skin_weight = halfweights((unsigned short *)&data->bone_weights);
				geometry->vertices[i] = vertex;
			}
			for (unsigned short i = 0; i < partition->nums->triangles; i++)
			{
				auto triangle = partition->triangles[i];
				geometry->elements.insert(geometry->elements.end(),
					{remap[triangle.a], remap[triangle.b], remap[triangle.c]} );
				
			}
			geometry->SetupMesh();
			smesh->lastGroup->Add(group);
			smesh->lastGroup->Update();
		}
	}

}