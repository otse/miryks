#ifndef OPENGL_GEOMETRY_H
#define OPENGL_GEOMETRY_H

#include "types.h"

#include "aabb.h"

struct Vertex
{
	vec3 position;
	vec2 uv;
	vec3 normal;
	vec4 color = vec4(1, 1, 1, 1);
	vec3 tangent;
	vec3 bitangent;
};

struct Geometry
{
	static int num;
	bool created = false;
	bool lines = false;
	bool collide = true;

	GLuint vao, vbo, ebo;

	AABB aabb;
	
	Group *parent = nullptr;
	Material *material = nullptr;
	//std::vector<texture_t> textures;

	vector<Vertex> vertices;
	vector<GLuint> elements;

	Geometry();
	~Geometry();

	void Draw(const mat4 &);

	void Clear(int, int);

	void SetupMesh();
};

#endif