#include <miryks/model.h>

#include <opengl/drawgroup.h>

namespace miryks
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