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
		Mesh *const mesh;

		Skeleton *skeleton;

		Group *lastShape;

		std::vector<ni_ref> shapes;
		std::vector<SkinPartition *> parts;

		SkinnedMesh(Mesh *mesh) : mesh(mesh)
		{
			lastShape = nullptr;
			skeleton = nullptr;
		}

		void Construct();
		void Initial();
		void Forward();
	};

	class SkinPartition
	{
	public:
		ni_ref shape;

		SkinnedMesh *const smesh;

		std::vector<Bone *> bones;

		SkinPartition(SkinnedMesh *smesh, ni_ref shape) : smesh(smesh), shape(shape)
		{
			//for ()
		}
	};
} // namespace gloom