#ifndef OPENGL_DRAWGROUP_H
#define OPENGL_DRAWGROUP_H

#include <functional>

#include <opengl/renderer.h>
#include <opengl/group.h>
#include <opengl/aabb.h>

struct drawgroup : group_type
{
	static int num, masks;
	int mask;
	group_type *const target;
	AABB aabb, obb;
	drawgroup(group_type *, mat4);
	virtual ~drawgroup();
	virtual void Draw(const mat4 &) override;
	bool Invisible();
	void DrawBounds();
	void Cubify();
	virtual void Reset();
};

struct DrawGroupFlatSorted : drawgroup
{
	bool hasTransparency = false;
	DrawGroupFlatSorted(group_type *, mat4);
	virtual ~DrawGroupFlatSorted(){};
	virtual void Draw(const mat4 &) override;
	virtual void Reset() override;
	void SortWith(std::function<bool(const group_type *, const group_type *)>);
	void SortTransparency();
};

#endif