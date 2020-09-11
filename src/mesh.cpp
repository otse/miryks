#include "mesh.h"

#include "opengl/shader.h"
#include "opengl/texture.h"
#include "opengl/types.h"

namespace dark2
{

	Mesh::Mesh()
	{
		int l;
		nif = nullptr;
		baseGroup = new Group();
		lastGroup = baseGroup;
	}

	void other(Rd *, int, int, const char *);
	void ni_node_callback(Rd *, ni_node *);
	void ni_tri_shape_callback(Rd *, ni_tri_shape *);
	void ni_tri_shape_data_callback(Rd *, ni_tri_shape_data *);
	void bs_lighting_shader_property_callback(Rd *, bs_lighting_shader_property *);
	void bs_shader_texture_set_callback(Rd *, bs_shader_texture_set *);

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

	void Mesh::Construct(Nif *bucket)
	{
		nif = bucket;
		rd *rd = nif_alloc_rd();
		rd->nif = bucket;
		rd->data = this;
		rd->other = other;
		rd->ni_node = ni_node_callback;
		rd->ni_tri_shape = ni_tri_shape_callback;
		rd->ni_tri_shape_data = ni_tri_shape_data_callback;
		rd->bs_lighting_shader_property = bs_lighting_shader_property_callback;
		rd->bs_shader_texture_set = bs_shader_texture_set_callback;
		nif_rd(bucket, rd, this);
		nif_free_rd(&rd);
		baseGroup->Update();
	}

	Group *Mesh::Nested(Rd *rd)
	{
		Group *group = new Group();
		Group *parent = rd->parent == -1 ? baseGroup : groups[rd->parent];
		groups[rd->current] = group;
		parent->Add(group);
		lastGroup = group;
		return group;
	}

	void other(rd *rd, int parent, int current, const char *block_type)
	{
		Mesh *mesh = (Mesh *)rd->data;
	}

	void matrix_from_common(Group *group, ni_common_layout *common)
	{
		group->matrix = translate(group->matrix, *cast_vec_3((float *)&common->translation));
		group->matrix *= mat4((*cast_mat_3((float *)&common->rotation)));
		group->matrix = scale(group->matrix, vec3(common->scale));
	}

	void ni_node_callback(rd *rd, ni_node *block)
	{
		// printf("ni node callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->Nested(rd);
		//matrix_from_common(group, &block->common);
	}

	void ni_tri_shape_callback(rd *rd, ni_tri_shape *block)
	{
		// printf("ni tri shape callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->Nested(rd);
		matrix_from_common(group, &block->common);
		group->geometry = new Geometry();
		group->geometry->material = new Material();
		group->geometry->material->src = &simple;
		//vec3 v = *cast_vec_3((float *)&block->common.translation);
		//printf("block name %s translate %f %f %f", nif_get_string(rd->nif, block->common.name), v.x, v.y, v.z);
	}

	void ni_tri_shape_data_callback(rd *rd, ni_tri_shape_data *block)
	{
		// printf("ni tri shape data callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Geometry *geometry = mesh->lastGroup->geometry;
		geometry->Clear(0, 0);
		if (!block->num_vertices)
			return;
		if (block->has_triangles)
		{
			geometry->Clear(block->num_vertices, block->num_triangles * 3);
			for (int i = 0; i < block->num_triangles; i++)
			{
				unsigned short *triangle = (unsigned short *)&block->triangles[i];
				geometry->elements.insert(geometry->elements.end(), {triangle[0], triangle[1], triangle[2]});
			}
		}
		for (int i = 0; i < block->num_vertices; i++)
		{
			geometry->vertices[i].position = *cast_vec_3((float *)&block->vertices[i]);
			if (block->bs_vector_flags & 0x00000001)
				geometry->vertices[i].uv = *cast_vec_2((float *)&block->uv_sets[i]);
			geometry->vertices[i].normal = *cast_vec_3((float *)&block->normals[i]);
			if (block->bs_vector_flags & 0x00001000)
			{
				geometry->vertices[i].tangent = *cast_vec_3((float *)&block->tangents[i]);
				geometry->vertices[i].bitangent = *cast_vec_3((float *)&block->bitangents[i]);
			}
			if (block->has_vertex_colors)
				geometry->vertices[i].color = *cast_vec_4((float *)&block->vertex_colors[i]);
		}

		geometry->SetupMesh();
	}

	void bs_lighting_shader_property_callback(rd *rd, bs_lighting_shader_property *block)
	{
		// printf("bs lighting shader property callback\n");
		Mesh *mesh = (Mesh *)rd->data;
	}

	void bs_shader_texture_set_callback(rd *rd, bs_shader_texture_set *block)
	{
		// printf("bs shader texture set callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->lastGroup;
		for (int i = 0; i < block->num_textures; i++)
		{
			string path = block->textures[i];
			if (path.empty())
				continue;
			if (path.find("skyrimhd\\build\\pc\\data\\") != std::string::npos)
				path = path.substr(23, std::string::npos);
			if (i == 0)
				group->geometry->material->map = GetProduceTexture(block->textures[i]);
			if (i == 1)
				group->geometry->material->normalMap = GetProduceTexture(block->textures[i]);
		}
	}
} // namespace dark2