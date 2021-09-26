#include <skyrim/model.h>

#include <renderer/drawgroup.h>

namespace skyrim
{
	SkinnedMesh::SkinnedMesh(const char *path)
	{
		skel = nullptr;
		animation = nullptr;
		modelSkinned = new ModelSkinned(path);
		drawGroup = new DrawGroup(
			modelSkinned->baseGroup, mat4(1.0));
	}

	void SkinnedMesh::Step()
	{
		if (modelSkinned && skel)
			modelSkinned->Step(skel);
	}
}