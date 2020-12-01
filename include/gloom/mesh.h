#pragma once

#include <gloom/dark2.h>

#include <gloom/skeleton.h>

extern "C"
{
#include <bsa.h>
#include <nifp/nifp.h>
}

#include <opengl/types.h>
#include <opengl/scene.h>
#include <opengl/group.h>
#include <opengl/geometry.h>
#include <opengl/material.h>

namespace gloom
{
	class Mesh
	{
	public:
		Mesh();

		struct nifp *nif;

		std::map<int, Group *> groups;

		Group *baseGroup, *lastGroup;

		void Construct(nifp *);
		Group *Nested(nifprd *);

		static void Store(void *, Mesh *);
		static Mesh *GetStored(void *);
	};

	class SkinnedMesh
	{
	public:
		Mesh *mesh;

		Skeleton *skeleton;

		Group *lastShape;

		std::vector<ni_ref> shapes;

		std::vector<SkinPartition *> skins;

		SkinnedMesh()
		{
			mesh = nullptr;
			skeleton = nullptr;
			lastShape = nullptr;
		}

		void Construct();
		void Initial();
		void Forward();
		void SmeshToSkelBone();
	};

	class SkinPartition
	{
	public:
		SkinnedMesh *const smesh;

		skin_partition *const skin_partition;

		ni_ref shape;

		std::vector<Bone *> bones;

		SkinPartition(SkinnedMesh *smesh, ni_ref shape, struct skin_partition *skin_partition)
			: smesh(smesh), shape(shape), skin_partition(skin_partition)
		{
			//for ()
		}
	};
} // namespace gloom