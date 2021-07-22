#ifndef OPENGL_DRAGROUP_H
#define OPENGL_DRAWGROUP_H

// part of gloom

#include <functional>

#include <renderer/types.h>

#include <renderer/Aabb.h>

struct DrawGroup
{
	Aabb aabb, obb;
	mat4 matrix;
	Group *const group;
	DrawGroup(Group *, mat4 = mat4(1.0));
	virtual ~DrawGroup(){};
	void DrawBounds();
	virtual void Draw();
	virtual void Reset();
	void Rebound();
	float GetZ() const;
};

struct DrawGroupSortable : DrawGroup
{
	DrawGroupSortable(Group *, mat4 = mat4(1.0));
	virtual ~DrawGroupSortable(){};
	virtual void Draw();
	virtual void Reset();
	void Sort(std::function<bool(const Group *, const Group *)>);
	void SortDefault();
};

#endif