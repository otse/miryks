#include "material"
#include "texture"

#include "camera"
#include "shader"
#include "scene"

int Material_t::pool = 0;

Material_t *Material_t::active = nullptr;

Material_t::Material_t()
{
	id = pool++;

	shader = sha_simple;

	transparent = double_sided = blending = testing = decal = false;

	opacity = 1.f;
	treshold = 0;
	shininess = 20.f;
	glossiness = 1.f;
	rotation = 0;

	depth_func = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

	repeat = vec2(1);
	emissive = vec3(17 / 255.f);

	compose_uv_transform();

	color = vec3(
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX);
}

void Material_t::compose_uv_transform()
{
	set_uv_transform_directly(
		offset.x, offset.y,
		repeat.x, repeat.y,
		rotation,
		center.x, center.y);
}

void Material_t::set_uv_transform_directly(
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

	uv_transform = mat3(
		a, d, g,
		b, e, h,
		c, f, i);
}

void Material_t::Use()
{
	if (this == active)
		return;

	Unuse(active, this);

	shader->Use();
	shader->SetVec3("color", color);
	shader->SetVec3("specular", specular);
	shader->SetFloat("opacity", opacity);
	shader->SetFloat("shininess", shininess);
	shader->SetFloat("glossiness", glossiness);

	shader->SetMat3("uvTransform", uv_transform);

	shader->SetFloat("alphaTest", testing ? treshold / 255.f : 0);
	shader->SetBool("doubleSided", double_sided);

	if (map)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, map->tid);
		shader->SetInt("map", 0);
	}
	if (normal_map)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, normal_map->tid);
		shader->SetInt("normalMap", 1);
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
	if (testing)
	{
		glDepthFunc(GL_LEQUAL);
	}
	if (transparent)
	{
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LESS);
	}
	if (double_sided)
	{
		glDisable(GL_CULL_FACE);
	}

	active = this;
}

void Material_t::Unuse(Material_t *a, Material_t *b)
{
	if (!a || a->map && !b->map)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (!a || a->normal_map && !b->normal_map)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (!a || a->decal && !b->decal)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0, 0);
	}
	if (!a || a->blending && !b->blending)
	{
		glDisable(GL_BLEND);
	}
	if (!a || a->testing && !b->testing)
	{
		glDepthFunc(GL_LEQUAL);
	}
	if (!a || a->transparent && !b->transparent)
	{
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
	}
	if (!a || a->double_sided && !b->double_sided)
	{
		glEnable(GL_CULL_FACE);
	}
}