extern "C"
{	
#include <half.h>
}

// see lib/rd.c for equal
// this has common rds used by regular and skinned meshes

#include "mesh.h"

#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/types.h>

using namespace dark;

namespace skyrim
{
	void matrix_from_common(Group *group, ni_common_layout_t *common)
	{
        // todo doesnt fit two columns on laptop
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

}