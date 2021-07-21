#ifndef OGL_RENDERABLE_H
#define OGL_RENDERABLE_H

// part of gloom

#include <opengl/Types.h>

#include <opengl/Aabb.h>

// A basic 3d object with a transformation

struct RenderItem;

struct Renderable
{
	Group *const group;

	Aabb aabb;

	mat4 matrix;

	std::vector<RenderItem> objects;

	Renderable(mat4, Group *);
	~Renderable();

	void Separate();
	void BeginDraws();
};

struct Triangle
{
	vec3 x, y, z, n;
	float dist;
};

struct RenderItem
{
	friend Renderable;

protected:
	RenderItem(Group *, Renderable *);

public:
	Aabb aabb, obb;

	Group *group;
	Renderable *renderable;

	mat4 matrix, modelview;

	std::vector<Triangle> triangles;

	int score = 0;
	int program = 0;
	Material *material = nullptr;

	void Draw();
	void SetInfo();

	inline void set_modelview(mat4 view)
	{
		modelview = view * matrix;
	};

	inline float get_z() const
	{
		return -vec3(modelview[3]).z;
	};

	void TransformVertices();
};
#endif