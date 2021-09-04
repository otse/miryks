#ifndef OPENGL_DRAWGROUP_H
#define OPENGL_DRAWGROUP_H

#include <functional>

#include <renderer/types.h>
#include <renderer/aabb.h>

extern int DGNum, DGMask;

struct DrawGroup : Group
{
	int mask;
	Group *const target;
	AABB aabb, obb;
	DrawGroup(Group *, mat4);
	virtual ~DrawGroup();
	virtual void DrawSelf(const mat4 &);
	void DrawBounds();
	void Cubify();
	bool Enabled();
	virtual void ManualReset();
};

struct DrawGroupSortable : DrawGroup
{
	bool hasTransparency = false;
	DrawGroupSortable(Group *, mat4);
	virtual ~DrawGroupSortable(){};
	virtual void DrawSelf(const mat4 &);
	virtual void ManualReset();
	void Sort(std::function<bool(const Group *, const Group *)>);
	void SortDefault();
};

#endif