#pragma once

// part of gloom

#include <Gloom/Dark2.h>

#include <Gloom/Skeleton.h>

extern "C"
{
#include <bsa.h>
#include <nifp/nifp.h>
}

#include <OpenGL/Types.h>
#include <OpenGL/Scene.h>
#include <OpenGL/Group.h>
#include <OpenGL/Geometry.h>
#include <OpenGL/Material.h>

namespace gloom
{
	class Mesh
	{
	public:
		Mesh();
		Nif *nif;
		std::map<int, Group *> groups;
		Group *baseGroup, *lastGroup;
		void Construct(Nif *);
		Group *Nested(Rd *);
		static void Store(void *, Mesh *);
		static Mesh *Cached(void *);
	};

	class SkinnedMesh
	{
	public:
		Mesh *mesh;
		Skeleton *skeleton;
		Group *lastShape;
		std::vector<ni_ref> shapes;
		//std::vector<SkinPartition *> skins;
		SkinnedMesh()
		{
			mesh = nullptr;
			skeleton = nullptr;
			lastShape = nullptr;
		}
		void Construct();
		void Initial();
		void Forward();
	};
} // namespace gloom