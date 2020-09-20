#include <opengl/aabb.h>

#include <opengl/geometry.h>
#include <opengl/material.h>
#include <opengl/shader.h>

#include <glm/gtx/norm.hpp>

// https://github.com/openscenegraph/OpenSceneGraph/blob/master/include/osg/BoundingBox

AABB::AABB()
{
	min = vec3(0);
	max = vec3(0);
	first = true;
}

AABB::~AABB()
{
	
}

void AABB::extend(const vec3 &vec)
{
	if (first)
	{
		min = vec;
		max = vec;
		first = false;
	}
	else
	{
		min = glm::min(vec, min);
		max = glm::max(vec, max);
	}
}

void AABB::extend(const AABB &bb)
{
	extend(bb.min);
	extend(bb.max);
}

//void AABB::extend(const float f)
//{
//	min -= vec3(f);
//	max += vec3(f);
//}

void AABB::translate(const vec3 &v)
{
	min += v;
	max += v;
}

vec3 AABB::diagional() const
{
	return max - min;
}

vec3 AABB::center() const
{
	return (min + max) * 0.5f;
}

float AABB::radius2() const
{
	return sqrt(0.25f * length2(max - min));
}

// todo change that fucking enum
AABB::INTERSECTION AABB::intersect(const AABB &b) const
{
	if (max.x < b.min.x || min.x > b.max.x ||
		max.y < b.min.y || min.y > b.max.y ||
		max.z < b.min.z || min.z > b.max.z)
		return OUTSIDE;

	if (min.x <= b.min.x && max.x >= b.max.x &&
		min.y <= b.min.y && max.y >= b.max.y &&
		min.z <= b.min.z && max.z >= b.max.z)
		return INSIDE;

	return INTERSECT;
}

AABB AABB::mult(const AABB &b, const mat4 &m)
{
	AABB a;
	std::vector<vec3> points = {
		vec3(b.min.x, b.min.y, b.min.z),
		vec3(b.max.x, b.min.y, b.min.z),
		vec3(b.min.x, b.max.y, b.min.z),
		vec3(b.min.x, b.min.y, b.max.z),

		vec3(b.max.x, b.max.y, b.max.z),
		vec3(b.min.x, b.max.y, b.max.z),
		vec3(b.max.x, b.min.y, b.max.z),
		vec3(b.max.x, b.max.y, b.min.z)};
	for (const vec3 &v : points)
	{
		a.extend(m * vec4(v, 1));
	}
	return a;
}

bool AABB::contains_vec(const vec3 &v) const
{
	return v.x >= min.x && v.x <= max.x &&
		   v.y >= min.y && v.y <= max.y &&
		   v.z >= min.z && v.z <= max.z;
}

float AABB::volume() const
{
	vec3 size = diagional();

	return size.x * size.y * size.z;
}

void AABB::geometrize()
{
	if (geometry)
		delete geometry;

	geometry = new Geometry;
	geometry->lines = true;

	Material *material = new Material;
	material->color = vec3(0.4f);
	material->src = &basic;
	geometry->material = material;

	std::vector<vec3> lines = {
		vec3(min.x, min.y, min.z), // 0 A
		vec3(max.x, min.y, min.z), // 1 C
		vec3(min.x, max.y, min.z), // 2 D
		vec3(min.x, min.y, max.z), // 3 B

		vec3(max.x, max.y, max.z),	// 4 E
		vec3(min.x, max.y, max.z),	// 5 G
		vec3(max.x, min.y, max.z),	// 6 H
		vec3(max.x, max.y, min.z)}; // 7 F

	/*GLushort*/ std::vector<GLuint> elements = {
		// axis
		0, 1,
		0, 2,
		0, 3,
		4, 5,
		4, 6,
		4, 7,
		1, 6,
		5, 2,
		7, 1,
		7, 2,
		3, 5,
		3, 6};

	geometry->Clear(lines.size(), elements.size());

	int i = 0;
	for (const vec3 &pos : lines)
	{
		Vertex &vertex = geometry->vertices[i++];
		vertex.position = pos;
	}

	geometry->elements.insert(geometry->elements.begin(), elements.begin(), elements.end());

	geometry->SetupMesh();
}