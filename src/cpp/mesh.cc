#include "mesh.h"

#include "opengl/types"

vec3 *cast_vec_3(float *f) {
	return reinterpret_cast<vec3 *>(f);
}
mat3 *cast_mat_3(float *f) {
	return reinterpret_cast<mat3 *>(f);
}
mat4 *cast_mat_4(float *f) {
	return reinterpret_cast<mat4 *>(f);
}

Mesh::Mesh()
{
	int l;
	nif = nullptr;
	base = new Group();
	lastGroup = base;
}

void other(rd_t *, int, int, const char *);
void ni_node_callback(rd_t *, ni_node_t *);
void ni_tri_shape_callback(rd_t *, ni_tri_shape_t *);
void ni_tri_shape_data_callback(rd_t *, ni_tri_shape_data_t *);
void bs_lighting_shader_property_callback(rd_t *, bs_lighting_shader_property_t *);
void bs_shader_texture_set_callback(rd_t *, bs_shader_texture_set_t *);

void Mesh::Construct(nif_t *bucket)
{
	nif = bucket;
	rd_t *rd = nif_alloc_rundown();
	rd->nif = bucket;
	rd->data = this;
	rd->other = other;
	rd->ni_node = ni_node_callback;
	rd->ni_tri_shape = ni_tri_shape_callback;
	rd->bs_lighting_shader_property = bs_lighting_shader_property_callback;
	rd->bs_shader_texture_set = bs_shader_texture_set_callback;
	nif_rundown(bucket, rd, this);
}

Group *Mesh::Nested(int parent)
{
	Group *group = new Group();
	if (parent == -1)
		lastGroup = base;
	lastGroup->Add(group);
	lastGroup = group;
	return group;
}

void other(rd_t *rd, int parent, int current, const char *block_type)
{
	Mesh *mesh = (Mesh *)rd->data;
}

void matrix_from_common(Group *group, ni_common_layout_t *common)
{
	group->matrix = mat4(*cast_mat_3((float *)&common->rotation));
	group->matrix = translate(group->matrix, *cast_vec_3((float *)&common->translation));
}

void ni_node_callback(rd_t *rd, ni_node_t *ni_node)
{
	printf("ni node callback\n");
	Mesh *mesh = (Mesh *)rd->data;
	Group *group = mesh->Nested(rd->parent);
	matrix_from_common(group, &ni_node->common);
}

void ni_tri_shape_callback(rd_t *rd, ni_tri_shape_t *ni_tri_shape)
{
	printf("ni tri shape callback\n");
	Mesh *mesh = (Mesh *)rd->data;
	Group *group = mesh->Nested(rd->parent);
	matrix_from_common(group, &ni_tri_shape->common);
}

void ni_tri_shape_data_callback(rd_t *rd, ni_tri_shape_data_t *ni_tri_shape_data)
{
	printf("ni tri shape data callback\n");
	Mesh *mesh = (Mesh *)rd->data;
}

void bs_lighting_shader_property_callback(rd_t *rd, bs_lighting_shader_property_t *bs_lighting_shader_property)
{
	printf("bs lighting shader property callback\n");
	Mesh *mesh = (Mesh *)rd->data;
}

void bs_shader_texture_set_callback(rd_t *rd, bs_shader_texture_set_t *bs_shader_texture_set)
{
	printf("bs shader texture set callback\n");
	Mesh *mesh = (Mesh *)rd->data;
}