#pragma once

// part of gloom

#include <dark/dark.h>

#include <skyrim/skeleton.h>

extern "C"
{
#include <bsa.h>
#include <nifp/nifp.h>
}

#include <renderer/types.h>
#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/geometry.h>
#include <renderer/material.h>

namespace dark
{
	class Mesh
	{
	public:
		Mesh();
		Mesh(Nif *);
		~Mesh();
		Nif *nif = nullptr;
		std::map<int, Group *> groups;
		Group *baseGroup, *lastGroup;
		void construct();
		Group *nested(Rd *);
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
		SkinnedMesh(Mesh *mesh, Skeleton *skeleton) : SkinnedMesh()
		{
			this->mesh = mesh;
			this->skeleton = skeleton;
			construct();
		}
		void construct();
		void initial();
		void forward();
	};
} // namespace dark