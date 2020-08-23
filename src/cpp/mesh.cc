#include "mesh.h"

#include "opengl/types"

vec2 *cast_vec_2(float *f) {
	return reinterpret_cast<vec2 *>(f);
}
vec3 *cast_vec_3(float *f) {
	return reinterpret_cast<vec3 *>(f);
}
vec4 *cast_vec_4(float *f) {
	return reinterpret_cast<vec4 *>(f);
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
	rd->ni_tri_shape_data = ni_tri_shape_data_callback;
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
	group->geometry = new Geometry();
	group->geometry->material = new Material();
	matrix_from_common(group, &ni_tri_shape->common);
}

void ni_tri_shape_data_callback(rd_t *rd, ni_tri_shape_data_t *block)
{
	printf("ni tri shape data callback\n");
	Mesh *mesh = (Mesh *)rd->data;
	Group *group = mesh->lastGroup;
	if (!block->num_vertices)
	{
	printf("ntsd no vertices\n");
	return;
	}
	group->geometry->Clear(block->num_vertices, block->num_triangles * 3);
	for (int i = 0; i < block->num_triangles; i++)
	{
	// todo
	group->geometry->elements.push_back(block->triangles[i].x);
	group->geometry->elements.push_back(block->triangles[i].y);
	group->geometry->elements.push_back(block->triangles[i].z);
	}
	for (int i = 0; i < block->num_vertices; i++)
	{
	vertex_t &vertex = group->geometry->vertices[i];

	vertex.position = *cast_vec_3((float *)&block->vertices[i]);
	vertex.uv = 	  *cast_vec_2((float *)&block->uv_sets[i]);
	vertex.normal =   *cast_vec_3((float *)&block->normals[i]);
	if (block->has_vertex_colors)
	vertex.color =    *cast_vec_4((float *)&block->vertex_colors[i]);
	printf("vertex %i set!", i);
	}

	group->geometry->SetupMesh();
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