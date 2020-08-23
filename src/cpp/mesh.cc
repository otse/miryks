#include "mesh.h"

Mesh::Mesh()
{
	int l;
	nif = nullptr;
	base = new Group;
	front = base;
}

void callback(int, int, const char *, nif_rundown_t *);
void ni_node_callback(int, int, ni_node_t *, nif_rundown_t *);
void ni_tri_shape_callback(int, int, ni_tri_shape_t *, nif_rundown_t *);

void Mesh::Construct(nif_t *bucket)
{
	nif = bucket;
	nif_rundown_t *rd = nif_alloc_rundown();
	rd->nif = bucket;
	rd->data = this;
	rd->generic = callback;
	rd->ni_node = ni_node_callback;
	rd->ni_tri_shape = ni_tri_shape_callback;
	nif_accept(bucket, rd, this);
}

Group *Mesh::Nest(int parent, int current)
{
	Group *group = new Group;
	if (parent == -1)
		front = base;
	front->Add(group);
	front = group;
	return group;
}

//nif_t *nif;
//Mesh *mesh = nullptr;

#define get_block(x) (x *)nif_get_block(rd->nif, current)
#define get_ni_common_layout get_block(ni_common_layout_t)
#define get_ni_node get_block(ni_node_t)
#define get_ni_tri_shape get_block(ni_tri_shape_t)

void callback(int parent, int current, const char *block_type, nif_rundown_t *rd)
{
	Mesh *mesh = (Mesh *)rd->data;
	if (0)
		;
	else if (ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE))
	{
		ni_common_layout_t *common = get_block(ni_common_layout_t);
		ni_node_t *ni_node = get_block(ni_node_t);

		Group *group = mesh->Nest(parent, current);
	}
	else if (ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL))
	{
		ni_common_layout_t *common = get_block(ni_common_layout_t);
		ni_tri_shape_t *ni_tri_shape = get_block(ni_tri_shape_t);
	}

	//mesh->front->matrix
}

void ni_node_callback(int parent, int current, ni_node_t *, nif_rundown_t *rd)
{
	printf("ni node callback\n");

	Mesh *mesh = (Mesh *)rd->data;

	Group *group = mesh->Nest(parent, current);
}

void ni_node_callback(int parent, int current, ni_tri_shape_t *, nif_rundown_t *rd)
{
	printf("ni tri shape callback\n");

	Mesh *mesh = (Mesh *)rd->data;

	Group *group = mesh->Nest(parent, current);
}