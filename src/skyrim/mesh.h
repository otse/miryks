#pragma once

#include <dark/dark.h>
#include <skyrim/skyrim.h>

#include <renderer/renderer.h>
#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/geometry.h>
#include <renderer/material.h>

#include "skeleton.h"

#define callback(x) void (x ## _callback) (Rd *, x ## _t *)

namespace skyrim
{
	callback(ni_node);
	callback(bs_lighting_shader_property);
	callback(bs_effect_shader_property);
	callback(bs_effect_shader_property_float_controller);
	callback(ni_float_interpolator);
	callback(ni_float_data);
	callback(bs_shader_texture_set);
	callback(ni_alpha_property);
	callback(ni_skin_instance);
	callback(ni_skin_data);
	callback(ni_skin_partition);
	callback(bs_tri_shape);

	class Mesh
	{
	public:
		Mesh();
		Mesh(const char *);
		~Mesh();
		Nif *model = nullptr;
		std::map<int, Group *> groups;
		Group *baseGroup, *lastGroup;
		Group *make_new_group(Rd *);
		void Construct();
		void Step();
		void Misty();
		virtual void v () {};
		
		std::vector<NiRef> shapes__;
	};

	class MeshSkinned : public Mesh
	{
	public:
		NiRef lastShape;
		MeshSkinned(const char *);
		void Construct();
		void Step(Skeleton *);
		void Initial(Skeleton *);
	};

	class SkinnedMesh
	{
	public:
		MeshSkinned *meshSkinned = nullptr;
		Skeleton *skeleton = nullptr;
		Animation *animation = nullptr;
		DrawGroup *drawGroup = nullptr;
		SkinnedMesh(const char *);
		~SkinnedMesh();
		void Step();
	};
}

#undef callback