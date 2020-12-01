#include <opengl/material.h>

extern "C"
{
#include "putc.h"
}

#include <opengl/texture.h>
#include <opengl/camera.h>
#include <opengl/shader.h>
#include <opengl/scene.h>

Material *Material::active = nullptr;

Material::Material()
{
	prepared = false;

	shader = nullptr;
	src = &basic;
	
	map = normalMap = glowMap = nullptr;

	transparent = doubleSided = blending = testing = decal = skinning = false;

	opacity = 1;
	treshold = 0;
	shininess = 5;
	glossiness = 100;
	rotation = 0;
	bones = 0;

	depth_func = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

	offset = vec2(0);
	repeat = vec2(1);
	center = vec2(0);

	composeUvTransform();

	color = vec3(1, 1, 1);
	specular = vec3(17.0 / 255.0);
	emissive = vec3(0);

	skin_partition = nullptr;
}

void Material::RandomColor()
{
	color = vec3(
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX);
}

void Material::Ready()
{
	header = "#version 330 core\n";
	char pepper[20];
	snprintf(pepper, 20, "// %s\n", (*src)[0]);
	header += pepper;
	if (!map)
		header += "#define DONT_USE_DIFFUSE_MAP\n";
	if (!normalMap)
		header += "#define DONT_USE_NORMAL_MAP\n";
	if (!glowMap)
		header += "#define DONT_USE_GLOW_MAP\n";
	if (!skinning)
		header += "#define DONT_USE_SKINNING\n";

	if (Shader::shaders.count(header))
	{
		shader = Shader::shaders[header];
	}
	else
	{
		//printf("new shader instance of type %s, header %s\n", (*src)[0], header);
		shader = new Shader(src);
		shader->header = header;
		shader->Compile();
		Shader::shaders.emplace(header, shader);
	}
	//shader = basicShader;
}

void Material::composeUvTransform()
{
	setUvTransformDirectly(
		offset.x, offset.y,
		repeat.x, repeat.y,
		rotation,
		center.x, center.y);
}

void Material::setUvTransformDirectly(
	float tx, float ty, float sx, float sy, float rotation, float cx, float cy)
{
	float x = cos(rotation);
	float y = sin(rotation);

	float a, b, c, d, e, f, g, h, i;
	a = sx * x;
	b = sx * y;
	c = -sx * (x * cx + y * cy) + cx + tx;
	d = -sy * y;
	e = sy * x;
	f = -sy * (-y * cx + c * cy) + cy + ty;
	g = 0;
	h = 0;
	i = 1;

	uvTransform = mat3(
		a, d, g,
		b, e, h,
		c, f, i);
}

void Material::Use()
{
	if (!prepared)
	{
		//cassert(prepared, "use material->Ready() when you are done setting fields for it");
		Ready();
		prepared = true;
	}

	if (this == active)
		return;

	Unuse(active, this);

	shader->Use();
	shader->SetVec3("color", color);
	shader->SetVec3("specular", specular);
	shader->SetVec3("emissive", emissive);
	shader->SetMat3("uvTransform", uvTransform);
	shader->SetBool("doubleSided", doubleSided);
	shader->SetFloat("opacity", opacity);
	shader->SetFloat("shininess", shininess);
	shader->SetFloat("glossiness", glossiness);
	shader->SetFloat("alphaTest", treshold);

	if (skinning)
	{
		shader->SetMat4("bindMatrix", bindMatrix);
		shader->SetMat4("bindMatrixInverse", glm::inverse(bindMatrix));
		for (unsigned int u = 0; u < bones; u ++)
		{
			char s[100];
			snprintf(s, 100, "boneMatrices[%u]", u);
			shader->SetMat4(s, boneMatrices[u]);
		}
	}

	if (map)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, map->tid);
		shader->SetInt("map", 0);
	}
	if (normalMap)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, normalMap->tid);
		shader->SetInt("normalMap", 1);
	}
	if (glowMap)
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, glowMap->tid);
		shader->SetInt("glowMap", 2);
	}
	if (decal)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-2.f, -2.f);
	}
	if (blending)
	{
		glEnable(GL_BLEND);
		glBlendFunc(depth_func.sfactor, depth_func.dfactor);
	}
	//if (testing)
	//	glDepthFunc(GL_LEQUAL);
	if (transparent)
	{
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LESS);
	}
	if (doubleSided)
		glDisable(GL_CULL_FACE);

	active = this;
}

void Material::Unuse(Material *a, Material *b)
{
	if (!a || a->map && !b->map)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (!a || a->normalMap && !b->normalMap)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (!a || a->glowMap && !b->glowMap)
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (!a || a->decal && !b->decal)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0, 0);
	}
	if (!a || a->blending && !b->blending)
		glDisable(GL_BLEND);
	//if (!a || a->testing && !b->testing)
	//	glDepthFunc(GL_LEQUAL);
	if (!a || a->transparent && !b->transparent)
	{
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
	}
	if (!a || a->doubleSided && !b->doubleSided)
		glEnable(GL_CULL_FACE);

	active = b;
}