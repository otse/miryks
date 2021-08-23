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
	}
	void ni_skin_partition_callback(Rd *rd, NiSkinPartition *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		Material *material = smesh->lastGroup->geometry->material;
		BSTriShape *shape = (BSTriShape *)nif_get_block(smesh->nif, smesh->lastShape);
		printf("good %i %s\n", material, nif_get_string(smesh->nif, shape->common->F->name));
		material->prepared = false;
		if (!block->vertex_data)
			return;
		unsigned int num_vertices = block->A->data_size / block->A->vertex_size;
		std::vector<Vertex> *vertices = new std::vector<Vertex>;
		vertices->reserve(num_vertices);
		for (unsigned short i = 0; i < num_vertices; i++)
		{
			auto data = &block->vertex_data[i];
			Vertex vertex;
			vertex.position = gloomVec3(data->vertex);
			vertex.uv = halftexcoord((unsigned short *)&data->uv);
			vertex.normal = bytestofloat((unsigned char *)&data->normal);
			vertex.tangent = bytestofloat((unsigned char *)&data->tangent);
			auto bi = data->bone_indices;
			vertex.skin_index = vec4(bi.a, bi.b, bi.c, bi.d);
			vertex.skin_weight = halfweights((unsigned short *)&data->bone_weights);
			vertices->push_back(vertex);
		}
		GLuint vbo;
		glGenBuffers(1, &vbo);
		printf("gen buffer vbo %u\n", vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Vertex), &(*vertices)[0], GL_STATIC_DRAW);

		for (unsigned int k = 0; k < block->A->num_partitions; k++)
		{
			SkinPartition *partition = block->partitions[k];
			if (k>0)
				continue;
			printf(
					"partition nums vertices %u, triangles %u, bones %u, strips %u, weights %u\n",
					partition->nums->vertices, partition->nums->triangles, partition->nums->bones, partition->nums->strips, partition->nums->weights_per_vertex);
			Group *group = new Group;
			Geometry *geometry = new Geometry();
			group->geometry = geometry;
			geometry->vbo = vbo;
			geometry->material = new Material(*smesh->lastGroup->geometry->material);
			printf("partition uses material %s\n", material->name.c_str());
			//geometry->material->map = GetProduceTexture("textures\\actors\\draugr\\Armor.dds");
			geometry->material->skinning = false;
			geometry->material->testing = false;
			geometry->material->zwrite = true;
			geometry->material->opacity = 1;
			geometry->skinning = false;
			geometry->Clear(0, 0);
			if (!*partition->has_vertex_map)
				break;
			if (*partition->has_faces)
			{
				geometry->Clear(0, partition->nums->triangles * 3);
				for (unsigned short i = 0; i < partition->nums->triangles; i++)
				{
					auto triangle = &partition->triangles[i];
					geometry->elements.insert(geometry->elements.end(), { triangle->a, triangle->b, triangle->c });
				}
			}
			group->geometry->SetupMesh();
			smesh->lastGroup->Add(group);
			group->Update();
		}
		smesh->lastGroup->Update();
	}

}