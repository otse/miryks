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
	virtual void Draw(const mat4 &);
	bool ShouldRender();
	void DrawBounds();
	void Cubify();
	virtual void ManualReset();
};

struct DrawGroupFlatSorted : DrawGroup
{
	bool hasTransparency = false;
	DrawGroupFlatSorted(Group *, mat4);
	virtual ~DrawGroupFlatSorted(){};
	virtual void Draw(const mat4 &);
	virtual void ManualReset();
	void Sort(std::function<bool(const Group *, const Group *)>);
	void SortDefault();
};

#endif