#include "dark2.h"

#include "files"

extern "C"
{
#include "bsa.h"
#include "nif.h"
}

#include "opengl/types"
#include "opengl/scene"
#include "opengl/group"
#include "opengl/geometry"
#include "opengl/material"

namespace dark2
{
class Mesh
{
public:
	Mesh();

	Nif *nif;

	std::map<int, Group *> groups;
	
	Group *baseGroup, *lastGroup;

	//Material *material;
	//Geometry *geometry;

	static void Store(void *, Mesh *);
	static Mesh *GetStored(void *);

	void Construct(Nif *);

	Group *Nested(Rd *);
};
}