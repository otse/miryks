#include <renderer/aabb.h>
#include <renderer/geometry.h>
#include <renderer/material.h>
#include <renderer/shader.h>

#include <glm/gtx/norm.hpp>

// https://github.com/openscenegraph/OpenSceneGraph/blob/master/include/osg/BoundingBox

Aabb::Aabb()
{
	min = max = vec3(0);
	valid = false;
}

Aabb::~Aabb()
{
	
}

void Aabb::extend(const vec3 &vec)
{
	if (!valid)
	{
		min = max = vec;
		valid = true;
	}
	else
	{
		min = glm::min(vec, min);
		max = glm::max(vec, max);
	}
}

void Aabb::extend(const Aabb &bb)
{
	extend(bb.min);
	extend(bb.max);
}

//void Aabb::extend(const float f)
//{
//	min -= vec3(f);
//	max += vec3(f);
//}

void Aabb::translate(const vec3 &v)
{
	min += v;
	max += v;
}

vec3 Aabb::diagional() const
{
	return max - min;
}

vec3 Aabb::center() const
{
	return (min + max) * 0.5f;
}

float Aabb::radius2() const
{
	return sqrt(0.25f * length2(max - min));
}

// todo change that fucking enum
Aabb::INTERSECTION Aabb::intersect(const Aabb &b) const
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

Aabb Aabb::mult(const Aabb &b, const mat4 &m)
{
	Aabb a;
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

bool Aabb::contains_vec(const vec3 &v) const
{
	return v.x >= min.x && v.x <= max.x &&
		   v.y >= min.y && v.y <= max.y &&
		   v.z >= min.z && v.z <= max.z;
}

float Aabb::volume() const
{
	vec3 size = diagional();

	return size.x * size.y * size.z;
}

void Aabb::geometrize()
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