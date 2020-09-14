#include <dark2.h>

extern "C"
{
#include <bsa.h>
#include <nif.h>
}

#include <opengl/types.h>
#include <opengl/scene.h>
#include <opengl/group.h>
#include <opengl/geometry.h>
#include <opengl/material.h>

namespace dark2
{
class Mesh
{
public:
	Mesh();

	AABB aabb;

	std::map<int, Group *> groups;
	
	Group *baseGroup, *lastGroup;

	void Construct(nif *);
	Group *Nested(rd *);

	static void Store(void *, Mesh *);
	static Mesh *GetStored(void *);
};
}