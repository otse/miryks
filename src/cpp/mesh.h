#include "dark2.h"

#include "files"

extern "C"
{
#include "c/bsa.h"
#include "c/nif.h"
}

#include "cpp/opengl/types"
#include "cpp/opengl/scene"
#include "cpp/opengl/group"
#include "cpp/opengl/geometry"
#include "cpp/opengl/material"

class Mesh
{
public:
	Mesh();

	nif_t *nif;

	std::map<int, Group *> groups;
	
	Group *base, *lastGroup;

	//Material *material;
	//Geometry *geometry;

	void Construct(nif_t *);

	Group *Nested(rd_t *);
};
