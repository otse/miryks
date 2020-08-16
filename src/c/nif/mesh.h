#ifndef NIF_MESH_H
#define NIF_MESH_H

#define api

typedef struct mesh_t mesh_t;

struct mesh_t
{
	void *nif;
};

api mesh_t *mesh_from_nif(void *nif);

#endif