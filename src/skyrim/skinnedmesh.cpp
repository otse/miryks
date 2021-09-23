#include <skyrim/mesh.h>

#include <renderer/drawgroup.h>

namespace skyrim
{
	SKSkinnedMesh::SKSkinnedMesh(const char *path)
	{
		skeleton = nullptr;
		animation = nullptr;
		meshSkinned = new SKMeshSkinned(path);
		drawGroup = new DrawGroup(
			meshSkinned->baseGroup, mat4(1.0));
	}

	void SKSkinnedMesh::Step()
	{
		if (meshSkinned && skeleton)
			meshSkinned->Step(skeleton);
	}
}