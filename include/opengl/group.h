#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

#include <OpenGL/Types.h>

#include <OpenGL/Aabb.h>

struct List {
	List();
};
struct Group // : List
{
	static int Num;
	bool visible = true;
	Group *parent = nullptr;
	Geometry *geometry = nullptr;
	Group();
	virtual Group::~Group();
	void Add(Group *);
	void Remove(Group *);
	void Draws(const mat4 &);
	virtual void Update();
	virtual void Draw(const mat4 &);
	void Flatten(Group *);
	std::vector<Group *> groups, flat;
	mat4 matrix, matrixWorld;
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