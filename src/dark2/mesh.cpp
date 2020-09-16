#include <dark2/mesh.h>

#include <opengl/shader.h>
#include <opengl/texture.h>
#include <opengl/types.h>

namespace dark2
{
	Mesh::Mesh()
	{
		baseGroup = new Group();
		lastGroup = baseGroup;
	}

	void other(nifprd *, int, int, const char *);
	void ni_node_callback(nifprd *, ni_node_pointer *);
	void ni_tri_shape_callback(nifprd *, ni_tri_shape_pointer *);
	void ni_tri_shape_data_callback(nifprd *, ni_tri_shape_data_pointer *);
	void bs_lighting_shader_property_callback(nifprd *, bs_lighting_shader_property_pointer *);
	void bs_shader_texture_set_callback(nifprd *, bs_shader_texture_set_pointer *);
	void ni_alpha_property_callback(nifprd *, ni_alpha_property_pointer *);

	static std::map<void *, Mesh *> store;

	void Mesh::Store(void *key, Mesh *mesh)
	{
		store.emplace(key, mesh);
	}

	Mesh *Mesh::GetStored(void *key)
	{
		if (store.count(key))
			return store[key];
		return nullptr;
	}

	void Mesh::Construct(nifp *bucket)
	{
		nifprd *rd = malloc_nifprd();
		rd->nif = bucket;
		rd->data = this;
		rd->other = other;
		rd->ni_node = ni_node_callback;
		rd->ni_tri_shape = ni_tri_shape_callback;
		rd->ni_tri_shape_data = ni_tri_shape_data_callback;
		rd->bs_lighting_shader_property = bs_lighting_shader_property_callback;
		rd->bs_shader_texture_set = bs_shader_texture_set_callback;
		rd->ni_alpha_property = ni_alpha_property_callback;
		nifp_rd(bucket, rd, this);
		free_nifprd(&rd);
		baseGroup->Update();
	}

	Group *Mesh::Nested(nifprd *rd)
	{
		Group *group = new Group();
		Group *parent = rd->parent == -1 ? baseGroup : groups[rd->parent];
		groups[rd->current] = group;
		parent->Add(group);
		lastGroup = group;
		return group;
	}

	void other(nifprd *rd, int parent, int current, const char *block_type)
	{
		Mesh *mesh = (Mesh *)rd->data;
	}

	void matrix_from_common(Group *group, ni_common_layout_pointer *common)
	{
		group->matrix = translate(group->matrix, *cast_vec_3((float *)&common->C->translation));
		group->matrix *= mat4((*cast_mat_3((float *)&common->C->rotation)));
		group->matrix = scale(group->matrix, vec3(common->C->scale));
	}

	void ni_node_callback(nifprd *rd, ni_node_pointer *block)
	{
		// printf("ni node callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->Nested(rd);
		//matrix_from_common(group, block->common);
	}

	void ni_tri_shape_callback(nifprd *rd, ni_tri_shape_pointer *block)
	{
		// printf("ni tri shape callback %s\n", block->common.name_string);
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->Nested(rd);
		matrix_from_common(group, block->common);
		group->geometry = new Geometry();
		group->geometry->material = new Material();
		group->geometry->material->src = &simple;
	}

	void ni_tri_shape_data_callback(nifprd *rd, ni_tri_shape_data_pointer *block)
	{
		// printf("ni tri shape data callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->lastGroup;
		Geometry *geometry = group->geometry;
		geometry->Clear(0, 0);
		if (!block->A->num_vertices)
			return;
		if (block->J->has_triangles)
		{
			geometry->Clear(block->A->num_vertices, block->J->num_triangles * 3);
			for (int i = 0; i < block->J->num_triangles; i++)
			{
				unsigned short *triangle = (unsigned short *)&block->triangles[i];
				geometry->elements.insert(geometry->elements.end(), { triangle[0], triangle[1], triangle[2] });
			}
		}
		for (int i = 0; i < block->A->num_vertices; i++)
		{
			geometry->vertices[i].position = *cast_vec_3((float *)&block->vertices[i]);
			if (block->C->bs_vector_flags & 0x00000001)
				geometry->vertices[i].uv = *cast_vec_2((float *)&block->uv_sets[i]);
			geometry->vertices[i].normal = *cast_vec_3((float *)&block->normals[i]);
			if (block->C->bs_vector_flags & 0x00001000)
			{
				geometry->vertices[i].tangent = *cast_vec_3((float *)&block->tangents[i]);
				geometry->vertices[i].bitangent = *cast_vec_3((float *)&block->bitangents[i]);
			}
			if (block->G->has_vertex_colors)
				geometry->vertices[i].color = *cast_vec_4((float *)&block->vertex_colors[i]);
		}

		geometry->SetupMesh();
	}

	void bs_lighting_shader_property_callback(nifprd *rd, bs_lighting_shader_property_pointer *block)
	{
		// printf("bs lighting shader property callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->lastGroup;
		Geometry *geometry = group->geometry;
		if (geometry)
		{
			geometry->material->emissive = *cast_vec_3((float *)&block->B->emissive_color);
			geometry->material->specular = *cast_vec_3((float *)&block->B->specular_color);
			geometry->material->specular *= block->B->specular_strength;
			geometry->material->opacity = block->B->alpha;
			geometry->material->glossiness = block->B->glossiness;
		}
	}

	void bs_shader_texture_set_callback(nifprd *rd, bs_shader_texture_set_pointer *block)
	{
		// printf("bs shader texture set callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->lastGroup;
		Geometry *geometry = group->geometry;
		for (int i = 0; i < block->A->num_textures; i++)
		{
			std::string path = std::string(block->textures[i]);
			if (path.empty())
				continue;
			if (path.find("skyrimhd\\build\\pc\\data\\") != std::string::npos)
				path = path.substr(23, std::string::npos);
			if (i == 0)
				geometry->material->map = GetProduceTexture(block->textures[i]);
			if (i == 1)
				geometry->material->normalMap = GetProduceTexture(block->textures[i]);
			if (i == 2)
			{
				printf("glowmap %s\n", block->textures[i]);
				geometry->material->glowMap = GetProduceTexture(block->textures[i]);
			}
		}
	}

	void ni_alpha_property_callback(nifprd *rd, ni_alpha_property_pointer *block)
	{
		// printf("ni alpha property");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->lastGroup;
		Geometry *geometry = group->geometry;
		if (geometry)
		{
			geometry->material->treshold = block->C->treshold / 255.f;
		}
	}
} // namespace dark2