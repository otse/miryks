#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

#include <renderer/types.h>

#include <renderer/Aabb.h>

struct Group
{
	static int Num;
	bool visible;
	Group *parent;
	Geometry *geometry;
	mat4 matrix, matrixWorld;
	std::vector<Group *> childGroups, flat;
	Group();
	virtual ~Group();
	void Add(Group *);
	void Remove(Group *);
	virtual void Update();
	virtual void Draw(const mat4 &);
	virtual void DrawRecursive(const mat4 &);
	void Flatten(Group *);
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

#endif