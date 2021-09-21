#include <skyrim/mesh.h>

#include <renderer/drawgroup.h>

namespace skyrim
{
	SkinnedMesh::SkinnedMesh(const char *path)
	{
		meshSkinned = new MeshSkinned(path);
		drawGroup = new DrawGroup(nullptr/*meshSkinned->baseGroup*/, mat4(1.0));
	}

	void SkinnedMesh::Step()
	{

	}
}