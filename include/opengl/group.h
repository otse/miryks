#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

// part of gloom

#include <opengl/types.h>

#include <opengl/Aabb.h>

struct Group
{
	static int Num;
	bool visible = true;
	Group *parent = nullptr;
	Geometry *geometry = nullptr;
	mat4 matrix;
	mat4 matrixWorld;
	std::vector<Group *> groups, flat;
	Group();
	virtual ~Group();
	void Add(Group *);
	void Remove(Group *);
	void DrawMultiple(const mat4 &);
	virtual void Update();
	virtual void DrawSingle(const mat4 &);
	void Flatten(Group *);
};

struct GroupBounded : Group
{
	Aabb aabb, obb;
	Geometry *axis = nullptr;
	GroupBounded();
	virtual ~GroupBounded(){};
	virtual void Update();
	virtual void DrawSingle(const mat4 &);
};

#endif