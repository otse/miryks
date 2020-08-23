#include "mesh.h"

#include "opengl/types"

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

void Mesh::Construct(nif_t *bucket)
{
	nif = bucket;
	rd_t *rd = nif_alloc_rundown();
	rd->nif = bucket;
	rd->data = this;
	rd->other = other;
	rd->ni_node = ni_node_callback;
	rd->ni_tri_shape = ni_tri_shape_callback;
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

vec3 *vec_3_vec3(float *f)
{
	return reinterpret_cast<vec3*>(f);
}
mat3 *mat_3_mat3(float *f)
{
	return reinterpret_cast<mat3*>(f);
}

void set_matrix_from_common(ni_common_layout_t *common)
{
	vec3 translation = *vec_3_vec3(&common->translation.x);
	mat3 rotation = *mat_3_mat3(&common->rotation.n[0]);
}

void ni_node_callback(rd_t *rd, ni_node_t *ni_node)
{
	printf("ni node callback\n");

	Mesh *mesh = (Mesh *)rd->data;

	Group *group = mesh->Nested(rd->parent);

	set_matrix_from_common(&ni_node->common);
}

void ni_tri_shape_callback(rd_t *rd, ni_tri_shape_t *ni_tri_shape)
{
	printf("ni tri shape callback\n");

	Mesh *mesh = (Mesh *)rd->data;

	Group *group = mesh->Nested(rd->parent);

	set_matrix_from_common(&ni_tri_shape->common);

}