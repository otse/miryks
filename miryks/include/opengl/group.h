#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

#include <opengl/renderer.h>
#include <opengl/aabb.h>

struct group_type
{
	static int num, drawCalls;
	bool visible;
	group_type *parent;
	Geometry *geometry, *axis;
	mat4 matrix, matrixWorld;
	std::vector<group_type *> childGroups, flat;
	group_type();
	virtual ~group_type();
	void Add(group_type *);
	void Remove(group_type *);
	void Flatten(group_type *);
	void DrawChilds(const mat4 &);
	virtual void Draw(const mat4 &);
	virtual void Update();
	float GetZ(const mat4 &) const;
};

struct GroupBounded : group_type
{
	AABB aabb, obb;
	GroupBounded();
	virtual ~GroupBounded(){};
	virtual void Update() override;
};

#endif