#include <miryks/model.h>

#include <ogl/scene_graph.hpp>

namespace miryks
{
	SkinnedMesh::SkinnedMesh(const char *path)
	{
		skel = nullptr;
		anim = nullptr;
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