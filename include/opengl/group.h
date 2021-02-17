#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

#include <opengl/types.h>
#include <opengl/aabb.h>

struct Group
{
	static int Num;
	mat4 matrix, matrixWorld;
	bool visible = true;
	Group *parent = nullptr;
	Geometry *geometry = nullptr;
	std::vector<Group *> groups, flat;
	Group();
	virtual Group::~Group();
	void Add(Group *);
	void Remove(Group *);
	void Draws(const mat4 &);
	void Flatten(Group *);
	virtual void Update();
	virtual void Draw(const mat4 &);
};
struct GroupBounded : Group
{
	Aabb aabb, obb;
	Geometry *axis = nullptr;
	GroupBounded();
	virtual ~GroupBounded(){};
	virtual void Update();
	virtual void Draw(const mat4 &);
};

struct DrawGroup
{
	Group *const group;
	DrawGroup(Group *, mat4 = mat4(1.0));
	void Draw();
	void Reset();
	Aabb aabb, obb;
	mat4 matrix;
};

#endif