#pragma once

#include <miryks/miryks.hpp>
#include <miryks/skeleton.h>

#include <opengl/renderer.h>
#include <opengl/scene.h>
#include <opengl/scene_graph.hpp>
#include <opengl/geometry.h>
#include <opengl/material.h>


#define callback(x) void (x ## _callback) (RD, x ## _t *)


namespace miryks
{
	class Model;
	extern std::vector<Model *> mists;

	callback(ni_node);
	callback(bs_lighting_shader_property);
	callback(bs_water_shader_property);
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
	callback(bs_dynamic_tri_shape);

	void matrix_from_common(Group *, ni_common_layout_t *);

	class animation;
	class skeleton;

	class Model
	{
	public:
		Model();
		Model(resource *);
		~Model();
		nif *model = nullptr;
		std::map<int, Group *> groups;
		Group *baseGroup, *lastGroup;
		Group *MakeNewGroup(rundown *);
		void Construct();
		void Step();
		void Misty();
		virtual void v () {};
		
		std::vector<NiRef> shapes__;
	};

	class ModelSkinned : public Model
	{
	public:
		bool first;
		std::vector<bone *> bonesCached;
		NiRef lastShape;
		ModelSkinned(const char *);
		void Construct();
		void Step(skeleton *);
		void Update(skeleton *);
	};

	class SkinnedMesh
	{
	public:
		ModelSkinned *modelSkinned;
		skeleton *skel;
		animation *anim;
		GroupDrawer *groupDrawer;
		SkinnedMesh(const char *);
		~SkinnedMesh();
		void Step();
	};
}

#undef callback