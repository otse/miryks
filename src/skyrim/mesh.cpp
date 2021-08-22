extern "C"
{	
#include <half.h>
}

#include "mesh.h"

#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/types.h>

using namespace dark;

#define callback(x) static void (x ## _callback) (Rd *, x ## _t *);

namespace skyrim
{
	Mesh::Mesh()
	{
		baseGroup = new GroupBounded();
		groups[-1] = baseGroup;
		lastGroup = baseGroup;
	}
	
	Mesh::Mesh(Nif *bucket) : Mesh()
	{
		nif = bucket;
		construct();
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::construct()
	{
		Rd *rd = calloc_nifprd();
		rd->nif = nif;
		rd->data = this;
		//rd->other = other;
		rd->ni_node_callback = ni_node_callback;
		rd->bs_tri_shape_callback = bs_tri_shape_callback;
		rd->bs_lighting_shader_property_callback = bs_lighting_shader_property_callback;
		rd->bs_effect_shader_property_callback = bs_effect_shader_property_callback;
		rd->bs_shader_texture_set_callback = bs_shader_texture_set_callback;
		rd->ni_alpha_property_callback = ni_alpha_property_callback;
		nif_rd(rd);
		free_nifprd(&rd);
		baseGroup->Update();
	}
	double easeInOutQuad( double t ) {
		return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
	}
	void Mesh::forward()
	{
		// for mists! do the following
		// stolen from nifskope i think
		auto callback = [](Rd *rd, BSEffectShaderPropertyFloatController *block) {
			Mesh *mesh = (Mesh *)rd->data;
			auto target = (BSEffectShaderProperty *)nif_get_block(rd->nif, block->A->target);
			//auto shape = (bs_tri_shape *)nif_get_block(rd->nif, target->meta.parent);
			Group *group = nullptr;
			auto next_controller = block;
			while(next_controller)
			{
				auto controller = next_controller;
				next_controller = nullptr;
				if (controller->A->next_controller > -1)
					next_controller = (BSEffectShaderPropertyFloatController *)nif_get_block(rd->nif, controller->A->next_controller);
				if (target)
					group = mesh->groups[target->meta.parent];
				if (!group || !group->geometry)
					return;
				controller->meta.time += delta * 1.0;
				if (controller->meta.time > controller->A->stop_time)
					controller->meta.time -= controller->A->stop_time;
				float *uv = &target->meta.u;
				if (controller->A->controlled_variable == 6)
					uv = &target->meta.u;
				else if (controller->A->controlled_variable == 8)
					uv = &target->meta.v;
				auto interpolator = (NiFloatInterpolator *)nif_get_block(rd->nif, controller->A->interpolator);
				if (controller->A->interpolator)
				{
					if (interpolator->A->data)
					{
						auto data = (NiFloatData *)nif_get_block(rd->nif, interpolator->A->data);
						for (unsigned int i = data->A->num_keys; i-- > 0;)
						{
							int j = (i + 1 >= data->A->num_keys) ? 0 : i + 1;
							if (data->linear_keys)
							{
								auto one = &data->linear_keys[i];
								auto two = &data->linear_keys[j];
								if (one->time > controller->meta.time)
									continue;
								float ratio = (controller->meta.time - two->time) / (two->time - one->time) + 1.0f;
								ratio = ratio < 0 ? 0 : ratio > 1 ? 1 : ratio;

								float a = one->value * (1.0f - ratio);
								float b = two->value * ratio;
								*uv = a + b;
								break;
							}

							else if (data->quadratic_keys)
							{
								auto one = &data->quadratic_keys[i];
								auto two = &data->quadratic_keys[j];
								if (one->time > controller->meta.time)
									continue;
								float ratio = (controller->meta.time - two->time) / (two->time - one->time) + 1.0f;
								ratio = ratio < 0 ? 0 : ratio > 1 ? 1 : ratio;

								float x2 = ratio * ratio;
								float x3 = x2 * ratio;

								float x = easeInOutQuad(ratio);
								*uv = (one->value * x) + (two->value * (1.0 - x));
								break;
							}
						}
					}
				}

				Material *material = group->geometry->material;
				
				float u, v, s, t;
				u = target->meta.u + target->B->uv_offset.x;
				v = target->meta.v + target->B->uv_offset.y;
				s = 1.0f / target->B->uv_scale.x;
				t = 1.0f / target->B->uv_scale.y;
				
				material->setUvTransformDirectly(u, v, s, t, 0, 0, 0);
			}
		};
		Rd *rd = calloc_nifprd();
		rd->nif = nif;
		rd->data = this;
		rd->bs_effect_shader_property_float_controller_callback = callback;
		nif_rd(rd);
		free_nifprd(&rd);
	}

	Group *Mesh::nested(Rd *rd)
	{
		Group *group = new GroupBounded();
		groups[rd->current] = group;
		groups[rd->parent]->Add(group);
		lastGroup = group;
		return group;
	}

}