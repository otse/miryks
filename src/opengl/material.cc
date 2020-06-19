#include "material"
#include "texture"

#include "camera"
#include "shader"
#include "scene"

int material_t::pool = 0;

material_t *material_t::active = nullptr;

material_t::material_t()
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

void material_t::compose_uv_transform()
{
	set_uv_transform_directly(
		offset.x, offset.y,
		repeat.x, repeat.y,
		rotation,
		center.x, center.y);
}

void material_t::set_uv_transform_directly(
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

void material_t::Use()
{
	if (this == active)
		return;

	Unuse(active, this);

	shader->Use();
	shader->setVec3("color", color);
	shader->setVec3("specular", specular);
	shader->setFloat("opacity", opacity);
	shader->setFloat("shininess", shininess);
	shader->setFloat("glossiness", glossiness);

	shader->setMat3("uvTransform", uv_transform);

	if (map)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, map->tid);
		shader->setInt("map", 0);
	}
	if (normal_map)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, normal_map->tid);
		shader->setInt("normalMap", 1);
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
		shader->setFloat("alphaTest", treshold / 255.f);
	}
	else
		shader->setFloat("alphaTest", 0);

	if (transparent)
	{
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LESS);
	}
	if (double_sided)
	{
		glDisable(GL_CULL_FACE);
		shader->setBool("doubleSided", true);
	}
	else
		shader->setBool("doubleSided", false);

	active = this;

	//if (checkGlError) {
	//	detect_opengl_error("bind material " + name);
	//}
}

void material_t::Unuse(material_t *a, material_t *b)
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