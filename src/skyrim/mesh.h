#pragma once

#include <dark/dark.h>
#include <skyrim/skyrim.h>

#include <renderer/types.h>
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
		Mesh(Nif *);
		~Mesh();
		Nif *nif = nullptr;
		//Material *aa;
		std::map<int, Group *> groups;
		Group *baseGroup, *lastGroup;
		Group *make_new_group(Rd *);
		void construct();
		void forward();
		virtual void v () {};
	};

	// should have its own .h XD
	class SkinnedMesh : public Mesh
	{
	public:
		Skeleton *skeleton = nullptr;
		Group *lastShape = nullptr;

		std::vector<BSTriShape *> shapes;
		//std::vector<SkinPartition *> skins;
		SkinnedMesh();
		SkinnedMesh(Nif *, Skeleton *);
		void construct();
		void initial();
		void forward();
	};
}

#undef callback