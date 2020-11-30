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

		std::vector<struct ni_tri_shape_pointer *> roots;
		
		SkinnedMesh(Mesh *mesh) : mesh(mesh)
		{
			
		}

		void Construct();
		void Initial();

		void Forward();

	};
} // namespace gloom