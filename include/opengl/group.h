#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

#include <opengl/types.h>
#include <opengl/aabb.h>

struct Group
{
	static int num;

	std::string name;
	std::vector<Group *> groups, flat;

	mat4 matrix, matrixWorld;

	AABB aabb, obb;

	Group *parent;

	Geometry *geometry, *axis;

	Group();
	~Group();
	void Add(Group *);
	void Update();
	void DrawBegin(const mat4 &);
	void Flatten(Group *);

protected:
	void Draw(const mat4 &);
};

struct DrawGroup
{
	Group *const group;
	DrawGroup(Group *, mat4 = mat4(1.0));
	void Draw();
	void Reset();
	std::vector<Group *> flat;
	AABB aabb, obb;
	mat4 matrix;
};

#endif