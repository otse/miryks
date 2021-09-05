#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

#include <renderer/types.h>
#include <renderer/aabb.h>

struct Group
{
	static int Num;
	bool visible;
	Group *parent;
	Geometry *geometry, *axis;
	mat4 matrix, matrixWorld;
	std::vector<Group *> childGroups, flat;
	Group();
	virtual ~Group();
	void Add(Group *);
	void Remove(Group *);
	void Draw(const mat4 &);
	virtual void DrawCoalesce(const mat4 &);
	virtual void Update();
	void DrawChilds(const mat4 &);
	void Flatten(Group *);
	float GetZ() const;
};

struct GroupBounded : Group
{
	AABB aabb, obb;
	GroupBounded();
	virtual ~GroupBounded(){};
	virtual void Update();
};

#endif