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
	static void other(Rd *, void *);
	callback(ni_node)
	#ifdef SLE
	callback(ni_tri_shape)
	callback(ni_tri_shape_data)
	#endif
	callback(bs_lighting_shader_property)
	callback(bs_effect_shader_property)
	callback(bs_effect_shader_property_float_controller)
	callback(ni_float_interpolator)
	callback(ni_float_data)
	callback(bs_shader_texture_set)
	callback(ni_alpha_property)
	callback(ni_skin_instance)
	callback(ni_skin_data)
	callback(ni_skin_partition)
	callback(bs_tri_shape)
	// for skinnedmesh
	static void ni_node_callback_2      (Rd *, NiNode *);
	static void ni_tri_shape_callback_2 (Rd *, NiTriShape *);

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
		rd->other = other;
		rd->ni_node_callback = ni_node_callback;
		rd->bs_tri_shape_callback = bs_tri_shape_callback;
		#ifdef SLE
		rd->ni_tri_shape_callback = ni_tri_shape_callback;
		rd->ni_tri_shape_data_callback = ni_tri_shape_data_callback;
		#endif
		rd->bs_lighting_shader_property_callback = bs_lighting_shader_property_callback;
		rd->bs_effect_shader_property_callback = bs_effect_shader_property_callback;
		rd->bs_shader_texture_set_callback = bs_shader_texture_set_callback;
		rd->ni_alpha_property_callback = ni_alpha_property_callback;
		nif_rd(rd);
		free_nifprd(&rd);
		baseGroup->Update();
	}
	void SkinnedMesh::construct()
	{
		assertm(skeleton, "smesh needs skeleton");
		Rd *rd = calloc_nifprd();
		rd->nif = mesh->nif;
		rd->data = this;
		rd->other = other;
		rd->ni_node_callback = ni_node_callback_2;
		#ifdef SLE
		rd->ni_tri_shape_callback = ni_tri_shape_callback_2;
		#endif
		rd->ni_skin_instance_callback = ni_skin_instance_callback;
		rd->ni_skin_data_callback = ni_skin_data_callback;
		rd->ni_skin_partition_callback = ni_skin_partition_callback;
		nif_rd(rd);
		free_nifprd(&rd);
		initial();
	}
	void SkinnedMesh::initial()
	{
		/*
		for (ni_ref index : shapes)
		{
			//Group *group = mesh->groups[index];
			auto shape = (ni_tri_shape *)nif_get_block(mesh->nif, index);
			auto si = (ni_skin_instance *)nif_get_block(mesh->nif, shape->A->skin_instance);
			auto sp = (ni_skin_partition *)nif_get_block(mesh->nif, si->A->skin_partition);
			
			for (unsigned int k = 0; k < *sp->num_skin_partition_blocks; k++)
			{
				struct skin_partition *part = sp->skin_partition_blocks[k];
				Group *group = mesh->groups[index]->groups[k];
				Material *material = group->geometry->material;
				material->boneMatrices.clear();
				material->boneMatrices.reserve(part->A->num_bones);
				material->bindMatrix = group->matrixWorld;
				for (int i = 0; i < part->A->num_bones; i++)
				{
					auto node = (ni_node *)nif_get_block(mesh->nif, si->bones[part->bones[i]]);
					char *name = nif_get_string(mesh->nif, node->common->F->name);
					auto has = skeleton->bonesNamed.find(name);
					if (has == skeleton->bonesNamed.end())
					{
						material->boneMatrices.push_back(mat4(1.0));
						continue;
					}
					Bone *bone = has->second;
					material->boneMatrices.push_back(bone->group->matrixWorld * inverse(bone->rest));
				}
			}
		}*/
	}
	void SkinnedMesh::forward()
	{
		if (skeleton)
			skeleton->step();
		initial();
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
	
	void other(Rd *rd, void *block)
	{
		Mesh *mesh = (Mesh *)rd->data;
		//printf("Rd unhandled other block type %s\n", nif_get_block_type(rd->nif, rd->current));
	}
	void matrix_from_common(Group *group, ni_common_layout_t *common)
	{
		group->matrix = translate(group->matrix, gloomVec3(common->A->translation));
		group->matrix *= inverse(mat4(gloomMat3(common->A->rotation)));
		group->matrix = scale(group->matrix, vec3(common->A->scale));
	}
	void ni_node_callback(Rd *rd, NiNode *block)
	{
		// printf("ni node callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->nested(rd);
		matrix_from_common(group, block->common);
	}
	#ifdef SLE
	void ni_tri_shape_callback(Rd *rd, NiTriShape *block)
	{
		// printf("ni tri shape callback %s\n", block->common.name_string);
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->nested(rd);
		matrix_from_common(group, block->common);
		if (strstr(nif_get_string(rd->nif, block->common->F->name), "Marker") != NULL)
			group->visible = false;
		//if (block->A->skin_instance == -1)
		{
			group->geometry = new Geometry();
			group->geometry->material->src = &simple;
		}
	}
	#endif
	void ni_node_callback_2(Rd *rd, NiNode *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		if (rd->current == 0)
			return;
		Group *group = smesh->mesh->groups[rd->current];
		//group->visible = false;
		//if (group->geometry)
		//	group->geometry->material->color = vec3(1);
	}
	#ifdef SLE
	void ni_tri_shape_callback_2(Rd *rd, NiTriShape *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		smesh->lastShape = smesh->mesh->groups[rd->current];
	}
	#endif
	#ifdef SLE
	void ni_tri_shape_data_callback(Rd *rd, NiTriShapeData *block)
	{
		// printf("ni tri shape data callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Geometry *geometry = mesh->lastGroup->geometry;
		geometry->Clear(0, 0);
		if (!block->A->num_vertices)
			return;
		if (block->J->has_triangles)
		{
			geometry->Clear(block->A->num_vertices, block->J->num_triangles * 3);
			for (int i = 0; i < block->J->num_triangles; i++)
			{
				unsigned short *triangle = (unsigned short *)&block->triangles[i];
				geometry->elements.insert(geometry->elements.end(), {triangle[0], triangle[1], triangle[2]});
			}
		}
		for (int i = 0; i < block->A->num_vertices; i++)
		{
			geometry->vertices[i].position = gloomVec3(block->vertices[i]);
			if (block->C->bs_vector_flags & 0x00000001)
				geometry->vertices[i].uv = *cast_vec_2((float *)&block->uv_sets[i]);
			geometry->vertices[i].normal = gloomVec3(block->normals[i]);
			if (block->C->bs_vector_flags & 0x00001000)
			{
				geometry->material->tangents = true;
				geometry->vertices[i].tangent = gloomVec3(block->tangents[i]);
				geometry->vertices[i].bitangent = gloomVec3(block->bitangents[i]);
			}
			if (block->G->has_vertex_colors)
				geometry->vertices[i].color = gloomVec4(block->vertex_colors[i]);
		}
		geometry->SetupMesh();
	}
	#endif
	vec3 bytestofloat(ByteVector3 &vec)
	{
		float xf, yf, zf;
		xf = (double( vec.x ) / 255.0) * 2.0 - 1.0;
		yf = (double( vec.y ) / 255.0) * 2.0 - 1.0;
		zf = (double( vec.z ) / 255.0) * 2.0 - 1.0;
		return vec3(xf, yf, zf);
	}
	vec2 halftexcoord(HalfTexCoord &uv)
	{
		union { float f; uint32_t i; } u, v;
		u.i = half_to_float(uv.u);
		v.i = half_to_float(uv.v);
		return vec2(u.f, v.f);
	}
	special_edition
	void bs_tri_shape_callback(Rd *rd, BSTriShape *block)
	{
		// printf("mesh.cpp bs tri shape callback !!! ");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->nested(rd);
		matrix_from_common(group, block->common);
		Geometry *geometry = new Geometry();
		group->geometry = geometry;
		geometry->material->src = &simple;
		const char *name = nif_get_string(rd->nif, block->common->F->name);
		if (strstr(name, "Marker"))
		{
			return;
		}
		if (!block->vertex_data_all && !block->vertex_data_no_clr)
			return;
		geometry->Clear(block->infos->num_vertices, block->infos->num_triangles * 3);
		if (!block->infos->num_vertices)
			return;
		if (block->infos->num_triangles)
		{
			for (int i = 0; i < block->infos->num_triangles; i++)
			{
				unsigned short *triangle = (unsigned short *)&block->triangles[i];
				geometry->elements.insert(geometry->elements.end(), {triangle[0], triangle[1], triangle[2]});
			}
		}
		if ( block->vertex_data_all )
		{
		for (int i = 0; i < block->infos->num_vertices; i++)
		{
			struct bs_vertex_data_sse_all *vertex_data = &block->vertex_data_all[i]; 
			geometry->vertices[i].position = gloomVec3(vertex_data->vertex);
			geometry->vertices[i].uv = halftexcoord(vertex_data->uv);
			geometry->vertices[i].normal = bytestofloat(vertex_data->normal);
			geometry->material->tangents = true;
			geometry->vertices[i].tangent = bytestofloat(vertex_data->tangent);
			auto c = vertex_data->vertex_colors;
			geometry->vertices[i].color = vec4(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
		}
		}
		if ( block->vertex_data_no_clr )
		{
		for (int i = 0; i < block->infos->num_vertices; i++)
		{
			struct bs_vertex_data_sse_no_clr *vertex_data = &block->vertex_data_no_clr[i]; 
			geometry->vertices[i].position = gloomVec3(vertex_data->vertex);
			geometry->vertices[i].uv = halftexcoord(vertex_data->uv);
			geometry->vertices[i].normal = bytestofloat(vertex_data->normal);
			geometry->material->tangents = true;
			geometry->vertices[i].tangent = bytestofloat(vertex_data->tangent);
		}
		}
		geometry->SetupMesh();
	}
	void bs_lighting_shader_property_callback(Rd *rd, BSLightingShaderProperty *block)
	{
		// printf("bs lighting shader property callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Geometry *geometry = mesh->lastGroup->geometry;
		if (geometry)
		{
			Material *material = geometry->material;
			material->color = vec3(1.0);
			material->emissive = gloomVec3(block->B->emissive_color);
			material->specular = gloomVec3(block->B->specular_color);
			material->specular *= block->B->specular_strength;
			material->opacity = block->B->alpha;
			material->glossiness = block->B->glossiness;
			if (block->B->shader_flags_1 & 0x00000002)
				material->skinning = true;
			else
				material->dust = true;
			if (block->B->shader_flags_1 & 0x04000000)
				material->decal = true;
			if (block->B->shader_flags_1 & 0x08000000) // dynamic
				material->decal = true;
			if (block->B->shader_flags_1 & 0x00001000)
				//printf("Model_Space_Normals\n");
				material->modelSpaceNormals = true;
			if (block->B->shader_flags_2 & 0x00000001)
				material->zwrite = true;
			if (block->B->shader_flags_2 & 0x00000020)
				material->vertexColors = true;
			if (block->B->shader_flags_2 & 0x00000010)
				material->doubleSided = true;
			if (block->B->shader_flags_1 & 0x00000008)
				material->defines += "#define VERTEX_ALPHA\n";
			if (block->B->shader_flags_2 & 0x20000000)
				material->defines += "#define TREE_ANIM\n";
		}
	}
	void bs_effect_shader_property_callback(Rd *rd, BSEffectShaderProperty *block)
	{
		Mesh *mesh = (Mesh *)rd->data;
		Geometry *geometry = mesh->lastGroup->geometry;
		if (geometry)
		{
			Material *material = geometry->material;
			material->src = &fxs;
			material->transparent = true;
			material->color = gloomVec3(block->D->base_color);
			material->opacity = block->D->base_color_scale;
			material->map = GetProduceTexture(block->source_texture);
			if (block->B->shader_flags_2 & 0x00000020)
				material->vertexColors = true;
			if (block->B->shader_flags_1 & 0x80000000)
				material->testing = true;
			if (block->B->shader_flags_2 & 0x00000001)
				material->zwrite = true;
			if (block->B->shader_flags_2 & 0x00000010)
				material->doubleSided = true;
		}
	}
	void bs_shader_texture_set_callback(Rd *rd, BSShaderTextureSet *block)
	{
		// printf("bs shader texture set callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->lastGroup;
		Geometry *geometry = group->geometry;
		if (geometry)
		{
			Material *material = geometry->material;
			for (int i = 0; i < block->A->num_textures; i++)
			{
				std::string path = std::string(block->textures[i]);
				if (path.empty())
					continue;
				if (path.find("skyrimhd\\build\\pc\\data\\") != std::string::npos)
					path = path.substr(23, std::string::npos);
				if (i == 0)
					material->map = GetProduceTexture(path.c_str());
				if (i == 1)
					material->normalMap = GetProduceTexture(path.c_str());
				if (i == 2)
					material->glowMap = GetProduceTexture(path.c_str());
			}
		}
	}
	void ni_alpha_property_callback(Rd *rd, NiAlphaProperty *block)
	{
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->lastGroup;
		Geometry *geometry = group->geometry;
		Material *material = geometry->material;
		if (geometry)
		{
			const int blendModes[] = {
				GL_ONE,
				GL_ZERO,
				GL_SRC_COLOR,
				GL_ONE_MINUS_SRC_COLOR,
				GL_DST_COLOR,
				GL_ONE_MINUS_DST_COLOR,
				GL_SRC_ALPHA,
				GL_ONE_MINUS_SRC_ALPHA,
				GL_DST_ALPHA,
				GL_ONE_MINUS_DST_ALPHA,
				GL_SRC_ALPHA_SATURATE};
			const int testModes[] = {
				GL_ALWAYS,
				GL_LESS,
				GL_EQUAL,
				GL_LEQUAL,
				GL_GREATER,
				GL_NOTEQUAL,
				GL_GEQUAL,
				GL_NEVER};
			unsigned short flags = block->C->flags;
			material->blending = (bool)(flags & 1);
			material->testing = (bool)(flags & (1 << 9));
			int src = blendModes[flags >> 1 & 0x0f];
			int dst = blendModes[flags >> 5 & 0x0f];
			material->blendFunc = {src, dst};
			material->treshold = block->C->treshold / 255.f;
			material->testFunc = testModes[flags >> 10 & 0x07];
		}
	}
	legendary_edition
	void ni_skin_instance_callback(Rd *rd, NiSkinInstance *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		Nif *nif = smesh->mesh->nif;
		assertm(0 == strcmp(nif_get_block_type(nif, rd->parent), NiTriShapeS), "root not shape");
		auto shape = (ni_tri_shape_t *)nif_get_block(nif, rd->parent);
		smesh->shapes.push_back(rd->parent);
	}
	legendary_edition
	void ni_skin_data_callback(Rd *rd, NiSkinData *block)
	{
		//
	}
	special_edition
	void ni_skin_partition_callback(Rd *rd, NiSkinPartition *block)
	{

	}
	/*
	legendary_edition
	void ni_skin_partition_callback(Rd *rd, ni_skin_partition *block)
	{
		
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		auto nif = smesh->mesh->nif;
		auto shape = (ni_tri_shape *)nif_get_block(nif, smesh->shapes.back());
		auto data = (ni_tri_shape_data *)nif_get_block(nif, shape->A->data);
		for (unsigned int k = 0; k < *block->num_skin_partition_blocks; k++)
		{
			struct skin_partition *part = block->skin_partition_blocks[k];
			Group *group = new Group;
			Geometry *geometry = new Geometry();
			geometry->skinning = true;
			geometry->Clear(0, 0);
			if (!data->A->num_vertices)
				continue;
			if (part->A->num_triangles > 0)
			{
				geometry->Clear(part->A->num_vertices, part->A->num_triangles * 3);
				for (int i = 0; i < part->A->num_triangles; i++)
				{
					unsigned short *triangle = (unsigned short *)&part->triangles[i];
					geometry->elements.insert(geometry->elements.end(), {triangle[0], triangle[1], triangle[2]});
				}
			}
			for (int i = 0; i < part->A->num_vertices; i++)
			{
				if (!*part->has_vertex_map)
					break;
				unsigned short j = part->vertex_map[i];
				geometry->vertices[i].position = gloomVec3(data->vertices[j]);
				if (data->C->bs_vector_flags & 0x00000001)
					geometry->vertices[i].uv = *cast_vec_2((float *)&data->uv_sets[j]);
				if (data->C->has_normals)
					geometry->vertices[i].normal = gloomVec3(data->normals[j]);
				if (data->C->bs_vector_flags & 0x00001000)
				{
					smesh->lastShape->geometry->material->tangents = true;
					geometry->vertices[i].tangent = gloomVec3(data->tangents[j]);
					geometry->vertices[i].bitangent = gloomVec3(data->bitangents[j]);
				}
				if (data->G->has_vertex_colors)
					geometry->vertices[i].color = gloomVec4(data->vertex_colors[j]);
				if (*part->has_bone_indices)
				{
					auto a = part->bone_indices[i];
					geometry->vertices[i].skin_index = vec4(a.a, a.b, a.c, a.d);
				}
				if (*part->has_vertex_weights)
					geometry->vertices[i].skin_weight = gloomVec4(part->vertex_weights[i]);
			}
			//if (smesh->lastShape->geometry->material->tangents)
			//	printf("has tangents");
			geometry->material = new Material(*smesh->lastShape->geometry->material);
			geometry->material->bones = part->A->num_bones;
			//geometry->material->skinning = true;
			geometry->material->prepared = false;
			geometry->SetupMesh();
			group->geometry = geometry;
			smesh->lastShape->Add(group);
		}
		smesh->lastShape->Update();
	}
	*/

} // namespace dark