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
		construct();
	}

	void SkinnedMesh::construct()
	{
		assertm(skeleton, "smesh needs skeleton");
		Rd *rd = calloc_nifprd();
		rd->nif = nif;
		rd->data = this;
		//rd->other = other;
		rd->ni_node_callback = ni_node_callback;
		rd->ni_skin_instance_callback = ni_skin_instance_callback;
		rd->ni_skin_data_callback = ni_skin_data_callback;
		rd->ni_skin_partition_callback = ni_skin_partition_callback;
		nif_rd(rd);
		free_nifprd(&rd);
		initial();
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
		Nif *nif = smesh->nif;
		assertm(0 == strcmp(nif_get_block_type(nif, rd->parent), NiTriShapeS), "root not shape");
		auto shape = (ni_tri_shape_t *)nif_get_block(nif, rd->parent);
		smesh->shapes.push_back(rd->parent);
	}

	void ni_skin_data_callback(Rd *rd, NiSkinData *block)
	{
		//
	}
	void ni_skin_partition_callback(Rd *rd, NiSkinPartition *block)
	{

	}
	/*
	legendary_edition
	void ni_skin_partition_callback(Rd *rd, ni_skin_partition *block)
	{
		
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		auto nif = smesh->mesh->nif;
		auto shape = (ni_tri_shape *)nif_get_block(nif, smesh->shapes.back());
		auto data = (ni_tri_shape_data *)nif_get_block(nif, shape->A->data);
		for (unsigned int k = 0; k < *block->num_skin_partition_blocks; k++)
		{
			struct skin_partition *part = block->skin_partition_blocks[k];
			Group *group = new Group;
			Geometry *geometry = new Geometry();
			geometry->skinning = true;
			geometry->Clear(0, 0);
			if (!data->A->num_vertices)
				continue;
			if (part->A->num_triangles > 0)
			{
				geometry->Clear(part->A->num_vertices, part->A->num_triangles * 3);
				for (int i = 0; i < part->A->num_triangles; i++)
				{
					unsigned short *triangle = (unsigned short *)&part->triangles[i];
					geometry->elements.insert(geometry->elements.end(), {triangle[0], triangle[1], triangle[2]});
				}
			}
			for (int i = 0; i < part->A->num_vertices; i++)
			{
				if (!*part->has_vertex_map)
					break;
				unsigned short j = part->vertex_map[i];
				geometry->vertices[i].position = gloomVec3(data->vertices[j]);
				if (data->C->bs_vector_flags & 0x00000001)
					geometry->vertices[i].uv = *cast_vec_2((float *)&data->uv_sets[j]);
				if (data->C->has_normals)
					geometry->vertices[i].normal = gloomVec3(data->normals[j]);
				if (data->C->bs_vector_flags & 0x00001000)
				{
					smesh->lastShape->geometry->material->tangents = true;
					geometry->vertices[i].tangent = gloomVec3(data->tangents[j]);
					geometry->vertices[i].bitangent = gloomVec3(data->bitangents[j]);
				}
				if (data->G->has_vertex_colors)
					geometry->vertices[i].color = gloomVec4(data->vertex_colors[j]);
				if (*part->has_bone_indices)
				{
					auto a = part->bone_indices[i];
					geometry->vertices[i].skin_index = vec4(a.a, a.b, a.c, a.d);
				}
				if (*part->has_vertex_weights)
					geometry->vertices[i].skin_weight = gloomVec4(part->vertex_weights[i]);
			}
			//if (smesh->lastShape->geometry->material->tangents)
			//	printf("has tangents");
			geometry->material = new Material(*smesh->lastShape->geometry->material);
			geometry->material->bones = part->A->num_bones;
			//geometry->material->skinning = true;
			geometry->material->prepared = false;
			geometry->SetupMesh();
			group->geometry = geometry;
			smesh->lastShape->Add(group);
		}
		smesh->lastShape->Update();
	}
	*/

}