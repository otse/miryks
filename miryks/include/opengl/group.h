#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

#include <opengl/renderer.h>
#include <opengl/aabb.h>

struct Group
{
	static int num, drawCalls;
	bool visible;
	Group *parent;
	Geometry *geometry, *axis;
	mat4 matrix, matrixWorld;
	std::vector<Group *> childGroups, flat;
	Group();
	virtual ~Group();
	void Add(Group *);
	void Remove(Group *);
	void Flatten(Group *);
	void DrawChilds(const mat4 &);
	virtual void Draw(const mat4 &);
	virtual void Update();
	float GetZ(const mat4 &) const;
};

struct GroupBounded : Group
{
	AABB aabb, obb;
	GroupBounded();
	virtual ~GroupBounded(){};
	virtual void Update() override;
};

#endif