#include "mesh.h"

Mesh::Mesh()
{
	int l;
	nif = nullptr;
	base = new Group;
	front = base;
}

void callback(int, int, const char *, nif_visitor_t *);

void Mesh::Construct(nif_t *bucket)
{
	nif = bucket;
	nif_visitor_t *visitor = nif_alloc_visitor();
	visitor->nif = bucket;
	visitor->data = this;
	visitor->callback = callback;
	//visitor->ni_node_callback = ni_node_callback;
	nif_accept(bucket, visitor, this);
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

#define get_block(x) (x*) nif_get_block(visitor->nif, current)
#define get_ni_common_layout get_block(ni_common_layout_t)
#define get_ni_node get_block(ni_node_t)
#define get_ni_tri_shape get_block(ni_node_t)

void callback(int parent, int current, const char *block_type, nif_visitor_t *visitor)
{
	Mesh *mesh = (Mesh *) visitor->data;
	if (0) ;
	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) )
	{
	if (ni_is_type(NI_NODE))
	{
	auto ni_node = get_ni_node;
	}
	auto common = get_ni_common_layout;

	Group* group = mesh->Nest(parent, current);
	}
	else if ( ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) )
	{
	auto ni_tri_shape = get_ni_tri_shape;
	auto common = get_ni_common_layout;
	}

	//mesh->front->matrix
}