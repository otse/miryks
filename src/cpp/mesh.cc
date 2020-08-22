#include "mesh.h"

Mesh::Mesh(nif_t *nif) :
    nif(nif)
{
    int l;
}

void meshify_ni_node(blockcb);

void Mesh::Construct()
{
    nif_visitor_t *visitor = nif_alloc_visitor();
    visitor->ni_node = meshify_ni_node;
}

void meshify_ni_node(blockcb) {
    printf("mesh.cc meshify ni node\n");
}