#include "mesh.h"

Mesh::Mesh()
{
    int l;
}

void meshify_ni_node(int, int, void *);

void Mesh::Construct(nif_t *bucket)
{
    nif_visitor_t *visitor = nif_alloc_visitor();
    visitor->ni_node = meshify_ni_node;
	nif_accept(bucket, visitor, this);
}

void meshify_ni_node(int parent, int block, void *) {
    printf("mesh.cc meshify ni node\n");
}