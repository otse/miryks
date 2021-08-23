extern "C"
{	
#include <half.h>
}

#include "mesh.h"

#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/types.h>

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
		/*
		for (ni_ref index : shapes)
		{
			//Group *group = mesh->groups[index];
			auto shape = (ni_tri_shape *)nif_get_block(mesh->nif, index);
			auto si = (ni_skin_instance *)nif_get_block(mesh->nif, shape->A->skin_instance);
			auto sp = (ni_skin_partition *)nif_get_block(mesh->nif, si->A->skin_partition);
			
			for (unsigned int k = 0; k < *sp->num_skin_partition_blocks; k++)
			{
				struct skin_partition *part = sp->skin_partition_blocks[k];
				Group *group = mesh->groups[index]->groups[k];
				Material *material = group->geometry->material;
				material->boneMatrices.clear();
				material->boneMatrices.reserve(part->A->num_bones);
				material->bindMatrix = group->matrixWorld;
				for (int i = 0; i < part->A->num_bones; i++)
				{
					auto node = (ni_node *)nif_get_block(mesh->nif, si->bones[part->bones[i]]);
					char *name = nif_get_string(mesh->nif, node->common->F->name);
					auto has = skeleton->bonesNamed.find(name);
					if (has == skeleton->bonesNamed.end())
					{
						material->boneMatrices.push_back(mat4(1.0));
						continue;
					}
					Bone *bone = has->second;
					material->boneMatrices.push_back(bone->group->matrixWorld * inverse(bone->rest));
				}
			}
		}*/
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

	void ni_skin_data_callback(Rd *rd, NiSkinData *block)
	{
		//printf("ni skin data\n");
	}
	void ni_skin_partition_callback(Rd *rd, NiSkinPartition *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		Material *material = smesh->lastGroup->geometry->material;
		BSTriShape *shape = (BSTriShape *)nif_get_block(smesh->nif, smesh->lastShape);
		printf("good %s\n", nif_get_string(smesh->nif, shape->common->F->name));
		material->prepared = false;
		//smesh->lastGroup->geometry = nullptr;
		unsigned int numVertices = block->A->data_size / block->A->vertex_size;
		if (!block->vertex_data)
			return;
		if (!numVertices)
			return;
		printf("block->A->num_partitions %u\n", block->A->num_partitions);
		for (unsigned int k = 0; k < block->A->num_partitions; k++)
		{
			SkinPartition *partition = block->partitions[k];
			Group *group = new Group;
			group->geometry = new Geometry();
			Geometry *geometry = group->geometry;
			geometry->material = material;
			//geometry->skinning = true;
			geometry->Clear(0, 0);
			if (*partition->has_faces)
			{
				geometry->Clear(partition->nums->vertices, partition->nums->triangles * 3);
				for (int i = 0; i < partition->nums->triangles; i++)
				{
					unsigned short *triangle = (unsigned short *)&partition->triangles[i];
					geometry->elements.insert(geometry->elements.end(), {triangle[0], triangle[1], triangle[2]});
				}
			}
			for (int i = 0; i < partition->nums->vertices; i++)
			{
				if (!*partition->has_vertex_map)
					break;
				unsigned short j = partition->vertex_map[i];
				auto vertex = &block->vertex_data[j];
				geometry->vertices[i].position = gloomVec3(vertex->vertex);
				//if (block->vertex_desc & 0x00000001)
					geometry->vertices[i].uv = halftexcoord((unsigned short *)&vertex->uv);
				//if (block->C->has_normals)
					geometry->vertices[i].normal = bytestofloat((unsigned char *)&vertex->normal);
				//if (data->C->bs_vector_flags & 0x00001000)
				{
					material->tangents = true; // ?
					geometry->vertices[i].tangent = bytestofloat((unsigned char *)&vertex->tangent);
					//geometry->vertices[i].bitangent = gloomVec3(data->bitangents[j]);
				}
				//if (data->G->has_vertex_colors)
				if (*partition->has_bone_indices)
				{
					auto a = partition->bone_indices[i];
					geometry->vertices[i].skin_index = vec4(a.x, a.y, a.z, a.w);
				}
				//if (partition->C->has_vertex_weights)
				//	geometry->vertices[i].skin_weight = gloomVec4(part->vertex_weights[i]);
			}
			//if (smesh->lastShape->geometry->material->tangents)
			//	printf("has tangents");
			//geometry->material->bones = part->A->num_bones;
			group->geometry->SetupMesh();
			smesh->lastGroup->Add(group);
		}
		smesh->lastGroup->Update();
	}

}