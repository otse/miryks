#include <skyrim/mesh.h>

#include <renderer/drawgroup.h>

namespace skyrim
{
	SkinnedMesh::SkinnedMesh(const char *path)
	{
		skeleton = nullptr;
		animation = nullptr;
		meshSkinned = new MeshSkinned(path);
		drawGroup = new DrawGroup(
			meshSkinned->baseGroup, mat4(1.0));
	}

	void SkinnedMesh::Step()
	{
		if (meshSkinned && skeleton)
			meshSkinned->Step(skeleton);
	}
}