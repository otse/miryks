#ifndef OPENGL_GEOMETRY_H
#define OPENGL_GEOMETRY_H

// part of gloom

#include <opengl/renderer.h>
#include <opengl/aabb.h>

struct Vertex
{
	vec3 position;
	vec2 uv;
	vec3 normal;
	vec4 color = vec4(1, 1, 1, 1);
	vec3 tangent = vec3(1, 1, 1);
	vec3 bitangent = vec3(1, 1, 1);
	vec4 skin_index;
	vec4 skin_weight;
};
struct Geometry
{
	static int num;
	bool created = false;
	bool lines = false;
	bool collide = true;
	bool skinning = false;
	bool landscape = false;
	GLuint vao = 0, vbo = 0, ebo = 0;
	Aabb aabb;
	Group *parent = nullptr;
	Material *material = nullptr;
	//std::vector<texture_t> textures;
	void recomputeTangents();
	std::vector<Vertex> vertices;
	std::vector<GLuint> elements;
	Geometry();
	~Geometry();
	void Draw(const mat4 &);
	void Clear(int, int);
	void SetupMesh();
};

struct VertexLand
{
	vec3 position;
	vec2 uv;
	vec3 normal;
	vec4 color = vec4(1, 1, 1, 1);
	vec3 tangent = vec3(1, 1, 1);
	vec3 bitangent = vec3(1, 1, 1);
	float land_alpha_0;
	float land_alpha_1;
	float land_alpha_2;
	float land_alpha_3;
	float land_alpha_4;
	float land_alpha_5;
	float land_alpha_6;
	float land_alpha_7;
};
struct GeometryLand
{

};

#endif