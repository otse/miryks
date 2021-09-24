#include <skyrim/model.h>

#include <renderer/drawgroup.h>

namespace skyrim
{
	SKSkinnedMesh::SKSkinnedMesh(const char *path)
	{
		skeleton = nullptr;
		animation = nullptr;
		modelSkinned = new SKModelSkinned(path);
		drawGroup = new DrawGroup(
			modelSkinned->baseGroup, mat4(1.0));
	}

	void SKSkinnedMesh::Step()
	{
		if (modelSkinned && skeleton)
			modelSkinned->Step(skeleton);
	}
}