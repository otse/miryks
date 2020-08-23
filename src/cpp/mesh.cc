#include "mesh.h"

Mesh::Mesh()
{
	int l;
	nif = nullptr;
	base = new Group;
	front = base;
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
	Group *group = new Group;
	if (parent == -1)
		front = base;
	front->Add(group);
	front = group;
	return group;
}

void other(rd_t *rd, int parent, int current, const char *block_type)
{
	Mesh *mesh = (Mesh *)rd->data;
}

void ni_node_callback(rd_t *rd, ni_node_t *ni_node)
{
	printf("ni node callback\n");

	Mesh *mesh = (Mesh *)rd->data;

	Group *group = mesh->Nested(rd->parent);
}

void ni_tri_shape_callback(rd_t *rd, ni_tri_shape_t *ni_tri_shape)
{
	printf("ni tri shape callback\n");

	Mesh *mesh = (Mesh *)rd->data;

	Group *group = mesh->Nested(rd->parent);
}